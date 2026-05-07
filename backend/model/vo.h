#pragma once
#include <string>
#include <vector>
#include "model.h"
/********************************************************
* 用于发送数据
********************************************************/
//订单列表页VO
class OrderVO {
private:
    int order_id;
    int canteen_id;
    std::string order_for_user_name;
    std::string canteen_name;
    std::string create_time;
    double total_price;
    double discount_rate;
    double original_total;
    double saved_amount;
    int status;
    bool has_rating = false;
    int rating_score = 0;
    std::string rating_comment;
    std::string rating_time;
public:
    int getOrderId() const { return order_id; }
    void setOrderId(int id) { order_id = id; }

    int getCanteenId() const { return canteen_id; }
    void setCanteenId(int id) { canteen_id = id; }

    std::string getOrderForUserName() const { return order_for_user_name; }
    void setOrderForUserName(const std::string& name) { order_for_user_name = name; }

    std::string getCanteenName() const { return canteen_name; }
    void setCanteenName(const std::string& name) { canteen_name = name; }

    std::string getCreateTime() const { return create_time; }
    void setCreateTime(const std::string& t) { create_time = t; }

    double getTotalPrice() const { return total_price; }
    void setTotalPrice(double price) { total_price = price; }

    double getDiscountRate() const { return discount_rate; }
    void setDiscountRate(double rate) { discount_rate = rate; }

    double getOriginalTotal() const { return original_total; }
    void setOriginalTotal(double total) { original_total = total; }

    double getSavedAmount() const { return saved_amount; }
    void setSavedAmount(double amount) { saved_amount = amount; }

    int getStatus() const { return status; }
    void setStatus(int s) { status = s; }

    bool getHasRating() const { return has_rating; }
    void setHasRating(bool value) { has_rating = value; }

    int getRatingScore() const { return rating_score; }
    void setRatingScore(int score) { rating_score = score; }

    std::string getRatingComment() const { return rating_comment; }
    void setRatingComment(const std::string& comment) { rating_comment = comment; }

    std::string getRatingTime() const { return rating_time; }
    void setRatingTime(const std::string& time) { rating_time = time; }
};

// 评价列表 VO（用于食堂管理查看评价）
class RatingVO {
private:
    int user_id;
    std::string username;
    int canteen_id;
    int order_id;
    int score;
    std::string comment;
    std::string time;

public:

    int getUserId() const { return user_id; }
    void setUserId(int id) { user_id = id; }

    std::string getUsername() const { return username; }
    void setUsername(const std::string& name) { username = name; }

    int getCanteenId() const { return canteen_id; }
    void setCanteenId(int id) { canteen_id = id; }

    int getOrderId() const { return order_id; }
    void setOrderId(int id) { order_id = id; }

    int getScore() const { return score; }
    void setScore(int s) { score = s; }

    std::string getComment() const { return comment; }
    void setComment(const std::string& c) { comment = c; }

    std::string getTime() const { return time; }
    void setTime(const std::string& t) { time = t; }
};

// 食堂评价明细 VO（含该评价订单菜品）
class CanteenRatingVO {
private:
    int order_id;
    int user_id;
    std::string username;
    int score;
    std::string comment;
    std::string time;
    std::vector<std::string> dishes;
public:
    int getOrderId() const { return order_id; }
    void setOrderId(int id) { order_id = id; }

    int getUserId() const { return user_id; }
    void setUserId(int id) { user_id = id; }

    std::string getUsername() const { return username; }
    void setUsername(const std::string& name) { username = name; }

    int getScore() const { return score; }
    void setScore(int s) { score = s; }

    std::string getComment() const { return comment; }
    void setComment(const std::string& c) { comment = c; }

    std::string getTime() const { return time; }
    void setTime(const std::string& t) { time = t; }

    std::vector<std::string> getDishes() const { return dishes; }
    void setDishes(const std::vector<std::string>& ds) { dishes = ds; }
    void addDish(const std::string& dish) { dishes.push_back(dish); }
};

