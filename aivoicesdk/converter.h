#pragma once

#ifdef AIVOICESDK_EXPORTS
#define AIVOICESDK_API __declspec(dllexport)
#else
#define AIVOICESDK_API __declspec(dllimport)
#endif

#include <iostream>
#include <map>

#include "config.h"
#include "enums.h"
#include "units.h"

extern "C" AIVOICESDK_API int Add(int a, int b);

namespace AiVoice {

namespace Converter {

class converterResult {
private:
    std::function<void(const std::string&)> callbackFunc;

public:
    AiVoice::Enums::ConverterStatus status;
    std::string detail;
    std::string errorMsg;

    AIVOICESDK_API converterResult();
    AIVOICESDK_API ~converterResult();

    void setCallback(std::function<void(const std::string&)> callback);

    AIVOICESDK_API void save(std::string filename);
};

// ================================================================================

class voiceConverter {
private:
    std::string text;
    std::string taskId;
    AiVoice::Units::restfulApiHandler apiHandler;

    std::string checkReservedWord(const std::string& text);
    std::string translateResultCode(json resultJson);
    void getAudio(std::string filenamme);
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
    AIVOICESDK_API converterResult getSpeech(std::string filename);
};
extern "C" AIVOICESDK_API Converter::voiceConverter* createConverter(
    AiVoice::Config::converterConfig srcConfig);
extern "C" AIVOICESDK_API AiVoice::Converter::converterResult* createResult();

}  // namespace Converter

}  // namespace AiVoice