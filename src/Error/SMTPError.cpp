#include "SMTPError.hpp"

#include "NetworkError.hpp"
#include "SMTPResponse.hpp"

using namespace mailclient::net;

SMTPError::SMTPError(const boost::system::error_code& net_error)
    : NetworkError(net_error) {}

SMTPError::SMTPError(const SMTPResponse& smtp_error)
    : smtp_error_(smtp_error) {}

SMTPError::SMTPError(const NetworkError& network_err)
    : NetworkError(network_err) {}

std::string SMTPError::what() const {
  if (net_error_) {
    return "Network error: " + net_error_.message();
  }

  return "SMTP error: " + smtp_error_.text();
}