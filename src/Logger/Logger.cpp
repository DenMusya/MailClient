#include "Logger.hpp"

#include <iostream>
#include <mutex>

namespace mailclient {

namespace {
std::mutex console_mutex;
}

void LOG(const std::string& error) {
  std::lock_guard<std::mutex> lock(console_mutex);
  std::cout << "[ERROR] " << error << std::endl;
}

void LOG(const boost::system::error_code& ec) {
  std::lock_guard<std::mutex> lock(console_mutex);
  std::cout << "[ERROR] " << ec.message() << std::endl;
}

void PRINT(const std::string& msg) {
  std::lock_guard<std::mutex> lock(console_mutex);
  std::cout << msg << std::endl;
}

}  // namespace mailclient
