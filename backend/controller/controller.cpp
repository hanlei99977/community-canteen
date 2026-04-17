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

    registerUserRoutes(server);
    registerCanteenRoutes(server);
    registerOrderRoutes(server);
    registerUserCenterRoutes(server);

}

// 用户相关路由
void Controller::registerUserRoutes(httplib::Server& server) {
    //登录注册
    server.Post("/login", handleLogin);
    server.Post("/register", handleRegister);
    //管理员管理
    server.Get("/adminList", handleAdminList);
    server.Get("/adminApplyList", handleAdminApplyList);
    server.Post("/adminApplyReview", handleAdminApplyReview);
    server.Get("/managerApplyList", handleManagerApplyList);
    server.Post("/managerApplyReview", handleManagerApplyReview);
    server.Get("/dinerList", handleDinerList);
    server.Post("/updateStatus", handleUpdateStatus);
    // 公告
    server.Get("/announcementList", handleAnnouncementList);
    server.Post("/announcementPublish", handleAnnouncementPublish);
    server.Post("/announcementDelete", handleAnnouncementDelete);
    // 投诉处理
    server.Get("/reportList", handleReportList);
    server.Post("/reportHandle", handleReportHandle);
}

// 订单相关路由
void Controller::registerOrderRoutes(httplib::Server& server) {
    server.Post("/placeOrder", handlePlaceOrder);
    server.Get("/orderTargets", handleOrderTargets);
    server.Get("/getOrders", handleGetOrders);
    server.Get("/order_details", handleOrderDetails);
    server.Post("/rating", handleRating);
    server.Post("/report", handleReport);
}

// 食堂相关路由
void Controller::registerCanteenRoutes(httplib::Server& server) {
    server.Get("/canteens", handleCanteens);
    // 餐单
    server.Get("/menu", handleMenu);
    server.Get("/getMenus", handleGetCanteenMenus);
    server.Post("/menuCreate", handleCreateMenu);
    server.Post("/menuDelete", handleDeleteMenu);
    //菜品
    server.Get("/getDishes", handleGetDishes);
    server.Post("/dishCreate", handleCreateDish);
    server.Post("/dishDisable", handleDisableDish);
    server.Post("/dishEnable", handleEnableDish);

}

// 个人中心相关路由
void Controller::registerUserCenterRoutes(httplib::Server& server) {
    server.Get("/userCenter", handleUserCenter);
    server.Post("/userCenterUpdate", handleUserCenterUpdate);
    server.Get("/familyList", handleFamilyList);
    server.Post("/createFamily", handleCreateFamily);
    server.Get("/regionList", handleRegionList);
    // 食堂管理
    server.Get("/myCanteen", handleMyCanteen);
    server.Post("/updateCanteenAddress", handleUpdateCanteenAddress);
    server.Get("/canteenList", handleCanteenList);
    server.Post("/updateCanteenStatus", handleUpdateCanteenStatus);
}


//用户登录
void Controller::handleLogin(const httplib::Request& req, httplib::Response& res)
{
    try {
        json body = json::parse(req.body);
        std::string username = getStringSafe(body, "username");
        std::string password = getStringSafe(body, "password");

        if (username.empty() || password.empty()) {
            res.status = 400;
            res.set_content(Response::error(400, "用户名或密码不能为空"), "application/json");
            return;
        }

        UserService userService;
        CanteenService canteenService; 
        auto user = userService.login(username, password);

        if (!user) {
            res.status = 401;
            res.set_content(Response::error(401, "用户名或密码错误"), "application/json");
            return;
        }

        int user_id = user->getId();
        std::string role = userService.getUserRole(user_id);

        // ⭐ 新增：canteen_id
        int canteen_id = -1;

        if (role == "canteen_manager") {
            canteen_id = canteenService.getCanteenIdByUserId(user_id);
        }

        // ⭐ 返回数据
        json data = {
            {"user_id", user_id},
            {"username", user->getUsername()},
            {"role", role},
            {"canteen_id", canteen_id}   // ⭐ 新增
        };

        res.status = 200;
        res.set_content(Response::success(data), "application/json");

        std::cout << "用户 " << username << "role" << role << " canteen_id " << canteen_id << " 登录成功" << std::endl; 

    } catch (...) {
        res.status = 500;
        res.set_content(Response::error(500, "服务器错误"), "application/json");
    }
}

