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
        } catch (const std::exception& e) {
            std::cerr << "[getIntSafe] Error converting " << key << ": " << e.what() << std::endl;
            return defaultVal;
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
    server.Get("/recentOrder", handleRecentOrder);
    server.Get("/orderTargets", handleOrderTargets);
    server.Get("/getOrders", handleGetOrders);
    server.Get("/orderDetails", handleOrderDetails);
    server.Post("/rating", handleRating);
    // 食堂订单相关
    server.Get("/canteenOrders", handleCanteenOrders);
    server.Post("/updateOrderStatus", handleUpdateOrderStatus);
    server.Post("/createCancelApply", handleCreateCancelApply);
    server.Get("/getCancelApplies", handleGetCancelApplies);
    server.Get("/getCancelApplyByOrderId", handleGetCancelApplyByOrderId);
    server.Post("/handleCancelApply", handleHandleCancelApply);
    server.Post("/report", handleReport);
    // 用餐偏好
    server.Get("/diningPreference", handleDiningPreference);
}

// 食堂相关路由
void Controller::registerCanteenRoutes(httplib::Server& server) {
    server.Get("/canteens", handleCanteens);
    // 餐单
    server.Get("/menu", handleMenu);
    server.Get("/getMenus", handleGetCanteenMenus);
    server.Post("/menuUpdate", handleUpdateMenu);
    server.Get("/historyMenus", handleGetHistoryMenus);
    //菜品
    server.Get("/getDishes", handleGetDishes);
    server.Post("/dishCreate", handleCreateDish);
    server.Post("/dishDisable", handleDisableDish);
    server.Post("/dishEnable", handleEnableDish);
    server.Post("/dishUpdate", handleUpdateDish);
    //标签
    server.Get("/getAllTags", handleGetAllTags);
    server.Get("/getDishTags", handleGetDishTags);
    // 留言板
    server.Post("/messageCreate", handleCreateMessage);
    server.Get("/userMessages", handleGetUserMessages);
    server.Get("/canteenMessages", handleGetCanteenMessages);
    server.Post("/messageReply", handleReplyMessage);

}

// 个人中心相关路由
void Controller::registerUserCenterRoutes(httplib::Server& server) {
    server.Get("/userCenter", handleUserCenter);
    server.Post("/userCenterUpdate", handleUserCenterUpdate);
    server.Get("/familyList", handleFamilyList);
    server.Post("/createFamily", handleCreateFamily);
    server.Get("/regionList", handleRegionList);
    server.Get("/districtRegionList", handleDistrictRegionList);
    // 食堂管理
    server.Get("/myCanteen", handleMyCanteen);
    server.Post("/updateCanteenAddress", handleUpdateCanteenAddress);
    server.Get("/canteenList", handleCanteenList);
    server.Post("/updateCanteenStatus", handleUpdateCanteenStatus);
    server.Get("/purchaseList", handlePurchaseList);
    server.Post("/createPurchase", handleCreatePurchase);
    server.Post("/updatePurchase", handleUpdatePurchase);
    server.Post("/deletePurchase", handleDeletePurchase);
    // 财务统计
    server.Get("/financialStatistics", handleFinancialStatistics);
    // 消息中心
    server.Get("/messages", handleGetMessages);
    server.Post("/markMessageAsRead", handleMarkMessageAsRead);
    // 修改密码
    server.Post("/changePassword", handleChangePassword);
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

    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleLogin] Error: " << e.what() << std::endl;
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

    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleRegister] Error: " << e.what() << std::endl;
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
        std::string meal_type = req.get_param_value("meal_type");
        std::cout << "请求菜单参数：canteen_id=" << canteen_id << ", meal_type=" << meal_type << std::endl;
        MenuService service;
        auto dishes = service.getMenuByMealType(canteen_id, meal_type);

        json arr = json::array();

        for (const auto& d : dishes) {
            arr.push_back({
                {"id", d.getId()},
                {"name", d.getName()},
                {"price", d.getPrice()},
                {"type", d.getType()},
                {"calories", d.getCalories()},
                {"nutrition_info", d.getNutritionInfo()}
            });
            std::cout << "菜单项：id=" << d.getId() << ", name=" << d.getName() << ", price=" << d.getPrice() << ", type=" << d.getType() << ", calories=" << d.getCalories() << std::endl;
        }

        res.set_content(Response::success(arr), "application/json");

    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleMenu] Error: " << e.what() << std::endl;
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

