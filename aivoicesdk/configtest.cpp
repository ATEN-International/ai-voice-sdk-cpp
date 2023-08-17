// clang-format off
#include "pch.h"
// clang-format on

#include "configtest.h"

#include <iostream>

namespace AiVoice {
namespace Config {

Test::Test()
{
    msg = "This is test class msg";
}

Test::~Test() {}

void Test::sayhi()
{
    std::cout << Test::msg << std::endl;
}

}  // namespace Config
}  // namespace AiVoice