#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <json/json.h>

/* TODO: Add function to check value is null -> return */

Json::Value stringToJson(const std::string &jsonString);

std::string jsonToString(const Json::Value &jsonValue);

#endif // UTILS_H