#include "SMTPResponse.hpp"

using namespace mailclient::net;

SMTPCode smtpCodeFromInt(int code) {
  switch (code) {
    case 211:
      return SMTPCode::SystemStatus;
    case 214:
      return SMTPCode::HelpMessage;
    case 220:
      return SMTPCode::Ready;
    case 221:
      return SMTPCode::ClosingTransmission;
    case 235:
      return SMTPCode::AuthSuccessful;
    case 250:
      return SMTPCode::Ok;
    case 251:
      return SMTPCode::UserNotLocalWillForward;
    case 334:
      return SMTPCode::AuthContinue;
    case 354:
      return SMTPCode::StartMailInput;
    case 421:
      return SMTPCode::ServiceNotAvailable;
    case 450:
      return SMTPCode::MailboxUnavailable;
    case 451:
      return SMTPCode::LocalError;
    case 452:
      return SMTPCode::InsufficientStorage;
    case 500:
      return SMTPCode::SyntaxError;
    case 501:
      return SMTPCode::ArgSyntaxError;
    case 502:
      return SMTPCode::CommandNotImplemented;
    case 503:
      return SMTPCode::BadSequence;
    case 530:
      return SMTPCode::AuthRequired;
    case 550:
      return SMTPCode::MailboxUnavailableFinal;

    default:
      return SMTPCode::Unknown;
  }
}

void SMTPResponse::addLine(const std::string& line) {
  rawCode_ = std::stoi(line.substr(0, 3));
  lines_.push_back(line);
  code_ = smtpCodeFromInt(rawCode_);
}

int SMTPResponse::rawCode() const { return rawCode_; }

SMTPCode SMTPResponse::code() const { return code_; }

bool SMTPResponse::is(SMTPCode code) const { return code_ == code; }

std::string SMTPResponse::text() const {
  std::string result;
  for (auto& line : lines_) result += line + "\n";
  return result;
}

SMTPStatus SMTPResponse::status() const {
  if (rawCode_ < 100 || rawCode_ > 599) return SMTPStatus::Unknown;
  return static_cast<SMTPStatus>(rawCode_ / 100);
}
