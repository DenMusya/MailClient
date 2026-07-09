#ifndef IMAPCLIENT
#define IMAPCLIENT

#include <boost/asio/experimental/channel.hpp>
#include <boost/asio/io_context.hpp>
#include <atomic>
#include <cstdint>
#include <memory>
#include <string>

#include "BaseMailClient.hpp"
#include "IMAPResponse.hpp"
#include "VirtualMailbox.hpp"

namespace mailclient::net {

class IMAPClient : public BaseMailClient<IMAPResponse> {
 public:
  static std::shared_ptr<IMAPClient> create(boost::asio::io_context& io);

  awaitable_result<IMAPResponse> sendCommand(
      const std::string& command) override;

  awaitable_result<void> login(const std::string& username,
                               const std::string& password);
  awaitable_result<void> select(const std::string& mailbox);
  awaitable_result<MailboxMessage> fetchHeaders(uint32_t seq);
  awaitable_result<MailboxMessage> fetchBody(uint32_t seq);
  awaitable_result<void> loadRecent(uint32_t count);
  awaitable_result<void> logout();

  // Background poll loop: detects new mail and refreshes the virtual mailbox.
  awaitable_result<void> startSync();
  void stopSync();

  std::shared_ptr<VirtualMailbox> mailbox() const { return mailbox_; }

 private:
  IMAPClient(boost::asio::io_context& io);

  std::string nextTag();
  awaitable_result<IMAPResponse> readResponse(const std::string& tag);

  // Async mutex guarding exclusive access to the single connection so the
  // background sync loop and on-demand user requests never interleave.
  boost::asio::awaitable<void> acquire();
  void release();

  boost::asio::io_context& io_;
  std::shared_ptr<VirtualMailbox> mailbox_;
  boost::asio::experimental::channel<void(boost::system::error_code)>
      conn_mutex_;

  uint32_t tag_counter_ = 0;
  uint32_t message_count_ = 0;
  std::atomic<bool> is_running_ = false;
};

}  // namespace mailclient::net
#endif
