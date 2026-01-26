#ifndef SMTPRESPONSE
#define SMTPRESPONSE

#include <string>
#include <vector>

namespace mailclient::net {

enum class SMTPStatus {
  PositivePreliminary = 1,
  PositiveCompletion = 2,
  PositiveIntermediate = 3,
  TransientNegative = 4,
  PermanentNegative = 5,
  Unknown = 0
};

enum class SMTPCode {
  Unknown = 0,
  SystemStatus = 211,
  HelpMessage = 214,
  Ready = 220,
  ClosingTransmission = 221,
  Ok = 250,
  UserNotLocalWillForward = 251,
  AuthSuccessful = 235,
  AuthContinue = 334,
  StartMailInput = 354,
  ServiceNotAvailable = 421,
  MailboxUnavailable = 450,
  LocalError = 451,
  InsufficientStorage = 452,
  SyntaxError = 500,
  ArgSyntaxError = 501,
  CommandNotImplemented = 502,
  BadSequence = 503,
  AuthRequired = 530,
  MailboxUnavailableFinal = 550
};

class SMTPResponse {
 public:
  int rawCode() const;
  std::string text() const;
  SMTPCode code() const;
  SMTPStatus status() const;

  bool is(SMTPCode code) const;
  void addLine(const std::string& line);

 private:
  int rawCode_;
  SMTPCode code_;
  std::vector<std::string> lines_;
};

}  // namespace mailclient::net
#endif