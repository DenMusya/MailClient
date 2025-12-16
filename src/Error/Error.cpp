#include "Error.hpp"

using namespace mailclient::net;

std::string Error::what() const { return error_message_; }

ErrorKind Error::kind() const { return error_kind_; }

Error::Error(const SMTPResponse& smtp_resp)
    : error_kind_(ErrorKind::SMTP),
      error_message_("SMTP error: " + smtp_resp.text()),
      error_data_(SMTPData()) {}

Error::Error(const IMAPResponse& imap_resp)
    : error_kind_(ErrorKind::IMAP),
      error_message_("IMAP error: " + imap_resp.raw),
      error_data_(IMAPData()) {}

Error::Error(const boost::system::error_code& err_code)
    : error_kind_(ErrorKind::Network),
      error_message_("Network error: " + err_code.message()),
      error_data_(NetworkData(err_code)) {}

Error::ErrorData Error::data() const { return error_data_; }