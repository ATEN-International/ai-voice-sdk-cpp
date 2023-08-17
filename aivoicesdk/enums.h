#pragma once

#ifdef AIVOICESDK_EXPORTS
#define AIVOICESDK_API __declspec(dllexport)
#else
#define AIVOICESDK_API __declspec(dllimport)
#endif

#include <string>

namespace AiVoice {

namespace Enums {

enum class Voice {
    Noetic,
    Literary,
    Cheerful,
    Host,
    Vivid,
    Graceful,
    None
};

enum class SsmlVersion {
    // V1 = '1.0.demo'
    V1
};

enum class SsmlLanguage {
    // TW = 'zh-TW'
    TW
};

enum class SsmlPhoneme {
    // TW = 'bopomo'
    TW
};

enum class ConverterStatus {
    ConverterStartUp,
    ConverVoiceStart,
    ConverVoiceRunning,
    ConverVoiceCompleted,
    ConverVoiceFail,
    ServerBusy,
    GetSpeechSuccess,
    GetSpeechFail
};

AIVOICESDK_API std::string getVoiceValue(const AiVoice::Enums::Voice& voice, bool& isOk);

}  // namespace Enums

}  // namespace AiVoice