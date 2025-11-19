#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>

using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;
using ssl_socket = ssl::stream<tcp::socket>;

int main() {
  ssl::context ctx(ssl::context::sslv23);
  ctx.set_default_verify_paths();

  boost::asio::io_context io_context;
  ssl_socket sock(io_context, ctx);
  tcp::resolver resolver(io_context);

  boost::asio::connect(sock.lowest_layer(), resolver.resolve("smtp.gmail.com", "465"));
  sock.lowest_layer().set_option(tcp::no_delay(true));

  sock.set_verify_mode(ssl::verify_peer);
  sock.set_verify_callback(ssl::host_name_verification("smtp.gmail.com"));
  sock.handshake(ssl_socket::client);
  std::string str;
  boost::asio::write(sock, boost::asio::buffer("EHLO localhost\r\n"));
  while (true) {
    std::string str;
    str.resize(256);
    auto len = sock.read_some(boost::asio::buffer(str));
    std::cout << std::string(str.data(), len);
  }
  std::cout << str << std::endl;
  return 0;
}