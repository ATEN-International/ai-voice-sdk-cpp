#include "pch.h"

#include "aivoicesdk.h"

#include <iostream>

//
// #include "src\config.h"
// #include "src\converter.h"

int Add(int a, int b)
{
    std::cout << "Hello from aivoice." << std::endl;
    return a + b;
}

namespace AiVoice {

// TestConfig::TestConfig(std::string teken, std::string serverUrl)
// {
//     TestConfig::msg = "hi from test config class.";
// };

// TestConfig::TestConfig() : config(new Config::Test())
TestConfig::TestConfig() : config("token", "http")
{
    // AiVoice::Config::converterConfig config2;
    msg = "hi from test config class.";
};

TestConfig::~TestConfig(){};

void TestConfig::DoSomething()
{
    std::cout << TestConfig::msg << std::endl;
};

void TestConfig::callConfig()
{
    TestConfig::configTest.sayhi();
};

TestConfig* CreateTestConfig()
{
    return new TestConfig();
};

AiVoice::Config::converterConfig* AiVoice::Config::CreateConfig(std::string token, std::string serverUrl)
{
    return new AiVoice::Config::converterConfig(token, serverUrl);
};

// AiVoice::Converter::voiceConverter* CreateConverter()
// {
//     return new AiVoice::Converter::voiceConverter();
// };

}  // namespace AiVoice