void Controller::handleUpdateMenu(const httplib::Request& req, httplib::Response& res)
{
    std::cout<< " json内容是 " << req.body<<std::endl;
    try {
        json body = json::parse(req.body);
        MenuCreateDTO dto;
        dto.setCanteenId(getIntSafe(body, "canteen_id"));
        dto.setMealType(getStringSafe(body, "meal_type"));

        std::cout<<" 餐厅 " << dto.getCanteenId() << " 更新餐单 "<<std::endl;

       std::vector<int> dish_ids;
        for (const auto& id : body["dish_ids"]) {
            dish_ids.push_back(id.get<int>());
        }
        dto.setDishIds(dish_ids);

        MenuService service;

        if (service.updateMenu(dto)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "更新菜单失败"), "application/json");
        }

    } catch (const std::exception& e) {
        std::cerr << "更新餐单失败: " << e.what() << std::endl;
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

    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleGetDishes] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

void Controller::handleCreateDish(const httplib::Request& req, httplib::Response& res)
{
    std::cout<< " json内容是 " << req.body<<std::endl;
    try {
        json body = json::parse(req.body);
        Dish dish;
        dish.setCanteenId(getIntSafe(body, "canteen_id"));
        dish.setName(getStringSafe(body, "name"));
        dish.setType(getStringSafe(body, "type"));
        dish.setPrice(body["price"].get<double>());
        dish.setCalories(getIntSafe(body, "calories"));
        dish.setNutritionInfo(getStringSafe(body, "nutrition_info"));

        std::vector<int> tag_ids;
        if (body.contains("tag_ids") && body["tag_ids"].is_array()) {
            for (const auto& id : body["tag_ids"]) {
                tag_ids.push_back(id.get<int>());
            }
        }

        DishService service;
        int dish_id = service.insertDish(dish, tag_ids);

        if (dish_id > 0) {
            res.set_content(Response::success({{"dish_id", dish_id}}), "application/json");
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
        std::cout<< "下架菜品:"<< dish_id << std::endl;
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

void Controller::handleGetHistoryMenus(const httplib::Request& req, httplib::Response& res)
{
    try {
        int canteen_id = std::stoi(req.get_param_value("canteen_id"));
        std::cout << "请求历史餐单参数：canteen_id=" << canteen_id << std::endl;

        MenuService menuService;
        
        // 获取历史餐单列表
        auto historyMenus = menuService.getHistoryMenusByCanteen(canteen_id);
        std::cout << "查询到历史餐单数量：" << historyMenus.size() << std::endl;

        json arr = json::array();

        for (const auto& menu : historyMenus) {
            // 获取历史餐单的菜品
            auto dishes = menuService.getHistoryMenuDishes(menu.getHistoryMenuId());
            
            json dishes_json = json::array();
            for (const auto& dish : dishes) {
                dishes_json.push_back({
                    {"dish_id", dish.getId()},
                    {"name", dish.getName()},
                    {"price", dish.getPrice()}
                });
            }

            arr.push_back({
                {"history_menu_id", menu.getHistoryMenuId()},
                {"meal_type", menu.getMealType()},
                {"start_time", menu.getStartTime()},
                {"end_time", menu.getEndTime()},
                {"dishes", dishes_json}
            });
        }

        res.set_content(Response::success(arr), "application/json");

    } catch (const std::exception& e) {
        std::cerr << "获取历史餐单失败: " << e.what() << std::endl;
        res.set_content(Response::error(400, "参数错误"), "application/json");
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

    } catch (const std::exception& e) {
        std::cerr << "[Controller::handlePlaceOrder] Error: " << e.what() << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleOrderTargets] Error: " << e.what() << std::endl;
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
                {"discount_rate", o.getDiscountRate()},
                {"original_total", o.getOriginalTotal()},
                {"saved_amount", o.getSavedAmount()},
                {"status", o.getStatus()},
                {"create_time", o.getCreateTime()},
                {"has_rating", o.getHasRating()},
                {"rating_score", o.getRatingScore()},
                {"rating_comment", o.getRatingComment()},
                {"rating_time", o.getRatingTime()}
            });
            std::cout << "订单：order_id=" << o.getOrderId() << ", canteen_name=" << o.getCanteenName()
                        << ", total_price=" << o.getTotalPrice() << ", discount_rate=" << o.getDiscountRate()
                        << ", original_total=" << o.getOriginalTotal() << ", saved_amount=" << o.getSavedAmount()
                        << ", create_time=" << o.getCreateTime() << std::endl;
        }

        res.set_content(Response::success(arr), "application/json");

    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleGetOrders] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

