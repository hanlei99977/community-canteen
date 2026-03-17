#pragma once
#include <memory>
#include <vector>
#include <mutex>
#include "../model/model.h"
#include "../dao/dao.h"

// ================================
// 用户服务
// ================================
class UserService {
public:
    bool registerUser(const User& user);
    std::shared_ptr<User> login(const std::string& username, const std::string& password);
};

// ================================
// 食堂服务
// ================================
class CanteenService {
public:
    std::vector<Canteen> getAllCanteens();
    std::shared_ptr<Canteen> getCanteenById(int id);
};

// ================================
// 菜单服务
// ================================
class MenuService {
public:
    std::vector<Dish> getTodayMenu(int canteen_id, const std::string& date);
};

// ================================
// 订单服务（🔥重点）
// ================================
class OrderService {
public:
    bool placeOrder(int user_id,
                    int canteen_id,
                    const std::vector<OrderItem>& items);

private:
    std::mutex orderMutex; // ⭐ 防止并发重复下单
};

// ================================
// 评价服务
// ================================
class RatingService {
public:
    bool submitRating(const Rating& rating);
    std::vector<Rating> getRatings(int canteen_id);
};

// ================================
// 举报服务
// ================================
class ReportService {
public:
    bool submitReport(const Report& report);
    std::vector<Report> getReports(int canteen_id);
};