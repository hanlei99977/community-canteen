#include <iostream>
#include <memory>

#include "../service/UserService.h"
#include "../model/User.h"
#include "../model/Role.h"
#include "../../MySQL/Database.h"

using namespace std;

int main() {
    try {
        cout << "====== 数据库基础测试 ======" << endl;

        // 1️⃣ 测试 Database 单例
        Database& db = Database::getInstance();
        cout << "Database getInstance() 成功" << endl;

        // 2️⃣ 测试 getConnection
        sql::Connection* conn = db.getConnection();
        if (conn && conn->isValid()) {
            cout << "数据库连接有效" << endl;
        } else {
            cout << "数据库连接无效" << endl;
            return 0;
        }

        // 3️⃣ 测试 SELECT 查询
        cout << "\n====== 查询 users 表 ======" << endl;
        unique_ptr<sql::ResultSet> res(
            db.query("SELECT user_id, username FROM users")
        );

        while (res->next()) {
            cout << "user_id=" << res->getInt("user_id")
                 << ", username=" << res->getString("username") << endl;
        }

        // 4️⃣ 测试注册
        cout << "\n====== 用户注册测试 ======" << endl;
        UserService userService;

        string testUsername = "test_user_cpp";
        string testPassword = "123456";
        string testPhone = "13911112222";

        bool registerResult = userService.registerUser(
            testUsername, testPassword, testPhone
        );

        if (registerResult) {
            cout << "注册成功：" << testUsername << endl;
        } else {
            cout << "注册失败（可能用户名已存在）" << endl;
        }

        // 5️⃣ 测试登录
        cout << "\n====== 用户登录测试 ======" << endl;
        User user;
        bool loginResult = userService.login(
            testUsername, testPassword, user
        );

        if (!loginResult) {
            cout << "登录失败" << endl;
            return 0;
        }

        cout << "登录成功：" << user.username << endl;
        cout << "用户ID：" << user.userId << endl;
        cout << "用户角色：" << endl;

        for (auto role : user.roles) {
            cout << " - " << roleToString(role) << endl;
        }

        cout << "\n====== 所有测试通过 ======" << endl;
    }
    catch (sql::SQLException& e) {
        cerr << "[SQL Exception] " << e.what() << endl;
    }
    catch (exception& e) {
        cerr << "[Exception] " << e.what() << endl;
    }

    return 0;
}