// 投诉/举报管理 VO
class ReportVO {
private:
    int report_id;
    int user_id;
    std::string username;
    int canteen_id;
    std::string canteen_name;
    int type;
    std::string content;
    int status;
    std::string create_time;
    std::string handle_time;
    int handler_id;
    std::string handler_name;
public:
    int getReportId() const { return report_id; }
    void setReportId(int id) { report_id = id; }

    int getUserId() const { return user_id; }
    void setUserId(int id) { user_id = id; }

    std::string getUsername() const { return username; }
    void setUsername(const std::string& n) { username = n; }

    int getCanteenId() const { return canteen_id; }
    void setCanteenId(int id) { canteen_id = id; }

    std::string getCanteenName() const { return canteen_name; }
    void setCanteenName(const std::string& n) { canteen_name = n; }

    int getType() const { return type; }
    void setType(int t) { type = t; }

    std::string getContent() const { return content; }
    void setContent(const std::string& c) { content = c; }

    int getStatus() const { return status; }
    void setStatus(int s) { status = s; }

    std::string getCreateTime() const { return create_time; }
    void setCreateTime(const std::string& t) { create_time = t; }

    std::string getHandleTime() const { return handle_time; }
    void setHandleTime(const std::string& t) { handle_time = t; }

    int getHandlerId() const { return handler_id; }
    void setHandlerId(int id) { handler_id = id; }

    std::string getHandlerName() const { return handler_name; }
    void setHandlerName(const std::string& n) { handler_name = n; }
};

// 公告列表 VO
class AnnouncementVO {
private:
    int announce_id;
    std::string title;
    std::string content;
    std::string publish_time;
    int publisher_id;
    std::string publisher_name;
public:
    int getId() const { return announce_id; }
    void setId(int id) { announce_id = id; }

    std::string getTitle() const { return title; }
    void setTitle(const std::string& t) { title = t; }

    std::string getContent() const { return content; }
    void setContent(const std::string& c) { content = c; }

    std::string getPublishTime() const { return publish_time; }
    void setPublishTime(const std::string& t) { publish_time = t; }

    int getPublisherId() const { return publisher_id; }
    void setPublisherId(int id) { publisher_id = id; }

    std::string getPublisherName() const { return publisher_name; }
    void setPublisherName(const std::string& n) { publisher_name = n; }
};

//订单详情页VO
class OrderDetailVO {
private:
    std::string dish_name;
    int quantity;
    double unit_price;
    double discount_price;
    double subtotal;
public:
    OrderDetailVO() = default;
    OrderDetailVO(const std::string& dname, int qty, double unitPrice, double discountPrice, double sub)
        : dish_name(dname), quantity(qty), unit_price(unitPrice), discount_price(discountPrice), subtotal(sub) {}

    std::string getDishName() const { return dish_name; }
    void setDishName(const std::string& name) { dish_name = name; }

    int getQuantity() const { return quantity; }
    void setQuantity(int q) { quantity = q; }

    double getUnitPrice() const { return unit_price; }
    void setUnitPrice(double price) { unit_price = price; }

    double getDiscountPrice() const { return discount_price; }
    void setDiscountPrice(double price) { discount_price = price; }

    double getSubtotal() const { return subtotal; }
    void setSubtotal(double total) { subtotal = total; }

};

// 最近订单菜品VO
class RecentOrderItemVO {
private:
    int dish_id;
    std::string dish_name;
    int quantity;
    double unit_price;
    double discount_price;
    double subtotal;

public:
    RecentOrderItemVO() = default;

    int getDishId() const { return dish_id; }
    void setDishId(int id) { dish_id = id; }

    std::string getDishName() const { return dish_name; }
    void setDishName(const std::string& name) { dish_name = name; }

    int getQuantity() const { return quantity; }
    void setQuantity(int q) { quantity = q; }

    double getUnitPrice() const { return unit_price; }
    void setUnitPrice(double price) { unit_price = price; }

    double getDiscountPrice() const { return discount_price; }
    void setDiscountPrice(double price) { discount_price = price; }

    double getSubtotal() const { return subtotal; }
    void setSubtotal(double total) { subtotal = total; }
};