void Controller::handleOrderDetails(const httplib::Request& req, httplib::Response& res)
{
    try {
        std::cout << "订单详情请求参数：order_id=" << req.get_param_value("order_id") << ", user_id=" << req.get_param_value("user_id") << std::endl;
        int order_id = std::stoi(req.get_param_value("order_id"));
        OrderService service;
        auto items = service.getOrdersDetailsByOrderId(order_id);

        json arr = json::array();

        for (const auto& i : items) {
            json item_json;
            item_json["dish_name"] = i.getDishName();
            item_json["unit_price"] = i.getUnitPrice();
            item_json["discount_price"] = i.getDiscountPrice();
            item_json["quantity"] = i.getQuantity();
            item_json["subtotal"] = i.getSubtotal();
            arr.push_back(item_json);
            std::cout << "订单详情项：dish_name=" << i.getDishName() << ", quantity=" << i.getQuantity() << ", unit_price=" << i.getUnitPrice() << ", discount_price=" << i.getDiscountPrice() << ", subtotal=" << i.getSubtotal() << std::endl;
        }

        res.set_content(Response::success(arr), "application/json");

    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleOrderDetails] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

void Controller::handleRecentOrder(const httplib::Request& req, httplib::Response& res)
{
    try {
        std::cout << "最近订单请求参数：user_id=" << req.get_param_value("user_id") << ", order_for_user_id=" << req.get_param_value("order_for_user_id") << ", canteen_id=" << req.get_param_value("canteen_id") << std::endl;
        int user_id = std::stoi(req.get_param_value("user_id"));
        int order_for_user_id = std::stoi(req.get_param_value("order_for_user_id"));
        int canteen_id = std::stoi(req.get_param_value("canteen_id"));

        OrderService service;
        auto recent_order = service.getRecentOrder(user_id, order_for_user_id, canteen_id);

        if (recent_order) {
            json order_json;
            order_json["order_id"] = recent_order->getOrderId();
            order_json["canteen_id"] = recent_order->getCanteenId();
            order_json["canteen_name"] = recent_order->getCanteenName();
            order_json["order_time"] = recent_order->getOrderTime();
            order_json["total_price"] = recent_order->getTotalPrice();
            order_json["discount_rate"] = recent_order->getDiscountRate();
            order_json["original_total"] = recent_order->getOriginalTotal();
            order_json["saved_amount"] = recent_order->getSavedAmount();
            
            json items = json::array();
            for (const auto& item : recent_order->getItems()) {
                json item_json;
                item_json["dish_id"] = item.getDishId();
                item_json["dish_name"] = item.getDishName();
                item_json["quantity"] = item.getQuantity();
                item_json["unit_price"] = item.getUnitPrice();
                item_json["discount_price"] = item.getDiscountPrice();
                item_json["subtotal"] = item.getSubtotal();
                items.push_back(item_json);
            }
            order_json["items"] = items;
            
            res.set_content(Response::success(order_json), "application/json");
        } else {
            res.set_content(Response::success(nullptr), "application/json");
        }

    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleRecentOrder] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

void Controller::handleCreateCancelApply(const httplib::Request& req, httplib::Response& res) {
    try {
        json body = json::parse(req.body);
        int order_id = getIntSafe(body, "order_id");
        std::string cancel_reason = body["cancel_reason"].get<std::string>();

        OrderCancelService service;
        if (service.createCancelApply(order_id, cancel_reason)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "创建取消申请失败"), "application/json");
        }
    } catch (const std::exception& e) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleGetCancelApplies(const httplib::Request& req, httplib::Response& res) {
    try {
        int canteen_id = std::stoi(req.get_param_value("canteen_id"));
        OrderCancelService service;
        auto applies = service.getCancelAppliesByCanteen(canteen_id);

        json applies_json = json::array();
        for (const auto& apply : applies) {
            json apply_json;
            apply_json["cancel_id"] = apply.getCancelId();
            apply_json["order_id"] = apply.getOrderId();
            apply_json["cancel_time"] = apply.getCancelTime();
            apply_json["cancel_reason"] = apply.getCancelReason();
            apply_json["status"] = apply.getStatus();
            apply_json["reject_reason"] = apply.getRejectReason();
            apply_json["handle_time"] = apply.getHandleTime();
            apply_json["order_for_user_name"] = apply.getOrderForUserName();
            apply_json["canteen_name"] = apply.getCanteenName();
            apply_json["total_price"] = apply.getTotalPrice();
            apply_json["order_time"] = apply.getOrderTime();
            applies_json.push_back(apply_json);
        }

        res.set_content(Response::success(applies_json), "application/json");
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleGetCancelApplies] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

void Controller::handleHandleCancelApply(const httplib::Request& req, httplib::Response& res) {
    try {
        json body = json::parse(req.body);
        int cancel_id = getIntSafe(body, "cancel_id");
        int status = getIntSafe(body, "status");
        std::string reject_reason = body.value("reject_reason", "");

        OrderCancelService service;
        if (service.handleCancelApply(cancel_id, status, reject_reason)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "处理取消申请失败"), "application/json");
        }
    } catch (const std::exception& e) {
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleGetCancelApplyByOrderId(const httplib::Request& req, httplib::Response& res) {
    try {
        int order_id = std::stoi(req.get_param_value("order_id"));
        std::cout << "order_id=" << order_id << std::endl;
        
        OrderCancelService service;
        auto cancel_apply = service.getCancelApplyByOrderId(order_id);
        
        if (cancel_apply) {
            json data = {
                {"cancel_id", cancel_apply->getCancelId()},
                {"order_id", cancel_apply->getOrderId()},
                {"cancel_reason", cancel_apply->getCancelReason()},
                {"cancel_time", cancel_apply->getCancelTime()},
                {"status", cancel_apply->getStatus()},
                {"reject_reason", cancel_apply->getRejectReason()}
            };
            res.set_content(Response::success(data), "application/json");
            std::cout << "申请数据成功" << std::endl;
        } else {
            res.set_content(Response::success(json::array()), "application/json");
        }
    } catch (const std::exception& e) {
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

void Controller::handleRating(const httplib::Request& req, httplib::Response& res) {
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

    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleRating] Error: " << e.what() << std::endl;
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

        if (service.addReport(report)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "举报失败"), "application/json");
        }

    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleReport] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleUserCenter(const httplib::Request& req, httplib::Response& res)
{
    try {
        int user_id = std::stoi(req.get_param_value("user_id"));
        std::cout << "个人中心请求参数：user_id=" << user_id << std::endl;

        UserService service;
        auto user = service.getUserCenterByUserId(user_id);

        if (user) {
            json data = {
                {"user_id", user->getUserId()},
                {"username", user->getUsername()},
                {"age", user->getAge()},
                {"phone", user->getPhone()},
                {"idCard", user->getIdCard()},
                {"role", user->getRole()},
                {"regionId", user->getRegionId()},
                {"regionName", user->getRegionName()},
                {"familyId", user->getFamilyId()},
                {"familyName", user->getFamilyName()},
                {"diseaseHistory", user->getDiseaseHistory()},
                {"tastePreference", user->getTastePreference()},
                {"canteenId", user->getCanteenId()},
                {"canteenName", user->getCanteenName()},
                {"adminLevel", user->getAdminLevel()},
                {"adminRegion", user->getAdminRegion()}
            };

            res.status = 200;
            res.set_content(Response::success(data), "application/json");
            std::cout << "用户 " << user->getUsername() << " 个人中心数据获取成功" << std::endl;
        } else {
            res.status = 404;
            res.set_content(Response::error(404, "用户未找到"), "application/json");
        }

    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleUserCenter] Error: " << e.what() << std::endl;
        res.status = 500;
        res.set_content(Response::error(500, "服务器错误"), "application/json");
    }
}

