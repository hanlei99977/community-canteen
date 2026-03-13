#include "UserService.h"
#include "../model/Role.h"
#include <cppconn/prepared_statement.h>
#include "../../MySQL/ConnectionPool.h"

bool UserService::login
(
const std::string& username,
const std::string& password,
User& user
) 
{
    auto* conn = ConnectionPool::getInstance().getConnection();
    try {
       

        // 1️⃣ 校验用户名 + 密码
        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement(
                "SELECT user_id, username, phone, id_card, address, "
                "register_time, status "
                "FROM users "
                "WHERE username = ? AND password = ?"
            )
        );

        pstmt->setString(1, username);
        pstmt->setString(2, password);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        // 用户不存在 or 密码错误
        if (!res->next()) {
            return false;
        }

        // 2️⃣ 填充 User 基本信息
        user.userId = res->getInt("user_id");
        user.username = res->getString("username");
        user.phone = res->getString("phone");
        user.idCard = res->getString("id_card");
        user.address = res->getString("address");
        user.registerTime = res->getString("register_time");
        user.status = res->getInt("status");

        user.roles.clear();

        // 3️⃣ 查询用户角色
        std::unique_ptr<sql::PreparedStatement> pstmtRole(
            conn->prepareStatement(
                "SELECT role_id "
                "FROM user_role "
                "WHERE user_id = ?"
            )
        );

        pstmtRole->setInt(1, user.userId);

        std::unique_ptr<sql::ResultSet> roleRes(
            pstmtRole->executeQuery()
        );

        while (roleRes->next()) {
            int roleId = roleRes->getInt("role_id");
            user.roles.push_back(roleFromId(roleId));
        }
        ConnectionPool::getInstance().releaseConnection(conn);
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "[Login Error] " << e.what() << std::endl;
        ConnectionPool::getInstance().releaseConnection(conn);
        return false;
    }
}

bool UserService::registerUser(const std::string& username,
                               const std::string& password,
                               const std::string& phone) 
{
    auto* conn = ConnectionPool::getInstance().getConnection();
    try 
    {
        

        // 1️⃣ 开启事务
        conn->setAutoCommit(false);

        // 2️⃣ 校验用户名是否存在
        if (isUsernameExists(username)) {
            conn->rollback();
            conn->setAutoCommit(true);
            return false;
        }

        // 2️⃣ 插入用户
        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement(
                "INSERT INTO users(username, password, phone) VALUES (?, ?, ?)"
            )
        );
        stmt->setString(1, username);
        stmt->setString(2, password);   // 现在先明文，后面再加密
        stmt->setString(3, phone);
        stmt->executeUpdate();

        // 3️⃣ 获取新用户 ID
        std::unique_ptr<sql::Statement> stmt2(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(
            stmt2->executeQuery("SELECT LAST_INSERT_ID()")
        );

        int userId = 0;
        if (res->next()) {
            userId = res->getInt(1);
        }

        // 4️⃣ 分配默认角色：用餐者
        std::unique_ptr<sql::PreparedStatement> stmt3(
            conn->prepareStatement(
                "INSERT INTO user_role(user_id, role_id) VALUES (?, ?)"
            )
        );
        stmt3->setInt(1, userId);
        stmt3->setInt(2, static_cast<int>(Role::DINER));
        stmt3->executeUpdate();

        // 5️⃣ 提交事务
        conn->commit();
        conn->setAutoCommit(true);
        ConnectionPool::getInstance().releaseConnection(conn);
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "[Register Error] " << e.what() << std::endl;

        try {
            ConnectionPool::getInstance().getConnection()->rollback();
            ConnectionPool::getInstance().getConnection()->setAutoCommit(true);
        } catch (...) {}
        ConnectionPool::getInstance().releaseConnection(conn);
        return false;
    }
}

bool UserService::isUsernameExists(const std::string& username) 
{
    auto* conn = ConnectionPool::getInstance().getConnection();
    try 
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement(
                "SELECT 1 FROM users WHERE username = ? LIMIT 1"
            )
        );

        pstmt->setString(1, username);

        std::unique_ptr<sql::ResultSet> res(
            pstmt->executeQuery()
        );
        ConnectionPool::getInstance().releaseConnection(conn);
        return res->next(); // 查到即存在
    }
    catch (sql::SQLException& e) {
        std::cerr << "[Check Username Error] " << e.what() << std::endl;
        ConnectionPool::getInstance().releaseConnection(conn);
        return true; // 出异常时保守处理，认为存在
    }
}

