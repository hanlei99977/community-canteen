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

    static sql::mysql::MySQL_Driver* getDriver();//获取 MySQL 驱动实例

    // SELECT 查询
    static sql::ResultSet* query(sql::Connection* conn, const std::string& sql);

    // INSERT / UPDATE / DELETE
    static int update(sql::Connection* conn, const std::string& sql);
};

#endif
