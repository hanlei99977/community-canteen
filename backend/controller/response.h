#pragma once
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Response {
public:

    // 成功（带数据）
    static std::string success(const json& data = json::object()) {
        json res;
        res["code"] = 0;
        res["message"] = "success";
        res["data"] = data;
        return res.dump();
    }

    // 失败
    static std::string error(const std::string& msg, int code = 1) {
        json res;
        res["code"] = code;
        res["message"] = msg;
        return res.dump();
    }

    // 自定义（高级用法）
    static std::string build(int code, const std::string& msg, const json& data = json()) {
        json res;
        res["code"] = code;
        res["message"] = msg;
        if (!data.is_null()) {
            res["data"] = data;
        }
        return res.dump();
    }
};