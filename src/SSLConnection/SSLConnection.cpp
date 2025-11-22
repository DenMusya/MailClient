#include "SSLConnection.hpp"

#include <boost/asio/use_awaitable.hpp>

#include "NetworkError.hpp"

using namespace mailclient::net;

std::shared_ptr<SSLConnection> SSLConnection::create(
    boost::asio::io_context& io) {
  return std::shared_ptr<SSLConnection>(new SSLConnection(io));
}

SSLConnection::SSLConnection(boost::asio::io_context& io)
    : io_(io),
      ctx_(ssl::context::tlsv12_client),
      resolver_(io),
      stream_(io, ctx_) {
  ctx_.set_default_verify_paths();
}

awaitable_result<void, NetworkError> SSLConnection::asyncConnect(
    const std::string& host, const std::string& port) {
  try {
    auto results = co_await resolver_.async_resolve(host, port,
                                                    boost::asio::use_awaitable);
    co_await boost::asio::async_connect(stream_.lowest_layer(), results,
                                        boost::asio::use_awaitable);
    stream_.set_verify_mode(ssl::verify_peer);
    stream_.set_verify_callback(ssl::host_name_verification(host));

    co_await stream_.async_handshake(ssl::stream_base::client,
                                     boost::asio::use_awaitable);

    co_return std::expected<void, NetworkError>{};
  } catch (const boost::system::system_error& err) {
    co_return std::unexpected(NetworkError(err.code()));
  }
}

awaitable_result<void, NetworkError> SSLConnection::asyncWrite(
    const std::string& msg) {
  try {
    co_await boost::asio::async_write(stream_, boost::asio::buffer(msg),
                                      boost::asio::use_awaitable);
    co_return std::expected<void, NetworkError>{};
  } catch (const boost::system::system_error& err) {
    co_return std::unexpected(NetworkError(err.code()));
  }
}

awaitable_result<std::string, NetworkError> SSLConnection::asyncReadLine() {
  try {
    co_await boost::asio::async_read_until(stream_, buffer_, "\r\n",
                                           boost::asio::use_awaitable);
    std::istream is(&buffer_);
    std::string line;
    std::getline(is, line);

    if (!line.empty() && line.back() == '\r') line.pop_back();
    co_return line;
  } catch (const boost::system::system_error& err) {
    co_return std::unexpected(NetworkError(err.code()));
  }
}