void Controller::handleUserCenterUpdate(const httplib::Request& req, httplib::Response& res)
{
    try{
        json body = json::parse(req.body);
        std::cout << "个人中心更新请求体：" << req.body << std::endl;

        UserCenterVO user;
        user.setUserId(getIntSafe(body, "user_id"));
        user.setRole(getStringSafe(body, "role"));
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
        if (service.updateUserCenter(user)) {
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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleFamilyList] Error: " << e.what() << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleRegionList] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");

    }
}

void Controller::handleDistrictRegionList(const httplib::Request& req, httplib::Response& res)
{
    try{
        std::cout << "区级区域列表请求" << std::endl;

        RegionService service;
        auto regionList = service.getDistrictRegionList();

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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleDistrictRegionList] Error: " << e.what() << std::endl;
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

    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleCreateFamily] Error: " << e.what() << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleAdminList] Error: " << e.what() << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleDinerList] Error: " << e.what() << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleAdminApplyList] Error: " << e.what() << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleAdminApplyReview] Error: " << e.what() << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleManagerApplyList] Error: " << e.what() << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleManagerApplyReview] Error: " << e.what() << std::endl;
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

    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleUpdateStatus] Error: " << e.what() << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleAnnouncementList] Error: " << e.what() << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleAnnouncementPublish] Error: " << e.what() << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleAnnouncementDelete] Error: " << e.what() << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleReportList] Error: " << e.what() << std::endl;
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
        if (service.updateReportStatus(report_id, status, handler_id)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "投诉处理失败"), "application/json");
        }
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleReportHandle] Error: " << e.what() << std::endl;
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
        std::cout<< " user_id 是 " << user_id<<std::endl;
            
        CanteenService canteenService;
        // 获取食堂 ID
        int canteen_id = canteenService.getCanteenIdByUserId(user_id);
        if (canteen_id <= 0) {
            res.set_content(Response::error(404, "未找到食堂信息"), "application/json");
            return;
        }
        std::cout<< " canteen_id 是 " << canteen_id<<std::endl;

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
        auto reports = reportService.getReportsByCanteen(canteen_id);
        int complaint_count = reports.size();

        // 构建响应数据
        json data = {
            {"id", canteen->getId()},
            {"name", canteen->getName()},
            {"address", canteen->getAddress()},
            {"regionId", canteen->getRegionId()},
            {"regionName", canteen->getRegionName()},
            {"rating", average_rating},
            {"complaintCount", complaint_count}
        };

        std::cout<< " data 是 " << data.dump(2)<<std::endl;
        res.set_content(Response::success(data), "application/json");
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleMyCanteen] Error: " << e.what() << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleUpdateCanteenAddress] Error: " << e.what() << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleCanteenList] Error: " << e.what() << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleUpdateCanteenStatus] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

