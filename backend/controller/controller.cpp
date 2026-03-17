#include "controller.h"
#include <iostream>

// 简单 JSON 拼接（后面可以换 json库）
std::string jsonResponse(const std::string& key, const std::string& value) {
    return "{\"" + key + "\":\"" + value + "\"}";
}

void Controller::initRoutes(httplib::Server& server) {

    // ============================
    // 用户登录
    // ============================
    server.Post("/login", [](const httplib::Request& req, httplib::Response& res) {
        auto username = req.get_param_value("username");
        auto password = req.get_param_value("password");

        UserService service;
        auto user = service.login(username, password);

        if (user) {
            res.set_content("{\"status\":\"success\"}", "application/json");
        } else {
            res.set_content("{\"status\":\"fail\"}", "application/json");
        }
    });

    // ============================
    // 注册
    // ============================
    server.Post("/register", [](const httplib::Request& req, httplib::Response& res) {
        User user;
        user.setUsername(req.get_param_value("username"));
        user.setPassword(req.get_param_value("password"));
        user.setAge(std::stoi(req.get_param_value("age")));
        user.setPhone(req.get_param_value("phone"));
        user.setStatus(1);

        UserService service;

        if (service.registerUser(user)) {
            res.set_content("{\"status\":\"success\"}", "application/json");
        } else {
            res.set_content("{\"status\":\"fail\"}", "application/json");
        }
    });

    // ============================
    // 获取食堂列表
    // ============================
    server.Get("/canteens", [](const httplib::Request&, httplib::Response& res) {
        CanteenService service;
        auto list = service.getAllCanteens();

        std::string json = "[";
        for (size_t i = 0; i < list.size(); i++) {
            json += "{\"id\":" + std::to_string(list[i].getId()) +
                    ",\"name\":\"" + list[i].getName() + "\"}";
            if (i != list.size() - 1) json += ",";
        }
        json += "]";

        res.set_content(json, "application/json");
    });

    // ============================
    // 获取菜单
    // ============================
    server.Get("/menu", [](const httplib::Request& req, httplib::Response& res) {
        int canteen_id = std::stoi(req.get_param_value("canteen_id"));
        std::string date = req.get_param_value("date");

        MenuService service;
        auto dishes = service.getTodayMenu(canteen_id, date);

        std::string json = "[";
        for (size_t i = 0; i < dishes.size(); i++) {
            json += "{\"id\":" + std::to_string(dishes[i].getId()) +
                    ",\"name\":\"" + dishes[i].getName() +
                    "\",\"price\":" + std::to_string(dishes[i].getPrice()) + "}";

            if (i != dishes.size() - 1) json += ",";
        }
        json += "]";

        res.set_content(json, "application/json");
    });

    // ============================
    // 下单（简化版）
    // ============================
    server.Post("/order", [](const httplib::Request& req, httplib::Response& res) {
        int user_id = std::stoi(req.get_param_value("user_id"));
        int canteen_id = std::stoi(req.get_param_value("canteen_id"));

        // 简化：只支持一个菜
        OrderItem item;
        item.setDishId(std::stoi(req.get_param_value("dish_id")));
        item.setQuantity(std::stoi(req.get_param_value("quantity")));

        std::vector<OrderItem> items = { item };

        OrderService service;

        if (service.placeOrder(user_id, canteen_id, items)) {
            res.set_content("{\"status\":\"success\"}", "application/json");
        } else {
            res.set_content("{\"status\":\"fail\"}", "application/json");
        }
    });

    // ============================
    // 提交评价
    // ============================
    server.Post("/rating", [](const httplib::Request& req, httplib::Response& res) {
        Rating r;
        r.setUserId(std::stoi(req.get_param_value("user_id")));
        r.setCanteenId(std::stoi(req.get_param_value("canteen_id")));
        r.setScore(std::stoi(req.get_param_value("score")));
        r.setComment(req.get_param_value("comment"));

        RatingService service;

        if (service.submitRating(r)) {
            res.set_content("{\"status\":\"success\"}", "application/json");
        } else {
            res.set_content("{\"status\":\"fail\"}", "application/json");
        }
    });

    // ============================
    // 举报
    // ============================
    server.Post("/report", [](const httplib::Request& req, httplib::Response& res) {
        Report report;
        report.setUserId(std::stoi(req.get_param_value("user_id")));
        report.setCanteenId(std::stoi(req.get_param_value("canteen_id")));
        report.setType(std::stoi(req.get_param_value("type")));
        report.setContent(req.get_param_value("content"));

        ReportService service;

        if (service.submitReport(report)) {
            res.set_content("{\"status\":\"success\"}", "application/json");
        } else {
            res.set_content("{\"status\":\"fail\"}", "application/json");
        }
    });
}