#include "VirtualMailbox.hpp"

using namespace mailclient;

void VirtualMailbox::upsertMeta(const MailboxMessage& msg) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto& stored = messages_[msg.seq];
  stored.seq = msg.seq;
  stored.from = msg.from;
  stored.subject = msg.subject;
  stored.date = msg.date;
}

void VirtualMailbox::setBody(uint32_t seq, const std::string& body) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto& stored = messages_[seq];
  stored.seq = seq;
  stored.body = body;
  stored.has_body = true;
}

std::optional<MailboxMessage> VirtualMailbox::get(uint32_t seq) const {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = messages_.find(seq);
  if (it == messages_.end()) return std::nullopt;
  return it->second;
}

std::vector<MailboxMessage> VirtualMailbox::snapshot() const {
  std::lock_guard<std::mutex> lock(mutex_);
  std::vector<MailboxMessage> result;
  result.reserve(messages_.size());
  for (const auto& [seq, msg] : messages_) result.push_back(msg);
  return result;
}

std::size_t VirtualMailbox::size() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return messages_.size();
}
