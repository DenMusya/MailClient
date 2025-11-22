#ifndef SMTPERROR
#define SMTPERROR

#include <boost/asio.hpp>
#include <boost/system/detail/error_code.hpp>

#include "NetworkError.hpp"
#include "SMTPResponse.hpp"

namespace mailclient::net {

class SMTPError : public NetworkError {
 public:
  explicit SMTPError(const NetworkError& network_err);
  explicit SMTPError(const boost::system::error_code& net_error);
  explicit SMTPError(const SMTPResponse& smtp_error);

  std::string what() const override;

 private:
  SMTPResponse smtp_error_;
};

}  // namespace mailclient::net

#endif
