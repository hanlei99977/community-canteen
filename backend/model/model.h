#pragma once
#include <string>

/**********************************
 * 社区长者食堂管理系统 
**********************************/

// ================================
// Region
// ================================
class Region {
private:
    int region_id;
    std::string region_name;
    std::string region_level;
    int parent_id;

public:
    int getId() const { return region_id; }
    void setId(int id) { region_id = id; }

    std::string getName() const { return region_name; }
    void setName(const std::string& name) { region_name = name; }

    std::string getLevel() const { return region_level; }
    void setLevel(const std::string& level) { region_level = level; }

    int getParentId() const { return parent_id; }
    void setParentId(int pid) { parent_id = pid; }
};

// ================================
// Level
// ================================
class Level {
private:
    int level_id;
    std::string level_name;

public:
    int getId() const { return level_id; }
    void setId(int id) { level_id = id; }

    std::string getName() const { return level_name; }
    void setName(const std::string& name) { level_name = name; }
};

// ================================
// Family
// ================================
class Family {
private:
    int family_id;
    std::string family_name;

public:
    int getId() const { return family_id; }
    void setId(int id) { family_id = id; }

    std::string getName() const { return family_name; }
    void setName(const std::string& name) { family_name = name; }
};

// ================================
// User
// ================================
class User {
private:
    int user_id;
    std::string username;
    int age;
    std::string password;
    std::string phone;
    std::string id_card;
    std::string register_time;
    int status;

public:
    int getId() const { return user_id; }
    void setId(int id) { user_id = id; }

    std::string getUsername() const { return username; }
    void setUsername(const std::string& name) { username = name; }

    int getAge() const { return age; }
    void setAge(int a) { age = a; }

    std::string getPassword() const { return password; }
    void setPassword(const std::string& pwd) { password = pwd; }

    std::string getPhone() const { return phone; }
    void setPhone(const std::string& p) { phone = p; }

    std::string getIdCard() const { return id_card; }
    void setIdCard(const std::string& idc) { id_card = idc; }

    std::string getRegisterTime() const { return register_time; }
    void setRegisterTime(const std::string& t) { register_time = t; }

    int getStatus() const { return status; }
    void setStatus(int s) { status = s; }
};

// ================================
// Admin
// ================================
class Admin {
private:
    int user_id;
    int level_id;
    int region_id;

public:
    int getUserId() const { return user_id; }
    void setUserId(int id) { user_id = id; }

    int getLevelId() const { return level_id; }
    void setLevelId(int id) { level_id = id; }

    int getRegionId() const { return region_id; }
    void setRegionId(int id) { region_id = id; }
};

// ================================
// Diner
// ================================
class Diner {
private:
    int user_id;
    int family_id;
    int region_id;
    std::string disease_history;
    std::string taste_preference;

public:
    int getUserId() const { return user_id; }
    void setUserId(int id) { user_id = id; }

    int getFamilyId() const { return family_id; }
    void setFamilyId(int id) { family_id = id; }

    int getRegionId() const { return region_id; }
    void setRegionId(int id) { region_id = id;} 

    std::string getDiseaseHistory() const { return disease_history; }
    void setDiseaseHistory(const std::string& d) { disease_history = d; }

    std::string getTastePreference() const { return taste_preference; }
    void setTastePreference(const std::string& t) { taste_preference = t; }
};

// ================================
// Announcement
// ================================
class Announcement {
private:
    int announce_id;
    std::string title;
    std::string content;
    std::string publish_time;
    int publisher_id;

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
};

// ================================
// Canteen
// ================================
class Canteen {
private:
    int canteen_id;
    std::string code;
    std::string name;
    std::string address;
    int region_id;
    int manager_id;
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

    int getRegionId() const { return region_id; }
    void setRegionId(int id) { region_id = id; }

    int getManagerId() const { return manager_id; }
    void setManagerId(int id) { manager_id = id; }

    int getStatus() const { return status; }
    void setStatus(int s) { status = s; }
};

// ================================
// Dish
// ================================
class Dish {
private:
    int dish_id;
    int canteen_id;
    std::string name;
    std::string type;
    double price;
    int calories;
    std::string nutrition_info;
    int status;

public:
    int getId() const { return dish_id; }
    void setId(int id) { dish_id = id; }

