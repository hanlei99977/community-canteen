#ifndef ORDER_ITEM_H
#define ORDER_ITEM_H

class OrderItem {
public:
    int orderId;
    int dishId;
    int quantity;

    OrderItem() : orderId(0), dishId(0), quantity(0) {}
};

#endif
