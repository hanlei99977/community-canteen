#include "dao.h"
#include "../../MySQL/ConnectionPool.h"
#include <cppconn/prepared_statement.h>

/**********************************************
 * UserDao
 *********************************************/
bool UserDAO::insertUser(const User& user)
{
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO users(username, password, age, phone, id_card, address, register_time, status) "
                "VALUES (?, ?, ?, ?, ?, ?, NOW(), ?)"
            )
        );

        stmt->setString(1, user.getUsername());
        stmt->setString(2, user.getPassword());
        stmt->setInt(3, user.getAge());
        stmt->setString(4, user.getPhone());
        stmt->setString(5, user.getIdCard());
        stmt->setString(6, user.getAddress());
        stmt->setInt(7, user.getStatus());

        return stmt->executeUpdate() > 0;
    } catch (...) { return false; }
}

std::shared_ptr<User> UserDAO::getUserByUsernameAndPassword(const std::string& username, const std::string& password)
{
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT * FROM users WHERE username=? AND password=?"
            )
        );

        stmt->setString(1, username);
        stmt->setString(2, password);

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (res->next()) {
            auto u = std::make_shared<User>();
            u->setId(res->getInt("user_id"));
            u->setUsername(res->getString("username"));
            u->setAge(res->getInt("age"));
            u->setPhone(res->getString("phone"));
            u->setIdCard(res->getString("id_card"));
            u->setAddress(res->getString("address"));
            u->setRegisterTime(res->getString("register_time"));
            u->setStatus(res->getInt("status"));
            return u;
        }
    } catch (...) {}
    return nullptr;
}
std::shared_ptr<User> UserDAO::getUserById(int user_id)
{
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT * FROM users WHERE user_id=?"
            )
        );

        stmt->setInt(1, user_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (res->next()) {
            auto u = std::make_shared<User>();
            u->setId(res->getInt("user_id"));
            u->setUsername(res->getString("username"));
            u->setAge(res->getInt("age"));
            u->setPhone(res->getString("phone"));
            u->setIdCard(res->getString("id_card"));
            u->setAddress(res->getString("address"));
            u->setRegisterTime(res->getString("register_time"));
            u->setStatus(res->getInt("status"));
            return u;
        }
    } catch (...) {}
    return nullptr;
}



/**********************************************
 * AdminDao
 *********************************************/
bool AdminDAO::insertAdmin(const Admin& admin) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO admin(user_id, level_id, region_id) VALUES (?, ?, ?)"
            )
        );

        stmt->setInt(1, admin.getUserId());
        stmt->setInt(2, admin.getLevelId());
        stmt->setInt(3, admin.getRegionId());

        return stmt->executeUpdate() > 0;
    } catch (...) { return false; }
}

std::shared_ptr<Admin> AdminDAO::getAdminByUserId(int user_id) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT * FROM admin WHERE user_id=?"
            )
        );

        stmt->setInt(1, user_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (res->next()) {
            auto a = std::make_shared<Admin>();
            a->setUserId(res->getInt("user_id"));
            a->setLevelId(res->getInt("level_id"));
            a->setRegionId(res->getInt("region_id"));
            return a;
        }
    } catch (...) {}
    return nullptr;
}


/**********************************************
 * DinerDao
 *********************************************/
bool DinerDAO::insertDiner(const Diner& diner) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO diner(user_id, family_id, disease_history, taste_preference) VALUES (?, ?, ?, ?)"
            )
        );

        stmt->setInt(1, diner.getUserId());
        stmt->setInt(2, diner.getFamilyId());
        stmt->setString(3, diner.getDiseaseHistory());
        stmt->setString(4, diner.getTastePreference());

        return stmt->executeUpdate() > 0;
    } catch (...) { return false; }
}

std::shared_ptr<Diner> DinerDAO::getDinerByUserId(int user_id)
{
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT * FROM diner WHERE user_id=?"
            )
        );

        stmt->setInt(1, user_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (res->next()) {
            auto d = std::make_shared<Diner>();
            d->setUserId(res->getInt("user_id"));
            d->setFamilyId(res->getInt("family_id"));
            d->setDiseaseHistory(res->getString("disease_history"));
            d->setTastePreference(res->getString("taste_preference"));
            return d;
        }
    } catch (...) {}
    return nullptr;
}
/*****************************************
 * CanteenDao
 ****************************************/
