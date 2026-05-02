#pragma once
#include <memory>
#include <vector>
#include "../model/model.h"
#include "../model/vo.h"
#include "../model/dto.h"
#include "../../MySQL/ConnectionPool.h"

/************************用户权限类DAO*******************************/
// ================================
// 区域
// ================================
class RegionDAO {
public:
    std::vector<Region> getRegionList(sql::Connection *conn);
    std::shared_ptr<Region> getRegionById(sql::Connection *conn, int region_id);
    std::vector<Region> getDistrictRegions(sql::Connection *conn);
    bool isDistrictLevel(sql::Connection *conn, int region_id);
};

// ================================
// 用户
// ================================
class UserDAO {
public:
    int insertUser(sql::Connection *conn, const User& user);
    bool existsByUsername(sql::Connection *conn, const std::string& username);
    std::shared_ptr<User> getUserByUsernameAndPassword(sql::Connection *conn, const std::string&, const std::string&);
    std::shared_ptr<User> getUserById(sql::Connection *conn, int user_id);
    bool updateUser(sql::Connection *conn, const UserCenterVO& user);
    std::string getUserRole(sql::Connection *conn, int user_id);
    bool updateStatus(sql::Connection *conn, int user_id, int status);
    bool updateStatus(sql::Connection *conn, const User& user);
    bool updatePassword(sql::Connection *conn, int user_id, const std::string& new_password);
};

// ================================
// 管理员
// ================================
class AdminDAO {
public:
    bool insertAdmin(sql::Connection *conn, int user_id);
    bool insertAdmin(sql::Connection *conn, int user_id, int level_id, int region_id);
    std::shared_ptr<Admin> getAdminByUserId(sql::Connection *conn, int user_id);
    std::vector<AdminInformation> getAdminList(sql::Connection *conn);
};

// ================================
// 管理员申请
// ================================
class AdminApplyDAO {
public:
    bool insertApply(sql::Connection *conn, int user_id, int level_id, int region_id);
    std::vector<AdminApplyVO> getApplyList(sql::Connection *conn);
    std::shared_ptr<AdminApplyVO> getApplyById(sql::Connection *conn, int apply_id);
    bool reviewApply(sql::Connection *conn, int apply_id, int reviewer_id, int status);
};

// ================================
// 用餐者
// ================================
class DinerDAO {
public:
    bool insertDiner(sql::Connection *conn, int user_id, int region_id);
    std::shared_ptr<Diner> getDinerByUserId(sql::Connection *conn, int user_id);
    std::shared_ptr<UserCenterVO> getDinerCenterByUserId(sql::Connection *conn, int user_id);
    std::vector<FamilyMemberVO> getFamilyMembersByUserId(sql::Connection *conn, int user_id);
    bool updateDiner(sql::Connection *conn, const UserCenterVO& diner);
    bool updateFamilyId(sql::Connection *conn, int user_id, int family_id);
    std::vector<DinerInformation> getDinerList(sql::Connection *conn);
};

// ================================
// 管理人
// ================================
class ManagerDAO {
public:
    bool insertManager(sql::Connection *conn, int user_id);
    bool insertManager(sql::Connection *conn, int user_id, int canteen_id);
};

// ================================
// 食堂管理者申请
// ================================
class CanteenManagerApplyDAO {
public:
    bool insertApply(sql::Connection *conn, int user_id, const std::string& canteen_name, int region_id);
    std::vector<CanteenManagerApplyVO> getApplyList(sql::Connection *conn);
    std::shared_ptr<CanteenManagerApplyVO> getApplyById(sql::Connection *conn, int apply_id);
    bool reviewApply(sql::Connection *conn, int apply_id, int reviewer_id, int status);
};

// ================================
// 家庭
// ================================
class FamilyDAO {
public:
    int insertFamily(sql::Connection *conn, const Family& family);
    Family getFamilyByUserId(sql::Connection *conn, int user_id);
    std::vector<Family> getFamilyList(sql::Connection *conn);
};

