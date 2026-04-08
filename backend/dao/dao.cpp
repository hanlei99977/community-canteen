#include "dao.h"
#include <cppconn/prepared_statement.h>

/***************************************************************************************
 * UserDao
 ***************************************************************************************/
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
                "INSERT INTO canteen_manager(user_id) "
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

bool UserDAO::updateUser(sql::Connection *conn, const DinerCenterVO& user) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "UPDATE users SET age=?, phone=?, id_card=?, address=? "
                "WHERE user_id=?"
            )
        );

        stmt->setInt(1, user.getAge());
        stmt->setString(2, user.getPhone());
        stmt->setString(3, user.getIdCard());
        stmt->setString(4, user.getAddress());
        stmt->setInt(5, user.getUserId());
        if (stmt->executeUpdate() == 0) {
            std::cout<<"没有对 users 表进行更新"<<std::endl; 
        }
        
        return true;
    } catch (...) { return false; }
}

std::string UserDAO::getUserRole(int user_id)
{
    DBConnectionGuard guard;
    auto* conn = guard.get();
    // 查 admin 表
    auto stmt1 = std::unique_ptr<sql::PreparedStatement>(
        conn->prepareStatement("SELECT level_id FROM admin WHERE user_id = ?")
    );
    stmt1->setInt(1, user_id);
    auto res1 = std::unique_ptr<sql::ResultSet>(stmt1->executeQuery());

    if (res1->next()) {
        int level = res1->getInt("level_id");

        switch(level) {
            case 1: return "system_admin";
            case 2: return "admin";
            default: return "admin";
        }
    }

    // 查 diner 表
    auto stmt2 = std::unique_ptr<sql::PreparedStatement>(
        conn->prepareStatement("SELECT user_id FROM diner WHERE user_id = ?")
    );
    stmt2->setInt(1, user_id);
    auto res2 = std::unique_ptr<sql::ResultSet>(stmt2->executeQuery());

    if (res2->next()) {
        return "diner";
    }

    // 查canteen_manager表
    auto stmt3 = std::unique_ptr<sql::PreparedStatement>(
        conn->prepareStatement("SELECT * FROM canteen_manager WHERE user_id = ?")
    );
    stmt3->setInt(1, user_id);
    auto res3 = std::unique_ptr<sql::ResultSet>(stmt3->executeQuery());
    if (res3->next()) {
        return "canteen_manager";
    }

    return "unknown";
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

bool DinerDAO::updateDiner(sql::Connection *conn, const DinerCenterVO& diner) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "UPDATE diner SET family_id=?, disease_history=?, taste_preference=? "
                "WHERE user_id=?"
            )
        );

        stmt->setInt(1, diner.getFamilyId());
        stmt->setString(2, diner.getDiseaseHistory());
        stmt->setString(3, diner.getTastePreference());
        stmt->setInt(4, diner.getUserId());
        if (stmt->executeUpdate() == 0) {
            std::cout<<"没有对diner表进行更新"<<std::endl; 
        }

        return true;
    } catch (...) { return false; }
}


/***************************************************************************************
 * FamilyDao
***************************************************************************************/
bool FamilyDAO::insertFamily(const Family& family) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO family(family_name) "
                "VALUES (?)"
            )
        );

        stmt->setString(1, family.getName());

        if (stmt->executeUpdate() == 0) {
            return false;
        }

        return true;
    } catch (...) { return false; }
}

Family FamilyDAO::getFamilyByUserId(int user_id) {
    Family family;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT f.family_id, f.family_name FROM family f "
                "JOIN diner d ON f.family_id = d.family_id "
                "WHERE d.user_id=?"
            )
        );

        stmt->setInt(1, user_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        if (res->next()) {
            family.setId(res->getInt("family_id"));
            family.setName(res->getString("family_name"));
        }
    } catch (...) {}

    return family;
}

std::vector<Family> FamilyDAO::getFamilyList() {
    std::vector<Family> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT f.family_id, f.family_name FROM family f "
            )
        );

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            Family f;
            f.setId(res->getInt("family_id"));
            f.setName(res->getString("family_name"));
            list.push_back(f);
        }
    } catch (...) {}

    return list;
}

/***************************************************************************************
 * CanteenDao
 ***************************************************************************************/
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

int CanteenDAO::getCanteenIdByUserId(int user_id) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT canteen_id FROM canteen_manager WHERE user_id=?"
            )
        );

        stmt->setInt(1, user_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (res->next()) {
            return res->getInt("canteen_id");
        }
    } catch (...) {}

    return -1;
}

/***************************************************************************************
 * DishDao
 ***************************************************************************************/
std::vector<Dish> DishDAO::getDishesByCanteen(int canteen_id)
{
    std::vector<Dish> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT * FROM dish WHERE canteen_id=? AND status = 1")
        );

        stmt->setInt(1, canteen_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            Dish d;
            d.setId(res->getInt("dish_id"));
            d.setName(res->getString("name"));
            d.setType(res->getString("type"));
            d.setPrice(res->getDouble("price"));
            d.setCalories(res->getInt("calories"));
            d.setNutritionInfo(res->getString("nutrition_info"));
            list.push_back(d);
        }
    } catch (const std::exception& e) {
        std::cerr << "查询菜品失败: " << e.what() << std::endl;
    }

    return list;
}

