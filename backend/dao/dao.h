#pragma once
#include <memory>
#include <vector>
#include <optional>
#include "../model/model.h"
#include "../model/vo.h"
#include "../model/dto.h"
#include "../model/domain.h"
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
    // 获取市级区域下所有区级区域
    std::vector<Region> getDistrictRegionsByCity(sql::Connection *conn, int city_region_id);
    // 获取所有市级区域
    std::vector<Region> getCityRegions(sql::Connection *conn);
    // 检查是否是市级区域
    bool isCityLevel(sql::Connection *conn, int region_id);
    // 获取区域的父级区域ID
    int getParentRegionId(sql::Connection *conn, int region_id);
};

// ================================
// 用户
// ================================
class UserDAO {
public:
    int insertUser(sql::Connection *conn, const User& user);
    bool existsByUsername(sql::Connection *conn, const std::string& username);
    std::shared_ptr<User> getUserByUsername(sql::Connection *conn, const std::string& username);
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
    std::vector<CanteenManagerVO> getCanteensWithManagers(sql::Connection *conn, int city_id = 0, int district_id = 0);
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
    std::shared_ptr<Dish> getDishById(sql::Connection *conn, int dish_id);
    DishDetailVO getDishDetail(sql::Connection *conn, int dish_id);
    int insertDish(sql::Connection *conn, const Dish& dish);
    int updateDish(sql::Connection *conn, int dish_id, double price, int calories, const std::string& nutrition_info);
    bool disableDishByDishId(sql::Connection *conn, const int dish_id);
    bool enableDishByDishId(sql::Connection *conn, const int dish_id);
    std::vector<DishSaleVO> getDishSales(sql::Connection *conn, int canteen_id, const std::string& time_range, int limit);
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
    // 获取多个最近订单
    std::vector<std::shared_ptr<RecentOrderVO>> getRecentOrders(sql::Connection *conn, int user_id, int order_for_user_id, int canteen_id, int limit = 4);
    // 用餐偏好相关方法
    std::vector<DishPurchaseRankingItem> getDishPurchaseRanking(sql::Connection *conn, int user_id, const std::string& time_dimension);
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
    std::pair<std::vector<RatingVO>, int> getRatingsByCanteenPaginated(sql::Connection *conn, int canteen_id, int page, int page_size);
    std::optional<RatingVO> getRatingByOrderId(sql::Connection *conn, int order_id);
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
    std::pair<std::vector<ReportVO>, int> getReportsByCanteenPaginated(sql::Connection *conn, int canteen_id, int page, int page_size);
    int getUnprocessedCountByScope(sql::Connection *conn, int viewer_id, int range_type);
    ReportStatisticsVO getReportStatistics(sql::Connection *conn, int city_id, int district_id, int canteen_id, int days);

    std::vector<std::pair<std::string, int>> getReportTrend(sql::Connection *conn, int city_id, int district_id, int canteen_id, int days);

    std::vector<std::pair<int, std::pair<std::string, int>>> getReportTypeDistribution(sql::Connection *conn, int city_id, int district_id, int canteen_id, int days);

    std::vector<std::pair<std::string, int>> getTopCanteenReports(sql::Connection *conn, int city_id, int district_id, int canteen_id, int days, int limit);

    std::pair<std::vector<ReportVO>, int> getReportsByFilters(sql::Connection *conn, int city_id, int district_id, int canteen_id, int type, int days, int page, int page_size);
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
class CommentDAO {
public:
    bool insertComment(sql::Connection *conn, const Comment& comment);
    std::vector<Comment> getCommentsByCanteen(sql::Connection *conn, int canteen_id);
    std::vector<Comment> getCommentsByUser(sql::Connection *conn, int user_id, int canteen_id);
    bool replyComment(sql::Connection *conn, const Comment& comment);
};

// ================================
// 通知中心
// ================================
class NotificationDAO {
public:
    int createNotification(sql::Connection *conn, const NotificationMessage& notification);
    std::vector<NotificationMessage> getNotificationsByReceiver(sql::Connection *conn, int receiver_id);
    bool updateNotificationStatus(sql::Connection *conn, int notification_id, int status);
};

// ================================
// 标签
// ================================
class TagDAO {
public:
    std::vector<Tag> getAllTags(sql::Connection *conn);
    std::vector<Tag> getTagsByDishId(sql::Connection *conn, int dish_id);
    Tag getTagById(sql::Connection *conn, int tag_id);
};

// ================================
// 疾病
// ================================
class DiseaseDAO {
public:
    std::vector<Disease> getAllDiseases(sql::Connection *conn);
    std::vector<Disease> getDiseasesByUserId(sql::Connection *conn, int user_id);
    std::vector<int> getDiseaseIdsByUserId(sql::Connection *conn, int user_id);
    bool insertUserDisease(sql::Connection *conn, int user_id, int disease_id);
    bool deleteUserDisease(sql::Connection *conn, int user_id, int disease_id);
    
    // 疾病管理相关方法
    Disease getDiseaseById(sql::Connection *conn, int disease_id);
    int insertDisease(sql::Connection *conn, const std::string& disease_name);
    bool updateDisease(sql::Connection *conn, int disease_id, const std::string& disease_name);
    
    // 疾病标签关联方法
    std::vector<DiseaseTagVO> getDiseaseTags(sql::Connection *conn, int disease_id);
    bool insertDiseaseTag(sql::Connection *conn, int disease_id, int tag_id, int rule_type);
    bool updateDiseaseTag(sql::Connection *conn, int disease_id, int tag_id, int rule_type);
    bool deleteDiseaseTag(sql::Connection *conn, int disease_id, int tag_id);
    bool deleteAllDiseaseTags(sql::Connection *conn, int disease_id);
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

// ================================
// 收藏
// ================================
class FavoriteDAO {
public:
    bool insertFavorite(sql::Connection *conn, int user_id, int dish_id);
    bool deleteFavorite(sql::Connection *conn, int user_id, int dish_id);
    bool existsFavorite(sql::Connection *conn, int user_id, int dish_id);
    std::vector<Favorite> getFavoritesByUserId(sql::Connection *conn, int user_id);
};

// ================================
// 用户用餐偏好
// ================================
class DinerPreferenceDAO {
public:
    std::vector<DinerPreference> getUserPreferences(sql::Connection *conn, int user_id);
    std::vector<DinerPreference> getTopPreferences(sql::Connection *conn, int user_id, int limit = 5);
    bool updateOrInsertPreference(sql::Connection *conn, int user_id, int tag_id, int score_inc);
};