/************************食堂运营类DAO*******************************/
// ================================
// 食堂
// ================================
class CanteenDAO {
public:
    std::vector<Canteen> getAllCanteens(sql::Connection *conn);
    std::shared_ptr<CanteenVO> getCanteenById(sql::Connection *conn, int id);
    int getCanteenIdByUserId(sql::Connection *conn, int user_id);
    int insertCanteen(sql::Connection *conn, const std::string& canteen_name, int region_id);
    bool updateCanteenAddress(sql::Connection *conn, int canteen_id, const std::string& address);
    bool updateCanteenStatus(sql::Connection *conn, int canteen_id, int status);
    std::vector<CanteenManagerVO> getCanteensWithManagers(sql::Connection *conn);
    std::vector<PurchaseBill> getPurchaseBillsByCanteen(sql::Connection *conn, int canteen_id);
    int createPurchaseBill(sql::Connection *conn, const PurchaseBill& bill);
    bool updatePurchaseBill(sql::Connection *conn, const PurchaseBill& bill);
    bool deletePurchaseBill(sql::Connection *conn, int bill_id);
    // 财务统计相关方法
    double getTodayIncome(sql::Connection *conn, int canteen_id);
    double getTodayExpense(sql::Connection *conn, int canteen_id);
    double getIncomeByTimeDimension(sql::Connection *conn, int canteen_id, const std::string& time_dimension, const std::string& date_str);
    double getExpenseByTimeDimension(sql::Connection *conn, int canteen_id, const std::string& time_dimension, const std::string& date_str);
};

// ================================
// 菜品
// ================================
class DishDAO {
public:
    std::vector<Dish> getDishesByCanteen(sql::Connection *conn, int canteen_id);
    int insertDish(sql::Connection *conn, const Dish& dish);
    int updateDish(sql::Connection *conn, int dish_id, double price, int calories, const std::string& nutrition_info);
    bool disableDishByDishId(sql::Connection *conn, const int dish_id);
    bool enableDishByDishId(sql::Connection *conn, const int dish_id);
};

// ================================
// 餐单
// ================================
class MenuDAO {
public:
    std::vector<Dish> getMenuByMealType(sql::Connection *conn, int canteen_id, const std::string& meal_type);
    std::vector<CanteenMenuVO> getMenuByCanteen(sql::Connection *conn, int canteen_id);
    int getMenuIdByCanteenAndMealType(sql::Connection *conn, int canteen_id, const std::string& meal_type);
    bool isDishInMenu(sql::Connection *conn, int dish_id);
    bool deleteMenuDish(sql::Connection *conn, int menu_id, int dish_id);
    bool insertMenuDish(sql::Connection *conn, int menu_id, int dish_id);
    std::vector<int> getMenuDishIds(sql::Connection *conn, int menu_id);
};

// 历史餐单
class HistoryMenuDAO {
public:
    int saveHistoryMenu(sql::Connection *conn, const HistoryMenu& historyMenu);
    bool saveHistoryMenuDishes(sql::Connection *conn, int historyMenuId, const std::vector<int>& dishIds);
    std::vector<HistoryMenu> getHistoryMenusByCanteen(sql::Connection *conn, int canteen_id);
    std::vector<Dish> getHistoryMenuDishes(sql::Connection *conn, int history_menu_id);
    int getHistoryMenuIdByCanteenIdAndMealType(sql::Connection *conn, int canteen_id, const std::string& meal_type);
    bool updateHistoryMenuEndTime(sql::Connection *conn, int history_menu_id, int canteen_id, const std::string& meal_type);
};

/************************点餐订单类DAO*******************************/
// ================================
// 订单
// ================================
class OrderDAO {
public:
    int insertOrder(sql::Connection *conn, const Order& order);
    std::vector<OrderVO> getOrdersByUser(sql::Connection *conn, int user_id);
    std::vector<OrderDetailVO> getOrdersDetailsByOrderId(sql::Connection *conn, int order_id);
    std::shared_ptr<Order> getOrderById(sql::Connection *conn, int order_id);
    // 获取最近订单
    std::shared_ptr<RecentOrderVO> getRecentOrder(sql::Connection *conn, int user_id, int order_for_user_id, int canteen_id);
    // 用餐偏好相关方法
    DiningPreferenceSummary getDiningPreferenceSummary(sql::Connection *conn, int user_id, const std::string& time_dimension);
    std::vector<std::pair<std::string, int>> getCanteenConsumptionCount(sql::Connection *conn, int user_id, const std::string& time_dimension);
    std::vector<std::pair<std::string, int>> getDishConsumptionCount(sql::Connection *conn, int user_id, const std::string& time_dimension);
    // 获取食堂订单
    std::vector<OrderVO> getOrdersByCanteen(sql::Connection *conn, int canteen_id);
    // 更新订单状态
    bool updateOrderStatus(sql::Connection *conn, int order_id, int status);
};

