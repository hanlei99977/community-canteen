#ifndef DATABASE_H
#define DATABASE_H

#include <string>

#include <mysql_driver.h>
#include <mysql_connection.h>

#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

class Database {
public:
    // 获取单例
    static Database& getInstance();

    // 获取数据库连接
    sql::Connection* getConnection();

    // SELECT 查询
    sql::ResultSet* query(const std::string& sql);

    // INSERT / UPDATE / DELETE
    int update(const std::string& sql);

    // 禁止拷贝
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

private:
    // 私有构造（只允许 getInstance 调用）
    Database(
        const std::string& host,
        const std::string& user,
        const std::string& password,
        const std::string& database
    );

    ~Database();

private:
    static Database* instance;

    sql::mysql::MySQL_Driver* driver;
    sql::Connection* conn;
};

#endif
