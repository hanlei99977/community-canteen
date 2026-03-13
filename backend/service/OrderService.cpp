#include "OrderService.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <memory>
#include <iostream>
#include "../../MySQL/ConnectionPool.h"

using namespace std;

bool OrderService::createOrder(
int userId,
int canteenId,
const vector<OrderItem>& items
)
{
    auto* conn = ConnectionPool::getInstance().getConnection();

    try 
    {
        conn->setAutoCommit(false);

        // 1️⃣ 计算总价
        double total = 0.0;
        for (auto& item : items) 
        {
            unique_ptr<sql::PreparedStatement> pstmt
            (
                conn->prepareStatement("SELECT price FROM dish WHERE dish_id=?")
            );
            pstmt->setInt(1, item.dishId);
            auto res = unique_ptr<sql::ResultSet>(pstmt->executeQuery());
            if (res->next()) 
            {
                total += res->getDouble("price") * item.quantity;
            }
        }

        // 2️⃣ 插入订单
        unique_ptr<sql::PreparedStatement> pstmtOrder
        (
            conn->prepareStatement
            (
                "INSERT INTO orders(user_id, canteen_id, total_price, status) "
                "VALUES (?, ?, ?, '已下单')"
            )
        );
        pstmtOrder->setInt(1, userId);
        pstmtOrder->setInt(2, canteenId);
        pstmtOrder->setDouble(3, total);
        pstmtOrder->executeUpdate();

        // 3️⃣ 获取 order_id
        unique_ptr<sql::Statement> stmt(conn->createStatement());
        auto resId = unique_ptr<sql::ResultSet>(stmt->executeQuery("SELECT LAST_INSERT_ID()"));

        int orderId = 0;
        if (resId->next()) 
        {
            orderId = resId->getInt(1);
        }

        // 4️⃣ 插入订单明细
        for (auto& item : items) 
        {
            unique_ptr<sql::PreparedStatement> pstmtItem
            (
                conn->prepareStatement
                (
                    "INSERT INTO order_item(order_id, dish_id, quantity) "
                    "VALUES (?, ?, ?)"
                )
            );
            pstmtItem->setInt(1, orderId);
            pstmtItem->setInt(2, item.dishId);
            pstmtItem->setInt(3, item.quantity);
            pstmtItem->executeUpdate();
        }

        conn->commit();
        conn->setAutoCommit(true);

        ConnectionPool::getInstance().releaseConnection(conn);
        return true;
    }
    catch (sql::SQLException& e) 
    {
        cerr << "[Create Order Error] " << e.what() << endl;
        conn->rollback();
        conn->setAutoCommit(true);

        ConnectionPool::getInstance().releaseConnection(conn);
        return false;
    }
}

vector<Order> OrderService::listOrdersByUser(int userId) {
    vector<Order> list; 
    auto* conn = ConnectionPool::getInstance().getConnection();
    try 
    {
            unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement("SELECT * FROM orders WHERE user_id=?")
        );
        pstmt->setInt(1, userId);

        auto res = unique_ptr<sql::ResultSet>(pstmt->executeQuery());
        while (res->next()) 
        {
            Order o;
            o.orderId = res->getInt("order_id");
            o.userId = res->getInt("user_id");
            o.canteenId = res->getInt("canteen_id");
            o.totalPrice = res->getDouble("total_price");
            o.orderTime = res->getString("order_time");
            o.status = res->getString("status");
            list.push_back(o);
        }
    } catch (sql::SQLException& e) 
    {
        cerr << "[List Order Error] " << e.what() << endl;
    }
    ConnectionPool::getInstance().releaseConnection(conn);
    return list;
}