// ================================
// 订单详情
// ================================
class OrderItemDAO {
public:
    bool insertOrderItems(sql::Connection *conn, int order_id, const std::vector<OrderItem>& items);
    std::vector<OrderItem> getOrderItemsByOrderId(sql::Connection *conn, int order_id);
};

// ================================
// 订单取消
// ================================
class OrderCancelDAO {
public:
    int insertCancelApply(sql::Connection *conn, int order_id, const std::string& cancel_reason);
    std::vector<OrderCancelVO> getCancelAppliesByCanteen(sql::Connection *conn, int canteen_id);
    std::shared_ptr<OrderCancelVO> getCancelApplyByOrderId(sql::Connection *conn, int order_id);
    std::shared_ptr<OrderCancelVO> getCancelApplyByCancelId(sql::Connection *conn, int cancel_id);
    bool updateCancelStatus(sql::Connection *conn, int cancel_id, int status, const std::string& reject_reason = "");
};

/************************监督反馈类DAO*******************************/
// ================================
// 评价
// ================================
class RatingDAO {
public:
    bool insertRating(sql::Connection *conn, const Rating& rating);
    std::vector<Rating> getRatingsByCanteen(sql::Connection *conn, int canteen_id);
    std::vector<CanteenRatingVO> getCanteenRatingDetails(sql::Connection *conn, int canteen_id);
};

// ================================
// 举报
// ================================
class ReportDAO {
public:
    bool insertReport(sql::Connection *conn, const Report& report);
    std::vector<Report> getReportsByCanteen(sql::Connection *conn, int canteen_id);
    std::vector<ReportVO> getAllReports(sql::Connection *conn);
    bool updateReportStatus(sql::Connection *conn, int report_id, int status, int handler_id);
};

/************************内容消息类DAO*******************************/
// ================================
// 公告
// ================================
class AnnouncementDAO {
public:
    bool insertAnnouncement(sql::Connection *conn, const Announcement& announcement);
    std::vector<AnnouncementVO> getAnnouncementList(sql::Connection *conn);
    bool deleteAnnouncement(sql::Connection *conn, int announce_id, int publisher_id);
};

// ================================
// 留言板
// ================================
class MessageDAO {
public:
    bool insertMessage(sql::Connection *conn, const Message& message);
    std::vector<Message> getMessagesByCanteen(sql::Connection *conn, int canteen_id);
    std::vector<Message> getMessagesByUser(sql::Connection *conn, int user_id, int canteen_id);
    bool replyMessage(sql::Connection *conn, const Message& message);
};

// ================================
// 消息中心
// ================================
class MessageCenterDAO {
public:
    int createMessage(sql::Connection *conn, const MessageNotification& message);
    std::vector<MessageNotification> getMessagesByReceiver(sql::Connection *conn, int receiver_id);
    bool updateMessageStatus(sql::Connection *conn, int message_id, int status);
};

// ================================
// 标签
// ================================
class TagDAO {
public:
    std::vector<Tag> getAllTags(sql::Connection *conn);
    std::vector<Tag> getTagsByDishId(sql::Connection *conn, int dish_id);
};

// ================================
// 菜品标签关联
// ================================
class DishTagDAO {
public:
    bool insertDishTag(sql::Connection *conn, int dish_id, int tag_id);
    bool deleteDishTag(sql::Connection *conn, int dish_id, int tag_id);
    std::vector<int> getTagIdsByDishId(sql::Connection *conn, int dish_id);
};


