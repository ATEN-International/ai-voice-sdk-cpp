#pragma once

#ifdef AIVOICESDK_EXPORTS
#define AIVOICESDK_API __declspec(dllexport)
#else
#define AIVOICESDK_API __declspec(dllimport)
#endif

#include <string>

namespace AiVoice {
namespace Config {

class Test {
private:
public:
    std::string msg;

    Test();
    ~Test();

    AIVOICESDK_API void sayhi();
};

}  // namespace Config
}  // namespace AiVoice