#include "controller.h"
#include <nlohmann/json.hpp>
#include "response.h"

using json = nlohmann::json;

// 安全地从 JSON 中获取参数，支持数字字符串自动转换
int getIntSafe(const json& body, const std::string& key, int defaultVal = 0) {
    // 如果参数不存在或为 null，返回默认值
    if (!body.contains(key) || body[key].is_null()) 
    {
        std::cout << "参数 " << key << " 不存在或为 null" << defaultVal << std::endl;
        return defaultVal;
    }

    // 如果已经是整数，直接返回
    if (body[key].is_number_integer()) {
        return body[key];
    }

    // 支持字符串数字自动转换
    if (body[key].is_string()) {
        try {
            return std::stoi(body[key].get<std::string>());
        } catch (...) {
            return defaultVal;// 转换失败返回默认值
        }
    }

    return defaultVal;
}

// 获取字符串参数，支持数字自动转换为字符串
std::string getStringSafe(const json& body, const std::string& key, const std::string& defaultVal = "") {
    if (!body.contains(key) || body[key].is_null()) return defaultVal;

    if (body[key].is_string()) {
        return body[key];
    }

    // 支持 number → string
    if (body[key].is_number()) {
        return std::to_string(body[key].get<int>());
    }

    return defaultVal;
}

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
            std::string username = getStringSafe(body, "username");
            std::string password = getStringSafe(body, "password");

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
            user.setUsername(getStringSafe(body, "username"));
            user.setPassword(getStringSafe(body, "password"));
            user.setAge(getIntSafe(body, "age"));
            user.setPhone(getStringSafe(body, "phone"));
            user.setStatus(1);

            if (user.getUsername().empty() || user.getPassword().empty()) {
                res.status = 400;
                res.set_content(Response::error(400, "参数不完整"), "application/json");
                return;
            }

            if (user.getAge() <= 0 || user.getAge() > 150) {
                res.status = 400;
                res.set_content(Response::error(400, "年龄必须在1-150之间"), "application/json");
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
            user_id = getIntSafe(body, "user_id");
            canteen_id = getIntSafe(body, "canteen_id");
           
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
    // 查看订单
    // ============================
    server.Get("/orders", [](const httplib::Request& req, httplib::Response& res) {
        try {
            int user_id = std::stoi(req.get_param_value("user_id"));
            std::cout << "查看订单参数：user_id=" << user_id << std::endl;

            OrderService service;
            auto orders = service.getOrdersByUser(user_id);

            json arr = json::array();

            for (const auto& o : orders) {
                arr.push_back({
                    {"order_id", o.getOrderId()},
                    {"order_for_user_name", o.getOrderForUserName()},
                    {"canteen_name", o.getCanteenName()},
                    {"total_price", o.getTotalPrice()},
                    {"create_time", o.getCreateTime()}
                });
                std::cout << "订单：order_id=" << o.getOrderId() << ", canteen_name=" << o.getCanteenName()
                          << ", total_price=" << o.getTotalPrice() << ", create_time=" << o.getCreateTime() << std::endl;
            }

            res.set_content(Response::success(arr), "application/json");

        } catch (...) {
            res.set_content(Response::error(400, "参数错误"), "application/json");
        }
    });

    // ============================
    // 订单详情
    // ============================
    server.Get("/order_details", [](const httplib::Request& req, httplib::Response& res) {
        try {
            std::cout << "订单详情请求参数：order_id=" << req.get_param_value("order_id") << ", user_id=" << req.get_param_value("user_id") << std::endl;
            int order_id = std::stoi(req.get_param_value("order_id"));
            int user_id = std::stoi(req.get_param_value("user_id"));

            OrderService service;
            auto items = service.getOrdersDetailsByUser(user_id,order_id);

            json arr = json::array();

            for (const auto& i : items) {
                arr.push_back({
                    {"dish_name", i.getDishName()},
                    {"price", i.getPrice()},
                    {"quantity", i.getQuantity()}

                });
                std::cout << "订单详情项：dish_name=" << i.getDishName() << ", quantity=" << i.getQuantity() << ", price=" << i.getPrice() << std::endl;
            }

            res.set_content(Response::success(arr), "application/json");

        } catch (...) {
            res.set_content(Response::error(400, "参数错误"), "application/json");
        }
    });

    // ============================
    // 评价
    // ============================
    server.Post("/rating", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            Rating r;
            r.setUserId(getIntSafe(body, "user_id"));
            r.setCanteenId(getIntSafe(body, "canteen_id"));
            r.setScore(getIntSafe(body, "score"));
            r.setComment(getStringSafe(body, "comment"));
            r.setOrderId(getIntSafe(body, "order_id"));

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
            report.setUserId(getIntSafe(body, "user_id"));
            report.setCanteenId(getIntSafe(body, "canteen_id"));
            report.setType(getIntSafe(body, "type"));
            report.setContent(getStringSafe(body, "content"));

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

    // ============================
    // 用户个人中心
    // ============================
    //用户信息获取
    server.Get("/userCenter", [](const httplib::Request& req, httplib::Response& res) {
        try {
            int user_id = std::stoi(req.get_param_value("user_id"));
            std::cout << "个人中心请求参数：user_id=" << user_id << std::endl;

            UserService service;
            auto user = service.getDinerCenterByUserId(user_id);

            if (user) {
                json data = {
                    {"user_id", user->getUserId()},
                    {"username", user->getUsername()},
                    {"age", user->getAge()},
                    {"phone", user->getPhone()},
                    {"idCard", user->getIdCard()},
                    {"address", user->getAddress()},
                    {"familyId", user->getFamilyId()},
                    {"familyName", user->getFamilyName()},
                    {"diseaseHistory", user->getDiseaseHistory()},
                    {"tastePreference", user->getTastePreference()}
                };

                res.status = 200;
                res.set_content(Response::success(data), "application/json");
                std::cout << "用户 " << user->getUsername() << " 个人中心数据获取成功" << std::endl;
            } else {
                res.status = 404;
                res.set_content(Response::error(404, "用户未找到"), "application/json");
            }

        } catch (...) {
            res.status = 500;
            res.set_content(Response::error(500, "服务器错误"), "application/json");
        }
    });

    //用户信息修改
    server.Post("/userCenterUpdate", [](const httplib::Request& req, httplib::Response& res) {
        try{
            json body = json::parse(req.body);
            std::cout << "个人中心更新请求体：" << req.body << std::endl;

            DinerCenterVO user;
            user.setUserId(getIntSafe(body, "user_id"));
            user.setAge(getIntSafe(body, "age"));
            user.setPhone(getStringSafe(body, "phone"));
            user.setAddress(getStringSafe(body, "address"));
            user.setIdCard(getStringSafe(body, "id_card"));
            user.setDiseaseHistory(getStringSafe(body, "disease_history"));
            user.setTastePreference(getStringSafe(body, "taste_preference"));
            user.setFamilyId(getIntSafe(body, "family_id"));

            std::cout<<"个人信息解析成功"<<std::endl;

            if (user.getUserId() == 0) {
                res.status = 400;
                res.set_content(Response::error(400, "用户ID不能为空"), "application/json");
                return;
            }
            if (user.getAge() <= 0 || user.getAge() > 150) {
                res.status = 400;
                res.set_content(Response::error(400, "年龄必须在1-150之间"), "application/json");
                return;
            }
            if (user.getFamilyId() == 0) {
                user.setFamilyId(1); // 默认设置为“未设置家庭”
            }
            UserService service;
            if (service.updateDinerCenter(user)) {
                res.set_content(Response::success(), "application/json");
                std::cout << "用户 " << user.getUserId() << " 个人中心数据更新成功" << std::endl;
            } else {
                std::cerr << "用户 " << user.getUserId() << " 个人中心数据更新失败" << std::endl;
                res.set_content(Response::error(500, "更新失败"), "application/json");
            }
        }
       catch (const std::exception& e) {
            std::cerr << "更新异常: " << e.what() << std::endl;
            res.set_content(Response::error(500, "服务器错误"), "application/json");
        }
    });    
    // ============================
    // 家庭列表
    // ============================
    server.Get("/familyList", [](const httplib::Request& req, httplib::Response& res) {
        try{
            std::cout << "家庭列表请求" << std::endl;

            FamilyService service;
            auto familyList = service.getFamilyList();

            json arr = json::array();

            for (const auto& f : familyList) {
                arr.push_back({
                    {"family_id", f.getId()},
                    {"family_name", f.getName()}
                });
                std::cout << "家庭列表项：family_id=" << f.getId() << ", family_name=" << f.getName() << std::endl;
            }
            res.set_content(Response::success(arr), "application/json");
        } catch (...) {
            res.set_content(Response::error(400, "JSON格式错误"), "application/json");

        }
    });

}

