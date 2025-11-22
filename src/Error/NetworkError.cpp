#include "NetworkError.hpp"

using namespace mailclient::net;

NetworkError::NetworkError(const boost::system::error_code& net_error)
    : net_error_(net_error) {}

std::string NetworkError::what() const {
  if (net_error_) {
    return "Network error: " + net_error_.message();
  }

  return "No error";
}