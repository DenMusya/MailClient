#ifndef LOGGER
#define LOGGER

#include <boost/asio.hpp>
#include <string>

namespace mailclient {

void LOG(const std::string&);
void LOG(const boost::system::error_code& ec);
void PRINT(const std::string&);

}  // namespace mailclient
#endif