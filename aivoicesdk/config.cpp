// clang-format off
#include "pch.h"
// clang-format on

#include "config.h"

#include <iostream>

namespace AiVoice {

namespace Config {

converterConfig::converterConfig(std::string token = "", std::string serverUrl = "https://www.aivoice.com.tw")
{
    converterConfig::token = token;
    converterConfig::serverUrl = serverUrl;
}

converterConfig::~converterConfig() {}

void converterConfig::setToken(std::string token = "")
{
    converterConfig::token = token;
}

void converterConfig::setServer(std::string serverUrl = "https://www.aivoice.com.tw")
{
    if((serverUrl.find("http") == 0) || (serverUrl.find("HTTP") == 0)) {
        converterConfig::serverUrl = serverUrl;
    }
    else {
        throw std::invalid_argument("Please check URL, it should start with 'http' or 'https'.");
    }
}

void converterConfig::setVoice(AiVoice::Enums::Voice voice)
{
    converterConfig::voice = voice;
}

std::string converterConfig::getToken()
{
    return converterConfig::token;
}

std::string converterConfig::getServer()
{
    return converterConfig::serverUrl;
}

AiVoice::Enums::Voice converterConfig::getVoice()
{
    return converterConfig::voice;
}

std::string converterConfig::getVoiceValue()
{
    bool isOK = false;
    std::string voiceValue = AiVoice::Enums::getVoiceValue(converterConfig::voice, isOK);
    if(!isOK) {
        std::cerr << "[Enums] Can't find voice value" << std::endl;
    }
    return voiceValue;
}

std::string converterConfig::getSsmlVersion()
{
    return converterConfig::ssmlVersion;
}

std::string converterConfig::getSsmlLang()
{
    return converterConfig::ssmlLang;
}

AiVoice::Config::converterConfig* AiVoice::Config::CreateConfig(std::string token, std::string serverUrl)
{
    return new Config::converterConfig(token, serverUrl);
};

}  // namespace Config

}  // namespace AiVoice