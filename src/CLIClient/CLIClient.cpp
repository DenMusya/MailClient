#include "CLIClient.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_future.hpp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#include "Logger.hpp"

using namespace mailclient;

std::unordered_map<std::string, std::string> readEnv(const std::string& path) {
  std::unordered_map<std::string, std::string> env;
  std::ifstream file(path);
  std::string line;
  while (std::getline(file, line)) {
    auto eq = line.find('=');
    if (eq == std::string::npos) continue;
    env[line.substr(0, eq)] = line.substr(eq + 1);
  }
  return env;
}

std::shared_ptr<CLIClient> CLIClient::create(
    boost::asio::io_context& io, std::shared_ptr<net::SMTPClient> smtp_client,
    std::shared_ptr<net::IMAPClient> imap_client) {
  return std::shared_ptr<CLIClient>(
      new CLIClient(io, smtp_client, imap_client));
}

CLIClient::CLIClient(boost::asio::io_context& io,
                     std::shared_ptr<net::SMTPClient> smtp_client,
                     std::shared_ptr<net::IMAPClient> imap_client)
    : io_(io),
      smtp_client_(smtp_client),
      imap_client_(imap_client) {}

void CLIClient::run() {
  auto env = readEnv(std::string(PROJECT_ROOT) + "/.env");

  // Runs a coroutine on the io_context thread and blocks the CLI until it
  // finishes, handing back its std::expected result.
  auto dispatch = [this](auto awaitable) {
    return boost::asio::co_spawn(io_, std::move(awaitable),
                                 boost::asio::use_future).get();
  };

  while (true) {
    std::cout << "\nCommands:\n"
                 "[1] SMTP Connect\n[2] Send Mail\n[3] SMTP Auth\n[4] Quit\n"
                 "[5] SMTP Disconnect\n[6] IMAP Login & Sync\n[7] List mailbox\n"
                 "[8] Read message\n[9] IMAP Logout\nChoose: ";
    std::string cmd;
    if (!std::getline(std::cin, cmd)) break;

    if (cmd == "1") {
      auto res = dispatch(
          smtp_client_->connect(env["SMTP_HOST"], env["SMTP_PORT"]));
      if (!res)
        LOG(res.error().what());
      else
        smtp_connected_ = true;
    } else if (cmd == "2") {
      std::string to, body, topic;
      std::cout << "To: ";
      std::getline(std::cin, to);
      std::cout << "Topic: ";
      std::getline(std::cin, topic);
      std::cout << "Body: ";
      std::getline(std::cin, body);
      auto res = dispatch(
          smtp_client_->sendMail(env["EMAIL_ADDRESS"], to, topic, body));
      if (!res) LOG(res.error().what());
    } else if (cmd == "3") {
      auto res = dispatch(
          smtp_client_->login(env["EMAIL_ADDRESS"], env["EMAIL_PASSWORD"]));
      if (!res) LOG(res.error().what());
    } else if (cmd == "4") {
      if (smtp_connected_) {
        auto res = dispatch(smtp_client_->quit());
        if (!res)
          LOG(res.error().what());
        else
          std::cout << "Вы успешно вышли из сессии почты\n";
        smtp_connected_ = false;
      }
      if (imap_connected_) {
        imap_client_->stopSync();
        auto res = dispatch(imap_client_->logout());
        if (!res) LOG(res.error().what());
        imap_connected_ = false;
      }
      std::cout << "Exiting CLI.\n";
      break;
    } else if (cmd == "5") {
      if (!smtp_connected_) {
        std::cout << "SMTP is not connected.\n";
      } else {
        auto res = dispatch(smtp_client_->quit());
        if (!res)
          LOG(res.error().what());
        else
          std::cout << "Вы успешно вышли из сессии почты\n";
        smtp_connected_ = false;
      }
    } else if (cmd == "6") {
      if (imap_connected_) {
        std::cout << "IMAP is already connected.\n";
        continue;
      }
      auto connected = dispatch(
          imap_client_->connect(env["IMAP_HOST"], env["IMAP_PORT"]));
      if (!connected) {
        LOG(connected.error().what());
        continue;
      }
      auto logged = dispatch(
          imap_client_->login(env["EMAIL_ADDRESS"], env["EMAIL_PASSWORD"]));
      if (!logged) {
        LOG(logged.error().what());
        continue;
      }
      auto selected = dispatch(imap_client_->select("INBOX"));
      if (!selected) {
        LOG(selected.error().what());
        continue;
      }
      auto loaded = dispatch(imap_client_->loadRecent(10));
      if (!loaded) LOG(loaded.error().what());

      boost::asio::co_spawn(io_, imap_client_->startSync(),
                            boost::asio::detached);
      imap_connected_ = true;
      std::cout << "IMAP synced. New mail notifications are now on.\n";
    } else if (cmd == "7") {
      auto messages = imap_client_->mailbox()->snapshot();
      if (messages.empty()) {
        std::cout << "Mailbox is empty (or not synced yet).\n";
      }
      for (const auto& m : messages) {
        std::cout << "#" << m.seq << "  " << m.from << "  |  " << m.subject
                  << "  |  " << m.date << (m.has_body ? "  [cached]" : "")
                  << "\n";
      }
    } else if (cmd == "8") {
      std::cout << "Message #: ";
      std::string input;
      std::getline(std::cin, input);
      uint32_t seq = 0;
      try {
        seq = static_cast<uint32_t>(std::stoul(input));
      } catch (...) {
        std::cout << "Invalid number.\n";
        continue;
      }

      auto cached = imap_client_->mailbox()->get(seq);
      if (cached && cached->has_body) {
        std::cout << "--- Message #" << seq << " (cached) ---\n"
                  << cached->body << "\n";
      } else {
        std::cout << "Fetching message #" << seq << "...\n";
        auto res = dispatch(imap_client_->fetchBody(seq));
        if (!res)
          LOG(res.error().what());
        else
          std::cout << "--- Message #" << seq << " ---\n"
                    << res->body << "\n";
      }
    } else if (cmd == "9") {
      if (!imap_connected_) {
        std::cout << "IMAP is not connected.\n";
      } else {
        imap_client_->stopSync();
        auto res = dispatch(imap_client_->logout());
        if (!res)
          LOG(res.error().what());
        else
          std::cout << "IMAP session closed.\n";
        imap_connected_ = false;
      }
    } else {
      std::cout << "Unknown command.\n";
    }
  }
}
