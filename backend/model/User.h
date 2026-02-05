#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include "Role.h"

class User {
public:
    int userId;
    std::string username;
    std::string password;
    std::string phone;
    std::string idCard;
    std::string address;
    std::string registerTime;
    int status;

    // 使用枚举存角色
    std::vector<Role> roles;

public:
    User() : userId(0), status(1) {}

    // 角色判断（非常有用）
    bool hasRole(Role role) const {
        for (auto r : roles) {
            if (r == role) return true;
        }
        return false;
    }
};

#endif
