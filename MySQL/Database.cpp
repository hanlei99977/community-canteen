#include "Database.h"
#include <iostream>

// 初始化静态成员
Database* Database::instance = nullptr;

Database::Database(
    const std::string& host,
    const std::string& user,
    const std::string& password,
    const std::string& database
) {
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        conn = driver->connect(host, user, password);
        conn->setSchema(database);
    }
    catch (sql::SQLException& e) {
        std::cerr << "Database connection failed: "
                  << e.what() << std::endl;
        conn = nullptr;
    }
}

Database::~Database() {
    if (conn) {
        delete conn;
    }
}

Database& Database::getInstance() {
    if (instance == nullptr) {
        instance = new Database(
            "tcp://127.0.0.1:3306",
            "user",                 // MySQL 用户名
            "Hl@123456",            // MySQL 密码
            "community_canteen"
        );
    }
    return *instance;
}

sql::Connection* Database::getConnection() {
    return conn;
}

sql::ResultSet* Database::query(const std::string& sqlStr) {
    if (!conn) return nullptr;

    try {
        sql::Statement* stmt = conn->createStatement();
        return stmt->executeQuery(sqlStr);
        // stmt 的释放交给调用者（重要）
    }
    catch (sql::SQLException& e) {
        std::cerr << "Query error: " << e.what() << std::endl;
        return nullptr;
    }
}

int Database::update(const std::string& sqlStr) {
    if (!conn) return -1;

    try {
        sql::Statement* stmt = conn->createStatement();
        int rows = stmt->executeUpdate(sqlStr);
        delete stmt;
        return rows;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Update error: " << e.what() << std::endl;
        return -1;
    }
}