//用户注册
void Controller::handleRegister(const httplib::Request& req, httplib::Response& res)
{
    try {
        json body = json::parse(req.body);

        User user;
        user.setUsername(getStringSafe(body, "username"));
        user.setPassword(getStringSafe(body, "password"));
        user.setAge(getIntSafe(body, "age"));
        user.setPhone(getStringSafe(body, "phone"));
        user.setStatus(1);
        std::string role = getStringSafe(body, "role", "diner");

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

        UserService userService;
        if (userService.isUsernameTaken(user.getUsername())) {
            res.status = 409;
            res.set_content(Response::error(409, "用户名已存在，请更换"), "application/json");
            return;
        }

        if (role == "admin") {
            int level_id = getIntSafe(body, "level_id");
            int region_id = getIntSafe(body, "region_id");
            AdminService service;
            if (service.submitAdminApply(user, level_id, region_id)) {
                std::cout << "管理员申请 " << user.getUsername() << " 提交成功" << std::endl;
                res.set_content(Response::success(json::object(), "管理员申请已提交，等待系统管理员审核"), "application/json");
            } else {
                res.set_content(Response::error(500, "管理员申请提交失败"), "application/json");
            }
            return;
        }

        if (role == "manager") {
            std::string canteen_name = getStringSafe(body, "canteen_name");
            int region_id = getIntSafe(body, "region_id");
            if (region_id <= 0) {
                res.status = 400;
                res.set_content(Response::error(400, "食堂管理者注册必须选择食堂所在区域"), "application/json");
                return;
            }
            ManagerService service;
            if (service.submitManagerApply(user, canteen_name, region_id)) {
                std::cout << "食堂管理者申请 " << user.getUsername() << " 提交成功" << std::endl;
                res.set_content(Response::success(json::object(), "食堂管理者申请已提交，等待系统管理员审核"), "application/json");
            } else {
                res.set_content(Response::error(500, "食堂管理者申请提交失败"), "application/json");
            }
            return;
        }

        int region_id = getIntSafe(body, "region_id");
        if (region_id <= 0) {
            res.status = 400;
            res.set_content(Response::error(400, "用餐者注册必须选择所在区域"), "application/json");
            return;
        }

        if (userService.registerUser(user, 1, region_id)) {
            std::cout << "用户 " << user.getUsername() << " 注册成功" << std::endl;
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "注册失败"), "application/json");
        }

    } catch (...) {
        res.set_content(Response::error(500, "JSON解析失败"), "application/json");
    }
}

// 获取食堂列表
void Controller::handleCanteens(const httplib::Request& req, httplib::Response& res)
{
    CanteenService service;
    RatingService ratingService;
    auto list = service.getAllCanteens();

    json arr = json::array();

    for (const auto& c : list) {
        auto ratings = ratingService.getRatings(c.getId());
        auto details = ratingService.getCanteenRatingDetails(c.getId());

        double sum_score = 0.0;
        for (const auto& r : ratings) {
            sum_score += r.getScore();
        }
        int rating_count = static_cast<int>(ratings.size());
        double avg_score = rating_count > 0 ? sum_score / rating_count : 0.0;

        json reviews = json::array();
        for (const auto& item : details) {
            reviews.push_back({
                {"order_id", item.getOrderId()},
                {"user_id", item.getUserId()},
                {"username", item.getUsername()},
                {"score", item.getScore()},
                {"comment", item.getComment()},
                {"time", item.getTime()},
                {"dishes", item.getDishes()}
            });
        }

        arr.push_back({
            {"id", c.getId()},
            {"name", c.getName()},
            {"rating_count", rating_count},
            {"avg_score", avg_score},
            {"rating_text", rating_count < 20 ? "评价过少" : std::to_string(avg_score)},
            {"reviews", reviews}
        });
    }

    res.set_content(Response::success(arr), "application/json");
}

