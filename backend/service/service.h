#pragma once
#include <memory>
#include <vector>
#include <mutex>
#include "../model/model.h"
#include "../model/vo.h"
#include "../dao/dao.h"

// ================================
// 用户服务
// ================================
class UserService {
public:
    bool registerUser(const User& user, int role=1);
    std::shared_ptr<User> login(std::string& username, std::string& password);
    std::shared_ptr<DinerCenterVO> getDinerCenterByUserId(int user_id);
    bool updateDinerCenter(const DinerCenterVO& diner);
    std::string getUserRole(int user_id);
};

// ================================
// 家庭服务
// ================================
class FamilyService {
public:
    Family getFamilyByUserId(int user_id);
    std::vector<Family> getFamilyList();
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
    std::vector<OrderVO> getOrdersByUser(int user_id);
    std::vector<OrderDetailVO> getOrdersDetailsByUser(int user_id,int order_id);

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