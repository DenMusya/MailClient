#ifndef IMAPClient
#define IMAPClient

#include <boost/asio/awaitable.hpp>
#include <memory>

#include "BaseMailClient.hpp"
#include "NetworkError.hpp"
#include "SMTPError.hpp"
#include "SMTPResponse.hpp"

namespace mailclient::net {

class SMTPClient : public BaseMailClient<SMTPResponse> {
 public:
  static std::shared_ptr<SMTPClient> create(boost::asio::io_context& io);

  awaitable_result<void, SMTPError> login(const std::string& username,
                                          const std::string& password);

  awaitable_result<void, SMTPError> sendMail(const std::string& from,
                                             const std::string& to,
                                             const std::string& subject,
                                             const std::string& body);
  awaitable_result<SMTPResponse, NetworkError> sendCommand(
      const std::string& command);

 private:
  SMTPClient(boost::asio::io_context& io);
};

}  // namespace mailclient::net

#endif