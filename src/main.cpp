#include <boost/asio.hpp>

#include "CLIClient.hpp"
#include "SMTPClient.hpp"

int main() {
  boost::asio::io_context io;
  auto smtp = mailclient::net::SMTPClient::create(io);
  auto cli = mailclient::CLIClient::create(smtp);

  boost::asio::co_spawn(io, cli->run(), boost::asio::detached);

  io.run();

  return 0;
}