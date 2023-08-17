// clang-format off
#include "pch.h"
// clang-format on

#include "enums.h"

#include <map>

namespace AiVoice {

namespace Enums {

static const std::map<Enums::Voice, std::string> g_voice_enum_to_string = {
    {Enums::Voice::Noetic, "Aurora_noetic"},   {Enums::Voice::Literary, "Aaron_literary"},
    {Enums::Voice::Cheerful, "Bill_cheerful"}, {Enums::Voice::Host, "Bella_host"},
    {Enums::Voice::Vivid, "Bella_vivid"},      {Enums::Voice::Graceful, "Bella_graceful"}};

std::string getVoiceValue(const AiVoice::Enums::Voice& voice, bool& isOk)
{
    isOk = false;

    auto it = g_voice_enum_to_string.find(voice);
    if(it != g_voice_enum_to_string.end()) {
        isOk = true;
        return it->second;
    }
    else {
        return "Aurora_noetic";  // Default value in case of error
    }
}

}  // namespace Enums

}  // namespace AiVoice
