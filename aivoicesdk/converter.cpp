// clang-format off
#include "pch.h"
// clang-format on

#include "converter.h"

#include <fstream>
#include <iostream>

namespace AiVoice {

namespace Converter {

static const std::map<int, std::string> g_status_and_error_codes = {
    {20001, "Success"},
    {40001, "The required parameters for the request are incomplete."},
    {40002, "SSML format error."},
    {40003, "SSML <speak> format error."},
    {40004, "SSML <voice> format error."},
    {40005, "SSML <phoneme> format error."},
    {40006, "SSML <break> format error."},
    {40007, "SSML <prosody> format error."},
    {40008, "orator name does not exist."},
    {40009, "The text is empty, contains only whitespace, or has no valid characters."},
    {40010, "The number of characters exceeds the limit."},
    {50001, "Converter encountered an unknown error."},
    {50002,
     "API/syn_ssml, the synthesized text parsed from the tag is an empty string or contains illegal "
     "characters."},
    {50301, "Server busy."},
    {50302, "file not found, Please confirm if the audio file has been successfully converted."},
    {50303, "'task_id' not found"},
    {50304, "Convert fail, please resend the request."},
    {401, "token authentication error or API Access not open(Web page)."},
    {404, "Url error."},
    {40199, "Do not support self-signed certificate server."},
    {40499, "Unknown error. Can not get Restful API response, maybe \"server url\" is wrong."}};

converterResult::converterResult()
{
    status = AiVoice::Enums::ConverterStatus::ConverterStartUp;
    detail = "";
    errorMsg = "";
}

converterResult::~converterResult() {}

void converterResult::save(std::string fileName = "aivoice")
{
    if(converterResult::data.size() < 1) {
        std::cerr << "AiVoice: Audio data is empty, please 'getSpeech' first." << std::endl;
        return;
    }

    std::ofstream outputFile((fileName + ".wav"), std::ios::binary);
    if(outputFile.is_open()) {
        outputFile.write(converterResult::data.data(), converterResult::data.size());
        outputFile.close();
    }
    else {
        std::cerr << "AiVoice: Error opening output file." << std::endl;
    }
}

// ================================================================================

voiceConverter::voiceConverter(AiVoice::Config::converterConfig srcConfig)
    : config(srcConfig.getToken(), srcConfig.getServer()), apiHandler()
{
    voiceConverter::config.setToken(srcConfig.getToken());
    voiceConverter::config.setServer(srcConfig.getServer());
    voiceConverter::config.setVoice(srcConfig.voice);
    updateHandlerConfig();
}

voiceConverter::~voiceConverter() {}

void voiceConverter::updateConfig(AiVoice::Config::converterConfig srcConfig)
{
    voiceConverter::config.setToken(srcConfig.getToken());
    voiceConverter::config.setServer(srcConfig.getServer());
    voiceConverter::config.setVoice(srcConfig.voice);
    updateHandlerConfig();
}

void voiceConverter::updateHandlerConfig()
{
    voiceConverter::apiHandler.config["token"] = voiceConverter::config.getToken();
    voiceConverter::apiHandler.config["serverUrl"] = voiceConverter::config.getServer();
    voiceConverter::apiHandler.config["voice"] = voiceConverter::config.getVoiceValue();
    voiceConverter::apiHandler.config["ssmlVersion"] = voiceConverter::config.getSsmlVersion();
    voiceConverter::apiHandler.config["ssmlLang"] = voiceConverter::config.getSsmlLang();
}

std::string voiceConverter::checkReservedWord(const std::string& text)
{
    std::string result;

    for(char c : text) {
        switch(c) {
        case '"': result += "&quot;"; break;
        case '&': result += "&amp;"; break;
        case '\'': result += "&apos;"; break;
        case '<': result += "&lt;"; break;
        case '>': result += "&gt;"; break;
        default: result += c; break;
        }
    }
    return result;
}

std::string voiceConverter::translateResultCode(json resultJson)
{
    int code = resultJson["code"];

    auto it = Converter::g_status_and_error_codes.find(code);
    if(it != Converter::g_status_and_error_codes.end()) {
        return it->second;
    }
    else {
        std::cout << "status not found." << std::endl;
    }
    return "status not found.";
}

void voiceConverter::addText(std::string text)
{
    std::string tempText = voiceConverter::checkReservedWord(text);
    if(voiceConverter::text.size() > 1700) {
        std::cerr << "AiVoice: text too long, please input shorter text" << std::endl;
        return;
    }

    voiceConverter::text = text;
}

std::string voiceConverter::getText()
{
    return voiceConverter::text;
}

void voiceConverter::deleteText()
{
    voiceConverter::text = "";
}

void voiceConverter::showText()
{
    std::cout << voiceConverter::text << std::endl;
}

converterResult voiceConverter::run()
{
    converterResult result;

    if(voiceConverter::text.size() < 1) {
        std::cerr << "Converter text is empty, Please add text first." << std::endl;
        result.status = AiVoice::Enums::ConverterStatus::ConverVoiceFail;
        result.errorMsg = "Converter text is empty, Please add text first.";
        return result;
    }

    result.status = AiVoice::Enums::ConverterStatus::ConverterStartUp;
    result.detail = "";
    result.errorMsg = "";

    if(voiceConverter::apiHandler.config["voice"] != voiceConverter::config.getVoiceValue()) {
        voiceConverter::updateHandlerConfig();
    }

    json resultJson = voiceConverter::apiHandler.addSsmlTask(voiceConverter::text);

    if(resultJson["code"] == 20001) {
        result.status = AiVoice::Enums::ConverterStatus::ConverVoiceRunning;
        result.detail = voiceConverter::translateResultCode(resultJson);
        voiceConverter::taskId = resultJson["data"]["task_id"];
    }
    else {
        result.status = AiVoice::Enums::ConverterStatus::ConverVoiceFail;
        result.errorMsg = voiceConverter::translateResultCode(resultJson);
    }
    return result;
}

converterResult voiceConverter::checkStatus()
{
    converterResult result;

    if(voiceConverter::text.size() < 1) {
        std::cerr << "Converter text is empty, Please add text first." << std::endl;
        result.status = AiVoice::Enums::ConverterStatus::ConverVoiceFail;
        result.errorMsg = "Converter text is empty, Please add text first.";
        return result;
    }

    if(voiceConverter::taskId.size() < 1) {
        std::cerr << "Please start convert first." << std::endl;
        result.status = AiVoice::Enums::ConverterStatus::ConverVoiceFail;
        result.errorMsg = "Please start convert first.";
        return result;
    }

    result.status = AiVoice::Enums::ConverterStatus::ConverterStartUp;
    result.detail = "";
    result.errorMsg = "";

    json resultJson = voiceConverter::apiHandler.getTaskStatus(voiceConverter::taskId);

    if(resultJson["code"] == 20001) {
        // std::cout << resultJson["data"]["status"] << std::endl;

        if(resultJson["data"]["status"] == "SUCCESS") {
            result.status = AiVoice::Enums::ConverterStatus::ConverVoiceCompleted;
            result.detail = voiceConverter::translateResultCode(resultJson);
        }
        else if(resultJson["data"]["status"] == "RUNNING") {
            result.status = AiVoice::Enums::ConverterStatus::ConverVoiceRunning;
            result.detail = voiceConverter::translateResultCode(resultJson);
        }
        else {
            result.status = AiVoice::Enums::ConverterStatus::ConverVoiceFail;
            result.errorMsg = voiceConverter::translateResultCode(resultJson);
        }
    }
    else {
    }

    return result;
}

converterResult voiceConverter::getSpeech()
{
    converterResult result;

    if(voiceConverter::taskId.size() < 1) {
        // std::cerr << "Please start convert first." << std::endl;
        result.status = AiVoice::Enums::ConverterStatus::ConverVoiceFail;
        result.errorMsg = "Please execute 'run' first.";
        return result;
    }

    result.status = AiVoice::Enums::ConverterStatus::ConverterStartUp;
    result.detail = "";
    result.errorMsg = "";

    json resultJson = voiceConverter::apiHandler.getTaskAudio(voiceConverter::taskId, result.data);

    if(resultJson["code"] != 20001) {
        result.status = AiVoice::Enums::ConverterStatus::GetSpeechFail;
        result.errorMsg = voiceConverter::translateResultCode(resultJson);
        return result;
    }

    result.status = AiVoice::Enums::ConverterStatus::GetSpeechSuccess;
    result.detail = voiceConverter::translateResultCode(resultJson);
    return result;
}

// ================================================================================

Converter::voiceConverter* createConverter(AiVoice::Config::converterConfig srcConfig)
{
    return new Converter::voiceConverter(srcConfig);
}

Converter::converterResult* createResult()
{
    return new Converter::converterResult();
}

}  // namespace Converter

}  // namespace AiVoice