// 最近订单VO
class RecentOrderVO {
private:
    int order_id;
    int canteen_id;
    std::string canteen_name;
    std::string order_time;
    double total_price;
    double discount_rate;
    double original_total;
    double saved_amount;
    std::vector<RecentOrderItemVO> items;

public:
    RecentOrderVO() = default;

    int getOrderId() const { return order_id; }
    void setOrderId(int id) { order_id = id; }

    int getCanteenId() const { return canteen_id; }
    void setCanteenId(int id) { canteen_id = id; }

    std::string getCanteenName() const { return canteen_name; }
    void setCanteenName(const std::string& name) { canteen_name = name; }

    std::string getOrderTime() const { return order_time; }
    void setOrderTime(const std::string& time) { order_time = time; }

    double getTotalPrice() const { return total_price; }
    void setTotalPrice(double price) { total_price = price; }

    double getDiscountRate() const { return discount_rate; }
    void setDiscountRate(double rate) { discount_rate = rate; }

    double getOriginalTotal() const { return original_total; }
    void setOriginalTotal(double total) { original_total = total; }

    double getSavedAmount() const { return saved_amount; }
    void setSavedAmount(double amount) { saved_amount = amount; }

    std::vector<RecentOrderItemVO> getItems() const { return items; }
    void addItem(const RecentOrderItemVO& item) { items.push_back(item); }
};

//通用用户中心VO
class UserCenterVO {
private:
    int user_id;
    std::string username;
    int age;
    std::string phone;
    std::string id_card;
    std::string role;
    
    // 用餐者字段
    int regionId;
    std::string regionName;
    int familyId;
    std::string familyName;
    std::vector<std::string> diseases;
    
    // 食堂管理者字段
    int canteenId;
    std::string canteenName;
    
    // 管理员字段
    int adminLevel;
    std::string adminRegion;

public:
    int getUserId() const { return user_id; }
    void setUserId(int id) { user_id = id; }

    std::string getUsername() const { return username; }
    void setUsername(const std::string& name) { username = name; }

    int getAge() const { return age; }
    void setAge(int a) { age = a; }

    std::string getPhone() const { return phone; }
    void setPhone(const std::string& p) { phone = p; }

    std::string getIdCard() const { return id_card; }
    void setIdCard(const std::string& id) { id_card = id; }

    std::string getRole() const { return role; }
    void setRole(const std::string& r) { role = r; }

    int getRegionId() const { return regionId; }
    void setRegionId(int id) { regionId = id; }

    std::string getRegionName() const { return regionName; }
    void setRegionName(const std::string& name) { regionName = name; }

    int getFamilyId() const { return familyId; }
    void setFamilyId(int id) { familyId = id; }

    std::string getFamilyName() const { return familyName; }
    void setFamilyName(const std::string& name) { familyName = name; }
    
    std::vector<std::string> getDiseases() const { return diseases; }
    void setDiseases(const std::vector<std::string>& dis) { diseases = dis; }
    void addDisease(const std::string& disease) { diseases.push_back(disease); }

    int getCanteenId() const { return canteenId; }
    void setCanteenId(int id) { canteenId = id; }

    std::string getCanteenName() const { return canteenName; }
    void setCanteenName(const std::string& name) { canteenName = name; }

    int getAdminLevel() const { return adminLevel; }
    void setAdminLevel(int level) { adminLevel = level; }

    std::string getAdminRegion() const { return adminRegion; }
    void setAdminRegion(const std::string& region) { adminRegion = region; }
};

//每日餐单
class CanteenMenuVO {
private:
    int menu_id;
    std::string date;
    std::string type;

    std::vector<Dish> dishes; // 菜品名称列表
public:
    int getMenuId() const { return menu_id; }
    void setMenuId(int id) { menu_id = id; }

    std::string getDate() const { return date; }
    void setDate(const std::string& d) { date = d; }

    std::string getType() const { return type; }
    void setType(const std::string& t) { type = t; }

    std::vector<Dish>& getDishes() { return dishes; }
    // ⭐ 添加菜品
    void addDish(const Dish& dish) {
        dishes.push_back(dish);
    }
};


