// clang-format off
#include "pch.h"
// clang-format on

#include "units.h"

#include <curl/curl.h>

#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

namespace AiVoice {

namespace Units {

static void (*g_update_config_callback)(std::map<std::string, std::string> config) = nullptr;
size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp);

restfulApiHandler::restfulApiHandler()
{
    config = {
        {"token", ""}, {"serverUrl", ""}, {"voice", ""}, {"ssmlVersion", "1.0.demo"}, {"ssmlLang", "zh-TW"}};

    // init libcurl
    curl_global_init(CURL_GLOBAL_ALL);
}

restfulApiHandler::~restfulApiHandler()
{
    // close libcurl
    curl_global_cleanup();
}

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

size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t total_size = size * nmemb;
    std::stringstream* response = static_cast<std::stringstream*>(userp);
    response->write(static_cast<char*>(contents), total_size);
    return total_size;
}

void restfulApiHandler::restfulSender(std::string apiUrl, std::string payload, requestResponse& result)
{
    // create CURL object
    CURL* curl = curl_easy_init();
    if(!curl) {
        std::cerr << "Error initializing libcurl." << std::endl;
        return;
    }

    // ========================================
    // Set request URL
    curl_easy_setopt(curl, CURLOPT_URL, (restfulApiHandler::config["serverUrl"] + apiUrl).c_str());

    // Set timeout with 10s
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    // ========================================
    // skip check certificate
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

    // ========================================
    // Set post data(json format)
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

    // ========================================
    // Set Content-Type header
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    // Set Authorization header
    headers =
        curl_slist_append(headers, ("Authorization: Bearer " + restfulApiHandler::config["token"]).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // ========================================
    // Set callback func and callback data
    std::stringstream response_stream;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result.response);

    // ========================================
    // Execute request
    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        std::cerr << "Curl error: " << curl_easy_strerror(res) << std::endl;
    }

    // Get response status Code and Content-Type
    long statusCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
    result.statusCode = statusCode;

    char* contentType = nullptr;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &contentType);
    if(contentType)
        // audio/wav or application/json
        result.contentType = contentType;

    // ========================================
    // Release resouce
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

json restfulApiHandler::addTextTask(std::string text)
{
    if(restfulApiHandler::config["voice"].size() < 1)
        throw std::runtime_error("Converter voice is 'None'");

    std::string apiUrl = "/api/v1.0/syn/syn_text";

    json payload = {{"orator_name", restfulApiHandler::config["voice"]}, {"text", text}};

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

    std::string apiUrl = "/api/v1.0/syn/syn_ssml";

    json payload = {{"ssml", ("<speak xmlns=\"http://www.w3.org/2001/10/synthesis\" version=\""
                              + restfulApiHandler::config["ssmlVersion"] + "\" xml:lang=\""
                              + restfulApiHandler::config["ssmlLang"] + "\"><voice name=\""
                              + restfulApiHandler::config["voice"] + "\">" + text + "</voice></speak>")}};

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
        return restfulApiHandler::responseHandler(result);
    }
    catch(const std::exception& e) {
        std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
        // throw std::runtime_error(("An unexpected error occurred: " << e.what()));
    }

    return {{"data", "Unknown error. Can not get Restful API response, maybe 'server url' is wrong."},
            {"code", 40499}};
}

json restfulApiHandler::getTaskAudio(std::string taskId, std::vector<char>& audioData)
{
    std::string apiUrl = "/api/v1.0/syn/get_file";
    json payload = {{"filename", (taskId + ".wav")}};

    try {
        requestResponse result;
        restfulApiHandler::restfulSender(apiUrl, payload.dump(), result);
        if(result.contentType == "audio/wav") {
            result.response.seekg(0, std::ios::beg);
            audioData.assign(std::istreambuf_iterator<char>(result.response),
                             std::istreambuf_iterator<char>());

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