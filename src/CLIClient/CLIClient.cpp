#include "CLIClient.hpp"

#include <iostream>

using namespace mailclient;

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#include "Logger.hpp"

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

std::shared_ptr<CLIClient> CLIClient::create(std::shared_ptr<net::SMTPClient> smtp_client) {
  return std::shared_ptr<CLIClient>(new CLIClient(smtp_client));
}

CLIClient::CLIClient(std::shared_ptr<net::SMTPClient> smtp_client) : smtp_client_(smtp_client) {}

boost::asio::awaitable<void> CLIClient::run() {
  auto env = readEnv(std::string(PROJECT_ROOT) + "/.env");
  while (true) {
    std::cout << "\nCommands:\n[1] Connect\n[2] Send Mail\n[3] Auth\n[4] Quit\nChoose: ";
    std::string cmd;
    std::getline(std::cin, cmd);

    if (cmd == "1") {
      std::string host = env["SMTP_HOST"], port = env["SMTP_PORT"];
      auto res = co_await smtp_client_->asyncConnect(host, port);
      if (!res) LOG(res.error());
    } else if (cmd == "2") {
      std::string to, body, topic;
      std::cout << "To: ";
      std::getline(std::cin, to);
      std::cout << "Topic: ";
      std::getline(std::cin, topic);
      std::cout << "Body: ";
      std::getline(std::cin, body);
      auto res = co_await smtp_client_->sendMail(env["EMAIL_ADDRESS"], to, topic, body);
      if (!res) LOG(res.error());
    } else if (cmd == "3") {
      std::string email = env["EMAIL_ADDRESS"], password = env["EMAIL_PASSWORD"];
      auto res = co_await smtp_client_->login(email, password);
      if (!res) LOG(res.error());
    } else if (cmd == "4") {
      std::cout << "Exiting CLI.\n";
      break;
    } else {
      std::cout << "Unknown command.\n";
    }
  }
}