std::vector<Canteen> CanteenDAO::getAllCanteens() {
    std::vector<Canteen> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT * FROM canteen")
        );

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            Canteen c;
            c.setId(res->getInt("canteen_id"));
            c.setName(res->getString("name"));
            list.push_back(c);
        }
    } catch (...) {}

    return list;
}

std::shared_ptr<Canteen> CanteenDAO::getCanteenById(int id) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT * FROM canteen WHERE canteen_id=?")
        );

        stmt->setInt(1, id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (res->next()) {
            auto c = std::make_shared<Canteen>();
            c->setId(res->getInt("canteen_id"));
            c->setName(res->getString("name"));
            return c;
        }
    } catch (...) {}

    return nullptr;
}

/*****************************************
 * DishDao
 ****************************************/
std::vector<Dish> DishDAO::getDishesByCanteen(int canteen_id)
{
    std::vector<Dish> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT * FROM dish WHERE canteen_id=?")
        );

        stmt->setInt(1, canteen_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            Dish d;
            d.setId(res->getInt("dish_id"));
            d.setName(res->getString("name"));
            d.setPrice(res->getDouble("price"));
            list.push_back(d);
        }
    } catch (...) {}

    return list;
}

/*****************************************
 * MenuDao
 ****************************************/
std::vector<Dish> MenuDAO::getMenuByDate(int canteen_id, const std::string& date) {
    std::vector<Dish> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT d.* FROM daily_menu m "
                "JOIN menu_dish md ON m.menu_id = md.menu_id "
                "JOIN dish d ON md.dish_id = d.dish_id "
                "WHERE m.canteen_id=? AND m.date=?"
            )
        );

        stmt->setInt(1, canteen_id);
        stmt->setString(2, date);

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            Dish d;
            d.setId(res->getInt("dish_id"));
            d.setName(res->getString("name"));
            d.setPrice(res->getDouble("price"));
            list.push_back(d);
        }
    } catch (...) {}

    return list;
}

/*****************************************
 * OrderDao
 ****************************************/
 bool OrderDAO::createOrder(const Order& order, const std::vector<OrderItem>& items) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        conn->setAutoCommit(false); // 开启事务

        // 1️⃣ 插入订单
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO `orders`(user_id, order_for_user_id, canteen_id, total_price, order_time, status) "
                "VALUES (?, ?, ?, ?, NOW(), ?)"
            )
        );

        stmt->setInt(1, order.getUserId());
        stmt->setInt(2, order.getOrderForUserId());
        stmt->setInt(3, order.getCanteenId());
        stmt->setDouble(4, order.getTotalPrice());
        stmt->setString(5, order.getStatus());

        if (stmt->executeUpdate() == 0) {
            conn->rollback();
            return false;
        }

        // 获取自增ID
        std::unique_ptr<sql::Statement> cstmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> rs(
            cstmt->executeQuery("SELECT LAST_INSERT_ID()")
        );

        int order_id = -1;
        if (rs->next()) {
            order_id = rs->getInt(1);
        }


        // 2️⃣ 插入订单项
        for (const auto& item : items) {
            auto itemStmt = std::unique_ptr<sql::PreparedStatement>(
                conn->prepareStatement(
                    "INSERT INTO order_item(order_id, dish_id, quantity) VALUES (?, ?, ?)"
                )
            );

            itemStmt->setInt(1, order_id);
            itemStmt->setInt(2, item.getDishId());
            itemStmt->setInt(3, item.getQuantity());
            //executeUpdate返回受影响的行数，如果为0说明插入失败
            if (itemStmt->executeUpdate() == 0) {
                conn->rollback();
                return false;
            }
        }

        conn->commit(); // 提交事务
        return true;
    } catch (...) { return false; }
}