// 获取采购记录
void Controller::handlePurchaseList(const httplib::Request& req, httplib::Response& res)
{
    try {
        int canteen_id = std::stoi(req.get_param_value("canteen_id"));
        if (canteen_id <= 0) {
            res.set_content(Response::error(400, "参数错误"), "application/json");
            return;
        }

        CanteenService canteenService;
        auto bills = canteenService.getPurchaseBills(canteen_id);

        json data = json::array();
        for (const auto& bill : bills) {
            json billJson = {
                {"bill_id", bill.getId()},
                {"canteen_id", bill.getCanteenId()},
                {"amount", bill.getAmount()},
                {"purchase_date", bill.getPurchaseDate()},
                {"remark", bill.getRemark()}
            };
            data.push_back(billJson);
        }

        res.set_content(Response::success(data), "application/json");
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handlePurchaseList] Error: " << e.what() << std::endl;
        res.set_content(Response::error(500, "获取采购记录失败"), "application/json");
    }
}

// 创建采购记录
void Controller::handleCreatePurchase(const httplib::Request& req, httplib::Response& res)
{
    try {
        std::cout<< " json内容是 " << req.body<<std::endl;
        
        json body = json::parse(req.body);
        PurchaseBill bill;
        bill.setCanteenId(getIntSafe(body, "canteen_id"));
        bill.setAmount(body["amount"].get<double>());
        bill.setPurchaseDate(getStringSafe(body, "purchase_date"));
        bill.setRemark(getStringSafe(body, "remark"));

        if (bill.getCanteenId() <= 0 || bill.getAmount() <= 0 || bill.getPurchaseDate().empty() || bill.getRemark().empty()) {
            res.set_content(Response::error(400, "参数错误"), "application/json");
            return;
        }

        CanteenService canteenService;
        int bill_id = canteenService.createPurchaseBill(bill);
        if (bill_id > 0) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "创建采购记录失败"), "application/json");
        }
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleCreatePurchase] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleUpdatePurchase(const httplib::Request& req, httplib::Response& res)
{
    try {
        std::cout<< " json内容是 " << req.body<<std::endl;
        
        json body = json::parse(req.body);
        PurchaseBill bill;
        bill.setId(getIntSafe(body, "bill_id"));
        bill.setAmount(body["amount"].get<double>());
        bill.setPurchaseDate(getStringSafe(body, "purchase_date"));
        bill.setRemark(getStringSafe(body, "remark"));

        if (bill.getId() <= 0 || bill.getAmount() <= 0 || bill.getPurchaseDate().empty()) {
            res.set_content(Response::error(400, "参数错误"), "application/json");
            return;
        }

        CanteenService canteenService;
        bool success = canteenService.updatePurchaseBill(bill);
        if (success) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "更新采购记录失败"), "application/json");
        }
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleUpdatePurchase] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleDeletePurchase(const httplib::Request& req, httplib::Response& res)
{
    try {
        std::cout<< " json内容是 " << req.body<<std::endl;
        
        json body = json::parse(req.body);
        int bill_id = getIntSafe(body, "bill_id");

        if (bill_id <= 0) {
            res.set_content(Response::error(400, "参数错误"), "application/json");
            return;
        }

        CanteenService canteenService;
        bool success = canteenService.deletePurchaseBill(bill_id);
        if (success) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "删除采购记录失败"), "application/json");
        }
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleDeletePurchase] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

