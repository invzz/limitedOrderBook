#include <string>
#include <nlohmann/json.hpp>

class Config
{
  std::string serverAddress;
  std::string pub;
  std::string sub;

  public:
  Config(std::string serverAddress, std::string pub, std::string sub) : serverAddress(serverAddress), pub(pub), sub(sub) {}
  std::string getServerAddress() const { return serverAddress; }
  std::string getPub() const { return pub; }
  std::string getSub() const { return sub; }

  static std::unique_ptr<Config> fromJson(const nlohmann::json &configData)
  {
    std::string serverAddress = configData["serverAddress"];
    std::string pub           = configData["pub"];
    std::string sub           = configData["sub"];

    return std::make_unique<Config>(serverAddress, pub, sub);
  }
};