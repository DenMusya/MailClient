#ifndef IMAPClient
#define IMAPClient

#include <memory>

#include "BaseMailClient.hpp"
#include "CommandQueue.hpp"

namespace mailclient::net {

class SMTPClient : public BaseMailClient {
 public:
  static std::shared_ptr<SMTPClient> create(boost::asio::io_context& io);

  awaitable_result<void> login(const std::string& username, const std::string& password);

  awaitable_result<void> sendMail(const std::string& from, const std::string& to,
                                  const std::string& subject, const std::string& body);

 private:
  SMTPClient(boost::asio::io_context& io);
};

}  // namespace mailclient::net

#endif