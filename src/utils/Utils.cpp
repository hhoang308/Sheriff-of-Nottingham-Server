#include "Utils.h"
#include <sstream>
#include <iostream>

Json::Value stringToJson(const std::string& jsonString) {
    Json::Value root;
    Json::CharReaderBuilder readerBuilder;
    std::string errors;

    std::istringstream stream(jsonString);
    if (!Json::parseFromStream(readerBuilder, stream, &root, &errors)) {
        std::cerr << "Error parsing JSON string: " << errors << std::endl;
        return Json::Value();
    }

    return root;
}

std::string jsonToString(const Json::Value& jsonValue) {
    Json::StreamWriterBuilder writerBuilder;
    writerBuilder["indentation"] = "";
    return Json::writeString(writerBuilder, jsonValue);
}
