#ifndef VIRTUALMAILBOX
#define VIRTUALMAILBOX

#include <cstdint>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace mailclient {

struct MailboxMessage {
  uint32_t seq = 0;
  std::string from;
  std::string subject;
  std::string date;
  std::string body;
  bool has_body = false;
};

// Thread-safe in-memory view of a remote mailbox. The IMAP sync loop writes to
// it from the network thread while the CLI reads from it on the main thread.
class VirtualMailbox {
 public:
  void upsertMeta(const MailboxMessage& msg);
  void setBody(uint32_t seq, const std::string& body);

  std::optional<MailboxMessage> get(uint32_t seq) const;
  std::vector<MailboxMessage> snapshot() const;
  std::size_t size() const;

 private:
  mutable std::mutex mutex_;
  std::map<uint32_t, MailboxMessage> messages_;
};

}  // namespace mailclient

#endif
