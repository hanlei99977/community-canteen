#include <iostream>

#include <mysql_driver.h>
#include <mysql_connection.h>

#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

using namespace std;

int main() {
    try {
        // 1. 获取驱动
        sql::mysql::MySQL_Driver* driver;
        sql::Connection* conn;

        driver = sql::mysql::get_mysql_driver_instance();

        // 2. 连接数据库
        conn = driver->connect(
            "tcp://127.0.0.1:3306",
            "user",
            "Hl@123456"
        );

        // 3. 选择数据库
        conn->setSchema("community_canteen");

        // 4. 执行 SQL
        sql::Statement* stmt = conn->createStatement();
        sql::ResultSet* res = stmt->executeQuery(
            "SELECT * FROM user LIMIT 5"
        );

        // 5. 输出结果
        while (res->next()) {
            cout << res->getInt("user_id") << " "
                 << res->getString("username") << endl;
        }

        // 6. 释放资源
        delete res;
        delete stmt;
        delete conn;
    }
    catch (sql::SQLException& e) {
        cerr << "SQL error: " << e.what() << endl;
    }

    return 0;
}

