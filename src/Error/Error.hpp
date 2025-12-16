#ifndef _ERROR
#define _ERROR
#include <boost/asio.hpp>
#include <boost/system/detail/error_code.hpp>
#include <string>
#include <variant>

#include "IMAPResponse.hpp"
#include "SMTPResponse.hpp"

namespace mailclient::net {

enum class ErrorKind {
  Network,
  SMTP,
  IMAP,
};

class Error {
 public:
  struct NetworkData {
    boost::system::error_code err_code;
  };
  struct SMTPData {};
  struct IMAPData {};
  using ErrorData = std::variant<NetworkData, SMTPData, IMAPData>;

  std::string what() const;
  ErrorKind kind() const;
  ErrorData data() const;

  explicit Error(const SMTPResponse& smtp_resp);
  explicit Error(const IMAPResponse& imap_resp);
  explicit Error(const boost::system::error_code& err_code);

 private:
  ErrorKind error_kind_;
  std::string error_message_;
  ErrorData error_data_;
};

}  // namespace mailclient::net

#endif