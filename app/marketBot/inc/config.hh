#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <memory>
#include <utility>

class Config
{
    private:
    std::string serverAddress;
    std::string pub;
    std::string sub;

    public:
    Config(std::string &&serverAddress, std::string &&pub, std::string &&sub)
        : serverAddress(std::move(serverAddress)), pub(std::move(pub)), sub(std::move(sub))
    {}
    [[nodiscard]]
    std::string getServerAddress() const
    {
        return serverAddress;
    }
    [[nodiscard]]
    std::string getPub() const
    {
        return pub;
    }
    [[nodiscard]]
    std::string getSub() const
    {
        return sub;
    }

    static std::unique_ptr<Config> fromJson(const nlohmann::json &configData)
    {
        std::string serverAddress = configData["serverAddress"];
        std::string pub           = configData["pub"];
        std::string sub           = configData["sub"];

        return std::make_unique<Config>(std::move(serverAddress), std::move(pub), std::move(sub));
    }
};