//管理员管理
class AdminInformation
{
private:
    // 1. 基础用户信息 (对应 users 表)
    int user_id;
    std::string username;
    int age;
    std::string phone;
    int status;
    // 2. 管理员专属信息 (对应 admin 表)
    int level_id;       // 关联级别表ID
    std::string level_name; // 级别名称 (对应 level 表)
    int region_id;
    std::string region_name;

public:
    // --- Getter 和 Setter 方法 ---
    int getUserId() const { return user_id; }
    void setUserId(int id) { user_id = id; }

    std::string getUsername() const { return username; }
    void setUsername(const std::string& name) { username = name; }

    int getAge() const { return age; }
    void setAge(int a) { age = a; }

    std::string getPhone() const { return phone; }
    void setPhone(const std::string& p) { phone = p; }

    int getStatus() const { return status; }
    void setStatus(int s) { status = s; }

    int getLevelId() const { return level_id; }
    void setLevelId(int lid) { level_id = lid; }

    std::string getLevelName() const { return level_name; }
    void setLevelName(const std::string& ln) { level_name = ln; }

    int getRegionId() const { return region_id; }
    void setRegionId(int id) { region_id = id; }

    std::string getRegionName() const { return region_name; }
    void setRegionName(const std::string& name) { region_name = name; }
};

// 用餐者管理
class DinerInformation
{
private:
    int user_id;
    std::string username;
    int age;
    std::string phone;
    int region_id;
    std::string region_name;
    std::vector<std::string> diseases;
    int status;


public:
    // GETTER & SETTER 完全统一风格
    int getUserId() const { return user_id; }
    void setUserId(int id) { user_id = id; }

    std::string getUsername() const { return username; }
    void setUsername(const std::string& name) { username = name; }

    int getAge() const { return age; }
    void setAge(int a) { age = a; }

    std::string getPhone() const { return phone; }
    void setPhone(const std::string& p) { phone = p; }

    int getRegionId() const { return region_id; }
    void setRegionId(int id) { region_id = id; }

    std::string getRegionName() const { return region_name; }
    void setRegionName(const std::string& addr) { region_name = addr; }

    std::vector<std::string> getDiseases() const { return diseases; }
    void setDiseases(const std::vector<std::string>& dis) { diseases = dis; }
    void addDisease(const std::string& disease) { diseases.push_back(disease); }

    int getStatus() const { return status; }
    void setStatus(int s) { status = s; }
};

// 家庭成员（下单对象）VO
class FamilyMemberVO
{
private:
    int user_id;
    std::string username;

public:
    int getUserId() const { return user_id; }
    void setUserId(int id) { user_id = id; }

    std::string getUsername() const { return username; }
    void setUsername(const std::string& name) { username = name; }
};

// 管理员申请审核列表 VO
class AdminApplyVO
{
private:
    int apply_id;
    int user_id;
    std::string username;
    int age;
    std::string phone;
    int level_id;
    std::string level_name;
    int region_id;
    std::string region_name;
    int status;
    std::string apply_time;
    std::string review_time;
    int reviewer_id;
    std::string reviewer_name;

public:
    int getApplyId() const { return apply_id; }
    void setApplyId(int id) { apply_id = id; }

    int getUserId() const { return user_id; }
    void setUserId(int id) { user_id = id; }

    std::string getUsername() const { return username; }
    void setUsername(const std::string& name) { username = name; }

    int getAge() const { return age; }
    void setAge(int a) { age = a; }

    std::string getPhone() const { return phone; }
    void setPhone(const std::string& p) { phone = p; }

    int getLevelId() const { return level_id; }
    void setLevelId(int id) { level_id = id; }

    std::string getLevelName() const { return level_name; }
    void setLevelName(const std::string& name) { level_name = name; }

    int getRegionId() const { return region_id; }
    void setRegionId(int id) { region_id = id; }

    std::string getRegionName() const { return region_name; }
    void setRegionName(const std::string& name) { region_name = name; }

    int getStatus() const { return status; }
    void setStatus(int s) { status = s; }

    std::string getApplyTime() const { return apply_time; }
    void setApplyTime(const std::string& t) { apply_time = t; }

