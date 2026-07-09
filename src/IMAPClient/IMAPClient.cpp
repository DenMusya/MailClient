#include "IMAPClient.hpp"

#include <boost/asio/redirect_error.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <expected>
#include <optional>
#include <sstream>
#include <strings.h>

#include "Error.hpp"
#include "Logger.hpp"

using namespace mailclient;
using namespace mailclient::net;

namespace {

std::optional<std::size_t> parseLiteralSize(const std::string& line) {
  if (line.empty() || line.back() != '}') return std::nullopt;
  auto open = line.rfind('{');
  if (open == std::string::npos) return std::nullopt;

  std::string num = line.substr(open + 1, line.size() - open - 2);
  if (!num.empty() && num.back() == '+') num.pop_back();
  if (num.empty()) return std::nullopt;

  for (char c : num)
    if (!std::isdigit(static_cast<unsigned char>(c))) return std::nullopt;

  return static_cast<std::size_t>(std::stoul(num));
}

IMAPStatus parseStatus(const std::string& line) {
  std::istringstream iss(line);
  std::string tag, status;
  iss >> tag >> status;
  if (status == "OK") return IMAPStatus::OK;
  if (status == "NO") return IMAPStatus::NO;
  return IMAPStatus::BAD;
}

std::optional<uint32_t> parseExists(const std::string& line) {
  std::istringstream iss(line);
  std::string star, keyword;
  uint32_t num = 0;
  if (!(iss >> star >> num >> keyword)) return std::nullopt;
  if (star == "*" && keyword == "EXISTS") return num;
  return std::nullopt;
}

std::string parseHeaderField(const std::string& headers,
                             const std::string& name) {
  std::istringstream iss(headers);
  std::string line;
  std::string prefix = name + ":";

  while (std::getline(iss, line)) {
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (line.size() >= prefix.size() &&
        strncasecmp(line.c_str(), prefix.c_str(), prefix.size()) == 0) {
      std::string value = line.substr(prefix.size());
      auto start = value.find_first_not_of(" \t");
      return start == std::string::npos ? "" : value.substr(start);
    }
  }
  return "";
}

}  // namespace

IMAPClient::IMAPClient(boost::asio::io_context& io)
    : BaseMailClient(io),
      io_(io),
      mailbox_(std::make_shared<VirtualMailbox>()),
      conn_mutex_(io, 1) {
  conn_mutex_.try_send(boost::system::error_code{});
}

std::shared_ptr<IMAPClient> IMAPClient::create(boost::asio::io_context& io) {
  return std::shared_ptr<IMAPClient>(new IMAPClient(io));
}

boost::asio::awaitable<void> IMAPClient::acquire() {
  boost::system::error_code ec;
  co_await conn_mutex_.async_receive(
      boost::asio::redirect_error(boost::asio::use_awaitable, ec));
}

void IMAPClient::release() {
  conn_mutex_.try_send(boost::system::error_code{});
}

std::string IMAPClient::nextTag() {
  char buf[16];
  std::snprintf(buf, sizeof(buf), "A%04u", ++tag_counter_);
  return buf;
}

awaitable_result<IMAPResponse> IMAPClient::readResponse(
    const std::string& tag) {
  IMAPResponse response;
  response.tag = tag;

  while (true) {
    auto line = co_await connection_->asyncReadLine();
    if (!line) co_return std::unexpected(line.error());

    const std::string& value = line.value();
    response.lines.push_back(value);
    response.raw += value + "\n";

    if (auto size = parseLiteralSize(value)) {
      auto literal = co_await connection_->asyncReadN(*size);
      if (!literal) co_return std::unexpected(literal.error());
      response.literals.push_back(literal.value());
      response.raw += literal.value();
      continue;
    }

    if (value.rfind(tag, 0) == 0) {
      response.status = parseStatus(value);
      break;
    }
  }

  co_return response;
}

awaitable_result<IMAPResponse> IMAPClient::sendCommand(
    const std::string& command) {
  co_await acquire();

  std::string tag = nextTag();
  auto res = co_await connection_->asyncWrite(tag + " " + command + "\r\n");
  if (!res) {
    release();
    co_return std::unexpected(res.error());
  }

  auto response = co_await readResponse(tag);
  release();
  co_return response;
}

