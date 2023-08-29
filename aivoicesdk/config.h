#pragma once

#ifdef AIVOICESDK_EXPORTS
#define AIVOICESDK_API __declspec(dllexport)
#else
#define AIVOICESDK_API __declspec(dllimport)
#endif

#include <map>
#include <string>
#include <vector>

#include "enums.h"

namespace AiVoice {

namespace Config {

class settings {
private:
    /* data */
public:
    int textLimit = 1500;
    int elasticValue = 200;
    int eachTaskTextLimit = textLimit + elasticValue;

    std::vector<std::string> supportFileType = {".txt", ".ssml", ".xml"};
};

class converterConfig {
private:
    std::string token;
    std::string serverUrl;
    std::string ssmlVersion = "1.0.demo";
    std::string ssmlLang = "zh-TW";

public:
    AiVoice::Enums::Voice voice = AiVoice::Enums::Voice::None;

    AIVOICESDK_API converterConfig(std::string token, std::string serverUrl);
    AIVOICESDK_API ~converterConfig();

    AIVOICESDK_API void setToken(std::string token);
    AIVOICESDK_API void setServer(std::string serverUrl);
    AIVOICESDK_API void setVoice(AiVoice::Enums::Voice voice);

    AIVOICESDK_API std::string getToken();
    AIVOICESDK_API std::string getServer();
    AIVOICESDK_API AiVoice::Enums::Voice getVoice();
    AIVOICESDK_API std::string getVoiceValue();
    AIVOICESDK_API std::string getSsmlVersion();
    AIVOICESDK_API std::string getSsmlLang();
};

extern "C" AIVOICESDK_API AiVoice::Config::converterConfig* createConfig(std::string token,
                                                                         std::string serverUrl);

}  // namespace Config

}  // namespace AiVoice