void Controller::handleMenu(const httplib::Request& req, httplib::Response& res)
{
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
}

void Controller::handleGetCanteenMenus(const httplib::Request& req, httplib::Response& res)
{
    try {
        int canteen_id = std::stoi(req.get_param_value("canteen_id"));
        std::cout << "请求食堂菜单参数：canteen_id=" << canteen_id << std::endl;

        MenuService service;
        auto menus = service.getMenuByCanteen(canteen_id);
        std::cout << "查询到菜单数量：" << menus.size() << std::endl;

        json arr = json::array();

        for (auto& m : menus) {

            json dishes = json::array();

            // ⭐ 遍历菜品
            for (const auto& d : m.getDishes()) {
                dishes.push_back({
                    {"dish_id", d.getId()},
                    {"name", d.getName()},
                    {"price", d.getPrice()}
                });
            }

            // ⭐ 一个菜单
            arr.push_back({
                {"menu_id", m.getMenuId()},
                {"date", m.getDate()},
                {"meal_type", m.getType()},
                {"dishes", dishes}
            });
        }

        std::cout << "菜单数量：" << menus.size() << std::endl;

        res.set_content(Response::success(arr), "application/json");

    } catch (const std::exception& e) {
        std::cerr << "handleGetCanteenMenus error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

void Controller::handleCreateMenu(const httplib::Request& req, httplib::Response& res)
{
    std::cout<< " json内容是 " << req.body<<std::endl;
    try {
        json body = json::parse(req.body);
        MenuCreateDTO dto;
        dto.setCanteenId(getIntSafe(body, "canteen_id"));
        dto.setDate(getStringSafe(body, "date"));
        dto.setMealType(getStringSafe(body, "meal_type"));

        std::cout<<" 餐厅 " << dto.getCanteenId() << " 新建每日餐单 "<<std::endl;

       std::vector<int> dish_ids;
        for (const auto& id : body["dish_ids"]) {
            dish_ids.push_back(id.get<int>());
        }
        dto.setDishIds(dish_ids);

        MenuService service;

        if (service.insertMenu(dto)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "创建菜单失败"), "application/json");
        }

    } catch (const std::exception& e) {
        std::cerr << "新建餐单失败: " << e.what() << std::endl;
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleDeleteMenu(const httplib::Request& req, httplib::Response& res)
{
    std::cout<< " json内容是 " << req.body<<std::endl;
    try {
        json body = json::parse(req.body);
        int menu_id = getIntSafe(body, "menu_id");

        MenuService service;

        if (service.eraseMenu(menu_id)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "删除餐单失败"), "application/json");
        }

    } catch (const std::exception& e) {
        std::cerr << "删除餐单失败: " << e.what() << std::endl;
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleGetDishes(const httplib::Request& req, httplib::Response& res)
{
    try {
        int canteen_id = std::stoi(req.get_param_value("canteen_id"));
        std::cout << "请求菜品列表参数：canteen_id=" << canteen_id << std::endl;
        DishService service;
        auto dishes = service.getDishsByCanteen(canteen_id);

        json arr = json::array();

        for (const auto& d : dishes) {
            arr.push_back({
                {"dish_id", d.getId()},
                {"name", d.getName()},
                {"type", d.getType()},
                {"price", d.getPrice()},
                {"calories", d.getCalories()},
                {"nutrition_info", d.getNutritionInfo()},
                {"status",d.getStatus()}
            });
        }

        std::cout << "菜品数量：" << dishes.size() << std::endl;
        res.set_content(Response::success(arr), "application/json");

    } catch (...) {
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

void Controller::handleCreateDish(const httplib::Request& req, httplib::Response& res)
{
    std::cout<< " json内容是 " << req.body<<std::endl;
    try {
        json body = json::parse(req.body);
        Dish dish;
        // 1. 食堂ID（int类型，对应canteen_id字段）
        dish.setCanteenId(getIntSafe(body, "canteen_id"));
        // 2. 菜品名称（string类型，对应name字段）
        dish.setName(getStringSafe(body, "name"));
        // 3. 菜品类型（荤/素，对应type字段）
        dish.setType(getStringSafe(body, "type"));
        // 4. 菜品价格（decimal类型，用getDoubleSafe读取）
        dish.setPrice(body["price"].get<double>());
        // 5. 菜品热量（int类型，对应calories字段）
        dish.setCalories(getIntSafe(body, "calories"));
        // 6. 营养信息（string类型，对应nutrition_info字段）
        dish.setNutritionInfo(getStringSafe(body, "nutrition_info"));

       DishService service;

        if (service.insertDish(dish)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "创建菜品失败"), "application/json");
        }

    } catch (const std::exception& e) {
        std::cerr << "创建菜品失败: " << e.what() << std::endl;
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleDisableDish(const httplib::Request& req, httplib::Response& res)
{
    try {
        json body = json::parse(req.body);
        int dish_id = getIntSafe(body, "dish_id");

        DishService service;

        if (service.disableDishByDishId(dish_id)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "下架菜品失败"), "application/json");
        }

    } catch (const std::exception& e) {
        std::cerr << "下架菜品失败: " << e.what() << std::endl;
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleEnableDish(const httplib::Request& req, httplib::Response& res)
{
    try {
        json body = json::parse(req.body);
        int dish_id = getIntSafe(body, "dish_id");

        DishService service;

        if (service.enableDishByDishId(dish_id)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "上架菜品失败"), "application/json");
        }

    } catch (const std::exception& e) {
        std::cerr << "上架菜品失败: " << e.what() << std::endl;
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handlePlaceOrder(const httplib::Request& req, httplib::Response& res)
{
    try {
        std::cout << "下单请求体：" << req.body << std::endl;
        json body = json::parse(req.body);

        int user_id     = 0;
        int canteen_id  = 0;
        int order_for_user_id = 0;
        user_id = getIntSafe(body, "user_id");
        canteen_id = getIntSafe(body, "canteen_id");
        order_for_user_id = getIntSafe(body, "order_for_user_id", user_id);
        
        std::cout << "下单参数：user_id=" << user_id
                  << ", order_for_user_id=" << order_for_user_id
                  << ", canteen_id=" << canteen_id << std::endl;
        std::vector<OrderItem> items;

        for (auto& item : body["items"]) {
            OrderItem oi;
            oi.setDishId(item["dish_id"]);
            oi.setQuantity(item["quantity"]);
            items.push_back(oi);
        }

        OrderService service;

        if (service.placeOrder(user_id, canteen_id, order_for_user_id, items)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "下单失败"), "application/json");
        }

    } catch (...) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleOrderTargets(const httplib::Request& req, httplib::Response& res)
{
    try {
        int user_id = std::stoi(req.get_param_value("user_id"));
        std::cout << "下单对象请求参数：user_id=" << user_id << std::endl;

        OrderService service;
        auto targets = service.getOrderTargetsByUser(user_id);
        json arr = json::array();

        for (const auto& t : targets) {
            arr.push_back({
                {"user_id", t.getUserId()},
                {"username", t.getUsername()}
            });
        }

        res.set_content(Response::success(arr), "application/json");
    } catch (...) {
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

void Controller::handleGetOrders(const httplib::Request& req, httplib::Response& res)
{
    try {
        int user_id = std::stoi(req.get_param_value("user_id"));
        std::cout << "查看订单参数：user_id=" << user_id << std::endl;

        OrderService service;
        auto orders = service.getOrdersByUser(user_id);

        json arr = json::array();

        for (const auto& o : orders) {
            arr.push_back({
                {"order_id", o.getOrderId()},
                {"canteen_id", o.getCanteenId()},
                {"order_for_user_name", o.getOrderForUserName()},
                {"canteen_name", o.getCanteenName()},
                {"total_price", o.getTotalPrice()},
                {"create_time", o.getCreateTime()},
                {"has_rating", o.getHasRating()},
                {"rating_score", o.getRatingScore()},
                {"rating_comment", o.getRatingComment()},
                {"rating_time", o.getRatingTime()}
            });
            std::cout << "订单：order_id=" << o.getOrderId() << ", canteen_name=" << o.getCanteenName()
                        << ", total_price=" << o.getTotalPrice() << ", create_time=" << o.getCreateTime() << std::endl;
        }

        res.set_content(Response::success(arr), "application/json");

    } catch (...) {
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

void Controller::handleOrderDetails(const httplib::Request& req, httplib::Response& res)
{
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
}

void Controller::handleRating(const httplib::Request& req, httplib::Response& res)
{
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
}

void Controller::handleReport(const httplib::Request& req, httplib::Response& res)
{
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
}

void Controller::handleUserCenter(const httplib::Request& req, httplib::Response& res)
{
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
                {"regionId", user->getRegionId()},
                {"regionName", user->getRegionName()},
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
}

void Controller::handleUserCenterUpdate(const httplib::Request& req, httplib::Response& res)
{
    try{
        json body = json::parse(req.body);
        std::cout << "个人中心更新请求体：" << req.body << std::endl;

        DinerCenterVO user;
        user.setUserId(getIntSafe(body, "user_id"));
        user.setAge(getIntSafe(body, "age"));
        user.setPhone(getStringSafe(body, "phone"));
        user.setIdCard(getStringSafe(body, "id_card"));
        user.setDiseaseHistory(getStringSafe(body, "disease_history"));
        user.setTastePreference(getStringSafe(body, "taste_preference"));
        user.setRegionId(getIntSafe(body, "region_id"));
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
}

void Controller::handleFamilyList(const httplib::Request& req, httplib::Response& res)
{
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
}


void Controller::handleRegionList(const httplib::Request& req, httplib::Response& res)
{
    try{
        std::cout << "区域列表请求" << std::endl;

        RegionService service;
        auto regionList = service.getRegionList();

        json arr = json::array();

        for (const auto& r : regionList) {
            arr.push_back({
                {"region_id", r.getId()},
                {"region_name", r.getName()},
                {"region_level", r.getLevel()},
                {"parent_id", r.getParentId()}
            });
        }
        res.set_content(Response::success(arr), "application/json");
    } catch (...) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");

    }
}

void Controller::handleCreateFamily(const httplib::Request& req, httplib::Response& res)
{
    try {
        json body = json::parse(req.body);
        int user_id = getIntSafe(body, "user_id");
        std::string family_name = getStringSafe(body, "family_name");

        std::cout << "创建家庭请求参数：user_id=" << user_id << ", family_name=" << family_name << std::endl;

        if (user_id <= 0 || family_name.empty()) {
            res.status = 400;
            res.set_content(Response::error(400, "用户ID和家庭名称不能为空"), "application/json");
            return;
        }

        FamilyService service;
        if (service.createFamily(user_id, family_name)) {
            res.set_content(Response::success(), "application/json");
            std::cout << "用户 " << user_id << " 创建家庭 " << family_name << " 成功" << std::endl;
        } else {
            res.set_content(Response::error(500, "创建家庭失败"), "application/json");
        }

    } catch (...) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}


void Controller::handleAdminList(const httplib::Request& req, httplib::Response& res)
{
    try{
        std::cout << "管理员列表请求" << std::endl;

        AdminService service;
        auto adminList = service.getAdminList();

        json arr = json::array();

        for (const auto& adminInfo : adminList) {
            arr.push_back({
                {"user_id", adminInfo.getUserId()},
                {"username", adminInfo.getUsername()},
                {"age", adminInfo.getAge()},
                {"phone", adminInfo.getPhone()},
                {"status", adminInfo.getStatus()},
                {"level_id", adminInfo.getLevelId()},
                {"level_name", adminInfo.getLevelName()},
                {"region_id", adminInfo.getRegionId()},
                {"region_name", adminInfo.getRegionName()}
            });
        }
        res.set_content(Response::success(arr), "application/json");
    } catch (...) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");

    }
}

void Controller::handleDinerList(const httplib::Request& req, httplib::Response& res)
{
    try{
        std::cout << "用餐者列表请求" << std::endl;

        DinerService service;
        auto dinerList = service.getDinerList();

        json arr = json::array();

        for (const auto& dinerInfo : dinerList) {
            arr.push_back({
                {"user_id", dinerInfo.getUserId()},
                {"username", dinerInfo.getUsername()},
                {"age", dinerInfo.getAge()},
                {"phone", dinerInfo.getPhone()},
                {"status", dinerInfo.getStatus()},
                {"region_id", dinerInfo.getRegionId()},
                {"region_name", dinerInfo.getRegionName()},
                {"disease_history", dinerInfo.getDiseaseHistory()},
                {"taste_preference", dinerInfo.getTastePreference()}
            });
        }
        res.set_content(Response::success(arr), "application/json");
    } catch (...) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");

    }
}

void Controller::handleAdminApplyList(const httplib::Request& req, httplib::Response& res)
{
    try{
        std::cout << "管理员申请列表请求" << std::endl;

        AdminService service;
        auto applyList = service.getAdminApplyList();

        json arr = json::array();
        for (const auto& applyInfo : applyList) {
            arr.push_back({
                {"apply_id", applyInfo.getApplyId()},
                {"user_id", applyInfo.getUserId()},
                {"username", applyInfo.getUsername()},
                {"age", applyInfo.getAge()},
                {"phone", applyInfo.getPhone()},
                {"level_id", applyInfo.getLevelId()},
                {"level_name", applyInfo.getLevelName()},
                {"region_id", applyInfo.getRegionId()},
                {"region_name", applyInfo.getRegionName()},
                {"status", applyInfo.getStatus()},
                {"apply_time", applyInfo.getApplyTime()},
                {"review_time", applyInfo.getReviewTime()},
                {"reviewer_id", applyInfo.getReviewerId()},
                {"reviewer_name", applyInfo.getReviewerName()}
            });
        }
        res.set_content(Response::success(arr), "application/json");
    } catch (...) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");

    }
}

void Controller::handleAdminApplyReview(const httplib::Request& req, httplib::Response& res)
{
    try {
        json body = json::parse(req.body);
        int apply_id = getIntSafe(body, "apply_id");
        int reviewer_id = getIntSafe(body, "reviewer_id");
        int status = getIntSafe(body, "status");

        AdminService service;
        if (service.reviewAdminApply(apply_id, reviewer_id, status)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "审核失败"), "application/json");
        }
    } catch (...) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleManagerApplyList(const httplib::Request& req, httplib::Response& res)
{
    try{
        std::cout << "食堂管理者申请列表请求" << std::endl;

        ManagerService service;
        auto applyList = service.getManagerApplyList();

        json arr = json::array();
        for (const auto& applyInfo : applyList) {
            arr.push_back({
                {"apply_id", applyInfo.getApplyId()},
                {"user_id", applyInfo.getUserId()},
                {"username", applyInfo.getUsername()},
                {"age", applyInfo.getAge()},
                {"phone", applyInfo.getPhone()},
                {"canteen_name", applyInfo.getCanteenName()},
                {"region_id", applyInfo.getRegionId()},
                {"region_name", applyInfo.getRegionName()},
                {"status", applyInfo.getStatus()},
                {"apply_time", applyInfo.getApplyTime()},
                {"review_time", applyInfo.getReviewTime()},
                {"reviewer_id", applyInfo.getReviewerId()},
                {"reviewer_name", applyInfo.getReviewerName()}
            });
        }
        res.set_content(Response::success(arr), "application/json");
    } catch (...) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleManagerApplyReview(const httplib::Request& req, httplib::Response& res)
{
    try {
        json body = json::parse(req.body);
        int apply_id = getIntSafe(body, "apply_id");
        int reviewer_id = getIntSafe(body, "reviewer_id");
        int status = getIntSafe(body, "status");

        ManagerService service;
        if (service.reviewManagerApply(apply_id, reviewer_id, status)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "审核失败"), "application/json");
        }
    } catch (...) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleUpdateStatus(const httplib::Request& req, httplib::Response& res)
{
    try {
        json body = json::parse(req.body);

        User user;
        user.setId(getIntSafe(body, "user_id"));
        user.setStatus(getIntSafe(body, "status"));

        UserService service;

        if (service.updateStatus(user)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "用户状态更新失败"), "application/json");
        }

    } catch (...) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleAnnouncementList(const httplib::Request& req, httplib::Response& res)
{
    try {
        AnnouncementService service;
        auto list = service.getAnnouncementList();

        json arr = json::array();
        for (const auto& a : list) {
            arr.push_back({
                {"announce_id", a.getId()},
                {"title", a.getTitle()},
                {"content", a.getContent()},
                {"publish_time", a.getPublishTime()},
                {"publisher_id", a.getPublisherId()},
                {"publisher_name", a.getPublisherName()}
            });
        }

        res.set_content(Response::success(arr), "application/json");
    } catch (...) {
        res.set_content(Response::error(500, "获取公告失败"), "application/json");
    }
}

void Controller::handleAnnouncementPublish(const httplib::Request& req, httplib::Response& res)
{
    try {
        json body = json::parse(req.body);
        Announcement announcement;
        announcement.setTitle(getStringSafe(body, "title"));
        announcement.setContent(getStringSafe(body, "content"));
        announcement.setPublisherId(getIntSafe(body, "publisher_id"));

        AnnouncementService service;
        if (service.publishAnnouncement(announcement)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "发布公告失败"), "application/json");
        }
    } catch (...) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleAnnouncementDelete(const httplib::Request& req, httplib::Response& res)
{
    try {
        json body = json::parse(req.body);
        int announce_id = getIntSafe(body, "announce_id");
        int publisher_id = getIntSafe(body, "publisher_id");

        AnnouncementService service;
        if (service.deleteAnnouncement(announce_id, publisher_id)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(403, "只能删除自己发布的公告"), "application/json");
        }
    } catch (...) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleReportList(const httplib::Request& req, httplib::Response& res)
{
    try {
        ReportService service;
        auto list = service.getAllReports();

        json arr = json::array();
        for (const auto& r : list) {
            arr.push_back({
                {"report_id", r.getReportId()},
                {"user_id", r.getUserId()},
                {"username", r.getUsername()},
                {"canteen_id", r.getCanteenId()},
                {"canteen_name", r.getCanteenName()},
                {"type", r.getType()},
                {"content", r.getContent()},
                {"status", r.getStatus()},
                {"create_time", r.getCreateTime()},
                {"handle_time", r.getHandleTime()},
                {"handler_id", r.getHandlerId()},
                {"handler_name", r.getHandlerName()}
            });
        }

        res.set_content(Response::success(arr), "application/json");
    } catch (...) {
        res.set_content(Response::error(500, "获取投诉列表失败"), "application/json");
    }
}

void Controller::handleReportHandle(const httplib::Request& req, httplib::Response& res)
{
    try {
        json body = json::parse(req.body);
        int report_id = getIntSafe(body, "report_id");
        int status = getIntSafe(body, "status");
        int handler_id = getIntSafe(body, "handler_id");

        ReportService service;
        if (service.handleReport(report_id, status, handler_id)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "投诉处理失败"), "application/json");
        }
    } catch (...) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

