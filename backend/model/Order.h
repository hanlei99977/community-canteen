#ifndef ORDER_H
#define ORDER_H

#include <string>

class Order {
public:
    int orderId;
    int userId;
    int canteenId;
    double totalPrice;
    std::string orderTime;
    std::string status;

    Order() : orderId(0), userId(0), canteenId(0), totalPrice(0.0) {}
};

#endif
