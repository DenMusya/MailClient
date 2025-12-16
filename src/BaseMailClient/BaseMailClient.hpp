#ifndef BASEMAILCLIENT
#define BASEMAILCLIENT

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <string>

#include "SSLConnection.hpp"

namespace mailclient::net {

template <typename Response>
class BaseMailClient {
 public:
  BaseMailClient(boost::asio::io_context& io);
  awaitable_result<void> connect(const std::string& host,
                                 const std::string& port);
  virtual awaitable_result<Response> sendCommand(
      const std::string& command) = 0;

  virtual ~BaseMailClient() = 0;

 protected:
  std::shared_ptr<SSLConnection> connection_;
};

}  // namespace mailclient::net

#endif