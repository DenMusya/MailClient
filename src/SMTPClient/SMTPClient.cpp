#include "SMTPClient.hpp"

#include "Base64Encode.hpp"

using namespace mailclient::net;

SMTPClient::SMTPClient(boost::asio::io_context& io) : BaseMailClient(io) {}

std::shared_ptr<SMTPClient> SMTPClient::create(boost::asio::io_context& io) {
  return std::shared_ptr<SMTPClient>(new SMTPClient(io));
}

awaitable_result<void> SMTPClient::login(const std::string& username, const std::string& password) {
  auto res = co_await connection_->asyncWrite("EHLO localhost\r\n");
  if (!res) co_return std::unexpected(res.error());
  if (auto tmp = co_await asyncReadResponse(); !tmp) co_return std::unexpected(tmp.error());

  res = co_await connection_->asyncWrite("AUTH LOGIN\r\n");
  if (!res) co_return std::unexpected(res.error());

  auto line = co_await connection_->asyncReadLine();
  if (!line) co_return std::unexpected(line.error());
  if (line.value().substr(0, 3) != "334")
    co_return std::unexpected(boost::system::error_code{1, boost::system::generic_category()});

  std::string encodedUser = base64Encode(username);
  res = co_await connection_->asyncWrite(encodedUser + "\r\n");
  if (!res) co_return std::unexpected(res.error());

  line = co_await connection_->asyncReadLine();
  if (!line) co_return std::unexpected(line.error());
  if (line.value().substr(0, 3) != "334")
    co_return std::unexpected(boost::system::error_code{2, boost::system::generic_category()});

  std::string encodedPass = base64Encode(password);
  res = co_await connection_->asyncWrite(encodedPass + "\r\n");
  if (!res) co_return std::unexpected(res.error());

  line = co_await connection_->asyncReadLine();
  if (!line) co_return std::unexpected(line.error());
  if (line.value().substr(0, 3) != "235")
    co_return std::unexpected(boost::system::error_code{3, boost::system::generic_category()});
}

awaitable_result<void> SMTPClient::sendMail(const std::string& from, const std::string& to,
                                            const std::string& subject, const std::string& body) {
  auto res = co_await connection_->asyncWrite("MAIL FROM:<" + from + ">\r\n");
  if (!res) co_return std::unexpected(res.error());

  auto line = co_await connection_->asyncReadLine();
  if (!line) co_return std::unexpected(line.error());
  if (line.value().substr(0, 3) != "250")
    co_return std::unexpected(boost::system::error_code{1, boost::system::generic_category()});

  res = co_await connection_->asyncWrite("RCPT TO:<" + to + ">\r\n");
  if (!res) co_return std::unexpected(res.error());

  line = co_await connection_->asyncReadLine();
  if (!line) co_return std::unexpected(line.error());
  if (line.value().substr(0, 3) != "250")
    co_return std::unexpected(boost::system::error_code{2, boost::system::generic_category()});

  res = co_await connection_->asyncWrite("DATA\r\n");
  if (!res) co_return std::unexpected(res.error());

  line = co_await connection_->asyncReadLine();
  if (!line) co_return std::unexpected(line.error());
  if (line.value().substr(0, 3) != "354")
    co_return std::unexpected(boost::system::error_code{3, boost::system::generic_category()});

  std::stringstream ss;
  ss << "Subject: " << subject << "\r\n";
  ss << "From: " << from << "\r\n";
  ss << "To: " << to << "\r\n";
  ss << "\r\n";
  ss << body << "\r\n.\r\n";

  res = co_await connection_->asyncWrite(ss.str());
  if (!res) co_return std::unexpected(res.error());

  line = co_await connection_->asyncReadLine();
  if (!line) co_return std::unexpected(line.error());
  if (line.value().substr(0, 3) != "250")
    co_return std::unexpected(boost::system::error_code{4, boost::system::generic_category()});
}
