// clang-format off
#include "pch.h"
// clang-format on

#include "units.h"

#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>
#include <curlpp/infos.hpp>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

using namespace curlpp::options;

namespace AiVoice {

namespace Units {

static void (*g_update_config_callback)(std::map<std::string, std::string> config) = nullptr;

restfulApiHandler::restfulApiHandler()
{
    config = {
        {"token", ""}, {"serverUrl", ""}, {"voice", ""}, {"ssmlVersion", "1.0.demo"}, {"ssmlLang", "zh-TW"}};
}

restfulApiHandler::~restfulApiHandler() {}

json restfulApiHandler::responseErrorHandler(AiVoice::Units::requestResponse& result)
{
    json resultJson;
    if(result.statusCode == 404) {
        resultJson = {{"data", "Not Found"}, {"code", result.statusCode}};
    }
    else if(result.statusCode == 401) {
        resultJson = {{"data", {"status", "Not authorized."}}, {"code", result.statusCode}};
    }
    else if(result.statusCode == 200) {
        resultJson = {
            {"data", "Unknown error. Can not get Restful API response, maybe 'server url' is wrong."},
            {"code", 40499}};
    }
    else
        resultJson = {{"data", result.response.str()}, {"code", result.statusCode}};
    return resultJson;
}

json restfulApiHandler::responseHandler(AiVoice::Units::requestResponse& result)
{
    if(std::find(restfulApiHandler::serverSupportJsonStatusCode.begin(),
                 restfulApiHandler::serverSupportJsonStatusCode.end(), result.statusCode)
       != restfulApiHandler::serverSupportJsonStatusCode.end()) {
        if(result.contentType == "application/json") {
            // std::cout << "Restful API: Success " << result.statusCode << std::endl;
            return json::parse(result.response.str());
        }
        else {
            std::cout << "Error in 200" << std::endl;
            return restfulApiHandler::responseErrorHandler(result);
        }
    }
    else {
        std::cout << "Error in undefined status code: " << result.statusCode << std::endl;
        return restfulApiHandler::responseErrorHandler(result);
    }
}

void restfulApiHandler::restfulSender(std::string apiUrl, std::string payload, requestResponse& result)
{
    // requestResponse result;
    try {
        curlpp::Cleanup myCleanup;

        curlpp::Easy request;

        // Set the URL.
        request.setOpt<Url>((restfulApiHandler::config["serverUrl"] + apiUrl));
        // request.setOpt<Url>(("" + apiUrl));

        // Set the headers
        std::list<std::string> headers;
        headers.push_back(("Authorization: Bearer " + restfulApiHandler::config["token"]));
        // headers.push_back("Authorization: Bearer ");
        request.setOpt(new curlpp::options::HttpHeader(headers));

        // Set timeout
        request.setOpt(new curlpp::options::Timeout(10));

        // 建立 JSON payload 字符串
        // std::string payload = "{\"task_id\": \"1234567\", \"another_key\": \"another_value\"}";
        // std::string payload = "{\"task_id\": \"1234567\"}";

        // Set HTTP POST
        request.setOpt(new curlpp::options::PostFields(payload));
        request.setOpt(new curlpp::options::PostFieldSize(payload.length()));

        request.setOpt(new curlpp::options::WriteStream(&result.response));

        // Send to request
        request.perform();

        // std::cout << "Token: " << restfulApiHandler::config["token"] << std::endl;

        // std::cout << "Response code: " << curlpp::infos::ContentType::get(request) << std::endl;

        // std::cout << "Response code: " << curlpp::infos::ResponseCode::get(request) << std::endl;

        // std::cout << "EffectiveUrl: " << curlpp::infos::EffectiveUrl::get(request) << std::endl;

        // std::cout << "Response: " << response.str() << std::endl;

        result.statusCode = curlpp::infos::ResponseCode::get(request);
        result.contentType = curlpp::infos::ContentType::get(request);
        // return result;
    }
    catch(curlpp::RuntimeError& e) {
        std::cout << e.what() << std::endl;
        result.statusCode = 40499;
    }
    catch(curlpp::LogicError& e) {
        std::cout << e.what() << std::endl;
        result.statusCode = 40499;
    }
}

json restfulApiHandler::addTextTask(std::string text)
{
    if(restfulApiHandler::config["voice"].size() < 1)
        throw std::runtime_error("Converter voice is 'None'");

    // bool isOk = false;
    std::string apiUrl = "/api/v1.0/syn/syn_text";

    json payload = {{"orator_name", restfulApiHandler::config["voice"]}, {"text", text}};

    // json payload = {{"orator_name", "Aurora_noetic"}, {"text", text}};

    // std::cerr << "[SSML]\n" << payload.dump() << "\n" << payload2 << std::endl;

    // if(!isOk) {
    //     // std::cerr << "[Enum] Get voice value fail." << std::endl;
    //     throw std::runtime_error("[Enum] Get voice value fail.");
    // }

    if(text.size() > 2000) {
        return {{"data", "Out of range"}, {"code", 40010}};
    }

    try {
        requestResponse result;
        restfulApiHandler::restfulSender(apiUrl, payload.dump(), result);
        return restfulApiHandler::responseHandler(result);
    }
    catch(const std::exception& e) {
        std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
    }

    return {{"data", "Unknown error. Can not get Restful API response, maybe 'server url' is wrong."},
            {"code", 40499}};
}

json restfulApiHandler::addSsmlTask(std::string text)
{
    if(restfulApiHandler::config["voice"].size() < 1)
        throw std::runtime_error("Converter voice is 'None'");

    // bool isOk = false;
    std::string apiUrl = "/api/v1.0/syn/syn_ssml";
    // AiVoice::Enums::getVoiceValue(restfulApiHandler::config.voice, isOk)

    json payload = {{"ssml", ("<speak xmlns=\"http://www.w3.org/2001/10/synthesis\" version=\""
                              + restfulApiHandler::config["ssmlVersion"] + "\" xml:lang=\""
                              + restfulApiHandler::config["ssmlLang"] + "\"><voice name=\""
                              + restfulApiHandler::config["voice"] + "\">" + text + "</voice></speak>")}};

    // json payload = {{"ssml", ""}};

    // std::cerr << "[SSML] " << payload.dump() << std::endl;

    // if(!isOk) {
    //     // std::cerr << "[Enums] Get voice value fail." << std::endl;
    //     throw std::runtime_error("[Enums] Get voice value fail.");
    // }

    if(payload["ssml"].size() > 1700) {
        return {{"data", "Out of range"}, {"code", 40010}};
    }

    try {
        requestResponse result;
        restfulApiHandler::restfulSender(apiUrl, payload.dump(), result);
        return restfulApiHandler::responseHandler(result);
    }
    catch(const std::exception& e) {
        std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
    }

    return {{"data", "Unknown error. Can not get Restful API response, maybe 'server url' is wrong."},
            {"code", 40499}};
}

json restfulApiHandler::getTaskStatus(std::string taskId)
{
    std::string apiUrl = "/api/v1.0/syn/task_status";
    // std::string payload = "{\"task_id\":\"" + taskId + "\"}";
    json payload = {{"task_id", taskId}};

    try {
        requestResponse result;
        restfulApiHandler::restfulSender(apiUrl, payload.dump(), result);
        // std::cout << "Status Code: " << result.statusCode << std::endl;
        // std::cout << "Content-Type: " << result.contentType << std::endl;
        // std::cout << "Response: " << result.response << std::endl;
        // std::cout << "Response(Json): " << json::parse(result.response)["code"] << std::endl;
        return restfulApiHandler::responseHandler(result);
        // return json::parse(result.response);
    }
    catch(const std::exception& e) {
        std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
        // throw std::runtime_error(("An unexpected error occurred: " << e.what()));
    }

    return {{"data", "Unknown error. Can not get Restful API response, maybe 'server url' is wrong."},
            {"code", 40499}};
}

json restfulApiHandler::getTaskAudio(std::string taskId, std::string fileName)
{
    std::string apiUrl = "/api/v1.0/syn/get_file";
    // std::string payload = "{\"task_id\":\"" + taskId + "\"}";
    json payload = {{"filename", (taskId + ".wav")}};

    try {
        requestResponse result;
        restfulApiHandler::restfulSender(apiUrl, payload.dump(), result);
        if(result.contentType == "audio/wav") {
            std::ofstream outputFile((fileName + ".wav"), std::ios::binary);
            if(outputFile.is_open()) {
                outputFile << result.response.rdbuf();
                outputFile.close();
                // std::cout << "Binary data written to output.wav" << std::endl;
            }
            else {
                std::cerr << "[GetAudio] Save audio data fail." << std::endl;
            }

            return {{"data", " "}, {"code", 20001}};
        }
        else {
            return restfulApiHandler::responseHandler(result);
        }
    }
    catch(const std::exception& e) {
        std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
    }

    return {{"data", "Unknown error. Can not get Restful API response, maybe 'server url' is wrong."},
            {"code", 40499}};
}

void restfulApiHandler::setConfigCallback(void (*func_t)(std::map<std::string, std::string> configMap))
{
    g_update_config_callback = func_t;
}

}  // namespace Units

}  // namespace AiVoice