// 获取我的食堂信息
void Controller::handleMyCanteen(const httplib::Request& req, httplib::Response& res)
{
    try {
        // 从请求参数获取用户 ID
        int user_id = std::stoi(req.get_param_value("user_id"));
        if (user_id <= 0) {
            res.set_content(Response::error(401, "未登录"), "application/json");
            return;
        }

        CanteenService canteenService;
        // 获取食堂 ID
        int canteen_id = canteenService.getCanteenIdByUserId(user_id);
        if (canteen_id <= 0) {
            res.set_content(Response::error(404, "未找到食堂信息"), "application/json");
            return;
        }

        // 获取食堂详情
        auto canteen = canteenService.getCanteenDetails(canteen_id);
        if (!canteen) {
            res.set_content(Response::error(404, "未找到食堂信息"), "application/json");
            return;
        }

        // 获取食堂评分
        RatingService ratingService;
        auto ratings = ratingService.getRatings(canteen_id);
        double average_rating = 0.0;
        if (!ratings.empty()) {
            int total_score = 0;
            for (const auto& rating : ratings) {
                total_score += rating.getScore();
            }
            average_rating = static_cast<double>(total_score) / ratings.size();
        }

        // 获取投诉数量
        ReportService reportService;
        auto reports = reportService.getReports(canteen_id);
        int complaint_count = reports.size();

        // 构建响应数据
        json data = {
            {"id", canteen->getId()},
            {"name", canteen->getName()},
            {"address", canteen->getAddress()},
            {"regionId", canteen->getRegionId()},
            {"regionName", ""}, // 需要从区域服务获取
            {"rating", average_rating},
            {"complaintCount", complaint_count}
        };

        res.set_content(Response::success(data), "application/json");
    } catch (...) {
        res.set_content(Response::error(500, "获取食堂信息失败"), "application/json");
    }
}

