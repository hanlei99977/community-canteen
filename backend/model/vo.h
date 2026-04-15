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
    double price;
public:
    OrderDetailVO() = default;
    OrderDetailVO(const std::string& dname, int qty, double p)
        : dish_name(dname), quantity(qty), price(p) {}

    std::string getDishName() const { return dish_name; }
    void setDishName(const std::string& name) { dish_name = name; }

    int getQuantity() const { return quantity; }
    void setQuantity(int q) { quantity = q; }

    double getPrice() const { return price; }
    void setPrice(double p) { price = p; }

};

//Diner个人中心VO
class DinerCenterVO {
private:
    int user_id;
    std::string username;
    int age;
    std::string phone;
    std::string id_card;
    int regionId;
    std::string regionName;
    int familyId;
    std::string familyName;
    std::string diseaseHistory;
    std::string tastePreference;
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
    
    int getRegionId() const { return regionId; }
    void setRegionId(int id) { regionId = id; }

    std::string getRegionName() const { return regionName; }
    void setRegionName(const std::string& addr) { regionName = addr; }

    int getFamilyId() const { return familyId; }
    void setFamilyId(int id) { familyId = id; }
    
    std::string getFamilyName() const { return familyName; }
    void setFamilyName(const std::string& name) { familyName = name; }

    std::string getDiseaseHistory() const { return diseaseHistory; }
    void setDiseaseHistory(const std::string& history) { diseaseHistory = history; }

    std::string getTastePreference() const { return tastePreference; }
    void setTastePreference(const std::string& pref) { tastePreference = pref; }

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
    int status;
    std::string disease_history;
    std::string taste_preference;

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

    int getStatus() const { return status; }
    void setStatus(int s) { status = s; }

    std::string getDiseaseHistory() const { return disease_history; }
    void setDiseaseHistory(const std::string& d) { disease_history = d; }

    std::string getTastePreference() const { return taste_preference; }
    void setTastePreference(const std::string& t) { taste_preference = t; }
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
