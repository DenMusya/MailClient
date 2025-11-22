#ifndef NETWORKERROR
#define NETWORKERROR

#include <boost/asio.hpp>
#include <boost/system/detail/error_code.hpp>

namespace mailclient::net {

class NetworkError {
 public:
  explicit NetworkError(const boost::system::error_code& net_error);

  virtual std::string what() const;

 protected:
  NetworkError() = default;
  boost::system::error_code net_error_;
};

}  // namespace mailclient::net

#endif
