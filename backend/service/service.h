#pragma once
#include <memory>
#include <vector>
#include <mutex>
#include <unordered_map>
#include "../dao/dao.h"

/*******************************用户权限模块*******************************/
// ================================
// 用户服务
// ================================
class UserService {
public:
    // 注册用户
    bool registerUser(const User& user, int role=1, int region_id=0);
    bool isUsernameTaken(const std::string& username);// 检查用户名是否已被注册
    // 登录用户（基础）
    std::shared_ptr<User> login(std::string& username, std::string& password);
    // 完整登录（包含角色、食堂ID、管理员级别和区域信息）
    std::shared_ptr<LoginResultVO> completeLogin(std::string& username, std::string& password);
    // 用户信息获取及更新
    std::shared_ptr<UserCenterVO> getUserCenterByUserId(int user_id);
    bool updateUserCenter(const UserCenterVO& diner);
    bool validateDistrictRegion(int region_id);// 区域校验
    bool changePassword(int user_id, const std::string& old_password, const std::string& new_password);
    // 用户状态更新
    std::string getUserRole(int user_id);
    bool updateStatus(const User& user);
    
};

// ================================
// 管理员服务
// ================================
class AdminService{
public:
    bool submitAdminApply(const User& user, int level_id, int region_id);
    // 根据查看者ID获取可管理的管理员列表
    std::vector<AdminInformation> getAdminList(int viewer_id, int city_id = 0, int district_id = 0);
    // 根据审核者ID获取可审核的管理员申请列表
    std::vector<AdminApplyVO> getAdminApplyList(int reviewer_id, int city_id = 0, int district_id = 0);
    // 管理员审核申请（带权限验证）
    bool reviewAdminApply(int apply_id, int reviewer_id, int status);
    // 获取管理员信息（级别和区域）
    std::shared_ptr<Admin> getAdminInfo(int user_id);
    // 检查管理员是否有权限审核管理员申请
    bool hasAdminApplyReviewPermission(int reviewer_id, int apply_region_id, int apply_level_id);
    // 检查管理员是否有权限审核食堂管理者申请
    bool hasManagerApplyReviewPermission(int reviewer_id, int apply_region_id);
};

// ================================
// 食堂管理者服务
// ================================
class ManagerService {
public:
    bool submitManagerApply(const User& user, const std::string& canteen_name, int region_id);
    bool validateDistrictRegion(int region_id);
    // 根据审核者ID获取可审核的食堂管理者申请列表
    std::vector<CanteenManagerApplyVO> getManagerApplyList(int reviewer_id, int city_id = 0, int district_id = 0);
    bool reviewManagerApply(int apply_id, int reviewer_id, int status);
};


// ================================
// 用餐者服务
// ================================
class DinerService{
public:
    // 根据查看者ID获取可管理的用餐者列表
    std::vector<DinerInformation> getDinerList(int viewer_id, int city_id = 0, int district_id = 0);
};

// ================================
// 区域服务
// ================================
class RegionService {
public:
    std::vector<Region> getRegionList();
    std::vector<Region> getDistrictRegionList();
    std::vector<Region> getCityRegionList();
    std::vector<Region> getDistrictsByCity(int city_id);
    // 检查region_id是否在parent_region_id的管辖范围内（包含直接和间接子区域）
    bool isRegionInScope(sql::Connection *conn, int region_id, int parent_region_id);
    bool isRegionInScope(int region_id, int parent_region_id);
};

/*******************************家庭模块*******************************/
// ================================
// 家庭服务
// ================================
class FamilyService {
public:
    Family getFamilyByUserId(int user_id);
    std::vector<Family> getFamilyList();
    bool createFamily(int user_id, const std::string& family_name);
};

/*******************************食堂模块*******************************/
// ================================
// 食堂服务
// ================================
class CanteenService {
public:
    // 获取所有食堂
    std::vector<Canteen> getAllCanteens();
    std::shared_ptr<CanteenVO> getCanteenById(int id);
    int getCanteenIdByUserId(int user_id);
    // 更新食堂
    bool updateCanteenAddress(int canteen_id, const std::string& address);
    bool updateCanteenStatus(int canteen_id, int viewer_id, int status);
    // 检查管理员是否有权限管理该食堂
    bool canManageCanteen(int viewer_id, int canteen_region_id);

    // 获取食堂详情
    std::shared_ptr<CanteenVO> getCanteenDetails(int canteen_id);
    std::vector<CanteenManagerVO> getCanteensWithManagers();
    // 根据查看者ID获取可管理的食堂列表
    std::vector<CanteenManagerVO> getCanteensWithManagers(int viewer_id, int city_id = 0, int district_id = 0);
    // 日常采购
    std::vector<PurchaseBill> getPurchaseBills(int canteen_id);
    int createPurchaseBill(const PurchaseBill& bill);
    bool updatePurchaseBill(const PurchaseBill& bill);
    bool deletePurchaseBill(int bill_id);
    // 财务统计
    TodayFinancialData getTodayFinancialData(int canteen_id);
    FinancialData getFinancialData(int canteen_id, const std::string& time_dimension, const std::string& stats_type);
};

