#ifndef CLICLIENT
#define CLICLIENT

#include <boost/asio.hpp>
#include <memory>

#include "IMAPClient.hpp"
#include "SMTPClient.hpp"

namespace mailclient {

class CLIClient {
 public:
  static std::shared_ptr<CLIClient> create(
      boost::asio::io_context& io, std::shared_ptr<net::SMTPClient> smtp_client,
      std::shared_ptr<net::IMAPClient> imap_client);

  // Blocking input loop, meant to run on the main thread while io_context
  // runs on its own thread.
  void run();

 private:
  CLIClient(boost::asio::io_context& io,
            std::shared_ptr<net::SMTPClient> smtp_client,
            std::shared_ptr<net::IMAPClient> imap_client);

  boost::asio::io_context& io_;
  std::shared_ptr<net::SMTPClient> smtp_client_;
  std::shared_ptr<net::IMAPClient> imap_client_;

  bool smtp_connected_ = false;
  bool imap_connected_ = false;
};

}  // namespace mailclient
#endif
