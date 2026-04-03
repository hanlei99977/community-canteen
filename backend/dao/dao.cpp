#include "dao.h"
#include <cppconn/prepared_statement.h>

/**********************************************
 * UserDao
 *********************************************/
int UserDAO::insertUser(sql::Connection *conn, const User& user)
{
    try {
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

        if (stmt->executeUpdate() == 0) {
            conn->rollback();
            return -1;
        }
        // 获取自增ID
        std::unique_ptr<sql::Statement> cstmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> rs(
            cstmt->executeQuery("SELECT LAST_INSERT_ID()")
        );

        int user_id = -1;
        if (rs->next()) {
            user_id = rs->getInt(1);
        }

        return user_id;
    } catch (...) { return -1; }
}

bool DinerDAO::insertDiner(sql::Connection *conn, int user_id)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO diner(user_id) "
                "VALUES (?)"
            )
        );

        stmt->setInt(1, user_id);
        if (stmt->executeUpdate() == 0) {
            conn->rollback();
            return false;
        }

        return true;
    } catch (...) { return false; }
}

bool AdminDAO::insertAdmin(sql::Connection *conn, int user_id)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO admin(user_id) "
                "VALUES (?)"
            )
        );

        stmt->setInt(1, user_id);
        if (stmt->executeUpdate() == 0) {
            return false;
        }

        return true;
    } catch (...) { return false; }
}

bool ManagerDAO::insertManager(sql::Connection *conn, int user_id)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO manager(user_id) "
                "VALUES (?)"
            )
        );

        stmt->setInt(1, user_id);
        if (stmt->executeUpdate() == 0) {
            return false;
        }

        return true;
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

std::shared_ptr<DinerCenterVO> DinerDAO::getDinerCenterByUserId(int user_id)
{
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT u.username, u.age, u.phone, u.id_card, u.address,d.family_id, f.family_name, d.disease_history, d.taste_preference "
                "FROM users u JOIN diner d ON u.user_id = d.user_id "
                "LEFT JOIN family f ON d.family_id = f.family_id "
                "WHERE u.user_id=?"
            )
        );

        stmt->setInt(1, user_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (res->next()) {
            auto vo = std::make_shared<DinerCenterVO>();
            vo->setUserId(user_id);
            vo->setUsername(res->getString("username"));
            vo->setAge(res->getInt("age"));
            vo->setPhone(res->getString("phone"));
            vo->setIdCard(res->getString("id_card"));
            vo->setAddress(res->getString("address"));
            vo->setFamilyId(res->getInt("family_id"));
            vo->setFamilyName(res->getString("family_name"));
            vo->setDiseaseHistory(res->getString("disease_history"));
            vo->setTastePreference(res->getString("taste_preference"));
            return vo;
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
 int OrderDAO::insertOrder(sql::Connection *conn, const Order& order, const std::vector<OrderItem>& items) {
    try {
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

        return order_id;
    } catch (...) { return -1; }
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
                u.username AS order_for_user_name,
                c.name AS canteen_name,
                o.total_price,
                o.order_time
            FROM orders o
            JOIN canteen c ON o.canteen_id = c.canteen_id
            JOIN users u ON u.user_id  = o.order_for_user_id 
            WHERE o.user_id = ?
            ORDER BY o.order_id DESC
        )")
    );

        stmt->setInt(1, user_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            OrderVO o;
            o.setOrderId(res->getInt("order_id"));
            o.setOrderForUserName(res->getString("order_for_user_name"));
            o.setCanteenName(res->getString("canteen_name"));
            o.setTotalPrice(std::stod(res->getString("total_price").c_str()));
            o.setCreateTime(res->getString("order_time"));

            list.push_back(o);
        }
    } catch (...) {}

    return list;
}

std::vector<OrderDetailVO> OrderDAO::getOrdersDetailsByUser(int user_id,int order_id)
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
                WHERE o.user_id = ? AND o.order_id = ?
                ORDER BY o.order_id DESC
            )")
        );

        stmt->setInt(1, user_id);
        stmt->setInt(2, order_id);

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
 * OrderItemDao
 ****************************************/
bool OrderItemDAO::insertOrderItems(sql::Connection *conn, int order_id, const std::vector<OrderItem>& items) {
    try {
        for (const auto& item : items) {
            auto stmt = std::unique_ptr<sql::PreparedStatement>(
                conn->prepareStatement(
                    "INSERT INTO order_item(order_id, dish_id, quantity) VALUES (?, ?, ?)"
                )
            );

            stmt->setInt(1, order_id);
            stmt->setInt(2, item.getDishId());
            stmt->setInt(3, item.getQuantity());

            if (stmt->executeUpdate() == 0) {
                return false;
            }
        }
        return true;
    } catch (...) { return false; }
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