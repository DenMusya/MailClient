#include "BaseMailClient.hpp"

#include "IMAPResponse.hpp"
#include "SMTPResponse.hpp"

using namespace mailclient::net;

template <typename Response>
BaseMailClient<Response>::BaseMailClient(boost::asio::io_context& io)
    : connection_(SSLConnection::create(io)) {}

template <typename Response>
awaitable_result<void> BaseMailClient<Response>::connect(
    const std::string& host, const std::string& port) {
  auto res = co_await connection_->asyncConnect(host, port);
  if (!res) co_return std::unexpected(res.error());

  auto res2 = co_await connection_->asyncReadLine();
  if (!res2) co_return std::unexpected(res2.error());

  co_return std::expected<void, Error>();
}

template <typename Response>
BaseMailClient<Response>::~BaseMailClient() {}

template class mailclient::net::BaseMailClient<IMAPResponse>;
template class mailclient::net::BaseMailClient<SMTPResponse>;