    std::string getReviewTime() const { return review_time; }
    void setReviewTime(const std::string& t) { review_time = t; }

    int getReviewerId() const { return reviewer_id; }
    void setReviewerId(int id) { reviewer_id = id; }

    std::string getReviewerName() const { return reviewer_name; }
    void setReviewerName(const std::string& name) { reviewer_name = name; }
};

// 食堂管理者申请审核列表 VO
class CanteenManagerApplyVO
{
private:
    int apply_id;
    int user_id;
    std::string username;
    int age;
    std::string phone;
    std::string canteen_name;
    int region_id;
    std::string region_name;
    int status;
    std::string apply_time;
    std::string review_time;
    int reviewer_id;
    std::string reviewer_name;

public:
    int getApplyId() const { return apply_id; }
    void setApplyId(int id) { apply_id = id; }

    int getUserId() const { return user_id; }
    void setUserId(int id) { user_id = id; }

    std::string getUsername() const { return username; }
    void setUsername(const std::string& name) { username = name; }

    int getAge() const { return age; }
    void setAge(int a) { age = a; }

    std::string getPhone() const { return phone; }
    void setPhone(const std::string& p) { phone = p; }

    std::string getCanteenName() const { return canteen_name; }
    void setCanteenName(const std::string& name) { canteen_name = name; }

    int getRegionId() const { return region_id; }
    void setRegionId(int id) { region_id = id; }

    std::string getRegionName() const { return region_name; }
    void setRegionName(const std::string& name) { region_name = name; }

    int getStatus() const { return status; }
    void setStatus(int s) { status = s; }

    std::string getApplyTime() const { return apply_time; }
    void setApplyTime(const std::string& t) { apply_time = t; }

    std::string getReviewTime() const { return review_time; }
    void setReviewTime(const std::string& t) { review_time = t; }

    int getReviewerId() const { return reviewer_id; }
    void setReviewerId(int id) { reviewer_id = id; }

    std::string getReviewerName() const { return reviewer_name; }
    void setReviewerName(const std::string& name) { reviewer_name = name; }
};

// 食堂管理 VO
class CanteenManagerVO
{
private:
    int canteen_id;
    std::string name;
    std::string address;
    int region_id;
    std::string region_name;
    int manager_id;
    std::string manager_name;
    int status;
    double rating;
    int complaint_count;

public:
    int getId() const { return canteen_id; }
    void setId(int id) { canteen_id = id; }

    std::string getName() const { return name; }
    void setName(const std::string& n) { name = n; }

    std::string getAddress() const { return address; }
    void setAddress(const std::string& a) { address = a; }

    int getRegionId() const { return region_id; }
    void setRegionId(int id) { region_id = id; }

    std::string getRegionName() const { return region_name; }
    void setRegionName(const std::string& rn) { region_name = rn; }

    int getManagerId() const { return manager_id; }
    void setManagerId(int id) { manager_id = id; }

    std::string getManagerName() const { return manager_name; }
    void setManagerName(const std::string& mn) { manager_name = mn; }

    int getStatus() const { return status; }
    void setStatus(int s) { status = s; }

    double getRating() const { return rating; }
    void setRating(double r) { rating = r; }

    int getComplaintCount() const { return complaint_count; }
    void setComplaintCount(int c) { complaint_count = c; }
};


// ================================
// CanteenVO
// ================================
class CanteenVO {
private:
    int canteen_id;
    std::string code;
    std::string name;
    std::string address;
    std::string region_name;
    int region_id;
    int status;

public:
    int getId() const { return canteen_id; }
    void setId(int id) { canteen_id = id; }

    std::string getCode() const { return code; }
    void setCode(const std::string& c) { code = c; }

    std::string getName() const { return name; }
    void setName(const std::string& n) { name = n; }

    std::string getAddress() const { return address; }
    void setAddress(const std::string& a) { address = a; }

    std::string getRegionName() const { return region_name; }
    void setRegionName(const std::string& n) { region_name = n; }

    int getRegionId() const { return region_id; }
    void setRegionId(int id) { region_id = id; }

