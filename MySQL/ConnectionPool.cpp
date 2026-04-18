#include "ConnectionPool.h"
#include <iostream>

using namespace std;

ConnectionPool& ConnectionPool::getInstance()
{
    static ConnectionPool instance;
    return instance;
}

ConnectionPool::ConnectionPool()
{
    driver = sql::mysql::get_mysql_driver_instance();
    maxSize = 16;

    initPool(maxSize);
}

void ConnectionPool::initPool(int size)
{
    for(int i = 0; i < size; i++)
    {
        sql::Connection* conn =
            driver->connect("tcp://127.0.0.1:3306",
                            "user",
                            "Hl@123456");

        conn->setSchema("community_canteen");

        pool.push(conn);
    }
}

sql::Connection* ConnectionPool::getConnection()
{
    unique_lock<mutex> lock(mtx);

    while(pool.empty())
    {
        cv.wait(lock);
    }

    sql::Connection* conn = pool.front();
    pool.pop();

    return conn;
}

void ConnectionPool::releaseConnection(sql::Connection* conn)
{
    unique_lock<mutex> lock(mtx);

    pool.push(conn);

    cv.notify_one();
}

//程序退出后销毁连接。
ConnectionPool::~ConnectionPool()
{
    while(!pool.empty())
    {
        auto conn = pool.front();
        pool.pop();
        delete conn;
    }
}