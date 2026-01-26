#ifndef LOGGER
#define LOGGER

#include <boost/asio.hpp>
#include <string>

namespace mailclient {

void LOG(const std::string&);
void LOG(const boost::system::error_code& ec);

}  // namespace mailclient
#endif