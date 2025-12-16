#include "SMTPClient.hpp"

#include "Base64Encode.hpp"
#include "Error.hpp"
#include "SMTPResponse.hpp"

using namespace mailclient::net;

SMTPClient::SMTPClient(boost::asio::io_context& io) : BaseMailClient(io) {}

std::shared_ptr<SMTPClient> SMTPClient::create(boost::asio::io_context& io) {
  return std::shared_ptr<SMTPClient>(new SMTPClient(io));
}

awaitable_result<SMTPResponse> SMTPClient::sendCommand(
    const std::string& command) {
  auto res = co_await connection_->asyncWrite(command);
  if (!res) co_return std::unexpected(res.error());

  SMTPResponse response;

  while (true) {
    auto line = co_await connection_->asyncReadLine();
    if (!line) co_return std::unexpected(line.error());

    response.addLine(line.value());
    if (line.value().size() >= 4 && line.value()[3] == ' ') {
      break;
    }
  }

  co_return response;
}

awaitable_result<void> SMTPClient::login(const std::string& username,
                                         const std::string& password) {
  auto res = co_await sendCommand("EHLO localhost\r\n");
  if (!res) co_return std::unexpected(res.error());

  res = co_await sendCommand("AUTH LOGIN\r\n");
  if (!res) co_return std::unexpected(res.error());

  if (!res->is(SMTPCode::AuthContinue)) co_return std::unexpected(Error(*res));

  std::string encodedUser = base64Encode(username);
  res = co_await sendCommand(encodedUser + "\r\n");
  if (!res) co_return std::unexpected(res.error());
  if (!res->is(SMTPCode::AuthContinue)) co_return std::unexpected(Error(*res));

  std::string encodedPass = base64Encode(password);
  res = co_await sendCommand(encodedPass + "\r\n");
  if (!res) co_return std::unexpected(res.error());
  if (!res->is(SMTPCode::AuthSuccessful))
    co_return std::unexpected(Error(*res));

  co_return std::expected<void, Error>();
}

awaitable_result<void> SMTPClient::sendMail(const std::string& from,
                                            const std::string& to,
                                            const std::string& subject,
                                            const std::string& body) {
  auto res = co_await sendCommand("MAIL FROM:<" + from + ">\r\n");
  if (!res) co_return std::unexpected(res.error());
  if (!res->is(SMTPCode::Ok)) co_return std::unexpected(Error(*res));

  res = co_await sendCommand("RCPT TO:<" + to + ">\r\n");
  if (!res) co_return std::unexpected(res.error());
  if (!res->is(SMTPCode::Ok)) co_return std::unexpected(Error(*res));

  res = co_await sendCommand("DATA\r\n");
  if (!res) co_return std::unexpected(res.error());
  if (!res->is(SMTPCode::StartMailInput))
    co_return std::unexpected(Error(*res));

  std::stringstream ss;
  ss << "Subject: " << subject << "\r\n";
  ss << "From: " << from << "\r\n";
  ss << "To: " << to << "\r\n";
  ss << "\r\n";
  ss << body << "\r\n.\r\n";

  res = co_await sendCommand(ss.str());
  if (!res) co_return std::unexpected(res.error());

  if (!res->is(SMTPCode::Ok)) co_return std::unexpected(Error(*res));

  co_return std::expected<void, Error>();
}

awaitable_result<void> SMTPClient::quit() {
  auto res = co_await sendCommand("QUIT\r\n");
  if (!res) co_return std::unexpected(res.error());

  if (!res->is(SMTPCode::ClosingTransmission))
    co_return std::unexpected(Error(*res));

  auto res2 = co_await connection_->asyncClose();

  if (res2) co_return std::expected<void, Error>();

  auto err_code = std::get<Error::NetworkData>(res2.error().data()).err_code;

  if (err_code == boost::asio::ssl::error::stream_truncated)
    co_return std::expected<void, Error>();

  co_return std::unexpected(res2.error());
}