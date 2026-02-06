#ifndef ORDER_SERVICE_H
#define ORDER_SERVICE_H

#include "../model/Order.h"
#include "../model/OrderItem.h"
#include <vector>

class OrderService {
public:
    bool createOrder(int userId,
                     int canteenId,
                     const std::vector<OrderItem>& items);

    std::vector<Order> listOrdersByUser(int userId);
};

#endif
