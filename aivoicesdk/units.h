#pragma once

#include <sstream>
#include <vector>

#include "config.h"
#include "enums.h"
#include "json/nlohmann/json.hpp"

using json = nlohmann::json;

namespace AiVoice {

namespace Units {

class requestResponse {
private:
public:
    int statusCode;
    std::string contentType;
    std::stringstream response;
};

class restfulApiHandler {
private:
    std::vector<int> serverSupportJsonStatusCode = {200, 400, 500, 503};

    void restfulSender(std::string apiUrl, std::string payload, requestResponse& result);
    json responseErrorHandler(AiVoice::Units::requestResponse& result);
    json responseHandler(AiVoice::Units::requestResponse& result);

public:
    std::map<std::string, std::string> config;
    restfulApiHandler();
    ~restfulApiHandler();

    void setConfigCallback(void (*func_t)(std::map<std::string, std::string> configMap));

    json addTextTask(std::string text);
    json addSsmlTask(std::string text);
    json getTaskStatus(std::string taskId);
    json getTaskAudio(std::string taskId, std::vector<char>& audioData);
};

}  // namespace Units

}  // namespace AiVoice