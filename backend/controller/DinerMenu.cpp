#include "DinerMenu.h"
#include "../service/DishService.h"
#include "../service/OrderService.h"
#include "../model/OrderItem.h"
#include <iostream>
#include <vector>

using namespace std;

void DinerMenu::show(const User& user) {
    if (!user.hasRole(Role::DINER)) {
        cout << "无权限访问点餐模块\n";
        return;
    }

    DishService dishService;
    OrderService orderService;
    int choice;

    while (true) {
        cout << "\n=== 点餐模块 ===\n";
        cout << "1. 查看菜品\n";
        cout << "2. 下单\n";
        cout << "3. 查看我的订单\n";
        cout << "0. 返回\n";
        cout << "选择：";
        cin >> choice;

        if (choice == 0) break;

        if (choice == 1) {
            auto dishes = dishService.listDishesByCanteen(1);
            for (auto& d : dishes) {
                cout << d.dishId << " | "
                     << d.name << " | "
                     << d.price << endl;
            }
        }

        if (choice == 2) {
            vector<OrderItem> items;
            while (true) {
                OrderItem item;
                cout << "菜品ID (0 结束)：";
                cin >> item.dishId;
                if (item.dishId == 0) break;
                cout << "数量：";
                cin >> item.quantity;
                items.push_back(item);
            }
            orderService.createOrder(user.userId, 1, items);
        }

        if (choice == 3) {
            auto orders = orderService.listOrdersByUser(user.userId);
            for (auto& o : orders) {
                cout << "订单ID：" << o.orderId
                     << " 总价：" << o.totalPrice
                     << " 状态：" << o.status << endl;
            }
        }
    }
}