    int getCanteenId() const { return canteen_id; }
    void setCanteenId(int id) { canteen_id = id; }

    std::string getName() const { return name; }
    void setName(const std::string& n) { name = n; }

    std::string getType() const { return type; }
    void setType(const std::string& t) { type = t; }

    double getPrice() const { return price; }
    void setPrice(double p) { price = p; }

    int getCalories() const { return calories; }
    void setCalories(int c) { calories = c; }

    std::string getNutritionInfo() const { return nutrition_info; }
    void setNutritionInfo(const std::string& n) { nutrition_info = n; }

    int getStatus() const { return status; }
    void setStatus(const int s) { status = s; }
};

// ================================
// DailyMenu
// ================================
class DailyMenu {
private:
    int menu_id;
    int canteen_id;
    std::string date;
    std::string meal_type;

public:
    int getId() const { return menu_id; }
    void setId(int id) { menu_id = id; }

    int getCanteenId() const { return canteen_id; }
    void setCanteenId(int id) { canteen_id = id; }

    std::string getDate() const { return date; }
    void setDate(const std::string& d) { date = d; }

    std::string getMealType() const { return meal_type; }
    void setMealType(const std::string& m) { meal_type = m; }
};

// ================================
// MenuDish
// ================================
class MenuDish {
private:
    int menu_id;
    int dish_id;

public:
    int getMenuId() const { return menu_id; }
    void setMenuId(int id) { menu_id = id; }

    int getDishId() const { return dish_id; }
    void setDishId(int id) { dish_id = id; }
};

// ================================
// Order
// ================================
class Order {
private:
    int order_id;
    int user_id;
    int order_for_user_id;
    int canteen_id;
    double total_price;
    std::string order_time;
    std::string status;

public:
    int getId() const { return order_id; }
    void setId(int id) { order_id = id; }

    int getUserId() const { return user_id; }
    void setUserId(int id) { user_id = id; }

    int getOrderForUserId() const { return order_for_user_id; }
    void setOrderForUserId(int id) { order_for_user_id = id; }

    int getCanteenId() const { return canteen_id; }
    void setCanteenId(int id) { canteen_id = id; }

    double getTotalPrice() const { return total_price; }
    void setTotalPrice(double p) { total_price = p; }

    std::string getOrderTime() const { return order_time; }
    void setOrderTime(const std::string& t) { order_time = t; }

    std::string getStatus() const { return status; }
    void setStatus(const std::string& s) { status = s; }
};

// ================================
// OrderItem
// ================================
class OrderItem {
private:
    int order_id;
    int dish_id;
    int quantity;

public:
    int getOrderId() const { return order_id; }
    void setOrderId(int id) { order_id = id; }

    int getDishId() const { return dish_id; }
    void setDishId(int id) { dish_id = id; }

    int getQuantity() const { return quantity; }
    void setQuantity(int q) { quantity = q; }
};

// ================================
// Rating
// ================================
class Rating {
private:
    int user_id;
    int canteen_id;
    int order_id;
    int score;
    std::string comment;
    std::string time;

public:
    int getUserId() const { return user_id; }
    void setUserId(int id) { user_id = id; }

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

// ================================
// PurchaseBill
// ================================
class PurchaseBill {
private:
    int bill_id;
    int canteen_id;
    double amount;
    std::string purchase_date;
    std::string remark;

public:
    int getId() const { return bill_id; }
    void setId(int id) { bill_id = id; }

    int getCanteenId() const { return canteen_id; }
    void setCanteenId(int id) { canteen_id = id; }

    double getAmount() const { return amount; }
    void setAmount(double a) { amount = a; }

    std::string getPurchaseDate() const { return purchase_date; }
    void setPurchaseDate(const std::string& d) { purchase_date = d; }

    std::string getRemark() const { return remark; }
    void setRemark(const std::string& r) { remark = r; }
};

// ================================
// Report
// ================================
class Report {
private:
    int report_id;
    int user_id;
    int canteen_id;
    int type;
    std::string content;
    int status;
    std::string create_time;
    std::string handle_time;
    int handler_id;

public:
    int getId() const { return report_id; }
    void setId(int id) { report_id = id; }

    int getUserId() const { return user_id; }
    void setUserId(int id) { user_id = id; }

    int getCanteenId() const { return canteen_id; }
    void setCanteenId(int id) { canteen_id = id; }

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
};