awaitable_result<void> IMAPClient::login(const std::string& username,
                                         const std::string& password) {
  auto res =
      co_await sendCommand("LOGIN \"" + username + "\" \"" + password + "\"");
  if (!res) co_return std::unexpected(res.error());
  if (!res->ok()) co_return std::unexpected(Error(*res));
  co_return std::expected<void, Error>();
}

awaitable_result<void> IMAPClient::select(const std::string& mailbox) {
  auto res = co_await sendCommand("SELECT " + mailbox);
  if (!res) co_return std::unexpected(res.error());
  if (!res->ok()) co_return std::unexpected(Error(*res));

  for (const auto& line : res->lines)
    if (auto count = parseExists(line)) message_count_ = *count;

  co_return std::expected<void, Error>();
}

awaitable_result<MailboxMessage> IMAPClient::fetchHeaders(uint32_t seq) {
  auto res = co_await sendCommand(
      "FETCH " + std::to_string(seq) +
      " (BODY.PEEK[HEADER.FIELDS (FROM SUBJECT DATE)])");
  if (!res) co_return std::unexpected(res.error());
  if (!res->ok()) co_return std::unexpected(Error(*res));

  MailboxMessage msg;
  msg.seq = seq;

  const std::string& headers =
      res->literals.empty() ? res->raw : res->literals.front();
  msg.from = parseHeaderField(headers, "From");
  msg.subject = parseHeaderField(headers, "Subject");
  msg.date = parseHeaderField(headers, "Date");

  mailbox_->upsertMeta(msg);
  co_return msg;
}

awaitable_result<MailboxMessage> IMAPClient::fetchBody(uint32_t seq) {
  auto res =
      co_await sendCommand("FETCH " + std::to_string(seq) + " (BODY.PEEK[TEXT])");
  if (!res) co_return std::unexpected(res.error());
  if (!res->ok()) co_return std::unexpected(Error(*res));

  std::string body = res->literals.empty() ? "" : res->literals.front();
  mailbox_->setBody(seq, body);

  co_return mailbox_->get(seq).value_or(MailboxMessage{});
}

awaitable_result<void> IMAPClient::loadRecent(uint32_t count) {
  if (message_count_ == 0) co_return std::expected<void, Error>();

  uint32_t start = message_count_ > count ? message_count_ - count + 1 : 1;
  for (uint32_t seq = start; seq <= message_count_; ++seq) {
    auto res = co_await fetchHeaders(seq);
    if (!res) co_return std::unexpected(res.error());
  }

  co_return std::expected<void, Error>();
}

awaitable_result<void> IMAPClient::logout() {
  is_running_ = false;

  auto res = co_await sendCommand("LOGOUT");
  if (!res) co_return std::unexpected(res.error());

  auto closed = co_await connection_->asyncClose();
  if (closed) co_return std::expected<void, Error>();

  auto err_code = std::get<Error::NetworkData>(closed.error().data()).err_code;
  if (err_code == boost::asio::ssl::error::stream_truncated)
    co_return std::expected<void, Error>();

  co_return std::unexpected(closed.error());
}

awaitable_result<void> IMAPClient::startSync() {
  is_running_ = true;
  boost::asio::steady_timer timer(io_);

  while (is_running_) {
    timer.expires_after(std::chrono::seconds(5));
    boost::system::error_code ec;
    co_await timer.async_wait(
        boost::asio::redirect_error(boost::asio::use_awaitable, ec));
    if (!is_running_) break;

    uint32_t before = message_count_;
    auto res = co_await sendCommand("NOOP");
    if (!res) {
      LOG(res.error().what());
      break;
    }
    if (!res->ok()) continue;

    for (const auto& line : res->lines)
      if (auto count = parseExists(line)) message_count_ = *count;

    for (uint32_t seq = before + 1; seq <= message_count_; ++seq) {
      auto msg = co_await fetchHeaders(seq);
      if (!msg) {
        LOG(msg.error().what());
        continue;
      }
      PRINT("\n[NEW MAIL] #" + std::to_string(msg->seq) +
            "  From: " + msg->from + "  Subject: " + msg->subject);
    }
  }

  co_return std::expected<void, Error>();
}

void IMAPClient::stopSync() { is_running_ = false; }