/*******************************菜品&&菜单模块*******************************/
// ================================
// 菜单服务
// ================================
class MenuService {
public:
    std::vector<Dish> getMenuByMealType(int canteen_id, const std::string& meal_type);
    std::vector<CanteenMenuVO> getMenuByCanteen(int canteen_id);
    bool updateMenu(const MenuCreateDTO& menu);
    std::vector<HistoryMenu> getHistoryMenusByCanteen(int canteen_id);
    std::vector<Dish> getHistoryMenuDishes(int history_menu_id);
};
// ================================
// 菜品服务
// ================================
class DishService{
public:
    std::vector<Dish> getDishsByCanteen(int canteen_id);
    int insertDish(const Dish& dish, const std::vector<int>& tag_ids);
    bool updateDish(int dish_id, double price, int calories, const std::string& nutrition_info, const std::vector<int>& tag_ids);
    bool disableDishByDishId(const int dish_id);
    bool enableDishByDishId(const int dish_id);
};

/*******************************订单模块*******************************/
// ================================
// 订单服务（🔥重点）
// ================================
class OrderService {
public:
    bool placeOrder(int user_id,
                    int canteen_id,
                    int order_for_user_id,
                    const std::vector<OrderItem>& items);
    // 获取订单目标用户
    std::vector<FamilyMemberVO> getOrderTargetsByUser(int user_id);
    // 获取用户订单
    std::vector<OrderVO> getOrdersByUser(int user_id);
    std::vector<OrderDetailVO> getOrdersDetailsByOrderId(int order_id);
    // 用餐偏好相关方法
    DiningPreference getDiningPreference(int user_id, const std::string& time_dimension);
    // 获取最近订单
    std::shared_ptr<RecentOrderVO> getRecentOrder(int user_id, int order_for_user_id, int canteen_id);
    // 获取食堂订单
    std::vector<OrderVO> getOrdersByCanteen(int canteen_id);
    // 更新订单状态
    bool updateOrderStatus(int order_id, int status);

private:
    static std::mutex userOrderMapMutex;
    static std::unordered_map<int, std::shared_ptr<std::mutex>> userOrderMutexes;
    static std::shared_ptr<std::mutex> getUserOrderMutex(int user_id);
};

// ================================
// 订单取消服务
// ================================
class OrderCancelService {
public:
    bool createCancelApply(int order_id, const std::string& cancel_reason);
    std::vector<OrderCancelVO> getCancelAppliesByCanteen(int canteen_id);
    std::shared_ptr<OrderCancelVO> getCancelApplyByOrderId(int order_id);
    bool handleCancelApply(int cancel_id, int status, const std::string& reject_reason = "");
};

/*******************************评价&&投诉模块*******************************/
// ================================
// 评价服务
// ================================
class RatingService {
public:
    bool submitRating(const Rating& rating);
    std::vector<Rating> getRatings(int canteen_id);
    std::vector<CanteenRatingVO> getCanteenRatingDetails(int canteen_id);
    std::pair<std::vector<RatingVO>, int> getCanteenRatingsPaginated(int canteen_id, int page, int page_size);
};

// ================================
// 投诉服务
// ================================
class ReportService {
public:
    bool addReport(const Report& report);
    std::vector<Report> getReportsByCanteen(int canteen_id);
    std::vector<ReportVO> getAllReports();
    bool updateReportStatus(int report_id, int status, int handler_id);
    std::pair<std::vector<ReportVO>, int> getCanteenReportsPaginated(int canteen_id, int page, int page_size);
    int getUnprocessedReportCount(int viewer_id, int range_type);
    std::vector<std::pair<std::string, int>> getReportSummary(int viewer_id, int range_type, const std::string& time_period, int complaint_type);
};

/*******************************公告模块*******************************/
// ================================
// 公告服务
// ================================
class AnnouncementService {
public:
    bool publishAnnouncement(const Announcement& announcement);
    std::vector<AnnouncementVO> getAnnouncementList();
    bool deleteAnnouncement(int announce_id, int publisher_id);
};

/*******************************消息互动模块*******************************/
// ================================
// 留言板服务
// ================================
class MessageService {
public:
    bool createMessage(const Message& message);
    std::vector<Message> getMessagesByCanteen(int canteen_id);
    std::vector<Message> getMessagesByUser(int user_id, int canteen_id);
    bool replyMessage(const Message& message);
};

// ================================
// 消息中心服务
// ================================
class MessageCenterService {
public:
    int createMessage(const MessageNotification& message);
    std::vector<MessageNotification> getMessagesByReceiver(int receiver_id);
    bool updateMessageStatus(int message_id, int status);
};

/*******************************标签模块*******************************/
// ================================
// 标签服务
// ================================
class TagService {
public:
    std::vector<Tag> getAllTags();
    std::vector<Tag> getTagsByDishId(int dish_id);
};

// ================================
// 菜品标签服务
// ================================
class DishTagService {
public:
    bool updateDishTags(int dish_id, const std::vector<int>& tag_ids);
    bool updateDishTags(sql::Connection *conn, int dish_id, const std::vector<int>& tag_ids);
    std::vector<int> getTagIdsByDishId(int dish_id);
};
