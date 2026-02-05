#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "../model/User.h"

class UserService {
public:
    // 登录
    bool login(const std::string& username,
               const std::string& password,
               User& user);

    // 注册
    bool registerUser(const std::string& username,
                      const std::string& password,
                      const std::string& phone);
};

#endif
