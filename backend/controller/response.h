#pragma once
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Response {
public:

    static std::string success(const json& data = json::object(), const std::string& msg = "success") {
        json res;
        res["code"] = 0;
        res["message"] = msg;
        res["data"] = data;
        return res.dump();
    }

    static std::string error(int code, const std::string& msg) {
        json res;
        res["code"] = code;
        res["message"] = msg;
        res["data"] = nullptr;
        return res.dump();
    }
};