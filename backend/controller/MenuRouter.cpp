#include "MenuRouter.h"
#include <iostream>

using namespace std;

void MenuRouter::showMainMenu(const User& user) {
    cout << "\n============================\n";
    cout << "欢迎进入社区长者食堂系统\n";
    cout << "当前用户：" << user.username << endl;
    cout << "============================\n";

    int index = 1;

    // 系统管理员
    if (user.hasRole(Role::SYSTEM_ADMIN)) {
        cout << index++ << ". 系统管理模块\n";
    }

    // 食堂管理员
    if (user.hasRole(Role::CANTEEN_ADMIN)) {
        cout << index++ << ". 食堂管理模块\n";
    }

    // 社区监督者
    if (user.hasRole(Role::SUPERVISOR)) {
        cout << index++ << ". 监督与评价模块\n";
    }

    // 用餐者
    if (user.hasRole(Role::DINER)) {
        cout << index++ << ". 点餐与消费模块\n";
    }

    cout << "0. 退出系统\n";
}
