#include "Database.h"
#include <iostream>

sql::mysql::MySQL_Driver* Database::getDriver()
{
    static sql::mysql::MySQL_Driver* driver =
        sql::mysql::get_mysql_driver_instance();

    return driver;
}

sql::ResultSet* Database::query(
    sql::Connection* conn,
    const std::string& sqlStr)
{
    try
    {
        sql::Statement* stmt = conn->createStatement();
        return stmt->executeQuery(sqlStr);
    }
    catch(sql::SQLException& e)
    {
        std::cerr << "Query error: "
                  << e.what() << std::endl;
        return nullptr;
    }
}

int Database::update(
    sql::Connection* conn,
    const std::string& sqlStr)
{
    try
    {
        sql::Statement* stmt = conn->createStatement();
        int rows = stmt->executeUpdate(sqlStr);
        delete stmt;
        return rows;
    }
    catch(sql::SQLException& e)
    {
        std::cerr << "Update error: "
                  << e.what() << std::endl;
        return -1;
    }
}
