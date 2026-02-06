#include "CanteenAdminMenu.h"
#include "../service/DishService.h"
#include <iostream>

using namespace std;

void CanteenAdminMenu::show(const User& user) {
    if (!user.hasRole(Role::CANTEEN_ADMIN)) {
        cout << "无权限访问食堂管理模块\n";
        return;
    }

    DishService dishService;
    int choice;

    while (true) {
        cout << "\n=== 食堂管理模块 ===\n";
        cout << "1. 新增菜品\n";
        cout << "2. 查看菜品\n";
        cout << "3. 修改菜品\n";
        cout << "4. 删除菜品\n";
        cout << "0. 返回\n";
        cout << "选择：";
        cin >> choice;

        if (choice == 0) break;

        if (choice == 1) {
            Dish d;
            d.canteenId = 1; // 先写死，后面可改成动态
            cout << "菜名："; cin >> d.name;
            cout << "类型(荤/素)："; cin >> d.type;
            cout << "价格："; cin >> d.price;
            cout << "热量："; cin >> d.calories;
            cout << "营养信息："; cin >> d.nutritionInfo;
            dishService.addDish(d);
        }

        if (choice == 2) {
            auto list = dishService.listDishesByCanteen(1);
            for (auto& d : list) {
                cout << d.dishId << " | " << d.name
                     << " | " << d.price << endl;
            }
        }

        if (choice == 3) {
            Dish d;
            cout << "菜品ID："; cin >> d.dishId;
            cout << "新菜名："; cin >> d.name;
            cout << "新类型："; cin >> d.type;
            cout << "新价格："; cin >> d.price;
            cout << "新热量："; cin >> d.calories;
            cout << "新营养信息："; cin >> d.nutritionInfo;
            dishService.updateDish(d);
        }

        if (choice == 4) {
            int id;
            cout << "菜品ID："; cin >> id;
            dishService.deleteDish(id);
        }
    }
}