bool DishDAO::insertDish(const Dish& dish){
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt1 = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO dish(canteen_id, name, type, price, calories, nutrition_info) "
                "VALUES (?, ?, ?, ?, ?, ?)"
            )
        );

        stmt1->setInt(1, dish.getCanteenId());          // 1: canteen_id (int)
        stmt1->setString(2, dish.getName());             // 2: name (string)
        stmt1->setString(3, dish.getType());             // 3: type (string)
        stmt1->setDouble(4, dish.getPrice());            // 4: price (decimal → double)
        stmt1->setInt(5, dish.getCalories());            // 5: calories (int)
        stmt1->setString(6, dish.getNutritionInfo());    // 6: nutrition_info (string)

        if (stmt1->executeUpdate() == 0) {
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "插入菜品失败: " << e.what() << std::endl;
        return false;
    }

}
bool DishDAO::disableDishByDishId(const int dish_id){
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "UPDATE dish SET status = 0 WHERE dish_id = ? "
            )
        );
        stmt->setInt(1, dish_id);

        if (stmt->executeUpdate() == 0) {
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "删除菜品失败: " << e.what() << std::endl;
        return false;
    }

}

/***************************************************************************************
 * MenuDao
 ***************************************************************************************/
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

std::vector<CanteenMenuVO> MenuDAO::getMenuByCanteen(int canteen_id) {
    std::vector<CanteenMenuVO> list;
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT m.menu_id, m.date, m.meal_type, d.dish_id, d.name, d.price "
                "FROM daily_menu m "
                "JOIN menu_dish md ON m.menu_id = md.menu_id "
                "JOIN dish d ON md.dish_id = d.dish_id "
                "WHERE m.canteen_id = ? "
                "ORDER BY m.date DESC"
            )
        );

        stmt->setInt(1, canteen_id);

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        std::map<int, CanteenMenuVO> menuMap;
        while (res->next()) {
            int menu_id = res->getInt("menu_id");

            if (menuMap.find(menu_id) == menuMap.end()) {
                CanteenMenuVO menu;
                menu.setMenuId(menu_id);
                menu.setDate(res->getString("date"));
                menu.setType(res->getString("meal_type"));
                menuMap[menu_id] = menu;
            }

            Dish dish;
            dish.setId(res->getInt("dish_id"));
            dish.setName(res->getString("name"));
            dish.setPrice(res->getDouble("price"));

            menuMap[menu_id].addDish(dish);
        }

        // 转 vector
        for (auto& [_, v] : menuMap) {
            list.push_back(v);
        }

    } catch (const std::exception& e) {
        std::cerr << "查询菜单失败: " << e.what() << std::endl;
    }

    return list;
}


bool MenuDAO::insertMenu(const MenuCreateDTO& menu) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        TransactionGuard tx(conn);
        // 1️⃣ 插入 daily_menu 表
        auto stmt1 = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO daily_menu(canteen_id, date, meal_type) "
                "VALUES (?, ?, ?)"
            )
        );

        stmt1->setInt(1, menu.getCanteenId());
        stmt1->setString(2, menu.getDate());
        stmt1->setString(3, menu.getMealType());

        if (stmt1->executeUpdate() == 0) {
            return false;
        }

        // 获取自增ID
        std::unique_ptr<sql::Statement> cstmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> rs(
            cstmt->executeQuery("SELECT LAST_INSERT_ID()")
        );

        int menu_id = -1;
        if (rs->next()) {
            menu_id = rs->getInt(1);
        }

        // 2️⃣ 插入 menu_dish 表
        for (int dish_id : menu.getDishIds()) {
            auto stmt2 = std::unique_ptr<sql::PreparedStatement>(
                conn->prepareStatement(
                    "INSERT INTO menu_dish(menu_id, dish_id) VALUES (?, ?)"
                )
            );

            stmt2->setInt(1, menu_id);
            stmt2->setInt(2, dish_id);

            if (stmt2->executeUpdate() == 0) {
                return false;
            }
        }

        tx.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "插入每日餐单失败: " << e.what() << std::endl;
        return false;
    }
}

bool MenuDAO::eraseMenu(const int menu_id){
    try {
        DBConnectionGuard guard;
        auto conn = guard.get();
        TransactionGuard tx(conn);
        //menu_dish
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "DELETE FROM menu_dish "
                "WHERE menu_id=? "
            )
        );

        stmt->setInt(1, menu_id);

        if (stmt->executeUpdate() == 0) {
            return false;
        }

        //daily_menu
        stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "DELETE FROM daily_menu "
                "WHERE menu_id=? "
            )
        );

        stmt->setInt(1, menu_id);

        if (stmt->executeUpdate() == 0) {
            return false;
        }
        
        tx.commit();
        return true;
    } catch (...) { return false; }
}
/***************************************************************************************
 * OrderDao
 ***************************************************************************************/
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

/***************************************************************************************
 * OrderItemDao
 ***************************************************************************************/
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

/***************************************************************************************
 * RatingDao
 ***************************************************************************************/
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

/***************************************************************************************
 * ReportDao
***************************************************************************************/
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