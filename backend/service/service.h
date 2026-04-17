#pragma once
#include <memory>
#include <vector>
#include <mutex>
#include <unordered_map>
#include "../dao/dao.h"

// ================================
// 用户服务
// ================================
class UserService {
public:
    bool registerUser(const User& user, int role=1, int region_id=0);
    bool isUsernameTaken(const std::string& username);
    std::shared_ptr<User> login(std::string& username, std::string& password);
    std::shared_ptr<DinerCenterVO> getDinerCenterByUserId(int user_id);
    bool updateDinerCenter(const DinerCenterVO& diner);
    std::string getUserRole(int user_id);
    bool updateStatus(const User& user);
};

// ================================
// 管理员服务
// ================================
class AdminService{
public:
    bool submitAdminApply(const User& user, int level_id, int region_id);
    std::vector<AdminInformation> getAdminList();
    std::vector<AdminApplyVO> getAdminApplyList();
    bool reviewAdminApply(int apply_id, int reviewer_id, int status);
};

// ================================
// 食堂管理者服务
// ================================
class ManagerService {
public:
    bool submitManagerApply(const User& user, const std::string& canteen_name, int region_id);
    std::vector<CanteenManagerApplyVO> getManagerApplyList();
    bool reviewManagerApply(int apply_id, int reviewer_id, int status);
};


// ================================
// 用餐者服务
// ================================
class DinerService{
public:
    std::vector<DinerInformation> getDinerList();
};

// ================================
// 区域服务
// ================================
class RegionService {
public:
    std::vector<Region> getRegionList();
};


// ================================
// 家庭服务
// ================================
class FamilyService {
public:
    Family getFamilyByUserId(int user_id);
    std::vector<Family> getFamilyList();
    bool createFamily(int user_id, const std::string& family_name);
};

// ================================
// 食堂服务
// ================================
class CanteenService {
public:
    std::vector<Canteen> getAllCanteens();
    std::shared_ptr<Canteen> getCanteenById(int id);
    int getCanteenIdByUserId(int user_id);
    bool updateCanteenAddress(int canteen_id, const std::string& address);
    bool updateCanteenStatus(int canteen_id, int status);
    std::shared_ptr<Canteen> getCanteenDetails(int canteen_id);
    std::vector<CanteenManagerVO> getCanteensWithManagers();
};

// ================================
// 菜单服务
// ================================
class MenuService {
public:
    std::vector<Dish> getTodayMenu(int canteen_id, const std::string& date);
    std::vector<CanteenMenuVO> getMenuByCanteen(int canteen_id);
    bool insertMenu(const MenuCreateDTO& menu);
    bool eraseMenu(const int menu_id);
};
// ================================
// 菜品服务
// ================================
class DishService{
public:
    std::vector<Dish> getDishsByCanteen(int canteen_id);
    bool insertDish(const Dish& dish );
    bool disableDishByDishId(const int dish_id);
    bool enableDishByDishId(const int dish_id);
};

// ================================
// 订单服务（🔥重点）
// ================================
class OrderService {
public:
    bool placeOrder(int user_id,
                    int canteen_id,
                    int order_for_user_id,
                    const std::vector<OrderItem>& items);
    std::vector<FamilyMemberVO> getOrderTargetsByUser(int user_id);
    std::vector<OrderVO> getOrdersByUser(int user_id);
    std::vector<OrderDetailVO> getOrdersDetailsByUser(int user_id,int order_id);

private:
    static std::mutex userOrderMapMutex;
    static std::unordered_map<int, std::shared_ptr<std::mutex>> userOrderMutexes;
    static std::shared_ptr<std::mutex> getUserOrderMutex(int user_id);
};

// ================================
// 评价服务
// ================================
class RatingService {
public:
    bool submitRating(const Rating& rating);
    std::vector<Rating> getRatings(int canteen_id);
    std::vector<CanteenRatingVO> getCanteenRatingDetails(int canteen_id);
};

// ================================
// 举报服务
// ================================
class ReportService {
public:
    bool submitReport(const Report& report);
    std::vector<Report> getReports(int canteen_id);
    std::vector<ReportVO> getAllReports();
    bool handleReport(int report_id, int status, int handler_id);
};

// ================================
// 公告服务
// ================================
class AnnouncementService {
public:
    bool publishAnnouncement(const Announcement& announcement);
    std::vector<AnnouncementVO> getAnnouncementList();
    bool deleteAnnouncement(int announce_id, int publisher_id);
};