    int getStatus() const { return status; }
    void setStatus(int s) { status = s; }
};

// ================================
// 订单取消申请 VO
// ================================
class OrderCancelVO {
private:
    int cancel_id;
    int order_id;
    std::string cancel_time;
    std::string cancel_reason;
    int status;
    std::string reject_reason;
    std::string handle_time;
    std::string order_for_user_name;
    std::string canteen_name;
    double total_price;
    std::string order_time;

public:
    int getCancelId() const { return cancel_id; }
    void setCancelId(int id) { cancel_id = id; }

    int getOrderId() const { return order_id; }
    void setOrderId(int id) { order_id = id; }

    std::string getCancelTime() const { return cancel_time; }
    void setCancelTime(const std::string& time) { cancel_time = time; }

    std::string getCancelReason() const { return cancel_reason; }
    void setCancelReason(const std::string& reason) { cancel_reason = reason; }

    int getStatus() const { return status; }
    void setStatus(int s) { status = s; }

    std::string getRejectReason() const { return reject_reason; }
    void setRejectReason(const std::string& reason) { reject_reason = reason; }

    std::string getHandleTime() const { return handle_time; }
    void setHandleTime(const std::string& time) { handle_time = time; }

    std::string getOrderForUserName() const { return order_for_user_name; }
    void setOrderForUserName(const std::string& name) { order_for_user_name = name; }

    std::string getCanteenName() const { return canteen_name; }
    void setCanteenName(const std::string& name) { canteen_name = name; }

    double getTotalPrice() const { return total_price; }
    void setTotalPrice(double price) { total_price = price; }

    std::string getOrderTime() const { return order_time; }
    void setOrderTime(const std::string& time) { order_time = time; }
};

// ================================
// 登录结果 VO
// ================================
class LoginResultVO {
private:
    int user_id;
    std::string username;
    std::string role;
    int canteen_id;
    int admin_level;
    int region_id;
    std::string region_name;

public:
    int getUserId() const { return user_id; }
    void setUserId(int id) { user_id = id; }

    std::string getUsername() const { return username; }
    void setUsername(const std::string& name) { username = name; }

    std::string getRole() const { return role; }
    void setRole(const std::string& r) { role = r; }

    int getCanteenId() const { return canteen_id; }
    void setCanteenId(int id) { canteen_id = id; }

    int getAdminLevel() const { return admin_level; }
    void setAdminLevel(int level) { admin_level = level; }

    int getRegionId() const { return region_id; }
    void setRegionId(int id) { region_id = id; }

    std::string getRegionName() const { return region_name; }
    void setRegionName(const std::string& name) { region_name = name; }
};


// 财务统计相关 VO
// 今日财务统计
class TodayFinancialData {
private:
    double income;
    double expense;
    double profit;
public:
    double getIncome() const { return income; }
    void setIncome(double value) { income = value; }

    double getExpense() const { return expense; }
    void setExpense(double value) { expense = value; }

    double getProfit() const { return profit; }
    void setProfit(double value) { profit = value; }
};

// 财务统计
class FinancialData {
private:
    std::vector<std::string> labels;
    std::vector<double> values;
public:
    std::vector<std::string> getLabels() const { return labels; }
    void setLabels(const std::vector<std::string>& value) { labels = value; }

    std::vector<double> getValues() const { return values; }
    void setValues(const std::vector<double>& value) { values = value; }

    void addLabel(const std::string& label) { labels.push_back(label); }
    void addValue(double value) { values.push_back(value); }
};

// 用餐偏好相关 VO
class ConsumptionItem {
private:
    std::string name;
    int count;
public:
    std::string getName() const { return name; }
    void setName(const std::string& value) { name = value; }

    int getCount() const { return count; }
    void setCount(int value) { count = value; }
};

class DiningPreference {
private:
    std::vector<ConsumptionItem> canteen_consumption;
    std::vector<ConsumptionItem> dish_consumption;
    std::vector<ConsumptionItem> tag_preference;
public:

    std::vector<ConsumptionItem> getCanteenConsumption() const { return canteen_consumption; }
    void setCanteenConsumption(const std::vector<ConsumptionItem>& value) { canteen_consumption = value; }