// 财务统计
void Controller::handleFinancialStatistics(const httplib::Request& req, httplib::Response& res) {
    try {
        int user_id = std::stoi(req.get_param_value("user_id"));
        std::string stats_type = req.get_param_value("stats_type");
        std::string time_dimension = req.get_param_value("time_dimension");
        
        if (stats_type.empty()) {
            stats_type = "profit";
        }
        if (time_dimension.empty()) {
            time_dimension = "day";
        }
        
        CanteenService service;
        int canteen_id = service.getCanteenIdByUserId(user_id);
        
        if (canteen_id <= 0) {
            res.status = 404;
            res.set_content(Response::error(404, "食堂未找到"), "application/json");
            return;
        }
        
        // 获取今日财务数据
        auto today_data = service.getTodayFinancialData(canteen_id);
        
        // 获取统计数据
        auto stats_data = service.getFinancialData(canteen_id, time_dimension, stats_type);
        
        // 构建返回数据
        json data;
        data["today_income"] = today_data.getIncome();
        data["today_expense"] = today_data.getExpense();
        data["today_profit"] = today_data.getProfit();
        data["labels"] = stats_data.getLabels();
        data["values"] = stats_data.getValues();
        
        res.status = 200;
        res.set_content(Response::success(data), "application/json");
        
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleFinancialStatistics] Error: " << e.what() << std::endl;
        res.status = 500;
        res.set_content(Response::error(500, "服务器错误"), "application/json");
    }
}

