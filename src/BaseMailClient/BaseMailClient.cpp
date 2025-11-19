#include "BaseMailClient.hpp"

#include "Logger.hpp"

using namespace mailclient::net;

BaseMailClient::BaseMailClient(boost::asio::io_context& io)
    : ssl_connection_(SSLConnection::create(io)) {}

awaitable_result<void> BaseMailClient::asyncConnect(const std::string& host,
                                                    const std::string& port) {
  auto res = ssl_connection_->asyncConnect(host, port);
}

awaitable_result<void> BaseMailClient::asyncWriteCommand(const std::string& command) {
  auto res = co_await ssl_connection_->asyncWrite(command);
  if (!res) {
    LOG(res.error());
  }
  co_return res;
}

awaitable_result<std::vector<std::string>> BaseMailClient::asyncReadResponse() {
  std::vector<std::string> response;

  while (true) {
    auto line = co_await ssl_connection_->asyncReadLine();
    if (!line) {
      LOG(line.error());
      co_return std::unexpected(line.error());
    }

    response.push_back(line.value());
    if (line.value().size() >= 4 && line.value()[3] == ' ') {
      break;
    }
  }

  co_return response;
}