#ifndef ROLE_H
#define ROLE_H

#include <string>

enum class Role {
    SYSTEM_ADMIN    = 1,    // 系统管理员
    CITY_ADMIN      = 2,    // 市级管理员
    DISTRICT_ADMIN  = 3,    // 区级管理员
    CANTEEN_ADMIN   = 4,    // 食堂管理员
    SUPERVISOR      = 5,    // 社区监督者
    DINER           = 6     // 用餐者
};

// 工具函数：role_id → enum
inline Role roleFromId(int roleId) {
    return static_cast<Role>(roleId);
}

// 工具函数：enum → 中文名（展示用）
inline std::string roleToString(Role role) {
    switch (role) {
        case Role::SYSTEM_ADMIN:   return "系统管理员";
        case Role::CITY_ADMIN:     return "市级管理员";
        case Role::DISTRICT_ADMIN: return "区级管理员";
        case Role::CANTEEN_ADMIN:  return "食堂管理员";
        case Role::SUPERVISOR:     return "社区监督者";
        case Role::DINER:          return "用餐者";
        default:                   return "未知角色";
    }
}

#endif
