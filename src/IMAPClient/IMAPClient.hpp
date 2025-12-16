#ifndef IMAPCLIENT
#define IMAPCLIENT

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <coroutine>
#include <expected>
#include <memory>

#include "BaseMailClient.hpp"
#include "Error.hpp"
#include "IMAPResponse.hpp"
#include "SSLConnection.hpp"

namespace mailclient::net {

class IMAPClient : BaseMailClient<IMAPResponse> {
 public:
  awaitable_result<IMAPResponse> sendCommand(
      const std::string& command) override;

  static std::shared_ptr<IMAPClient> create(boost::asio::io_context& io);

  awaitable_result<void> startLoop();

 private:
  awaitable_result<IMAPResponse> getResponse();
  IMAPClient(boost::asio::io_context& io);

  bool is_running_;

  struct PendingCommand {
    std::expected<IMAPResponse, Error> resp;
    std::unique_ptr<boost::asio::steady_timer> timer;
    bool is_ready;
  } pending_;

  // struct CommandWaiter {
  //   IMAPClient& client;

  //   bool await_ready();
  //   void await_suspend(std::coroutine_handle<> coro);
  //   std::expected<IMAPResponse, Error> await_resume();
  // };

  // friend struct CommandWaiter;
};

}  // namespace mailclient::net
#endif