#include "simpleBot.hh"
#include <spdlog/spdlog.h>

// Example main function for the bot
int main()
{
  std::string serverAddress = "tcp://localhost"; // Replace with your server address
  SimpleBot   myBot(serverAddress, 1);
  SimpleBot   myBot2(serverAddress, 2);
  myBot.start();
  myBot2.start();

  // stop when user hits q
  char input;
  do {
      std::cin >> input;
  } while(input != 'q');

  myBot.stop(); // Stop the bot

  return 0;
}