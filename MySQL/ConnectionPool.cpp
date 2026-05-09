#include "ConnectionPool.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

using namespace std;

map<string, string> loadConfig(const string& filename) {
    map<string, string> config;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Warning: Config file " << filename << " not found, using default values." << endl;
        return config;
    }

    string line; // 当前行
    string section; // 当前section
    while (getline(file, line)) {
        // 去除首尾空白
        size_t start = line.find_first_not_of(" \t");
        size_t end = line.find_last_not_of(" \t");
        if (start == string::npos || end == string::npos) continue;
        line = line.substr(start, end - start + 1);

        // 跳过注释和空行
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        // 处理section
        if (line[0] == '[' && line.back() == ']') {
            section = line.substr(1, line.size() - 2);
            continue;
        }

        // 处理 key=value
        size_t pos = line.find('=');
        if (pos == string::npos) continue;
        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);
        
        // 去除key的空白
        start = key.find_first_not_of(" \t");
        end = key.find_last_not_of(" \t");
        if (start != string::npos && end != string::npos) {
            key = key.substr(start, end - start + 1);
        }
        
        // 去除value的空白和引号
        start = value.find_first_not_of(" \t\"'");
        end = value.find_last_not_of(" \t\"'");
        if (start != string::npos && end != string::npos) {
            value = value.substr(start, end - start + 1);
        }

        if (!section.empty()) {
            config[section + "." + key] = value;
        } else {
            config[key] = value;
        }
    }

    file.close();
    return config;
}

ConnectionPool& ConnectionPool::getInstance()
{
    static ConnectionPool instance;
    return instance;
}

ConnectionPool::ConnectionPool()
{
    driver = sql::mysql::get_mysql_driver_instance();
    
    map<string, string> config = loadConfig("config.ini");
    
    host = config["database.host"];
    port = config["database.port"];
    username = config["database.username"];
    password = config["database.password"];
    database = config["database.database"];
    maxSize = std::stoi(config["database.max_pool_size"]);

    initPool(maxSize);
}

void ConnectionPool::initPool(int size)
{
    string url = "tcp://" + host + ":" + port;
    
    for(int i = 0; i < size; i++)
    {
        sql::Connection* conn =
            driver->connect(url, username, password);

        conn->setSchema(database);

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