#include "IMAPClient.hpp"

#include <expected>

#include "IMAPResponse.hpp"

using namespace mailclient::net;

awaitable_result<IMAPResponse> IMAPClient::getResponse() {
  if (!pending_.is_ready) {
    pending_.timer->expires_at(boost::asio::steady_timer::time_point::max());
    co_await pending_.timer->async_wait(boost::asio::use_awaitable);
  }

  co_return pending_.resp;
}

awaitable_result<IMAPResponse> IMAPClient::sendCommand(
    const std::string& command) {
  auto resp = co_await getResponse();
  co_return std::expected<IMAPResponse, Error>();
}

IMAPClient::IMAPClient(boost::asio::io_context& io) : BaseMailClient(io) {
  pending_.timer = std::make_unique<boost::asio::steady_timer>(io);
}

std::shared_ptr<IMAPClient> IMAPClient::create(boost::asio::io_context& io) {
  return std::shared_ptr<IMAPClient>(new IMAPClient(io));
}

awaitable_result<void> IMAPClient::startLoop() {
  while (is_running_) {
  }
  co_return std::expected<void, Error>();
}