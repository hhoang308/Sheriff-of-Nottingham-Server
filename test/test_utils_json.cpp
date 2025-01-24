#include "gtest/gtest.h"
#include "utils/Utils.h"
#include <string>

// Test case: Kiểm tra việc parse JSON từ string
TEST(UtilsJsonTest, ParseJsonFromString) {
    std::string inputJson = R"({"messagetype":"PLAYER_READY","name":"Sarah"})";

    Json::Value parsedJson = stringToJson(inputJson);

    ASSERT_EQ(parsedJson["messagetype"].asString(), "PLAYER_READY");
    ASSERT_EQ(parsedJson["name"].asString(), "Sarah");
}

TEST(UtilsJsonTest, ConvertJsonToString) {
    Json::Value jsonObject;
    jsonObject["messagetype"] = "PLAYER_READY";
    jsonObject["name"] = "Sarah";

    std::string jsonString = jsonToString(jsonObject);
    /* R"({"messagetype":"PLAYER_READY","name":"Sarah"})" is Raw String Literals */
    /* if we dont use R, we have to write like this "{\"messagetype\":\"PLAYER_READY\",\"name\":\"Sarah\"} */
    std::string expectedJson = R"({"messagetype":"PLAYER_READY","name":"Sarah"})";
    ASSERT_EQ(jsonString, expectedJson);
}
