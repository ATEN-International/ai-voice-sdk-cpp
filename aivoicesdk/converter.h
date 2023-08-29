#pragma once

#ifdef AIVOICESDK_EXPORTS
#define AIVOICESDK_API __declspec(dllexport)
#else
#define AIVOICESDK_API __declspec(dllimport)
#endif

#include <iostream>
#include <map>
#include <vector>

#include "config.h"
#include "enums.h"
#include "units.h"

namespace AiVoice {

namespace Converter {

class converterResult {
private:
public:
    AiVoice::Enums::ConverterStatus status;
    std::string detail;
    std::string errorMsg;
    std::vector<char> data;

    AIVOICESDK_API converterResult();
    AIVOICESDK_API ~converterResult();

    AIVOICESDK_API void save(std::string fileName);
};

// ================================================================================

class voiceConverter {
private:
    std::string text;
    std::string taskId;
    AiVoice::Units::restfulApiHandler apiHandler;

    std::string checkReservedWord(const std::string& text);
    std::string translateResultCode(json resultJson);
    void updateHandlerConfig();

public:
    AiVoice::Config::converterConfig config;
    voiceConverter(AiVoice::Config::converterConfig srcConfig);
    ~voiceConverter();

    AIVOICESDK_API void updateConfig(AiVoice::Config::converterConfig srcConfig);
    AIVOICESDK_API void addText(std::string text);
    AIVOICESDK_API std::string getText();
    AIVOICESDK_API void deleteText();
    AIVOICESDK_API void showText();
    AIVOICESDK_API converterResult run();
    AIVOICESDK_API converterResult checkStatus();
    AIVOICESDK_API converterResult getSpeech();
};
extern "C" AIVOICESDK_API Converter::voiceConverter* createConverter(
    AiVoice::Config::converterConfig srcConfig);
extern "C" AIVOICESDK_API AiVoice::Converter::converterResult* createResult();

}  // namespace Converter

}  // namespace AiVoice