// 获取消息列表
void Controller::handleGetMessages(const httplib::Request& req, httplib::Response& res) {
    try {
        int user_id = std::stoi(req.get_param_value("user_id"));
        std::cout<<"user_id is "<<user_id<<std::endl;
        MessageCenterService service;
        auto messages = service.getMessagesByReceiver(user_id);
        
        json arr = json::array();
        for (const auto& message : messages) {
            json message_json = {
                {"message_id", message.getMessageId()},
                {"sender_id", message.getSenderId()},
                {"receiver_id", message.getReceiverId()},
                {"content", message.getContent()},
                {"status", message.getStatus()},
                {"create_time", message.getCreateTime()}
            };
            arr.push_back(message_json);
        }
        
        res.set_content(Response::success(arr), "application/json");
        
    } catch (const std::exception& e) {
        std::cerr << "获取消息失败: " << e.what() << std::endl;
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

// 标记消息为已读
void Controller::handleMarkMessageAsRead(const httplib::Request& req, httplib::Response& res) {
    try {
        json body = json::parse(req.body);
        int message_id = getIntSafe(body, "message_id");
        
        MessageCenterService service;
        bool success = service.updateMessageStatus(message_id, 1);
        
        if (success) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "标记消息为已读失败"), "application/json");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "标记消息为已读失败: " << e.what() << std::endl;
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

// 处理用餐偏好请求
void Controller::handleDiningPreference(const httplib::Request& req, httplib::Response& res) {
    try {
        int user_id = std::stoi(req.get_param_value("user_id"));
        std::string time_dimension = req.get_param_value("time_dimension");
        
        if (time_dimension.empty()) {
            time_dimension = "month"; // 默认显示最近一月
        }
        
        OrderService service;
        auto preference = service.getDiningPreference(user_id, time_dimension);
        
        // 构建返回数据
        json data;
        data["total_amount"] = preference.getSummary().getTotalAmount();
        data["order_count"] = preference.getSummary().getOrderCount();
        data["canteen_count"] = preference.getSummary().getCanteenCount();
        
        json canteen_consumption = json::array();
        for (const auto& item : preference.getCanteenConsumption()) {
            json item_data;
            item_data["name"] = item.getName();
            item_data["count"] = item.getCount();
            canteen_consumption.push_back(item_data);
        }
        data["canteen_consumption"] = canteen_consumption;
        
        json dish_consumption = json::array();
        for (const auto& item : preference.getDishConsumption()) {
            json item_data;
            item_data["name"] = item.getName();
            item_data["count"] = item.getCount();
            dish_consumption.push_back(item_data);
        }
        data["dish_consumption"] = dish_consumption;
        
        res.status = 200;
        res.set_content(Response::success(data), "application/json");
        
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleGetMessages] Error: " << e.what() << std::endl;
        res.status = 500;
        res.set_content(Response::error(500, "服务器错误"), "application/json");
    }
}

// 处理食堂订单请求
void Controller::handleCanteenOrders(const httplib::Request& req, httplib::Response& res) {
    try {
        int canteen_id = std::stoi(req.get_param_value("canteen_id"));
        
        OrderService service;
        auto orders = service.getOrdersByCanteen(canteen_id);
        
        json orders_json = json::array();
        for (const auto& order : orders) {
            json order_json;
            order_json["order_id"] = order.getOrderId();
            order_json["canteen_id"] = order.getCanteenId();
            order_json["canteen_name"] = order.getCanteenName();
            order_json["order_for_user_name"] = order.getOrderForUserName();
            order_json["order_time"] = order.getCreateTime();
            order_json["total_price"] = order.getTotalPrice();
            order_json["discount_rate"] = order.getDiscountRate();
            order_json["original_total"] = order.getOriginalTotal();
            order_json["saved_amount"] = order.getSavedAmount();
            order_json["status"] = order.getStatus();
            order_json["has_rating"] = order.getHasRating();
            if (order.getHasRating()) {
                order_json["rating_score"] = order.getRatingScore();
                order_json["rating_comment"] = order.getRatingComment();
                order_json["rating_time"] = order.getRatingTime();
            }
            orders_json.push_back(order_json);
        }
        
        res.set_content(Response::success(orders_json), "application/json");
        
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleCanteenOrders] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

// 处理更新订单状态请求
void Controller::handleUpdateOrderStatus(const httplib::Request& req, httplib::Response& res) {
    try {
        json body = json::parse(req.body);
        int order_id = getIntSafe(body, "order_id");
        int status = getIntSafe(body, "status");
        
        OrderService service;
        
        if (service.updateOrderStatus(order_id, status)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "更新订单状态失败"), "application/json");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "更新订单状态失败: " << e.what() << std::endl;
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

// ================================
// 留言板相关
// ================================
void Controller::handleCreateMessage(const httplib::Request& req, httplib::Response& res) {
    try {
        json body = json::parse(req.body);
        Message message;
        message.setCanteenId(getIntSafe(body, "canteen_id"));
        message.setUserId(getIntSafe(body, "user_id"));
        message.setContent(getStringSafe(body, "content"));

        MessageService service;
        bool success = service.createMessage(message);

        if (success) {
            res.set_content(Response::success({}), "application/json");
        } else {
            res.set_content(Response::error(400, "创建留言失败"), "application/json");
        }
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleCreateMessage] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

void Controller::handleGetUserMessages(const httplib::Request& req, httplib::Response& res) {
    try {
        int user_id = std::stoi(req.get_param_value("user_id"));
        int canteen_id = std::stoi(req.get_param_value("canteen_id"));

        MessageService service;
        auto messages = service.getMessagesByUser(user_id, canteen_id);

        json arr = json::array();
        for (const auto& msg : messages) {
            arr.push_back({
                {"id", msg.getId()},
                {"canteen_id", msg.getCanteenId()},
                {"create_time", msg.getCreateTime()},
                {"reply_time", msg.getReplyTime()},
                {"user_id", msg.getUserId()},
                {"content", msg.getContent()},
                {"reply_content", msg.getReplyContent()},
                {"status", msg.getStatus()}
            });
        }

        res.set_content(Response::success(arr), "application/json");
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleGetUserMessages] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

void Controller::handleGetCanteenMessages(const httplib::Request& req, httplib::Response& res) {
    try {
        int canteen_id = std::stoi(req.get_param_value("canteen_id"));
        std::cout << "获取食堂ID为" << canteen_id << "的留言" << std::endl;
        MessageService service;
        auto messages = service.getMessagesByCanteen(canteen_id);

        json arr = json::array();
        for (const auto& msg : messages) {
            arr.push_back({
                {"id", msg.getId()},
                {"canteen_id", msg.getCanteenId()},
                {"create_time", msg.getCreateTime()},
                {"reply_time", msg.getReplyTime()},
                {"user_id", msg.getUserId()},
                {"content", msg.getContent()},
                {"reply_content", msg.getReplyContent()},
                {"status", msg.getStatus()}
            });
        }
        std::cout << "获取到 " << messages.size() << " 条留言" << std::endl;
        res.set_content(Response::success(arr), "application/json");
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleGetCanteenMessages] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

void Controller::handleReplyMessage(const httplib::Request& req, httplib::Response& res) {
    try {
        json body = json::parse(req.body);
        Message message;
        message.setId(getIntSafe(body, "id"));
        message.setReplyContent(getStringSafe(body, "reply_content"));



        MessageService service;
        bool success = service.replyMessage(message);

        if (success) {
            res.set_content(Response::success({}), "application/json");
        } else {
            res.set_content(Response::error(400, "回复留言失败"), "application/json");
        }
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleReplyMessage] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "参数错误"), "application/json");
    }
}

// 处理修改密码请求
void Controller::handleChangePassword(const httplib::Request& req, httplib::Response& res) {
    try {
        json body = json::parse(req.body);
        int user_id = getIntSafe(body, "user_id");
        std::string old_password = getStringSafe(body, "old_password");
        std::string new_password = getStringSafe(body, "new_password");

        std::cout << "修改密码请求参数：user_id=" << user_id << std::endl;

        if (user_id <= 0 || old_password.empty() || new_password.empty()) {
            res.status = 400;
            res.set_content(Response::error(400, "参数不完整"), "application/json");
            return;
        }

        UserService service;
        if (service.changePassword(user_id, old_password, new_password)) {
            res.set_content(Response::success(), "application/json");
            std::cout << "用户 " << user_id << " 密码修改成功" << std::endl;
        } else {
            res.set_content(Response::error(400, "旧密码错误或新密码长度不足"), "application/json");
            std::cout << "用户 " << user_id << " 密码修改失败" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleChangePassword] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}

void Controller::handleGetAllTags(const httplib::Request& req, httplib::Response& res) {
    try {
        TagService service;
        auto tags = service.getAllTags();

        json arr = json::array();
        for (const auto& tag : tags) {
            arr.push_back({
                {"tag_id", tag.getId()},
                {"tag_name", tag.getName()}
            });
        }

        res.set_content(Response::success(arr), "application/json");
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleGetAllTags] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "获取标签失败"), "application/json");
    }
}

void Controller::handleGetDishTags(const httplib::Request& req, httplib::Response& res) {
    try {
        int dish_id = std::stoi(req.get_param_value("dish_id"));
        std::cout << "获取菜品标签参数：dish_id=" << dish_id << std::endl;

        TagService tagService;
        DishTagService dishTagService;

        auto tags = tagService.getTagsByDishId(dish_id);
        auto tag_ids = dishTagService.getTagIdsByDishId(dish_id);

        json arr = json::array();
        for (const auto& tag : tags) {
            arr.push_back({
                {"tag_id", tag.getId()},
                {"tag_name", tag.getName()}
            });
        }

        res.set_content(Response::success({
            {"tags", arr},
            {"tag_ids", tag_ids}
        }), "application/json");
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleGetDishTags] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "获取菜品标签失败"), "application/json");
    }
}

