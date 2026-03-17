#include "dao.h"
#include "../../MySQL/ConnectionPool.h"
#include <cppconn/prepared_statement.h>

/**********************************************
 * AdminDao
 *********************************************/
bool AdminDAO::insertAdmin(const Admin& admin) {
    try {
        auto conn = ConnectionPool::getInstance().getConnection();

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
        auto conn = ConnectionPool::getInstance().getConnection();

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
        auto conn = ConnectionPool::getInstance().getConnection();

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

/*****************************************
 * CanteenDao
 ****************************************/
std::vector<Canteen> CanteenDAO::getAllCanteens() {
    std::vector<Canteen> list;

    try {
        auto conn = ConnectionPool::getInstance().getConnection();
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

/*****************************************
 * MenuDao
 ****************************************/
std::vector<Dish> MenuDAO::getMenuByDate(int canteen_id, const std::string& date) {
    std::vector<Dish> list;

    try {
        auto conn = ConnectionPool::getInstance().getConnection();

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
            list.push_back(d);
        }
    } catch (...) {}

    return list;
}

/*****************************************
 * RatingDao
 ****************************************/
bool RatingDAO::insertRating(const Rating& rating) {
    try {
        auto conn = ConnectionPool::getInstance().getConnection();

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

/*****************************************
 * ReportDao
 ****************************************/
bool ReportDAO::insertReport(const Report& report) {
    try {
        auto conn = ConnectionPool::getInstance().getConnection();

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