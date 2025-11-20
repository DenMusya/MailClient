#ifndef CLICLIENT
#define CLICLIENT

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <memory>

#include "SMTPClient.hpp"

namespace mailclient {

class CLIClient {
 public:
  static std::shared_ptr<CLIClient> create(std::shared_ptr<net::SMTPClient> smtp_client);
  boost::asio::awaitable<void> run();

 private:
  CLIClient(std::shared_ptr<net::SMTPClient> smtp_client);
  std::shared_ptr<net::SMTPClient> smtp_client_;
};

}  // namespace mailclient
#endif