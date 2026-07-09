#ifndef IMAPRESPONSE
#define IMAPRESPONSE

#include <string>
#include <vector>

namespace mailclient::net {

enum class IMAPStatus {
  OK,
  NO,
  BAD,
};

enum class EventType {
  Unknown,
  Exists,
  Recent,
  Expunge,
  Flags,
};

enum class IMAPCommandType {
  Unknown,
  Login,
  Select,
  Fetch,
  Search,
  Logout,
};

struct SessionMessage {
  std::string literal;
  std::string raw;  // command + literal
  IMAPCommandType command;
};

struct IMAPResponse {
  std::string tag;
  std::string raw;

  std::vector<std::string> lines;     // every line received for this response
  std::vector<std::string> literals;  // {n} literal blocks in arrival order

  IMAPStatus status = IMAPStatus::BAD;
  IMAPCommandType command = IMAPCommandType::Unknown;

  bool ok() const { return status == IMAPStatus::OK; }
};

struct EventMessage {
  std::string raw;
  EventType event;
};
}  // namespace mailclient::net

#endif