#include "Base64Encode.hpp"

namespace mailclient {

std::string base64Encode(const std::string& input) {
  static const char* chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789+/";

  std::string output;
  int val = 0;
  int valb = -6;

  for (unsigned char c : input) {
    val = (val << 8) + c;
    valb += 8;
    while (valb >= 0) {
      output.push_back(chars[(val >> valb) & 0x3F]);
      valb -= 6;
    }
  }

  if (valb > -6) {
    output.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
  }

  while (output.size() % 4) {
    output.push_back('=');
  }

  return output;
}

}