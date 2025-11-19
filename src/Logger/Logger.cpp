#include "Logger.hpp"

#include <iostream>

using namespace mailclient;

void LOG(const std::string& error) { std::cout << "[ERROR] " << error << std::endl; }
void LOG(const boost::system::error_code& ec) {
  std::cout << "[ERROR] " << ec.message() << std::endl;
}