std::vector<OrderVO> OrderDAO::getOrdersByUser(int user_id)
{
    std::vector<OrderVO> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

    auto stmt = std::unique_ptr<sql::PreparedStatement>(
        conn->prepareStatement(R"(
            SELECT 
                o.order_id AS order_id,
                o.order_for_user_id,
                c.name AS canteen_name,
                o.total_price,
                o.order_time
            FROM orders o
            JOIN canteen c ON o.canteen_id = c.canteen_id
            WHERE o.user_id = ?
            ORDER BY o.order_id DESC
        )")
    );

        stmt->setInt(1, user_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            OrderVO o;
            o.setOrderId(res->getInt("order_id"));
            o.setOrderForUserId(res->getInt("order_for_user_id"));
            o.setCanteenName(res->getString("canteen_name"));
            o.setTotalPrice(std::stod(res->getString("total_price").c_str()));
            o.setCreateTime(res->getString("order_time"));

            list.push_back(o);
        }
    } catch (...) {}

    return list;
}

std::vector<OrderDetailVO> OrderDAO::getOrdersDetailsByUser(int user_id)
{
    std::vector<OrderDetailVO> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(R"(
                SELECT 
                    d.name AS dish_name,
                    d.price,
                    oi.quantity
                FROM orders o
                JOIN order_item oi ON oi.order_id = o.order_id
                JOIN dish d ON oi.dish_id = d.dish_id
                WHERE o.user_id = ?
                ORDER BY o.order_id DESC
            )")
        );

        stmt->setInt(1, user_id);

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            OrderDetailVO vo(
                res->getString("dish_name"),
                res->getInt("quantity"),
                std::stod(res->getString("price").c_str())
            );

            list.push_back(vo);
        }

    } catch (const std::exception& e) {
        std::cout << "查询订单失败: " << e.what() << std::endl;
    }

    return list;
}

/*****************************************
 * RatingDao
 ****************************************/
bool RatingDAO::insertRating(const Rating& rating) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO rating(user_id, canteen_id, order_id, score, comment, time) "
                "VALUES (?, ?, ?, ?, ?, NOW())"
            )
        );

        stmt->setInt(1, rating.getUserId());
        stmt->setInt(2, rating.getCanteenId());
        stmt->setInt(3, rating.getOrderId());
        stmt->setInt(4, rating.getScore());
        stmt->setString(5, rating.getComment());

        return stmt->executeUpdate() > 0;
    } catch (...) { return false; }
}

 std::vector<Rating> RatingDAO::getRatingsByCanteen(int canteen_id)
 {
    std::vector<Rating> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT * FROM rating WHERE canteen_id=?"
            )
        );

        stmt->setInt(1, canteen_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            Rating r;
            r.setUserId(res->getInt("user_id"));
            r.setCanteenId(res->getInt("canteen_id"));
            r.setOrderId(res->getInt("order_id"));
            r.setScore(res->getInt("score"));
            r.setComment(res->getString("comment"));
            r.setTime(res->getString("time"));
            list.push_back(r);
        }
    } catch (...) {}

    return list;
}

/*****************************************
 * ReportDao
 ****************************************/
bool ReportDAO::insertReport(const Report& report) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO report(user_id, canteen_id, type, content, status, create_time) "
                "VALUES (?, ?, ?, ?, 0, NOW())"
            )
        );

        stmt->setInt(1, report.getUserId());
        stmt->setInt(2, report.getCanteenId());
        stmt->setInt(3, report.getType());
        stmt->setString(4, report.getContent());

        return stmt->executeUpdate() > 0;
    } catch (...) { return false; }
}

std::vector<Report> ReportDAO::getReportsByCanteen(int canteen_id)
{
    std::vector<Report> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT * FROM report WHERE canteen_id=?"
            )
        );

        stmt->setInt(1, canteen_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            Report r;
            r.setUserId(res->getInt("user_id"));
            r.setCanteenId(res->getInt("canteen_id"));
            r.setType(res->getInt("type"));
            r.setContent(res->getString("content"));
            r.setStatus(res->getInt("status"));
            r.setCreateTime(res->getString("create_time"));
            list.push_back(r);
        }
    } catch (...) {}

    return list;
}