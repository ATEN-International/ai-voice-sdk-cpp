#pragma once

#ifdef AIVOICESDK_EXPORTS
#define AIVOICESDK_API __declspec(dllexport)
#else
#define AIVOICESDK_API __declspec(dllimport)
#endif

#include <iostream>

#include "configtest.h"
#include "config.h"
#include "enums.h"
#include "units.h"

extern "C" AIVOICESDK_API int Add(int a, int b);

namespace AiVoice {
class TestConfig {
private:
public:
    std::string msg;

    AiVoice::Config::converterConfig config;
    AiVoice::Config::Test configTest;
    // AiVoice::Config::converterConfig config2;

    AIVOICESDK_API TestConfig();
    AIVOICESDK_API ~TestConfig();
    AIVOICESDK_API void DoSomething();
    AIVOICESDK_API void callConfig();
};

extern "C" AIVOICESDK_API TestConfig* CreateTestConfig();

namespace Config {

extern "C" AIVOICESDK_API AiVoice::Config::converterConfig* CreateConfig(std::string token,
                                                                         std::string serverUrl);
}  // namespace Config

}  // namespace AiVoice