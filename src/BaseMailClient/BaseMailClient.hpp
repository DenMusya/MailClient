#ifndef BASEMAILCLIENT
#define BASEMAILCLIENT

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "SSLConnection.hpp"

namespace mailclient::net {

class BaseMailClient {
 public:
  BaseMailClient(boost::asio::io_context& io);
  awaitable_result<void> asyncConnect(const std::string& host, const std::string& port);
  awaitable_result<void> asyncWriteCommand(const std::string& command);
  awaitable_result<std::vector<std::string>> asyncReadResponse();

 protected:
  std::shared_ptr<SSLConnection> connection_;
};

}  // namespace mailclient::net

#endif