#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <jsoncpp/json/json.h>

Json::Value stringToJson(const std::string& jsonString);

std::string jsonToString(const Json::Value& jsonValue);

#endif // UTILS_H