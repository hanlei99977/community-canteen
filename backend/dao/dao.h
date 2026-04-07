#pragma once
#include <memory>
#include <vector>
#include "../model/model.h"
#include "../model/vo.h"
#include "../model/dto.h"
#include "../../MySQL/ConnectionPool.h"

// ================================
// 用户
// ================================
class UserDAO {
public:
    int insertUser(sql::Connection *conn, const User& user);
    std::shared_ptr<User> getUserByUsernameAndPassword(const std::string&, const std::string&);
    std::shared_ptr<User> getUserById(int user_id);
    bool updateUser(sql::Connection *conn, const DinerCenterVO& user);
    std::string getUserRole(int user_id);
};

// ================================
// 管理员
// ================================
class AdminDAO {
public:
    bool insertAdmin(sql::Connection *conn, int user_id);
    std::shared_ptr<Admin> getAdminByUserId(int user_id);
};

// ================================
// 用餐者
// ================================
class DinerDAO {
public:
    bool insertDiner(sql::Connection *conn, int user_id);
    std::shared_ptr<Diner> getDinerByUserId(int user_id);
    std::shared_ptr<DinerCenterVO> getDinerCenterByUserId(int user_id);
    bool updateDiner(sql::Connection *conn, const DinerCenterVO& diner);
};

// ================================
// 管理人
// ================================
class ManagerDAO {
public:
    bool insertManager(sql::Connection *conn, int user_id);
};

// ================================
// 家庭
// ================================
class FamilyDAO {
public:
    bool insertFamily(const Family& family);
    Family getFamilyByUserId(int user_id);
    std::vector<Family> getFamilyList();
};

// ================================
// 食堂
// ================================
class CanteenDAO {
public:
    std::vector<Canteen> getAllCanteens();
    std::shared_ptr<Canteen> getCanteenById(int id);
    int getCanteenIdByUserId(int user_id);
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
    std::vector<CanteenMenuVO> getMenuByCanteen(int canteen_id);
    bool insertMenu(const MenuCreateDTO& menu);
};

// ================================
// 订单
// ================================
class OrderDAO {
public:
    int insertOrder(sql::Connection *conn, const Order& order, const std::vector<OrderItem>& items);
    std::vector<OrderVO> getOrdersByUser(int user_id);
    std::vector<OrderDetailVO> getOrdersDetailsByUser(int user_id,int order_id);
};

// ================================
// 订单详情
// ================================
class OrderItemDAO {
public:
    bool insertOrderItems(sql::Connection *conn, int order_id, const std::vector<OrderItem>& items);
    std::vector<OrderItem> getOrderItemsByOrderId(int order_id);
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