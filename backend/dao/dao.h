#pragma once
#include <memory>
#include <vector>
#include "../model/model.h"

// ================================
// 用户
// ================================
class UserDAO {
public:
    bool insertUser(const User& user);
    std::shared_ptr<User> getUserByUsernameAndPassword(const std::string&, const std::string&);
    std::shared_ptr<User> getUserById(int user_id);
};

// ================================
// 管理员
// ================================
class AdminDAO {
public:
    bool insertAdmin(const Admin& admin);
    std::shared_ptr<Admin> getAdminByUserId(int user_id);
};

// ================================
// 用餐者
// ================================
class DinerDAO {
public:
    bool insertDiner(const Diner& diner);
    std::shared_ptr<Diner> getDinerByUserId(int user_id);
};

// ================================
// 食堂
// ================================
class CanteenDAO {
public:
    std::vector<Canteen> getAllCanteens();
    std::shared_ptr<Canteen> getCanteenById(int id);
};

// ================================
// 菜品
// ================================
class DishDAO {
public:
    std::vector<Dish> getDishesByCanteen(int canteen_id);
};

// ================================
// 餐单
// ================================
class MenuDAO {
public:
    std::vector<Dish> getMenuByDate(int canteen_id, const std::string& date);
};

// ================================
// 订单
// ================================
class OrderDAO {
public:
    bool createOrder(const Order& order, const std::vector<OrderItem>& items);
    std::vector<Order> getOrdersByUser(int user_id);
};

// ================================
// 评价
// ================================
class RatingDAO {
public:
    bool insertRating(const Rating& rating);
    std::vector<Rating> getRatingsByCanteen(int canteen_id);
};

// ================================
// 举报
// ================================
class ReportDAO {
public:
    bool insertReport(const Report& report);
    std::vector<Report> getReportsByCanteen(int canteen_id);
};