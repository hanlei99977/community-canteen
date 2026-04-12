#pragma once
#include <string>
#include <vector>
#include "model.h"

//订单列表页VO
class OrderVO {
private:
    int order_id;
    std::string order_for_user_name;
    std::string canteen_name;
    std::string create_time;
    double total_price;
public:
    int getOrderId() const { return order_id; }
    void setOrderId(int id) { order_id = id; }

    std::string getOrderForUserName() const { return order_for_user_name; }
    void setOrderForUserName(const std::string& name) { order_for_user_name = name; }

    std::string getCanteenName() const { return canteen_name; }
    void setCanteenName(const std::string& name) { canteen_name = name; }

    std::string getCreateTime() const { return create_time; }
    void setCreateTime(const std::string& t) { create_time = t; }

    double getTotalPrice() const { return total_price; }
    void setTotalPrice(double price) { total_price = price; }

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