// 更新食堂地址
void Controller::handleUpdateCanteenAddress(const httplib::Request& req, httplib::Response& res)
{
    try {
        std::cout<< " json内容是 " << req.body<<std::endl;
        
        json body = json::parse(req.body);
        int canteen_id = getIntSafe(body, "canteen_id");
        std::string address = getStringSafe(body, "address");

        if (canteen_id <= 0 || address.empty()) {
            res.set_content(Response::error(400, "参数错误"), "application/json");
            return;
        }

        CanteenService canteenService;
        if (canteenService.updateCanteenAddress(canteen_id, address)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "更新地址失败"), "application/json");
        }
    } catch (...) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

// 获取食堂列表
void Controller::handleCanteenList(const httplib::Request& req, httplib::Response& res)
{
    try {
        CanteenService canteenService;
        auto canteens = canteenService.getCanteensWithManagers();

        json data = json::array();
        for (const auto& canteen : canteens) {
            json canteenJson = {
                {"id", canteen.getId()},
                {"name", canteen.getName()},
                {"address", canteen.getAddress()},
                {"regionId", canteen.getRegionId()},
                {"regionName", canteen.getRegionName()},
                {"managerId", canteen.getManagerId()},
                {"managerName", canteen.getManagerName()},
                {"status", canteen.getStatus()},
                {"rating", canteen.getRating()},
                {"complaintCount", canteen.getComplaintCount()}
            };
            data.push_back(canteenJson);
        }

        res.set_content(Response::success(data), "application/json");
    } catch (...) {
        res.set_content(Response::error(500, "获取食堂列表失败"), "application/json");
    }
}

// 更新食堂状态
void Controller::handleUpdateCanteenStatus(const httplib::Request& req, httplib::Response& res)
{
    try {
        std::cout<< " json内容是 " << req.body<<std::endl;
        
        json body = json::parse(req.body);
        int canteen_id = getIntSafe(body, "canteen_id");
        int status = getIntSafe(body, "status");

        if (canteen_id <= 0 || (status != 0 && status != 1)) {
            res.set_content(Response::error(400, "参数错误"), "application/json");
            return;
        }

        CanteenService canteenService;
        if (canteenService.updateCanteenStatus(canteen_id, status)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "更新状态失败"), "application/json");
        }
    } catch (...) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}