void Controller::handleUpdateDish(const httplib::Request& req, httplib::Response& res) {
    try {
        json body = json::parse(req.body);
        int dish_id = getIntSafe(body, "dish_id");
        double price = body["price"].get<double>();
        int calories = getIntSafe(body, "calories");
        std::string nutrition_info = getStringSafe(body, "nutrition_info");
        std::vector<int> tag_ids;

        if (body.contains("tag_ids") && body["tag_ids"].is_array()) {
            for (const auto& id : body["tag_ids"]) {
                tag_ids.push_back(id.get<int>());
            }
        }

        std::cout << "更新菜品参数：dish_id=" << dish_id << ", price=" << price
                  << ", calories=" << calories << ", nutrition_info=" << nutrition_info << ", tag_ids数量=" << tag_ids.size() << std::endl;

        DishService dishService;

        if (dishService.updateDish(dish_id, price, calories, nutrition_info, tag_ids)) {
            res.set_content(Response::success(), "application/json");
        } else {
            res.set_content(Response::error(500, "更新菜品失败"), "application/json");
        }
    } catch (const std::exception& e) {
        std::cerr << "[Controller::handleUpdateDish] Error: " << e.what() << std::endl;
        res.set_content(Response::error(400, "JSON格式错误"), "application/json");
    }
}
