#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <mysql_driver.h>
#include <mysql_connection.h>

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

//连接池类
class ConnectionPool {
public:

    static ConnectionPool& getInstance();

    sql::Connection* getConnection();

    void releaseConnection(sql::Connection* conn);

    ~ConnectionPool();

private:

    ConnectionPool();

    void initPool(int size);

private:

    std::queue<sql::Connection*> pool;

    std::mutex mtx;

    std::condition_variable cv;

    sql::mysql::MySQL_Driver* driver;

    int maxSize;

};


//连接控制制类，RAII风格
class DBConnectionGuard {
private:
    sql::Connection* conn;

public:
    DBConnectionGuard() {
        conn = ConnectionPool::getInstance().getConnection();
    }

    ~DBConnectionGuard() {
        if (conn) {
            ConnectionPool::getInstance().releaseConnection(conn);
        }
    }

    sql::Connection* get() {
        return conn;
    }
};

//事务控制
class TransactionGuard {
public:
    sql::Connection* conn;
    bool committed = false;

    TransactionGuard(sql::Connection* c) : conn(c) {
        conn->setAutoCommit(false);
    }

    void commit() {
        conn->commit();
        committed = true;
    }

    ~TransactionGuard() {
        if (!committed) {
            conn->rollback();
        }
    }
};
#endif