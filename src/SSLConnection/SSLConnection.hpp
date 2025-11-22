#ifndef SSLCONNECTION
#define SSLCONNECTION

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/ssl.hpp>
#include <expected>
#include <memory>

#include "NetworkError.hpp"

namespace mailclient::net {

template <typename T, typename E>
using awaitable_result = boost::asio::awaitable<std::expected<T, E>>;

using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;

class SSLConnection : public std::enable_shared_from_this<SSLConnection> {
 public:
  static std::shared_ptr<SSLConnection> create(boost::asio::io_context& io);

  awaitable_result<void, NetworkError> asyncConnect(const std::string& host,
                                                    const std::string& port);
  awaitable_result<void, NetworkError> asyncWrite(const std::string& msg);
  awaitable_result<std::string, NetworkError> asyncReadLine();

 private:
  SSLConnection(boost::asio::io_context& io);
  boost::asio::io_context& io_;
  ssl::context ctx_;
  tcp::resolver resolver_;
  ssl::stream<tcp::socket> stream_;
  boost::asio::streambuf buffer_;
};

}  // namespace mailclient::net

#endif