#include "service.h"
#include <iostream>
#include <map>
#include "../../MySQL/ConnectionPool.h"

/**********************************************
 * UserService
 *********************************************/
bool UserService::registerUser(const User& user ,int role) {
    UserDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    bool res = true;

    try {
        conn->setAutoCommit(false); // 开启事务

        // 简单校验（可以扩展）
        if (user.getUsername().empty() || user.getPassword().empty()) {
            conn->rollback();
            return false;
        }

        int user_id = dao.insertUser(conn, user);
        if (user_id == -1) {
            conn->rollback();
            return false;
        }

        switch (role) {
            case 1: // diner
                DinerDAO dinerDAO;
                res = dinerDAO.insertDiner(conn, user_id);
                break;
            case 2: // admin
                AdminDAO adminDAO;
                res = adminDAO.insertAdmin(conn, user_id);
                break;
            case 3: // manager
                ManagerDAO managerDAO;
                res = managerDAO.insertManager(conn, user_id);
                break;
            default:
                conn->rollback();
                return false;
        }

        if (!res) {
            conn->rollback();
            return false;
        }
        conn->commit(); // 提交事务
        return true;
    }
    catch (...) {
        conn->rollback();
        return false;
    }
}

std::shared_ptr<User> UserService::login(
    std::string& username,
    std::string& password
) {
    UserDAO dao;
     // 去空格（很重要）
    username.erase(0, username.find_first_not_of(" "));
    username.erase(username.find_last_not_of(" ") + 1);

    password.erase(0, password.find_first_not_of(" "));
    password.erase(password.find_last_not_of(" ") + 1);

    auto user = dao.getUserByUsernameAndPassword(username, password);
    if (user && user->getStatus() == 1) {
        return user;
    }
    return nullptr;
}

/**********************************************
 * CanteenService
 *********************************************/
std::vector<Canteen> CanteenService::getAllCanteens() {
    CanteenDAO dao;
    return dao.getAllCanteens();
}

std::shared_ptr<Canteen> CanteenService::getCanteenById(int id) {
    CanteenDAO dao;
    return dao.getCanteenById(id);
}

/**********************************************
 * MenuService
 *********************************************/
std::vector<Dish> MenuService::getTodayMenu(int canteen_id, const std::string& date) {
    MenuDAO dao;
    return dao.getMenuByDate(canteen_id, date);
}

/**********************************************
 * OrderService
 *********************************************/
bool OrderService::placeOrder(int user_id,
                              int canteen_id,
                              const std::vector<OrderItem>& items) {

    DBConnectionGuard guard;
    auto* conn = guard.get();

    try{
        conn->setAutoCommit(false); // 开启事务
        // ⭐ 并发保护（简单版）
        std::lock_guard<std::mutex> lock(orderMutex);
        
        if (items.empty()) return false;

        DishDAO dishDAO;
        OrderDAO orderDAO;

        double total = 0.0;

        auto dishes = dishDAO.getDishesByCanteen(canteen_id);// 获取食堂菜品（可以优化成批量查询）
        // 1️⃣ 计算总价（业务逻辑）
        for (const auto& item : items) {
            for (const auto& d : dishes) {
                if (d.getId() == item.getDishId()) {
                    total += d.getPrice() * item.getQuantity();
                }
            }
        }

        // 2️⃣ 构造订单
        Order order;
        order.setUserId(user_id);
        order.setOrderForUserId(user_id);
        order.setCanteenId(canteen_id);
        order.setTotalPrice(total);
        order.setStatus("pending");

        // 3️⃣ 创建订单（事务）
        int order_id = orderDAO.insertOrder(conn, order, items);
        if (order_id == -1) {
            conn->rollback();
            return false;
        }

        OrderItemDAO orderItemDAO;
        if (!orderItemDAO.insertOrderItems(conn, order_id, items)) {
            conn->rollback();
            return false;
        }

        conn->commit(); // 提交事务
        return true;
        
    }catch (...) {
            conn->rollback();
            return false;
        }
}

std::vector<OrderVO> OrderService::getOrdersByUser(int user_id)
{
    OrderDAO dao;
    return dao.getOrdersByUser(user_id);
}

std::vector<OrderDetailVO> OrderService::getOrdersDetailsByUser(int user_id,int order_id) {
    OrderDAO dao;
    return dao.getOrdersDetailsByUser(user_id,order_id);
}

/**********************************************
 * RatingService
 *********************************************/
bool RatingService::submitRating(const Rating& rating) {
    RatingDAO dao;

    // 校验评分
    if (rating.getScore() < 1 || rating.getScore() > 5) {
        return false;
    }

    return dao.insertRating(rating);
}

std::vector<Rating> RatingService::getRatings(int canteen_id) {
    RatingDAO dao;
    return dao.getRatingsByCanteen(canteen_id);
}

/**********************************************
 * ReportService
 *********************************************/
bool ReportService::submitReport(const Report& report) {
    ReportDAO dao;

    if (report.getContent().empty()) return false;

    return dao.insertReport(report);
}

std::vector<Report> ReportService::getReports(int canteen_id) {
    ReportDAO dao;
    return dao.getReportsByCanteen(canteen_id);
}