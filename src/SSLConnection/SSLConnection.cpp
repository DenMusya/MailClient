#include "SSLConnection.hpp"

#include <boost/asio/read.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <expected>

#include "Error.hpp"

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

awaitable_result<void> SSLConnection::asyncConnect(const std::string& host,
                                                   const std::string& port) {
  auto ex = resolver_.get_executor();
  stream_ = ssl::stream<tcp::socket>(ex, ctx_);

  boost::system::error_code ec;
  auto results = co_await resolver_.async_resolve(
      host, port, boost::asio::redirect_error(boost::asio::use_awaitable, ec));

  if (ec) co_return std::unexpected(Error(ec));

  co_await boost::asio::async_connect(
      stream_.lowest_layer(), results,
      boost::asio::redirect_error(boost::asio::use_awaitable, ec));

  if (ec) co_return std::unexpected(Error(ec));

  stream_.set_verify_mode(ssl::verify_peer);
  stream_.set_verify_callback(ssl::host_name_verification(host));

  co_await stream_.async_handshake(
      ssl::stream_base::client,
      boost::asio::redirect_error(boost::asio::use_awaitable, ec));

  if (ec) co_return std::unexpected(Error(ec));
  co_return std::expected<void, Error>{};
}

awaitable_result<void> SSLConnection::asyncWrite(const std::string& msg) {
  boost::system::error_code ec;
  co_await boost::asio::async_write(
      stream_, boost::asio::buffer(msg),
      boost::asio::redirect_error(boost::asio::use_awaitable, ec));
  if (ec) co_return std::unexpected(Error(ec));
  co_return std::expected<void, Error>{};
}

awaitable_result<std::string> SSLConnection::asyncReadLine() {
  boost::system::error_code ec;

  co_await boost::asio::async_read_until(
      stream_, buffer_, "\r\n",
      boost::asio::redirect_error(boost::asio::use_awaitable, ec));

  if (ec) co_return std::unexpected(Error(ec));

  std::istream is(&buffer_);
  std::string line;
  std::getline(is, line);

  if (!line.empty() && line.back() == '\r') line.pop_back();
  co_return line;
}

awaitable_result<std::string> SSLConnection::asyncReadN(std::size_t n) {
  boost::system::error_code ec;

  if (buffer_.size() < n) {
    co_await boost::asio::async_read(
        stream_, buffer_, boost::asio::transfer_at_least(n - buffer_.size()),
        boost::asio::redirect_error(boost::asio::use_awaitable, ec));
    if (ec) co_return std::unexpected(Error(ec));
  }

  std::string data(n, '\0');
  std::istream is(&buffer_);
  is.read(data.data(), static_cast<std::streamsize>(n));
  co_return data;
}

awaitable_result<void> SSLConnection::asyncClose() {
  boost::system::error_code ec;

  co_await stream_.async_shutdown(
      boost::asio::redirect_error(boost::asio::use_awaitable, ec));
  if (ec) co_return std::unexpected(Error(ec));

  stream_.lowest_layer().close();

  co_return std::expected<void, Error>();
}