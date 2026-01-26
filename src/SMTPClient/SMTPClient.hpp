#ifndef SMTPCLIENT
#define SMTPCLIENT

#include <boost/asio/awaitable.hpp>
#include <memory>

#include "BaseMailClient.hpp"
#include "SMTPResponse.hpp"

namespace mailclient::net {

class SMTPClient : public BaseMailClient<SMTPResponse> {
 public:
  static std::shared_ptr<SMTPClient> create(boost::asio::io_context& io);

  awaitable_result<void> login(const std::string& username,
                               const std::string& password);

  awaitable_result<void> sendMail(const std::string& from,
                                  const std::string& to,
                                  const std::string& subject,
                                  const std::string& body);
  awaitable_result<SMTPResponse> sendCommand(const std::string& command);

  awaitable_result<void> quit();

 private:
  SMTPClient(boost::asio::io_context& io);
};

}  // namespace mailclient::net

#endif