#ifndef IMAPRESPONSE
#define IMAPRESPONSE

#include <vector>

#include "string"

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

  IMAPStatus status;
  IMAPCommandType command;
};

struct EventMessage {
  std::string raw;
  EventType event;
};
}  // namespace mailclient::net

#endif