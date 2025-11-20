#include "BaseMailClient.hpp"

#include <iostream>

#include "Logger.hpp"

using namespace mailclient::net;

BaseMailClient::BaseMailClient(boost::asio::io_context& io)
    : connection_(SSLConnection::create(io)) {}

awaitable_result<void> BaseMailClient::asyncConnect(const std::string& host,
                                                    const std::string& port) {
  auto res = co_await connection_->asyncConnect(host, port);
  if (!res) co_return std::unexpected(res.error());

  auto res2 = co_await connection_->asyncReadLine();
  if (!res2) co_return std::unexpected(res2.error());
}

awaitable_result<void> BaseMailClient::asyncWriteCommand(const std::string& command) {
  auto res = co_await connection_->asyncWrite(command);
  if (!res) co_return std::unexpected(res.error());
}

awaitable_result<std::vector<std::string>> BaseMailClient::asyncReadResponse() {
  std::vector<std::string> response;

  while (true) {
    auto line = co_await connection_->asyncReadLine();
    if (!line) co_return std::unexpected(line.error());

    response.push_back(line.value());
    if (line.value().size() >= 4 && line.value()[3] == ' ') {
      break;
    }
  }

  co_return response;
}