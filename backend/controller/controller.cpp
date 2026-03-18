#include "controller.h"
#include <nlohmann/json.hpp>
#include "response.h"

using json = nlohmann::json;

void Controller::initRoutes(httplib::Server& server) {

    // ============================
    // 用户登录接口
    // ============================
    server.Post("/login", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            UserService service;
            auto user = service.login(body["username"], body["password"]);

            if (user) {
                json data = {
                    {"user_id", user->getId()},
                    {"username", user->getUsername()}
                };

                res.set_content(Response::success(data), "application/json");
            } else {
                res.set_content(Response::error("用户名或密码错误"), "application/json");
            }

        } catch (...) {
            res.set_content(Response::error("请求格式错误", -1), "application/json");
        }
    });

    // ============================
    // 注册接口
    // ============================
    server.Post("/register", [](const httplib::Request& req, httplib::Response& res) {
        User user;
        user.setUsername(req.get_param_value("username"));
        user.setPassword(req.get_param_value("password"));
        user.setAge(std::stoi(req.get_param_value("age")));
        user.setPhone(req.get_param_value("phone"));
        user.setStatus(1);

        UserService service;

        json response;
        response["status"] = service.registerUser(user) ? "success" : "fail";

        res.set_content(response.dump(), "application/json");
    });

    // ============================
    // 获取食堂列表
    // ============================
    server.Get("/canteens", [](const httplib::Request&, httplib::Response& res) {
        CanteenService service;
        auto list = service.getAllCanteens();

        json arr = json::array();

        for (const auto& c : list) {
            arr.push_back({
                {"id", c.getId()},
                {"name", c.getName()}
            });
        }

        res.set_content(Response::success(arr), "application/json");
    });

    // ============================
    // 获取菜单
    // ============================
    server.Get("/menu", [](const httplib::Request& req, httplib::Response& res) {
        int canteen_id = std::stoi(req.get_param_value("canteen_id"));
        std::string date = req.get_param_value("date");

        MenuService service;
        auto dishes = service.getTodayMenu(canteen_id, date);

        json arr = json::array();

        for (const auto& d : dishes) {
            json obj;
            obj["id"] = d.getId();
            obj["name"] = d.getName();
            obj["price"] = d.getPrice();
            arr.push_back(obj);
        }

        res.set_content(arr.dump(4), "application/json");
    });

    // ============================
    // 下单
    // ============================
    server.Post("/orders", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            int user_id = body["user_id"];
            int canteen_id = body["canteen_id"];

            std::vector<OrderItem> items;

            for (auto& item : body["items"]) {
                OrderItem oi;
                oi.setDishId(item["dish_id"]);
                oi.setQuantity(item["quantity"]);
                items.push_back(oi);
            }

            OrderService service;

            if (service.placeOrder(user_id, canteen_id, items)) {
                res.set_content(Response::success(), "application/json");
            } else {
                res.set_content(Response::error("下单失败"), "application/json");
            }

        } catch (...) {
            res.set_content(Response::error("JSON格式错误", -1), "application/json");
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

        json response;
        response["status"] = service.submitRating(r) ? "success" : "fail";

        res.set_content(response.dump(), "application/json");
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

        json response;
        response["status"] = service.submitReport(report) ? "success" : "fail";

        res.set_content(response.dump(), "application/json");
    });
}