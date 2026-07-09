#include <boost/asio.hpp>
#include <thread>

#include "CLIClient.hpp"
#include "IMAPClient.hpp"
#include "SMTPClient.hpp"

int main() {
  boost::asio::io_context io;
  auto work = boost::asio::make_work_guard(io);
  std::thread io_thread([&io] { io.run(); });

  auto smtp = mailclient::net::SMTPClient::create(io);
  auto imap = mailclient::net::IMAPClient::create(io);
  auto cli = mailclient::CLIClient::create(io, smtp, imap);

  cli->run();

  work.reset();
  io.stop();
  io_thread.join();

  return 0;
}