    std::vector<ConsumptionItem> getDishConsumption() const { return dish_consumption; }
    void setDishConsumption(const std::vector<ConsumptionItem>& value) { dish_consumption = value; }

    std::vector<ConsumptionItem> getTagPreference() const { return tag_preference; }
    void setTagPreference(const std::vector<ConsumptionItem>& value) { tag_preference = value; }

    void addCanteenConsumption(const ConsumptionItem& item) { canteen_consumption.push_back(item); }
    void addDishConsumption(const ConsumptionItem& item) { dish_consumption.push_back(item); }
    void addTagPreference(const ConsumptionItem& item) { tag_preference.push_back(item); }
};

// 疾病VO
class DiseaseVO {
private:
    int diseaseId;
    std::string diseaseName;
    std::vector<std::string> tags;
public:
    int getDiseaseId() const { return diseaseId; }
    void setDiseaseId(int value) { diseaseId = value; }

    std::string getDiseaseName() const { return diseaseName; }
    void setDiseaseName(const std::string& value) { diseaseName = value; }

    std::vector<std::string> getTags() const { return tags; }
    void setTags(const std::vector<std::string>& value) { tags = value; }
    void addTag(const std::string& tag) { tags.push_back(tag); }
};

// 疾病标签关联项
class DiseaseTagVO {
private:
    int tagId;
    std::string tagName;
    int ruleType;
public:
    int getTagId() const { return tagId; }
    void setTagId(int value) { tagId = value; }

    std::string getTagName() const { return tagName; }
    void setTagName(const std::string& value) { tagName = value; }

    int getRuleType() const { return ruleType; }
    void setRuleType(int value) { ruleType = value; }
};

// 疾病详情VO（用于疾病管理）
class DiseaseDetailVO {
private:
    int diseaseId;
    std::string diseaseName;
    std::vector<DiseaseTagVO> tags;
public:
    int getDiseaseId() const { return diseaseId; }
    void setDiseaseId(int value) { diseaseId = value; }

    std::string getDiseaseName() const { return diseaseName; }
    void setDiseaseName(const std::string& value) { diseaseName = value; }

    std::vector<DiseaseTagVO> getTags() const { return tags; }
    void setTags(const std::vector<DiseaseTagVO>& value) { tags = value; }
    void addTag(const DiseaseTagVO& tag) { tags.push_back(tag); }
};

// 餐品购买排名项
class DishPurchaseRankingItem {
private:
    int dishId;
    std::string dishName;
    std::string canteenName;
    int quantity;
public:
    int getDishId() const { return dishId; }
    void setDishId(int value) { dishId = value; }

    std::string getDishName() const { return dishName; }
    void setDishName(const std::string& value) { dishName = value; }

    std::string getCanteenName() const { return canteenName; }
    void setCanteenName(const std::string& value) { canteenName = value; }

    int getQuantity() const { return quantity; }
    void setQuantity(int value) { quantity = value; }
};

class DishDetailVO {
private:
    int dishId;
    std::string dishName;
    std::string canteenName;
    double price;
    int calories;
    std::string nutritionInfo;
    std::vector<std::string> tags;
public:
    int getDishId() const { return dishId; }
    void setDishId(int value) { dishId = value; }

    std::string getDishName() const { return dishName; }
    void setDishName(const std::string& value) { dishName = value; }

    std::string getCanteenName() const { return canteenName; }
    void setCanteenName(const std::string& value) { canteenName = value; }

    double getPrice() const { return price; }
    void setPrice(double value) { price = value; }

    int getCalories() const { return calories; }
    void setCalories(int value) { calories = value; }

    std::string getNutritionInfo() const { return nutritionInfo; }
    void setNutritionInfo(const std::string& value) { nutritionInfo = value; }

    std::vector<std::string> getTags() const { return tags; }
    void setTags(const std::vector<std::string>& value) { tags = value; }
    void addTag(const std::string& tag) { tags.push_back(tag); }
};

// 举报统计VO
struct ReportStatisticsVO {
    int unprocessed_count;
    int total_count;
    int today_count;
};
