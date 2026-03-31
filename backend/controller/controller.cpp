#include "controller.h"
#include <nlohmann/json.hpp>
#include "response.h"

using json = nlohmann::json;

void Controller::initRoutes(httplib::Server& server) {

    // ============================
    // ⭐ 全局 CORS
    // ============================
    server.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "POST, GET, OPTIONS"},
        {"Access-Control-Allow-Headers", "*"}
    });

    server.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 200;
    });

    // ============================
    // 用户登录
    // ============================
    server.Post("/login", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            std::string username = body["username"];
            std::string password = body["password"];

            if (username.empty() || password.empty()) {
                res.status = 400;
                res.set_content(Response::error(400, "用户名或密码不能为空"), "application/json");
                return;
            }

            UserService service;
            auto user = service.login(username, password);

            if (user) {
                json data = {
                    {"user_id", user->getId()},
                    {"username", user->getUsername()}
                };

                res.status = 200;
                res.set_content(Response::success(data), "application/json");
                std::cout << "用户 " << username << " 登录成功" << std::endl;
            } else {
                res.status = 401;
                res.set_content(Response::error(401, "用户名或密码错误"), "application/json");
            }

        } catch (...) {
            res.status = 500;
            res.set_content(Response::error(500, "服务器错误"), "application/json");
        }
    });

    // ============================
    // 注册
    // ============================
    server.Post("/register", [](const httplib::Request& req, httplib::Response& res) {
        try {

            std::cout <<"注册用户信息为：" << "req.body = " << req.body << std::endl;
            json body = json::parse(req.body);

            User user;
            user.setUsername(body["username"]);
            user.setPassword(body["password"]);
            if (body["age"].is_number()) {
                user.setAge(body["age"]);
            } else if (body["age"].is_string()) {
                user.setAge(std::stoi(body["age"].get<std::string>())); // 默认年龄为0
            }
            user.setPhone(body["phone"]);
            
            user.setStatus(1);

            if (user.getUsername().empty() || user.getPassword().empty()) {
                res.status = 400;
                res.set_content(Response::error(400, "参数不完整"), "application/json");
                return;
            }

            UserService service;

            if (service.registerUser(user)) {
                res.set_content(Response::success(), "application/json");
            } else {
                res.set_content(Response::error(500, "注册失败"), "application/json");
            }

        } catch (...) {
            res.set_content(Response::error(500, "JSON解析失败"), "application/json");
        }
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
        try {
            int canteen_id = std::stoi(req.get_param_value("canteen_id"));
            std::string date = req.get_param_value("date");
            std::cout << "请求菜单参数：canteen_id=" << canteen_id << ", date=" << date << std::endl;
            MenuService service;
            auto dishes = service.getTodayMenu(canteen_id, date);

            json arr = json::array();

            for (const auto& d : dishes) {
                arr.push_back({
                    {"id", d.getId()},
                    {"name", d.getName()},
                    {"price", d.getPrice()}
                });
                std::cout << "菜单项：id=" << d.getId() << ", name=" << d.getName() << ", price=" << d.getPrice() << std::endl;
            }

            res.set_content(Response::success(arr), "application/json");

        } catch (...) {
            res.set_content(Response::error(400, "参数错误"), "application/json");
        }
    });

    // ============================
    // 下单
    // ============================
    server.Post("/orders", [](const httplib::Request& req, httplib::Response& res) {
        try {
            std::cout << "下单请求体：" << req.body << std::endl;
            json body = json::parse(req.body);

            int user_id     = 0;
            int canteen_id  = 0;

            if (body["user_id"].is_number()) {
                user_id = body["user_id"];
            } else if (body["user_id"].is_string()) {
                user_id = std::stoi(body["user_id"].get<std::string>());
            }

            if (body["canteen_id"].is_number()) {
                canteen_id = body["canteen_id"];
            } else if (body["canteen_id"].is_string()) {
                canteen_id = std::stoi(body["canteen_id"].get<std::string>());
            }
            std::cout << "下单参数：user_id=" << user_id << ", canteen_id=" << canteen_id << std::endl;
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
                res.set_content(Response::error(500, "下单失败"), "application/json");
            }

        } catch (...) {
            res.set_content(Response::error(400, "JSON格式错误"), "application/json");
        }
    });

    // ============================
    // 评价
    // ============================
    server.Post("/rating", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            Rating r;
            if (body["user_id"].is_number()) {
                r.setUserId(body["user_id"]);
            } else if (body["user_id"].is_string()) {
                r.setUserId(std::stoi(body["user_id"].get<std::string>()));
            }
            if (body["canteen_id"].is_number()) {
                r.setCanteenId(body["canteen_id"]);
            } else if (body["canteen_id"].is_string()) {
                r.setCanteenId(std::stoi(body["canteen_id"].get<std::string>()));
            }
            if (body["score"].is_number()) {
                r.setScore(body["score"]);
            } else if (body["score"].is_string()) {
                r.setScore(std::stoi(body["score"].get<std::string>()));
            }
            if (body["comment"].is_string()) {
                r.setComment(body["comment"]);
            }else {
                r.setComment("");
                std::cout << "评价内容为空" << std::endl;
            }
            if (body["order_id"].is_number()) {
                r.setOrderId(body["order_id"]);
            } else if (body["order_id"].is_string()) {
                r.setOrderId(std::stoi(body["order_id"].get<std::string>()));
            }
            std::cout << "评价参数：user_id=" << r.getUserId() << ", canteen_id=" << r.getCanteenId()
                      << ", score=" << r.getScore() << ", comment=" << r.getComment() << std::endl;

            RatingService service;

            if (service.submitRating(r)) {
                res.set_content(Response::success(), "application/json");
            } else {
                res.set_content(Response::error(500, "评价失败"), "application/json");
            }

        } catch (...) {
            res.set_content(Response::error(400, "JSON格式错误"), "application/json");
        }
    });

    // ============================
    // 举报
    // ============================
    server.Post("/report", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            Report report;
            if (body["user_id"].is_number()) {
                report.setUserId(body["user_id"]);
            } else if (body["user_id"].is_string()) {
                report.setUserId(std::stoi(body["user_id"].get<std::string>()));
            }
            if (body["canteen_id"].is_number()) {
                report.setCanteenId(body["canteen_id"]);
            } else if (body["canteen_id"].is_string()) {
                report.setCanteenId(std::stoi(body["canteen_id"].get<std::string>()));
            }
            if (body["type"].is_number()) {
                report.setType(body["type"]);
            } else if (body["type"].is_string()) {
                report.setType(std::stoi(body["type"].get<std::string>()));
            }
            
            if (body["content"].is_string()) {
                report.setContent(body["content"]);
            } else {
                report.setContent("");
                std::cout << "投诉内容为空" << std::endl;
            }
            std::cout << "举报参数：user_id=" << report.getUserId() << ", canteen_id=" << report.getCanteenId()
                      << ", type=" << report.getType() << ", content=" << report.getContent() << std::endl;

            ReportService service;

            if (service.submitReport(report)) {
                res.set_content(Response::success(), "application/json");
            } else {
                res.set_content(Response::error(500, "举报失败"), "application/json");
            }

        } catch (...) {
            res.set_content(Response::error(400, "JSON格式错误"), "application/json");
        }
    });
}

