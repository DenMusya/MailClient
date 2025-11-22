#ifndef BASEMAILCLIENT
#define BASEMAILCLIENT

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <string>

#include "NetworkError.hpp"
#include "SSLConnection.hpp"

namespace mailclient::net {

template <typename Response>
class BaseMailClient {
 public:
  BaseMailClient(boost::asio::io_context& io);
  awaitable_result<void, NetworkError> connect(const std::string& host,
                                               const std::string& port);
  // awaitable_result<void> asyncWriteCommand(const std::string& command);
  // awaitable_result<std::vector<std::string>> asyncReadResponse();
  virtual awaitable_result<Response, NetworkError> sendCommand(
      const std::string& command) = 0;

  virtual ~BaseMailClient() = 0;

 protected:
  std::shared_ptr<SSLConnection> connection_;
};

}  // namespace mailclient::net

#endif