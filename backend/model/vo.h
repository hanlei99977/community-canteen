#pragma once
#include <string>
#include <vector>

class OrderVO {
private:
    int order_id;
    int order_for_user_id;
    std::string canteen_name;
    std::string create_time;
    double total_price;
public:
    int getOrderId() const { return order_id; }
    void setOrderId(int id) { order_id = id; }

    int getOrderForUserId() const { return order_for_user_id; }
    void setOrderForUserId(int id) { order_for_user_id = id; }

    std::string getCanteenName() const { return canteen_name; }
    void setCanteenName(const std::string& name) { canteen_name = name; }

    std::string getCreateTime() const { return create_time; }
    void setCreateTime(const std::string& t) { create_time = t; }

    double getTotalPrice() const { return total_price; }
    void setTotalPrice(double price) { total_price = price; }

};

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