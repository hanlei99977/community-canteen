#include "dao.h"
#include <cppconn/prepared_statement.h>

/***************************************************************************************
 * RegionDao
 ***************************************************************************************/

std::vector<Region> RegionDAO::getRegionList(sql::Connection *conn) {
    std::vector<Region> list;

    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT r.region_id, r.region_name, r.region_level, r.parent_id FROM region r "
            )
        );

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            Region r;
            r.setId(res->getInt("region_id"));
            r.setName(res->getString("region_name"));
            r.setLevel(res->getString("region_level"));
            r.setParentId(res->getInt("parent_id"));

            list.push_back(r);
        }
    } catch (const std::exception& e) {
        std::cerr << "[RegionDAO::getRegionList] Error: " << e.what() << std::endl;
    }

    return list;
}

std::shared_ptr<Region> RegionDAO::getRegionById(sql::Connection *conn, int region_id) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT region_id, region_name, region_level, parent_id FROM region WHERE region_id = ?"
            )
        );
        stmt->setInt(1, region_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (res->next()) {
            auto region = std::make_shared<Region>();
            region->setId(res->getInt("region_id"));
            region->setName(res->getString("region_name"));
            region->setLevel(res->getString("region_level"));
            region->setParentId(res->getInt("parent_id"));
            return region;
        }
    } catch (const std::exception& e) {
        std::cerr << "[RegionDAO::getRegionById] Error: " << e.what() << std::endl;
    }
    return nullptr;
}

std::vector<Region> RegionDAO::getDistrictRegions(sql::Connection *conn) {
    std::vector<Region> list;

    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT region_id, region_name, region_level, parent_id FROM region WHERE region_level = '区级'"
            )
        );

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            Region r;
            r.setId(res->getInt("region_id"));
            r.setName(res->getString("region_name"));
            r.setLevel(res->getString("region_level"));
            r.setParentId(res->getInt("parent_id"));
            list.push_back(r);
        }
    } catch (const std::exception& e) {
        std::cerr << "[RegionDAO::getDistrictRegions] Error: " << e.what() << std::endl;
    }

    return list;
}

bool RegionDAO::isDistrictLevel(sql::Connection *conn, int region_id) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT region_level FROM region WHERE region_id = ? AND region_level = '区级'"
            )
        );
        stmt->setInt(1, region_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        return res->next();
    } catch (const std::exception& e) {
        std::cerr << "[RegionDAO::isDistrictLevel] Error: " << e.what() << std::endl;
    }
    return false;
}


/***************************************************************************************
 * UserDao
 ***************************************************************************************/
int UserDAO::insertUser(sql::Connection *conn, const User& user)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO users(username, password, age, phone, id_card, register_time, status) "
                "VALUES (?, ?, ?, ?, ?, NOW(), ?)"
            )
        );

        stmt->setString(1, user.getUsername());
        stmt->setString(2, user.getPassword());
        stmt->setInt(3, user.getAge());
        stmt->setString(4, user.getPhone());
        stmt->setString(5, user.getIdCard());
        stmt->setInt(6, user.getStatus());

        if (stmt->executeUpdate() == 0) {
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
    } catch (const std::exception& e) {
        std::cerr << "[UserDAO::insertUser] Error: " << e.what() << std::endl;
        return -1;
    }
}

bool UserDAO::existsByUsername(sql::Connection *conn, const std::string& username)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT 1 FROM users WHERE username = ? LIMIT 1")
        );
        stmt->setString(1, username);

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        return res->next();
    } catch (const std::exception& e) {
        std::cerr << "[UserDAO::existsByUsername] Error: " << e.what() << std::endl;
        return false;
    }
}

bool DinerDAO::insertDiner(sql::Connection *conn, int user_id, int region_id)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO diner(user_id, region_id) "
                "VALUES (?, ?)"
            )
        );

        stmt->setInt(1, user_id);
        stmt->setInt(2, region_id);
        if (stmt->executeUpdate() == 0) {
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[DinerDAO::insertDiner] Error: " << e.what() << std::endl;
        return false;
    }
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
    } catch (const std::exception& e) {
        std::cerr << "[AdminDAO::insertAdmin] Error: " << e.what() << std::endl;
        return false;
    }
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
    } catch (const std::exception& e) {
        std::cerr << "[ManagerDAO::insertManager(int)] Error: " << e.what() << std::endl;
        return false;
    }
}

bool ManagerDAO::insertManager(sql::Connection *conn, int user_id, int canteen_id)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO canteen_manager(user_id, canteen_id) "
                "VALUES (?, ?)"
            )
        );

        stmt->setInt(1, user_id);
        stmt->setInt(2, canteen_id);
        if (stmt->executeUpdate() == 0) {
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[ManagerDAO::insertManager(int, int)] Error: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<User> UserDAO::getUserByUsernameAndPassword(sql::Connection *conn, const std::string& username, const std::string& password)
{
    try {
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
            u->setRegisterTime(res->getString("register_time"));
            u->setStatus(res->getInt("status"));
            return u;
        }
    } catch (const std::exception& e) {
        std::cerr << "[UserDAO::getUserByUsernameAndPassword] Error: " << e.what() << std::endl;
    }
    return nullptr;
}
std::shared_ptr<User> UserDAO::getUserById(sql::Connection *conn, int user_id)
{
    try {
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
            u->setPassword(res->getString("password"));
            u->setPhone(res->getString("phone"));
            u->setIdCard(res->getString("id_card"));
            u->setRegisterTime(res->getString("register_time"));
            u->setStatus(res->getInt("status"));
            return u;
        }
    } catch (const std::exception& e) {
        std::cerr << "[UserDAO::getUserById] Error: " << e.what() << std::endl;
    }
    return nullptr;
}

bool UserDAO::updateUser(sql::Connection *conn, const UserCenterVO& user) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "UPDATE users SET age=?, phone=?, id_card=? "
                "WHERE user_id=?"
            )
        );

        stmt->setInt(1, user.getAge());
        stmt->setString(2, user.getPhone());
        stmt->setString(3, user.getIdCard());
        stmt->setInt(4, user.getUserId());
        if (stmt->executeUpdate() == 0) {
            std::cout<<"没有对 users 表进行更新"<<std::endl; 
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[UserDAO::updateUser] Error: " << e.what() << std::endl;
        return false;
    }
}

std::string UserDAO::getUserRole(sql::Connection *conn, int user_id)
{
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

bool UserDAO::updateStatus(sql::Connection *conn, int user_id, int status)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "UPDATE users SET status = ? WHERE user_id = ? "
            )
        );

        stmt->setInt(1, status);
        stmt->setInt(2, user_id);

        if (stmt->executeUpdate() == 0) {
            std::cout<<"没有对user表进行更新"<<std::endl;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[UserDAO::updateStatus(int, int)] Error: " << e.what() << std::endl;
        return false;
    }
}

bool UserDAO::updateStatus(sql::Connection *conn, const User& user)
{
    try {
        return updateStatus(conn, user.getId(), user.getStatus());
    } catch (const std::exception& e) {
        std::cerr << "[UserDAO::updateStatus(const User&)] Error: " << e.what() << std::endl;
        return false;
    }
}

bool UserDAO::updatePassword(sql::Connection *conn, int user_id, const std::string& new_password) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "UPDATE users SET password = ? WHERE user_id = ?"
            )
        );

        stmt->setString(1, new_password);
        stmt->setInt(2, user_id);

        return stmt->executeUpdate() > 0;
    } catch (const std::exception& e) {
        std::cerr << "[UserDAO::updatePassword] Error: " << e.what() << std::endl;
        return false;
    }
}

bool AdminDAO::insertAdmin(sql::Connection *conn, int user_id, int level_id, int region_id)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO admin(user_id, level_id, region_id) "
                "VALUES (?, ?, ?)"
            )
        );

        stmt->setInt(1, user_id);
        stmt->setInt(2, level_id);
        stmt->setInt(3, region_id);
        if (stmt->executeUpdate() == 0) {
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[AdminDAO::insertAdmin(int, int, int)] Error: " << e.what() << std::endl;
        return false;
    }
}


std::shared_ptr<Admin> AdminDAO::getAdminByUserId(sql::Connection *conn, int user_id) {
    try {
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
    } catch (const std::exception& e) {
        std::cerr << "[AdminDAO::getAdminByUserId] Error: " << e.what() << std::endl;
    }
    return nullptr;
}

std::vector<AdminInformation> AdminDAO::getAdminList(sql::Connection *conn)
{
    std::vector<AdminInformation> list;

    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT u.user_id, username, age, phone, status, a.level_id, l.level_name, r.region_id, r.region_name " 
                    "FROM users u "
                    "JOIN admin a ON u.user_id = a.user_id "
                    "JOIN level l ON a.level_id = l.level_id "
                    "JOIN region r ON r.region_id  = a.region_id " 
            )
        );

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            AdminInformation adminInfo;
            adminInfo.setUserId(res->getInt("user_id"));
            adminInfo.setUsername(res->getString("username"));
            adminInfo.setAge(res->getInt("age"));
            adminInfo.setPhone(res->getString("phone"));
            adminInfo.setStatus(res->getInt("status"));
            adminInfo.setLevelId(res->getInt("level_id"));
            adminInfo.setLevelName(res->getString("level_name"));
            adminInfo.setRegionId(res->getInt("region_id"));
            adminInfo.setRegionName(res->getString("region_name"));

            list.push_back(adminInfo);
        }
    } catch (const std::exception& e) {
        std::cerr << "[AdminDAO::getAdminList] Error: " << e.what() << std::endl;
    }

    return list;
}

bool AdminApplyDAO::insertApply(sql::Connection *conn, int user_id, int level_id, int region_id)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO admin_apply(user_id, level_id, region_id, status, apply_time) "
                "VALUES (?, ?, ?, 0, NOW())"
            )
        );

        stmt->setInt(1, user_id);
        stmt->setInt(2, level_id);
        stmt->setInt(3, region_id);

        return stmt->executeUpdate() > 0;
    } catch (const std::exception& e) {
        std::cerr << "[AdminApplyDAO::insertApply] Error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<AdminApplyVO> AdminApplyDAO::getApplyList(sql::Connection *conn)
{
    std::vector<AdminApplyVO> list;

    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT aa.apply_id, aa.user_id, u.username, u.age, u.phone, aa.level_id, l.level_name, "
                "aa.region_id, r.region_name, aa.status, aa.apply_time, aa.review_time, aa.reviewer_id, "
                "ru.username AS reviewer_name "
                "FROM admin_apply aa "
                "JOIN users u ON aa.user_id = u.user_id "
                "LEFT JOIN level l ON aa.level_id = l.level_id "
                "LEFT JOIN region r ON aa.region_id = r.region_id "
                "LEFT JOIN users ru ON aa.reviewer_id = ru.user_id "
                "ORDER BY aa.apply_id DESC"
            )
        );

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        while (res->next()) {
            AdminApplyVO vo;
            vo.setApplyId(res->getInt("apply_id"));
            vo.setUserId(res->getInt("user_id"));
            vo.setUsername(res->getString("username"));
            vo.setAge(res->getInt("age"));
            vo.setPhone(res->getString("phone"));
            vo.setLevelId(res->getInt("level_id"));
            vo.setLevelName(res->getString("level_name"));
            vo.setRegionId(res->getInt("region_id"));
            vo.setRegionName(res->getString("region_name"));
            vo.setStatus(res->getInt("status"));
            vo.setApplyTime(res->getString("apply_time"));
            vo.setReviewTime(res->isNull("review_time") ? "" : res->getString("review_time"));
            vo.setReviewerId(res->isNull("reviewer_id") ? 0 : res->getInt("reviewer_id"));
            vo.setReviewerName(res->isNull("reviewer_name") ? "" : res->getString("reviewer_name"));

            list.push_back(vo);
        }
    } catch (const std::exception& e) {
        std::cerr << "[AdminApplyDAO::getApplyList] Error: " << e.what() << std::endl;
    }

    return list;
}

std::shared_ptr<AdminApplyVO> AdminApplyDAO::getApplyById(sql::Connection *conn, int apply_id)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT aa.apply_id, aa.user_id, aa.level_id, aa.region_id, aa.status "
                "FROM admin_apply aa WHERE aa.apply_id = ?"
            )
        );
        stmt->setInt(1, apply_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        if (res->next()) {
            auto vo = std::make_shared<AdminApplyVO>();
            vo->setApplyId(res->getInt("apply_id"));
            vo->setUserId(res->getInt("user_id"));
            vo->setLevelId(res->getInt("level_id"));
            vo->setRegionId(res->getInt("region_id"));
            vo->setStatus(res->getInt("status"));
            return vo;
        }
    } catch (const std::exception& e) {
        std::cerr << "[AdminApplyDAO::getApplyById] Error: " << e.what() << std::endl;
    }

    return nullptr;
}

bool AdminApplyDAO::reviewApply(sql::Connection *conn, int apply_id, int reviewer_id, int status)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "UPDATE admin_apply SET status = ?, reviewer_id = ?, review_time = NOW() "
                "WHERE apply_id = ? AND status = 0"
            )
        );
        stmt->setInt(1, status);
        stmt->setInt(2, reviewer_id);
        stmt->setInt(3, apply_id);

        return stmt->executeUpdate() > 0;
    } catch (const std::exception& e) {
        std::cerr << "[AdminApplyDAO::reviewApply] Error: " << e.what() << std::endl;
        return false;
    }
}

bool CanteenManagerApplyDAO::insertApply(sql::Connection *conn, int user_id, const std::string& canteen_name, int region_id)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO canteen_manager_apply(user_id, canteen_name, region_id, status, apply_time) "
                "VALUES (?, ?, ?, 0, NOW())"
            )
        );

        stmt->setInt(1, user_id);
        stmt->setString(2, canteen_name);
        stmt->setInt(3, region_id);
        return stmt->executeUpdate() > 0;
    } catch (const std::exception& e) {
        std::cerr << "[CanteenManagerApplyDAO::insertApply] Error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<CanteenManagerApplyVO> CanteenManagerApplyDAO::getApplyList(sql::Connection *conn)
{
    std::vector<CanteenManagerApplyVO> list;

    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT ca.apply_id, ca.user_id, u.username, u.age, u.phone, ca.canteen_name, ca.status, "
                "ca.region_id, rg.region_name, ca.apply_time, ca.review_time, ca.reviewer_id, ru.username AS reviewer_name "
                "FROM canteen_manager_apply ca "
                "JOIN users u ON ca.user_id = u.user_id "
                "LEFT JOIN region rg ON ca.region_id = rg.region_id "
                "LEFT JOIN users ru ON ca.reviewer_id = ru.user_id "
                "ORDER BY ca.apply_id DESC"
            )
        );

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        while (res->next()) {
            CanteenManagerApplyVO vo;
            vo.setApplyId(res->getInt("apply_id"));
            vo.setUserId(res->getInt("user_id"));
            vo.setUsername(res->getString("username"));
            vo.setAge(res->getInt("age"));
            vo.setPhone(res->getString("phone"));
            vo.setCanteenName(res->getString("canteen_name"));
            vo.setRegionId(res->getInt("region_id"));
            vo.setRegionName(res->isNull("region_name") ? "" : res->getString("region_name"));
            vo.setStatus(res->getInt("status"));
            vo.setApplyTime(res->getString("apply_time"));
            vo.setReviewTime(res->isNull("review_time") ? "" : res->getString("review_time"));
            vo.setReviewerId(res->isNull("reviewer_id") ? 0 : res->getInt("reviewer_id"));
            vo.setReviewerName(res->isNull("reviewer_name") ? "" : res->getString("reviewer_name"));

            list.push_back(vo);
        }
    } catch (const std::exception& e) {
        std::cerr << "[CanteenManagerApplyDAO::getApplyList] Error: " << e.what() << std::endl;
    }

    return list;
}

std::shared_ptr<CanteenManagerApplyVO> CanteenManagerApplyDAO::getApplyById(sql::Connection *conn, int apply_id)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT apply_id, user_id, canteen_name, status "
                ", region_id "
                "FROM canteen_manager_apply WHERE apply_id = ?"
            )
        );
        stmt->setInt(1, apply_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        if (res->next()) {
            auto vo = std::make_shared<CanteenManagerApplyVO>();
            vo->setApplyId(res->getInt("apply_id"));
            vo->setUserId(res->getInt("user_id"));
            vo->setCanteenName(res->getString("canteen_name"));
            vo->setRegionId(res->getInt("region_id"));
            vo->setStatus(res->getInt("status"));
            return vo;
        }
    } catch (const std::exception& e) {
        std::cerr << "[CanteenManagerApplyDAO::getApplyById] Error: " << e.what() << std::endl;
    }
    return nullptr;
}

bool CanteenManagerApplyDAO::reviewApply(sql::Connection *conn, int apply_id, int reviewer_id, int status)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "UPDATE canteen_manager_apply SET status = ?, reviewer_id = ?, review_time = NOW() "
                "WHERE apply_id = ? AND status = 0"
            )
        );
        stmt->setInt(1, status);
        stmt->setInt(2, reviewer_id);
        stmt->setInt(3, apply_id);

        return stmt->executeUpdate() > 0;
    } catch (const std::exception& e) {
        std::cerr << "[CanteenManagerApplyDAO::reviewApply] Error: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<Diner> DinerDAO::getDinerByUserId(sql::Connection *conn, int user_id)
{
    try {
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
    } catch (const std::exception& e) {
        std::cerr << "[DinerDAO::getDinerByUserId] Error: " << e.what() << std::endl;
    }
    return nullptr;
}

std::shared_ptr<UserCenterVO> DinerDAO::getDinerCenterByUserId(sql::Connection *conn, int user_id)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT u.username, u.age, u.phone, u.id_card, r.region_id, r.region_name, d.family_id, f.family_name, d.disease_history, d.taste_preference "
                "FROM users u JOIN diner d ON u.user_id = d.user_id "
                "JOIN region r ON r.region_id = d.region_id "
                "LEFT JOIN family f ON d.family_id = f.family_id "
                "WHERE u.user_id=?"
            )
        );

        stmt->setInt(1, user_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (res->next()) {
            auto vo = std::make_shared<UserCenterVO>();
            vo->setUserId(user_id);
            vo->setUsername(res->getString("username"));
            vo->setAge(res->getInt("age"));
            vo->setPhone(res->getString("phone"));
            vo->setIdCard(res->getString("id_card"));
            vo->setRegionId(res->getInt("region_id"));
            vo->setRegionName(res->getString("region_name"));
            vo->setFamilyId(res->getInt("family_id"));
            vo->setFamilyName(res->getString("family_name"));
            vo->setDiseaseHistory(res->getString("disease_history"));
            vo->setTastePreference(res->getString("taste_preference"));
            return vo;
        }
    } catch (const std::exception& e) {
        std::cerr << "[DinerDAO::getDinerCenterByUserId] Error: " << e.what() << std::endl;
    }
    return nullptr;
}

std::vector<FamilyMemberVO> DinerDAO::getFamilyMembersByUserId(sql::Connection *conn, int user_id)
{
    std::vector<FamilyMemberVO> list;

    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT u.user_id, u.username "
                "FROM users u "
                "JOIN diner d ON d.user_id = u.user_id "
                "WHERE d.user_id = ? "
                "OR ("
                "d.family_id = (SELECT family_id FROM diner WHERE user_id = ?) "
                "AND d.family_id IS NOT NULL "
                "AND d.family_id <> 1"
                ") "
                "ORDER BY u.user_id ASC"
            )
        );

        stmt->setInt(1, user_id);
        stmt->setInt(2, user_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            FamilyMemberVO member;
            member.setUserId(res->getInt("user_id"));
            member.setUsername(res->getString("username"));
            list.push_back(member);
        }
    } catch (const std::exception& e) {
        std::cerr << "[DinerDAO::getFamilyMembersByUserId] Error: " << e.what() << std::endl;
    }

    return list;
}

bool DinerDAO::updateDiner(sql::Connection *conn, const UserCenterVO& diner) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "UPDATE diner SET family_id=?, disease_history=?, taste_preference=?, region_id=?  "
                "WHERE user_id=?"
            )
        );

        stmt->setInt(1, diner.getFamilyId());
        stmt->setString(2, diner.getDiseaseHistory());
        stmt->setString(3, diner.getTastePreference());
        stmt->setInt(4, diner.getRegionId());
        stmt->setInt(5, diner.getUserId());
        if (stmt->executeUpdate() == 0) {
            std::cout<<"没有对diner表进行更新"<<std::endl; 
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[DinerDAO::updateDiner] Error: " << e.what() << std::endl;
        return false;
    }
}

bool DinerDAO::updateFamilyId(sql::Connection *conn, int user_id, int family_id) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "UPDATE diner SET family_id = ? WHERE user_id = ?"
            )
        );
        stmt->setInt(1, family_id);
        stmt->setInt(2, user_id);

        int affected_rows = stmt->executeUpdate();
        if (affected_rows == 0) {
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[DinerDAO::updateFamilyId] Error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<DinerInformation> DinerDAO::getDinerList(sql::Connection *conn)
{
    std::vector<DinerInformation> list;

    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT u.user_id, u.username, u.age, u.phone, u.status, r.region_id, r.region_name, d.disease_history, d.taste_preference "
                "FROM users u "
                "JOIN diner d ON u.user_id = d.user_id "
                "LEFT JOIN region r ON d.region_id = r.region_id "
            )
        );

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            DinerInformation dinerInfo;
            dinerInfo.setUserId(res->getInt("user_id"));
            dinerInfo.setUsername(res->getString("username"));
            dinerInfo.setAge(res->getInt("age"));
            dinerInfo.setPhone(res->getString("phone"));
            dinerInfo.setRegionId(res->getInt("region_id"));
            dinerInfo.setRegionName(res->getString("region_name"));
            dinerInfo.setStatus(res->getInt("status"));
            dinerInfo.setDiseaseHistory(res->getString("disease_history"));
            dinerInfo.setTastePreference(res->getString("taste_preference"));

            list.push_back(dinerInfo);
        }
    } catch (const std::exception& e) {
        std::cerr << "[DinerDAO::getDinerList] Error: " << e.what() << std::endl;
    }

    return list;
}
/***************************************************************************************
 * FamilyDao
***************************************************************************************/
int FamilyDAO::insertFamily(sql::Connection *conn, const Family& family) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO family(family_name) "
                "VALUES (?)")
        );

        stmt->setString(1, family.getName());

        if (stmt->executeUpdate() == 0) {
            return -1;
        }

        // 获取自增ID
        std::unique_ptr<sql::Statement> cstmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> rs(
            cstmt->executeQuery("SELECT LAST_INSERT_ID()")
        );

        int family_id = -1;
        if (rs->next()) {
            family_id = rs->getInt(1);
        }

        return family_id;
    } catch (const std::exception& e) {
        std::cerr << "[FamilyDAO::insertFamily] Error: " << e.what() << std::endl;
        return -1;
    }
}

Family FamilyDAO::getFamilyByUserId(sql::Connection *conn, int user_id) {
    Family family;

    try {
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
    } catch (const std::exception& e) {
        std::cerr << "[FamilyDAO::getFamilyByUserId] Error: " << e.what() << std::endl;
    }

    return family;
}

std::vector<Family> FamilyDAO::getFamilyList(sql::Connection *conn) {
    std::vector<Family> list;

    try {
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
    } catch (const std::exception& e) {
        std::cerr << "[FamilyDAO::getFamilyList] Error: " << e.what() << std::endl;
    }

    return list;
}

/***************************************************************************************
 * CanteenDao
 ***************************************************************************************/
std::vector<Canteen> CanteenDAO::getAllCanteens(sql::Connection *conn) {
    std::vector<Canteen> list;

    try {
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
    } catch (const std::exception& e) {
        std::cerr << "[CanteenDAO::getAllCanteens] Error: " << e.what() << std::endl;
    }

    return list;
}

std::shared_ptr<CanteenVO> CanteenDAO::getCanteenById(sql::Connection *conn, int id) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT c.canteen_id, c.name, c.address, r.region_id, r.region_name, c.status "
                "FROM canteen c "
                "JOIN region r ON r.region_id = c.region_id "
                "WHERE c.canteen_id = ?")
        );

        stmt->setInt(1, id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (res->next()) {
            auto c = std::make_shared<CanteenVO>();
            c->setId(res->getInt("canteen_id"));
            c->setName(res->getString("name"));
            c->setAddress(res->getString("address"));
            c->setRegionId(res->getInt("region_id"));
            c->setRegionName(res->getString("region_name"));
            c->setStatus(res->getInt("status"));
            return c;
        }
    } catch (const std::exception& e) {
        std::cerr << "[CanteenDAO::getCanteenById] Error: " << e.what() << std::endl;
    }

    return nullptr;
}

int CanteenDAO::getCanteenIdByUserId(sql::Connection *conn, int user_id) {
    try {
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
    } catch (const std::exception& e) {
        std::cerr << "[CanteenDAO::getCanteenIdByUserId] Error: " << e.what() << std::endl;
    }

    return -1;
}

int CanteenDAO::insertCanteen(sql::Connection *conn, const std::string& canteen_name, int region_id)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO canteen(name, region_id, status) VALUES (?, ?, 1)"
            )
        );
        stmt->setString(1, canteen_name);
        stmt->setInt(2, region_id);
        if (stmt->executeUpdate() == 0) {
            return -1;
        }

        std::unique_ptr<sql::Statement> cstmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> rs(
            cstmt->executeQuery("SELECT LAST_INSERT_ID()")
        );
        int canteen_id = -1;
        if (rs->next()) {
            canteen_id = rs->getInt(1);
        }

        // 插入早午晚三餐餐单
        auto menu_stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO daily_menu(canteen_id, meal_type) VALUES (?, ?)"
            )
        );
        std::vector<std::string> meal_types = {"早餐", "午餐", "晚餐"};
        for (const auto& meal_type : meal_types) {
            menu_stmt->setInt(1, canteen_id);
            menu_stmt->setString(2, meal_type);
            menu_stmt->executeUpdate();
        }

        return canteen_id;
    } catch (const std::exception& e) {
        std::cerr << "[CanteenDAO::insertCanteen] Error: " << e.what() << std::endl;
    }
    return -1;
}

bool CanteenDAO::updateCanteenAddress(sql::Connection *conn, int canteen_id, const std::string& address) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "UPDATE canteen SET address = ? WHERE canteen_id = ?"
            )
        );
        stmt->setString(1, address);
        stmt->setInt(2, canteen_id);

        int affected_rows = stmt->executeUpdate();
        if (affected_rows == 0) {
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[CanteenDAO::updateCanteenAddress] Error: " << e.what() << std::endl;
        return false;
    }
}

bool CanteenDAO::updateCanteenStatus(sql::Connection *conn, int canteen_id, int status) {
    try {
        // 开始事务
        TransactionGuard tx(conn);

        // 更新餐厅状态
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "UPDATE canteen SET status = ? WHERE canteen_id = ?"
            )
        );
        stmt->setInt(1, status);
        stmt->setInt(2, canteen_id);

        int affected_rows = stmt->executeUpdate();
        if (affected_rows == 0) {
            return false;
        }

        // 查询餐厅管理者 ID
        auto manager_stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT user_id FROM canteen_manager WHERE canteen_id = ?"
            )
        );
        manager_stmt->setInt(1, canteen_id);
        auto manager_res = std::unique_ptr<sql::ResultSet>(manager_stmt->executeQuery());

        if (manager_res->next()) {
            int manager_id = manager_res->getInt("user_id");
            
            // 更新管理者账号状态
            auto user_stmt = std::unique_ptr<sql::PreparedStatement>(
                conn->prepareStatement(
                    "UPDATE users SET status = ? WHERE user_id = ?"
                )
            );
            user_stmt->setInt(1, status);
            user_stmt->setInt(2, manager_id);
            user_stmt->executeUpdate();
        }

        // 提交事务
        tx.commit();

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[CanteenDAO::updateCanteenStatus] Error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<CanteenManagerVO> CanteenDAO::getCanteensWithManagers(sql::Connection *conn) {
    std::vector<CanteenManagerVO> canteens;
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT c.canteen_id, c.name, c.address, c.region_id, r.region_name, "
                "m.user_id as manager_id, u.username as manager_name, c.status "
                "FROM canteen c "
                "LEFT JOIN region r ON c.region_id = r.region_id "
                "LEFT JOIN canteen_manager m ON c.canteen_id = m.canteen_id "
                "LEFT JOIN users u ON m.user_id = u.user_id"
            )
        );

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        while (res->next()) {
            CanteenManagerVO canteen;
            canteen.setId(res->getInt("canteen_id"));
            canteen.setName(res->getString("name"));
            canteen.setAddress(res->getString("address"));
            canteen.setRegionId(res->getInt("region_id"));
            canteen.setRegionName(res->getString("region_name"));
            canteen.setManagerId(res->getInt("manager_id"));
            canteen.setManagerName(res->getString("manager_name"));
            canteen.setStatus(res->getInt("status"));
            canteen.setRating(0.0); // 后续计算
            canteen.setComplaintCount(0); // 后续计算
            canteens.push_back(canteen);
        }

        // 计算每个食堂的评分和投诉数量
        for (auto& canteen : canteens) {
            // 计算评分
            auto ratingStmt = std::unique_ptr<sql::PreparedStatement>(
                conn->prepareStatement("SELECT AVG(score) as avg_score FROM rating WHERE canteen_id = ?")
            );
            ratingStmt->setInt(1, canteen.getId());
            auto ratingRes = std::unique_ptr<sql::ResultSet>(ratingStmt->executeQuery());
            if (ratingRes->next() && !ratingRes->isNull("avg_score")) {
                canteen.setRating(ratingRes->getDouble("avg_score"));
            }

            // 计算投诉数量
            auto complaintStmt = std::unique_ptr<sql::PreparedStatement>(
                conn->prepareStatement("SELECT COUNT(*) as count FROM report WHERE canteen_id = ?")
            );
            complaintStmt->setInt(1, canteen.getId());
            auto complaintRes = std::unique_ptr<sql::ResultSet>(complaintStmt->executeQuery());
            if (complaintRes->next()) {
                canteen.setComplaintCount(complaintRes->getInt("count"));
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[CanteenDAO::getCanteensWithManagers] Error: " << e.what() << std::endl;
    }
    return canteens;
}

std::vector<PurchaseBill> CanteenDAO::getPurchaseBillsByCanteen(sql::Connection *conn, int canteen_id) {
    std::vector<PurchaseBill> bills;
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT * FROM purchase_bill WHERE canteen_id = ? ORDER BY purchase_date DESC")
        );
        stmt->setInt(1, canteen_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            PurchaseBill bill;
            bill.setId(res->getInt("bill_id"));
            bill.setCanteenId(res->getInt("canteen_id"));
            bill.setAmount(res->getDouble("amount"));
            bill.setPurchaseDate(res->getString("purchase_date"));
            bill.setRemark(res->getString("remark"));
            bills.push_back(bill);
        }
    } catch (const std::exception& e) {
        std::cerr << "[CanteenDAO::getPurchaseBillsByCanteen] Error: " << e.what() << std::endl;
    }
    return bills;
}

int CanteenDAO::createPurchaseBill(sql::Connection *conn, const PurchaseBill& bill) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("INSERT INTO purchase_bill (canteen_id, amount, purchase_date, remark) VALUES (?, ?, ?, ?)")
        );
        stmt->setInt(1, bill.getCanteenId());
        stmt->setDouble(2, bill.getAmount());
        stmt->setString(3, bill.getPurchaseDate());
        stmt->setString(4, bill.getRemark());

        stmt->executeUpdate();

        // 获取生成的ID
        auto idStmt = std::unique_ptr<sql::Statement>(conn->createStatement());
        auto res = std::unique_ptr<sql::ResultSet>(idStmt->executeQuery("SELECT LAST_INSERT_ID()"));
        if (res->next()) {
            return res->getInt(1);
        }
    } catch (const std::exception& e) {
        std::cerr << "[CanteenDAO::createPurchaseBill] Error: " << e.what() << std::endl;
    }
    return -1;
}

bool CanteenDAO::updatePurchaseBill(sql::Connection *conn, const PurchaseBill& bill) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("UPDATE purchase_bill SET amount = ?, purchase_date = ?, remark = ? WHERE bill_id = ?")
        );
        stmt->setDouble(1, bill.getAmount());
        stmt->setString(2, bill.getPurchaseDate());
        stmt->setString(3, bill.getRemark());
        stmt->setInt(4, bill.getId());

        int affected_rows = stmt->executeUpdate();
        return affected_rows > 0;
    } catch (const std::exception& e) {
        std::cerr << "[CanteenDAO::updatePurchaseBill] Error: " << e.what() << std::endl;
        return false;
    }
}

bool CanteenDAO::deletePurchaseBill(sql::Connection *conn, int bill_id) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("DELETE FROM purchase_bill WHERE bill_id = ?")
        );
        stmt->setInt(1, bill_id);

        int affected_rows = stmt->executeUpdate();
        return affected_rows > 0;
    } catch (const std::exception& e) {
        std::cerr << "[CanteenDAO::deletePurchaseBill] Error: " << e.what() << std::endl;
        return false;
    }
}

/***************************************************************************************
 * DishDao
 ***************************************************************************************/
std::vector<Dish> DishDAO::getDishesByCanteen(sql::Connection *conn, int canteen_id)
{
    std::vector<Dish> list;

    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT * FROM dish WHERE canteen_id=? ")
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
            d.setStatus(res->getInt("status"));
            list.push_back(d);
        }
    } catch (const std::exception& e) {
        std::cerr << "查询菜品失败: " << e.what() << std::endl;
    }

    return list;
}

int DishDAO::insertDish(sql::Connection *conn, const Dish& dish){
    try {
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
            return -1;
        }

        std::unique_ptr<sql::Statement> cstmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> rs(
            cstmt->executeQuery("SELECT LAST_INSERT_ID()")
        );

        int dish_id = -1;
        if (rs->next()) {
            dish_id = rs->getInt(1);
        }

        return dish_id;
    } catch (const std::exception& e) {
        std::cerr << "插入菜品失败: " << e.what() << std::endl;
        return -1;
    }

}
bool DishDAO::disableDishByDishId(sql::Connection *conn, const int dish_id){
    try {
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
        std::cerr << "下架菜品失败: " << e.what() << std::endl;
        return false;
    }

}

int DishDAO::updateDish(sql::Connection *conn, int dish_id, double price, int calories, const std::string& nutrition_info) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "UPDATE dish SET price = ?, calories = ?, nutrition_info = ? WHERE dish_id = ?"
            )
        );
        stmt->setDouble(1, price);
        stmt->setInt(2, calories);
        stmt->setString(3, nutrition_info);
        stmt->setInt(4, dish_id);

        return stmt->executeUpdate();
    } catch (const std::exception& e) {
        std::cerr << "更新菜品失败: " << e.what() << std::endl;
        return -1;
    }
}

bool DishDAO::enableDishByDishId(sql::Connection *conn, const int dish_id){
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "UPDATE dish SET status = 1 WHERE dish_id = ? "
            )
        );
        stmt->setInt(1, dish_id);

        if (stmt->executeUpdate() == 0) {
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "上架菜品失败: " << e.what() << std::endl;
        return false;
    }

}

/***************************************************************************************
 * MenuDao
 ***************************************************************************************/
std::vector<Dish> MenuDAO::getMenuByMealType(sql::Connection *conn, int canteen_id, const std::string& meal_type) {
    std::vector<Dish> list;

    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT d.* FROM daily_menu m "
                "JOIN menu_dish md ON m.menu_id = md.menu_id "
                "JOIN dish d ON md.dish_id = d.dish_id "
                "WHERE m.canteen_id=? AND m.meal_type=?"
            )
        );

        stmt->setInt(1, canteen_id);
        stmt->setString(2, meal_type);

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            Dish d;
            d.setId(res->getInt("dish_id"));
            d.setName(res->getString("name"));
            d.setPrice(res->getDouble("price"));
            d.setType(res->getString("type"));
            d.setCalories(res->getDouble("calories"));
            d.setNutritionInfo(res->getString("nutrition_info"));
            list.push_back(d);
        }
    } catch (const std::exception& e) {
        std::cerr << "[DishDAO::getDishesByCanteen] Error: " << e.what() << std::endl;
    }

    return list;
}

std::vector<CanteenMenuVO> MenuDAO::getMenuByCanteen(sql::Connection *conn, int canteen_id) {
    std::vector<CanteenMenuVO> list;
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT m.menu_id, m.meal_type, d.dish_id, d.name, d.price "
                "FROM daily_menu m "
                "LEFT JOIN menu_dish md ON m.menu_id = md.menu_id "
                "LEFT JOIN dish d ON md.dish_id = d.dish_id "
                "WHERE m.canteen_id = ? "
                "ORDER BY m.meal_type"
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
                menu.setType(res->getString("meal_type"));
                menuMap[menu_id] = menu;
            }

            if (!res->isNull("dish_id")) {
                Dish dish;
                dish.setId(res->getInt("dish_id"));
                dish.setName(res->getString("name"));
                dish.setPrice(res->getDouble("price"));

                menuMap[menu_id].addDish(dish);
            }
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


int MenuDAO::getMenuIdByCanteenAndMealType(sql::Connection *conn, int canteen_id, const std::string& meal_type) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT menu_id FROM daily_menu WHERE canteen_id = ? AND meal_type = ?"
            )
        );

        stmt->setInt(1, canteen_id);
        stmt->setString(2, meal_type);

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (res->next()) {
            return res->getInt("menu_id");
        }
    } catch (const std::exception& e) {
        std::cerr << "获取餐单 ID 失败: " << e.what() << std::endl;
    }

    return -1;
}

bool MenuDAO::isDishInMenu(sql::Connection *conn, int dish_id) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT COUNT(*) FROM menu_dish WHERE dish_id = ?"
            )
        );

        stmt->setInt(1, dish_id);

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (res->next()) {
            return res->getInt(1) > 0;
        }

        return false;
    } catch (const std::exception& e) {
        std::cerr << "检查菜品是否在餐单中失败: " << e.what() << std::endl;
        return false;
    }
}

bool MenuDAO::deleteMenuDish(sql::Connection *conn, int menu_id, int dish_id) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("DELETE FROM menu_dish WHERE menu_id = ? AND dish_id = ?")
        );
        stmt->setInt(1, menu_id);
        stmt->setInt(2, dish_id);
        stmt->executeUpdate();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "删除餐单菜品关联失败: " << e.what() << std::endl;
        return false;
    }
}

bool MenuDAO::insertMenuDish(sql::Connection *conn, int menu_id, int dish_id) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("INSERT INTO menu_dish(menu_id, dish_id) VALUES (?, ?)")
        );
        stmt->setInt(1, menu_id);
        stmt->setInt(2, dish_id);
        return stmt->executeUpdate() > 0;
    } catch (const std::exception& e) {
        std::cerr << "插入餐单菜品失败: " << e.what() << std::endl;
        return false;
    }
}

std::vector<int> MenuDAO::getMenuDishIds(sql::Connection *conn, int menu_id) {
    std::vector<int> dish_ids;
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT dish_id FROM menu_dish WHERE menu_id = ?")
        );
        stmt->setInt(1, menu_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        while (res->next()) {
            dish_ids.push_back(res->getInt("dish_id"));
        }
    } catch (const std::exception& e) {
        std::cerr << "获取餐单菜品ID失败: " << e.what() << std::endl;
    }
    return dish_ids;
}

/***************************************************************************************
 * OrderDao
 ***************************************************************************************/
 int OrderDAO::insertOrder(sql::Connection *conn, const Order& order) {
    try {
        // 1️⃣ 插入订单
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO `orders`(user_id, order_for_user_id, canteen_id, total_price, order_time, status, discount_rate, original_total, saved_amount) "
                "VALUES (?, ?, ?, ?, NOW(), ?, ?, ?, ?)"
            )
        );

        stmt->setInt(1, order.getUserId());
        stmt->setInt(2, order.getOrderForUserId());
        stmt->setInt(3, order.getCanteenId());
        stmt->setDouble(4, order.getTotalPrice());
        stmt->setInt(5, order.getStatus());
        stmt->setDouble(6, order.getDiscountRate());
        stmt->setDouble(7, order.getOriginalTotal());
        stmt->setDouble(8, order.getSavedAmount());

        if (stmt->executeUpdate() == 0) {
            return -1;
        }

        // 获取自增ID
        auto cstmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT LAST_INSERT_ID()")
        );
        std::unique_ptr<sql::ResultSet> rs(
            cstmt->executeQuery()
        );

        int order_id = -1;
        if (rs->next()) {
            order_id = rs->getInt(1);
        }

        return order_id;
    } catch (const std::exception& e) {
        std::cerr << "插入订单失败: " << e.what() << std::endl;
        return -1;
    }
}

std::vector<OrderVO> OrderDAO::getOrdersByUser(sql::Connection *conn, int user_id)
{
    std::vector<OrderVO> list;

    try {
    auto stmt = std::unique_ptr<sql::PreparedStatement>(
        conn->prepareStatement(R"(
             SELECT 
                o.order_id AS order_id,
                o.canteen_id AS canteen_id,
                u.username AS order_for_user_name,
                c.name AS canteen_name,
                o.total_price,
                o.discount_rate,
                o.original_total,
                o.saved_amount,
                o.status,
                o.order_time,
                r.score AS rating_score,
                r.comment AS rating_comment,
                r.time AS rating_time
            FROM orders o
            JOIN canteen c ON o.canteen_id = c.canteen_id
            JOIN users u ON u.user_id  = o.order_for_user_id 
            LEFT JOIN rating r ON r.order_id = o.order_id AND r.user_id = o.user_id
            WHERE o.user_id = ?
            ORDER BY o.order_id DESC
        )")
    );

        stmt->setInt(1, user_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            OrderVO o;
            o.setOrderId(res->getInt("order_id"));
            o.setCanteenId(res->getInt("canteen_id"));
            o.setOrderForUserName(res->getString("order_for_user_name"));
            o.setCanteenName(res->getString("canteen_name"));
            o.setTotalPrice(std::stod(res->getString("total_price").c_str()));
            o.setDiscountRate(std::stod(res->getString("discount_rate").c_str()));
            o.setOriginalTotal(std::stod(res->getString("original_total").c_str()));
            o.setSavedAmount(std::stod(res->getString("saved_amount").c_str()));
            o.setStatus(res->getInt("status"));
            o.setCreateTime(res->getString("order_time"));
            o.setHasRating(!res->isNull("rating_score"));
            if (!res->isNull("rating_score")) {
                o.setRatingScore(res->getInt("rating_score"));
                o.setRatingComment(res->isNull("rating_comment") ? "" : res->getString("rating_comment"));
                o.setRatingTime(res->isNull("rating_time") ? "" : res->getString("rating_time"));
            }

            list.push_back(o);
        }
    } catch (const std::exception& e) {
        std::cerr << "[OrderDAO::getOrdersByUser] Error: " << e.what() << std::endl;
    }

    return list;
}

std::vector<OrderVO> OrderDAO::getOrdersByCanteen(sql::Connection *conn, int canteen_id)
{
    std::vector<OrderVO> list;

    try {
    auto stmt = std::unique_ptr<sql::PreparedStatement>(
        conn->prepareStatement(R"(
             SELECT 
                o.order_id AS order_id,
                o.canteen_id AS canteen_id,
                u.username AS order_for_user_name,
                c.name AS canteen_name,
                o.total_price,
                o.discount_rate,
                o.original_total,
                o.saved_amount,
                o.status,
                o.order_time,
                r.score AS rating_score,
                r.comment AS rating_comment,
                r.time AS rating_time
            FROM orders o
            JOIN canteen c ON o.canteen_id = c.canteen_id
            JOIN users u ON u.user_id  = o.order_for_user_id 
            LEFT JOIN rating r ON r.order_id = o.order_id AND r.user_id = o.user_id
            WHERE o.canteen_id = ?
            ORDER BY o.status ASC, o.order_time DESC
        )")
    );

        stmt->setInt(1, canteen_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            OrderVO o;
            o.setOrderId(res->getInt("order_id"));
            o.setCanteenId(res->getInt("canteen_id"));
            o.setOrderForUserName(res->getString("order_for_user_name"));
            o.setCanteenName(res->getString("canteen_name"));
            o.setTotalPrice(std::stod(res->getString("total_price").c_str()));
            o.setDiscountRate(std::stod(res->getString("discount_rate").c_str()));
            o.setOriginalTotal(std::stod(res->getString("original_total").c_str()));
            o.setSavedAmount(std::stod(res->getString("saved_amount").c_str()));
            o.setStatus(res->getInt("status"));
            o.setCreateTime(res->getString("order_time"));
            o.setHasRating(!res->isNull("rating_score"));
            if (!res->isNull("rating_score")) {
                o.setRatingScore(res->getInt("rating_score"));
                o.setRatingComment(res->isNull("rating_comment") ? "" : res->getString("rating_comment"));
                o.setRatingTime(res->isNull("rating_time") ? "" : res->getString("rating_time"));
            }

            list.push_back(o);
        }
    } catch (const std::exception& e) {
        std::cerr << "[OrderDAO::getOrdersByCanteen] Error: " << e.what() << std::endl;
    }

    return list;
}

bool OrderDAO::updateOrderStatus(sql::Connection *conn, int order_id, int status)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("UPDATE orders SET status = ? WHERE order_id = ?")
        );

        stmt->setInt(1, status);
        stmt->setInt(2, order_id);

        return stmt->executeUpdate() > 0;
    } catch (const std::exception& e) {
        std::cerr << "更新订单状态失败: " << e.what() << std::endl;
        return false;
    }
}

std::vector<OrderDetailVO> OrderDAO::getOrdersDetailsByOrderId(sql::Connection *conn, int order_id)
{
    std::vector<OrderDetailVO> list;

    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(R"(
                SELECT 
                    d.name AS dish_name,
                    oi.unit_price,
                    oi.discount_price,
                    oi.quantity,
                    oi.subtotal
                FROM orders o
                JOIN order_item oi ON oi.order_id = o.order_id
                JOIN dish d ON oi.dish_id = d.dish_id
                WHERE o.order_id = ?
                ORDER BY o.order_id DESC
            )")
        );

        stmt->setInt(1, order_id);

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            OrderDetailVO vo(
                res->getString("dish_name"),
                res->getInt("quantity"),
                std::stod(res->getString("unit_price").c_str()),
                std::stod(res->getString("discount_price").c_str()),
                std::stod(res->getString("subtotal").c_str())
            );

            list.push_back(vo);
        }

    } catch (const std::exception& e) {
        std::cout << "查询订单失败: " << e.what() << std::endl;
    }

    return list;
}

std::shared_ptr<Order> OrderDAO::getOrderById(sql::Connection *conn, int order_id)
{
    std::shared_ptr<Order> order = std::make_shared<Order>();
    try{
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT order_id, user_id, order_for_user_id, canteen_id, total_price, order_time, status, discount_rate, original_total, saved_amount FROM orders WHERE order_id = ?")
        );
        stmt->setInt(1, order_id);

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        if (res->next()) {
            order->setId(res->getInt("order_id"));
            order->setUserId(res->getInt("user_id"));
            order->setOrderForUserId(res->getInt("order_for_user_id"));
            order->setCanteenId(res->getInt("canteen_id"));
            order->setTotalPrice(res->getDouble("total_price"));
            order->setOrderTime(res->getString("order_time"));
            order->setStatus(res->getInt("status"));
            order->setDiscountRate(res->getDouble("discount_rate"));
            order->setOriginalTotal(res->getDouble("original_total"));
            order->setSavedAmount(res->getDouble("saved_amount"));
        }
    } catch (const std::exception& e) {
        std::cerr << "获取订单失败: " << e.what() << std::endl;
    }
    return order;
}

std::shared_ptr<RecentOrderVO> OrderDAO::getRecentOrder(sql::Connection *conn, int user_id, int order_for_user_id, int canteen_id) {
    try {
        // 获取最近的订单ID
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(R"(
                SELECT order_id
                FROM orders
                WHERE order_for_user_id = ? AND canteen_id = ? AND status = 1 
                ORDER BY order_time DESC
                LIMIT 1
            )")
        );

        stmt->setInt(1, order_for_user_id);
        stmt->setInt(2, canteen_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (!res->next()) {
            return nullptr;
        }

        int order_id = res->getInt("order_id");

        // 获取订单详情
        auto detail_stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(R"(
                SELECT
                    o.order_id,
                    o.canteen_id,
                    c.name AS canteen_name,
                    o.order_time,
                    o.total_price,
                    o.discount_rate,
                    o.original_total,
                    o.saved_amount,
                    d.dish_id,
                    d.name AS dish_name,
                    d.price AS unit_price,
                    oi.discount_price,
                    oi.quantity,
                    oi.subtotal
                FROM orders o
                JOIN order_item oi ON o.order_id = oi.order_id
                JOIN dish d ON oi.dish_id = d.dish_id
                JOIN canteen c ON o.canteen_id = c.canteen_id
                WHERE o.order_id = ?
                ORDER BY d.dish_id ASC
            )")
        );

        detail_stmt->setInt(1, order_id);
        auto detail_res = std::unique_ptr<sql::ResultSet>(detail_stmt->executeQuery());

        if (!detail_res->next()) {
            return nullptr;
        }

        auto vo = std::make_shared<RecentOrderVO>();
        vo->setOrderId(detail_res->getInt("order_id"));
        vo->setCanteenId(detail_res->getInt("canteen_id"));
        vo->setCanteenName(detail_res->getString("canteen_name"));
        vo->setOrderTime(detail_res->getString("order_time"));
        vo->setTotalPrice(detail_res->getDouble("total_price"));
        vo->setDiscountRate(detail_res->getDouble("discount_rate"));
        vo->setOriginalTotal(detail_res->getDouble("original_total"));
        vo->setSavedAmount(detail_res->getDouble("saved_amount"));

        // 添加第一个菜品
        RecentOrderItemVO item;
        item.setDishId(detail_res->getInt("dish_id"));
        item.setDishName(detail_res->getString("dish_name"));
        item.setQuantity(detail_res->getInt("quantity"));
        item.setUnitPrice(detail_res->getDouble("unit_price"));
        item.setDiscountPrice(detail_res->getDouble("discount_price"));
        item.setSubtotal(detail_res->getDouble("subtotal"));
        vo->addItem(item);

        // 添加剩余菜品
        while (detail_res->next()) {
            RecentOrderItemVO item;
            item.setDishId(detail_res->getInt("dish_id"));
            item.setDishName(detail_res->getString("dish_name"));
            item.setQuantity(detail_res->getInt("quantity"));
            item.setUnitPrice(detail_res->getDouble("unit_price"));
            item.setDiscountPrice(detail_res->getDouble("discount_price"));
            item.setSubtotal(detail_res->getDouble("subtotal"));
            vo->addItem(item);
        }

        return vo;
    } catch (const std::exception& e) {
        std::cout << "获取最近订单失败: " << e.what() << std::endl;
        return nullptr;
    }
}

// 用餐偏好相关方法
DiningPreferenceSummary OrderDAO::getDiningPreferenceSummary(sql::Connection *conn, int user_id, const std::string& time_dimension) {
    DiningPreferenceSummary summary;
    try {
        // 构建时间条件
        std::string time_condition = "";
        if (time_dimension == "year") {
            time_condition = "AND DATE(o.order_time) >= DATE_SUB(NOW(), INTERVAL 1 YEAR)";
        } else if (time_dimension == "quarter") {
            time_condition = "AND DATE(o.order_time) >= DATE_SUB(NOW(), INTERVAL 3 MONTH)";
        } else if (time_dimension == "month") {
            time_condition = "AND DATE(o.order_time) >= DATE_SUB(NOW(), INTERVAL 1 MONTH)";
        } else if (time_dimension == "week") {
            time_condition = "AND DATE(o.order_time) >= DATE_SUB(NOW(), INTERVAL 1 WEEK)";
        } else if (time_dimension == "day") {
            time_condition = "AND DATE(o.order_time) = DATE(NOW())";
        }
        
        // 查询消费总额和订单数
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT COALESCE(SUM(total_price), 0) as total_amount, COUNT(*) as order_count "
                "FROM orders o "
                "WHERE o.order_for_user_id = ? AND o.status = 1 " + time_condition 
            )
        );
        stmt->setInt(1, user_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        if (res->next()) {
            summary.setTotalAmount(res->getDouble("total_amount"));
            summary.setOrderCount(res->getInt("order_count"));
        }
        
        // 查询消费餐厅数
        stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT COUNT(DISTINCT canteen_id) as canteen_count "
                "FROM orders o "
                "WHERE o.order_for_user_id = ? " + time_condition
            )
        );
        stmt->setInt(1, user_id);
        res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        if (res->next()) {
            summary.setCanteenCount(res->getInt("canteen_count"));
        }
    } catch (const std::exception& e) {
        std::cerr << "获取用餐偏好摘要失败: " << e.what() << std::endl;
    }
    return summary;
}

std::vector<std::pair<std::string, int>> OrderDAO::getCanteenConsumptionCount(sql::Connection *conn, int user_id, const std::string& time_dimension) {
    std::vector<std::pair<std::string, int>> result;
    try {
        // 构建时间条件
        std::string time_condition = "";
        if (time_dimension == "year") {
            time_condition = "AND DATE(o.order_time) >= DATE_SUB(NOW(), INTERVAL 1 YEAR)";
        } else if (time_dimension == "quarter") {
            time_condition = "AND DATE(o.order_time) >= DATE_SUB(NOW(), INTERVAL 3 MONTH)";
        } else if (time_dimension == "month") {
            time_condition = "AND DATE(o.order_time) >= DATE_SUB(NOW(), INTERVAL 1 MONTH)";
        } else if (time_dimension == "week") {
            time_condition = "AND DATE(o.order_time) >= DATE_SUB(NOW(), INTERVAL 1 WEEK)";
        } else if (time_dimension == "day") {
            time_condition = "AND DATE(o.order_time) = DATE(NOW())";
        }
        
        // 查询餐厅消费次数
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT c.name as canteen_name, COUNT(*) as count "
                "FROM orders o "
                "JOIN canteen c ON o.canteen_id = c.canteen_id "
                "WHERE o.order_for_user_id = ? AND o.status = 1 " + time_condition + " "
                "GROUP BY c.canteen_id, c.name "
                "ORDER BY count DESC"
            )
        );
        stmt->setInt(1, user_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        
        int total_count = 0;
        while (res->next()) {
            std::string canteen_name = res->getString("canteen_name");
            int count = res->getInt("count");
            result.emplace_back(canteen_name, count);
            total_count += count;
        }
        
        // 处理比例低于5%的情况
        if (total_count > 0) {
            std::vector<std::pair<std::string, int>> filtered_result;
            int other_count = 0;
            for (const auto& item : result) {
                double percentage = static_cast<double>(item.second) / total_count * 100;
                if (percentage >= 5) {
                    filtered_result.push_back(item);
                } else {
                    other_count += item.second;
                }
            }
            if (other_count > 0) {
                filtered_result.emplace_back("其他餐厅", other_count);
            }
            result = filtered_result;
        }
    } catch (const std::exception& e) {
        std::cerr << "获取餐厅消费次数失败: " << e.what() << std::endl;
    }
    return result;
}

std::vector<std::pair<std::string, int>> OrderDAO::getDishConsumptionCount(sql::Connection *conn, int user_id, const std::string& time_dimension) {
    std::vector<std::pair<std::string, int>> result;
    try {
        // 构建时间条件
        std::string time_condition = "";
        if (time_dimension == "year") {
            time_condition = "AND DATE(o.order_time) >= DATE_SUB(NOW(), INTERVAL 1 YEAR)";
        } else if (time_dimension == "quarter") {
            time_condition = "AND DATE(o.order_time) >= DATE_SUB(NOW(), INTERVAL 3 MONTH)";
        } else if (time_dimension == "month") {
            time_condition = "AND DATE(o.order_time) >= DATE_SUB(NOW(), INTERVAL 1 MONTH)";
        } else if (time_dimension == "week") {
            time_condition = "AND DATE(o.order_time) >= DATE_SUB(NOW(), INTERVAL 1 WEEK)";
        } else if (time_dimension == "day") {
            time_condition = "AND DATE(o.order_time) = DATE(NOW())";
        }
        
        // 查询菜品消费次数
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT CONCAT(d.name, ' (', c.name, ')') as dish_with_canteen, SUM(oi.quantity) as count "
                "FROM orders o "
                "JOIN order_item oi ON o.order_id = oi.order_id "
                "JOIN dish d ON oi.dish_id = d.dish_id "
                "JOIN canteen c ON d.canteen_id = c.canteen_id "
                "WHERE o.order_for_user_id = ? AND o.status = 1 " + time_condition + " "
                "GROUP BY d.dish_id, d.name, c.name "
                "ORDER BY count DESC"
            )
        );
        stmt->setInt(1, user_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        
        int total_count = 0;
        while (res->next()) {
            std::string dish_with_canteen = res->getString("dish_with_canteen");
            int count = res->getInt("count");
            result.emplace_back(dish_with_canteen, count);
            total_count += count;
        }
        
        // 处理比例低于5%的情况
        if (total_count > 0) {
            std::vector<std::pair<std::string, int>> filtered_result;
            int other_count = 0;
            for (const auto& item : result) {
                double percentage = static_cast<double>(item.second) / total_count * 100;
                if (percentage >= 5) {
                    filtered_result.push_back(item);
                } else {
                    other_count += item.second;
                }
            }
            if (other_count > 0) {
                filtered_result.emplace_back("其他菜品", other_count);
            }
            result = filtered_result;
        }
    } catch (const std::exception& e) {
        std::cerr << "获取菜品消费次数失败: " << e.what() << std::endl;
    }
    return result;
}

/***************************************************************************************
 * OrderItemDao
 ***************************************************************************************/
bool OrderItemDAO::insertOrderItems(sql::Connection *conn, int order_id, const std::vector<OrderItem>& items) {
    try {
        for (const auto& item : items) {
            auto stmt = std::unique_ptr<sql::PreparedStatement>(
                conn->prepareStatement(
                    "INSERT INTO order_item(order_id, dish_id, quantity, unit_price, discount_price, subtotal) VALUES (?, ?, ?, ?, ?, ?)"
                )
            );

            stmt->setInt(1, order_id);
            stmt->setInt(2, item.getDishId());
            stmt->setInt(3, item.getQuantity());
            stmt->setDouble(4, item.getUnitPrice());
            stmt->setDouble(5, item.getDiscountPrice());
            stmt->setDouble(6, item.getSubtotal());

            if (stmt->executeUpdate() == 0) {
                return false;
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[OrderDAO::insertOrder] Error: " << e.what() << std::endl;
        return false;
    }
}

/***************************************************************************************
 * RatingDao
 ***************************************************************************************/
bool RatingDAO::insertRating(sql::Connection *conn, const Rating& rating) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO rating(user_id, canteen_id, order_id, score, comment, time) "
                "SELECT ?, ?, ?, ?, ?, NOW() "
                "FROM orders o "
                "WHERE o.order_id = ? AND o.user_id = ? AND o.canteen_id = ? "
                "AND NOT EXISTS (SELECT 1 FROM rating r WHERE r.user_id = ? AND r.order_id = ?)"
            )
        );

        stmt->setInt(1, rating.getUserId());
        stmt->setInt(2, rating.getCanteenId());
        stmt->setInt(3, rating.getOrderId());
        stmt->setInt(4, rating.getScore());
        stmt->setString(5, rating.getComment());
        stmt->setInt(6, rating.getOrderId());
        stmt->setInt(7, rating.getUserId());
        stmt->setInt(8, rating.getCanteenId());
        stmt->setInt(9, rating.getUserId());
        stmt->setInt(10, rating.getOrderId());

        return stmt->executeUpdate() > 0;
    } catch (const std::exception& e) {
        std::cerr << "[RatingDAO::insertRating] Error: " << e.what() << std::endl;
        return false;
    }
}

 std::vector<Rating> RatingDAO::getRatingsByCanteen(sql::Connection *conn, int canteen_id)
 {
    std::vector<Rating> list;

    try {
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
    } catch (const std::exception& e) {
        std::cerr << "[RatingDAO::getRatingsByCanteen] Error: " << e.what() << std::endl;
    }

    return list;
}

std::vector<CanteenRatingVO> RatingDAO::getCanteenRatingDetails(sql::Connection *conn, int canteen_id)
{
    std::vector<CanteenRatingVO> list;

    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(R"(
                SELECT
                    r.order_id,
                    r.user_id,
                    u.username,
                    r.score,
                    r.comment,
                    r.time,
                    d.name AS dish_name
                FROM rating r
                JOIN users u ON u.user_id = r.user_id
                LEFT JOIN order_item oi ON oi.order_id = r.order_id
                LEFT JOIN dish d ON d.dish_id = oi.dish_id
                WHERE r.canteen_id = ?
                ORDER BY r.time DESC, r.order_id DESC, d.dish_id ASC
            )")
        );

        stmt->setInt(1, canteen_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        std::map<std::pair<int, int>, CanteenRatingVO> grouped;

        while (res->next()) {
            int order_id = res->getInt("order_id");
            int user_id = res->getInt("user_id");
            std::pair<int, int> key = {order_id, user_id};

            if (grouped.find(key) == grouped.end()) {
                CanteenRatingVO vo;
                vo.setOrderId(order_id);
                vo.setUserId(user_id);
                vo.setUsername(res->getString("username"));
                vo.setScore(res->getInt("score"));
                vo.setComment(res->getString("comment"));
                vo.setTime(res->getString("time"));
                grouped[key] = vo;
            }

            if (!res->isNull("dish_name")) {
                grouped[key].addDish(res->getString("dish_name"));
            }
        }

        for (auto& [_, vo] : grouped) {
            list.push_back(vo);
        }
    } catch (const std::exception& e) {
        std::cerr << "[RatingDAO::getCanteenRatingDetails] Error: " << e.what() << std::endl;
    }

    return list;
}

/***************************************************************************************
 * ReportDao
***************************************************************************************/
bool ReportDAO::insertReport(sql::Connection *conn, const Report& report) {
    try {
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
    } catch (const std::exception& e) {
        std::cerr << "[ReportDAO::insertReport] Error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<Report> ReportDAO::getReportsByCanteen(sql::Connection *conn, int canteen_id)
{
    std::vector<Report> list;

    try {
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
    } catch (const std::exception& e) {
        std::cerr << "[ReportDAO::getReportsByCanteen] Error: " << e.what() << std::endl;
    }

    return list;
}

std::vector<ReportVO> ReportDAO::getAllReports(sql::Connection *conn)
{
    std::vector<ReportVO> list;

    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(R"(
                SELECT
                    r.report_id,
                    r.user_id,
                    u.username,
                    r.canteen_id,
                    c.name AS canteen_name,
                    r.type,
                    r.content,
                    r.status,
                    r.create_time,
                    r.handle_time,
                    r.handler_id,
                    hu.username AS handler_name
                FROM report r
                JOIN users u ON u.user_id = r.user_id
                JOIN canteen c ON c.canteen_id = r.canteen_id
                LEFT JOIN users hu ON hu.user_id = r.handler_id
                ORDER BY r.create_time DESC, r.report_id DESC
            )")
        );

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        while (res->next()) {
            ReportVO vo;
            vo.setReportId(res->getInt("report_id"));
            vo.setUserId(res->getInt("user_id"));
            vo.setUsername(res->getString("username"));
            vo.setCanteenId(res->getInt("canteen_id"));
            vo.setCanteenName(res->getString("canteen_name"));
            vo.setType(res->getInt("type"));
            vo.setContent(res->getString("content"));
            vo.setStatus(res->getInt("status"));
            vo.setCreateTime(res->getString("create_time"));
            if (!res->isNull("handle_time")) {
                vo.setHandleTime(res->getString("handle_time"));
            }
            if (!res->isNull("handler_id")) {
                vo.setHandlerId(res->getInt("handler_id"));
            }
            if (!res->isNull("handler_name")) {
                vo.setHandlerName(res->getString("handler_name"));
            }
            list.push_back(vo);
        }
    } catch (const std::exception& e) {
        std::cerr << "[ReportDAO::getAllReports] Error: " << e.what() << std::endl;
    }

    return list;
}

bool ReportDAO::updateReportStatus(sql::Connection *conn, int report_id, int status, int handler_id)
{
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "UPDATE report SET status = ?, handler_id = ?, handle_time = NOW() "
                "WHERE report_id = ? AND status = 0"
            )
        );

        stmt->setInt(1, status);
        stmt->setInt(2, handler_id);
        stmt->setInt(3, report_id);
        return stmt->executeUpdate() > 0;
    } catch (const std::exception& e) {
        std::cerr << "[ReportDAO::updateReportStatus] Error: " << e.what() << std::endl;
        return false;
    }
}

/***************************************************************************************
 * AnnouncementDao
***************************************************************************************/
bool AnnouncementDAO::insertAnnouncement(sql::Connection *conn, const Announcement& announcement) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO announcement(title, content, publisher_id, publish_time) "
                "VALUES (?, ?, ?, NOW())"
            )
        );

        stmt->setString(1, announcement.getTitle());
        stmt->setString(2, announcement.getContent());
        stmt->setInt(3, announcement.getPublisherId());

        return stmt->executeUpdate() > 0;
    } catch (const std::exception& e) {
        std::cerr << "[AnnouncementDAO::insertAnnouncement] Error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<AnnouncementVO> AnnouncementDAO::getAnnouncementList(sql::Connection *conn) {
    std::vector<AnnouncementVO> list;
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT a.announce_id, a.title, a.content, a.publish_time, a.publisher_id, u.username "
                "FROM announcement a "
                "JOIN users u ON u.user_id = a.publisher_id "
                "ORDER BY a.publish_time DESC, a.announce_id DESC"
            )
        );

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        while (res->next()) {
            AnnouncementVO vo;
            vo.setId(res->getInt("announce_id"));
            vo.setTitle(res->getString("title"));
            vo.setContent(res->getString("content"));
            vo.setPublishTime(res->getString("publish_time"));
            vo.setPublisherId(res->getInt("publisher_id"));
            vo.setPublisherName(res->getString("username"));
            list.push_back(vo);
        }
    } catch (const std::exception& e) {
        std::cerr << "[AnnouncementDAO::getAnnouncementList] Error: " << e.what() << std::endl;
    }

    return list;
}

bool AnnouncementDAO::deleteAnnouncement(sql::Connection *conn, int announce_id, int publisher_id) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("DELETE FROM announcement WHERE announce_id = ? AND publisher_id = ?")
        );

        stmt->setInt(1, announce_id);
        stmt->setInt(2, publisher_id);

        return stmt->executeUpdate() > 0;
    } catch (const std::exception& e) {
        std::cerr << "[AnnouncementDAO::deleteAnnouncement] Error: " << e.what() << std::endl;
        return false;
    }
}

/***************************************************************************************
 * MessageDAO
 ***************************************************************************************/
bool MessageDAO::insertMessage(sql::Connection *conn, const Message& message) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("INSERT INTO messageboard(canteen_id, create_time, user_id, content, status) VALUES (?, NOW(), ?, ?, 0)")
        );

        stmt->setInt(1, message.getCanteenId());
        stmt->setInt(2, message.getUserId());
        stmt->setString(3, message.getContent());

        return stmt->executeUpdate() > 0;
    } catch (const std::exception& e) {
        std::cerr << "[MessageDAO::insertMessage] Error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<Message> MessageDAO::getMessagesByCanteen(sql::Connection *conn, int canteen_id) {
    std::vector<Message> list;
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT * FROM messageboard WHERE canteen_id = ? ORDER BY status ASC, create_time DESC")
        );

        stmt->setInt(1, canteen_id);

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            Message msg;
            msg.setId(res->getInt("message_id"));
            msg.setCanteenId(res->getInt("canteen_id"));
            msg.setCreateTime(res->getString("create_time"));
            msg.setReplyTime(res->getString("reply_time"));
            msg.setUserId(res->getInt("user_id"));
            msg.setContent(res->getString("content"));
            msg.setReplyContent(res->getString("reply_content"));
            msg.setStatus(res->getInt("status"));
            list.push_back(msg);
        }
    } catch (const std::exception& e) {
        std::cerr << "[MessageDAO::getMessagesByCanteen] Error: " << e.what() << std::endl;
    }

    return list;
}

std::vector<Message> MessageDAO::getMessagesByUser(sql::Connection *conn, int user_id, int canteen_id) {
    std::vector<Message> list;
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT * FROM messageboard WHERE user_id = ? AND canteen_id = ? ORDER BY create_time DESC")
        );

        stmt->setInt(1, user_id);
        stmt->setInt(2, canteen_id);

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            Message msg;
            msg.setId(res->getInt("message_id"));
            msg.setCanteenId(res->getInt("canteen_id"));
            msg.setCreateTime(res->getString("create_time"));
            msg.setReplyTime(res->getString("reply_time"));
            msg.setUserId(res->getInt("user_id"));
            msg.setContent(res->getString("content"));
            msg.setReplyContent(res->getString("reply_content"));
            msg.setStatus(res->getInt("status"));
            list.push_back(msg);
        }
    } catch (const std::exception& e) {
        std::cerr << "[MessageDAO::getMessagesByUser] Error: " << e.what() << std::endl;
    }

    return list;
}

bool MessageDAO::replyMessage(sql::Connection *conn, const Message& message) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("UPDATE messageboard SET reply = ?, reply_time = NOW(), status = 1 WHERE id = ?")
        );

        stmt->setString(1, message.getReplyContent());
        stmt->setInt(2, message.getId());

        return stmt->executeUpdate() > 0;
    } catch (const std::exception& e) {
        std::cerr << "[MessageDAO::replyMessage] Error: " << e.what() << std::endl;
        return false;
    }
}

/***************************************************************************************
 * MessageCenterDAO
 ***************************************************************************************/
int MessageCenterDAO::createMessage(sql::Connection *conn, const MessageNotification& message) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("INSERT INTO message (sender_id, receiver_id, content, status, create_time) VALUES (?, ?, ?, ?, NOW())")
        );
        stmt->setInt(1, message.getSenderId());
        stmt->setInt(2, message.getReceiverId());
        stmt->setString(3, message.getContent());
        stmt->setInt(4, message.getStatus());

        stmt->executeUpdate();

        // 获取生成的ID
        auto idStmt = std::unique_ptr<sql::Statement>(conn->createStatement());
        auto res = std::unique_ptr<sql::ResultSet>(idStmt->executeQuery("SELECT LAST_INSERT_ID()"));
        if (res->next()) {
            return res->getInt(1);
        }
    } catch (const std::exception& e) {
        std::cout << "创建消息失败: " << e.what() << std::endl;
    }
    return -1;
}

std::vector<MessageNotification> MessageCenterDAO::getMessagesByReceiver(sql::Connection *conn, int receiver_id) {
    std::vector<MessageNotification> messages;
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT message_id, sender_id, receiver_id, content, status, create_time FROM message WHERE receiver_id = ? ORDER BY status ASC, create_time DESC")
        );
        stmt->setInt(1, receiver_id);

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        while (res->next()) {
            MessageNotification message;
            message.setMessageId(res->getInt("message_id"));
            message.setSenderId(res->getInt("sender_id"));
            message.setReceiverId(res->getInt("receiver_id"));
            message.setContent(res->getString("content"));
            message.setStatus(res->getInt("status"));
            message.setCreateTime(res->getString("create_time"));
            messages.push_back(message);
        }
    } catch (const std::exception& e) {
        std::cout << "获取消息失败: " << e.what() << std::endl;
    }
    return messages;
}

bool MessageCenterDAO::updateMessageStatus(sql::Connection *conn, int message_id, int status) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("UPDATE message SET status = ? WHERE message_id = ?")
        );
        stmt->setInt(1, status);
        stmt->setInt(2, message_id);

        return stmt->executeUpdate() > 0;
    } catch (const std::exception& e) {
        std::cout << "更新消息状态失败: " << e.what() << std::endl;
        return false;
    }
}

/***************************************************************************************
 * CanteenDAO - 财务统计相关方法
 ***************************************************************************************/
double CanteenDAO::getTodayIncome(sql::Connection *conn, int canteen_id) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT COALESCE(SUM(total_price), 0) FROM orders WHERE canteen_id = ? AND status = 1 AND DATE(order_time) = DATE(NOW())"
            )
        );

        stmt->setInt(1, canteen_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (res->next()) {
            return res->getDouble(1);
        }
    } catch (const std::exception& e) {
        std::cerr << "[CanteenDAO::getTodayRevenue] Error: " << e.what() << std::endl;
    }

    return 0.0;
}

// ================================
// 订单取消
// ================================

int OrderCancelDAO::insertCancelApply(sql::Connection *conn, int order_id, const std::string& cancel_reason) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("INSERT INTO order_cancel (order_id, cancel_reason) VALUES (?, ?)")
        );
        stmt->setInt(1, order_id);
        stmt->setString(2, cancel_reason);
        stmt->executeUpdate();
        // 由于 getGeneratedKeys 方法不可用，我们返回 1 表示成功
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "创建取消申请失败: " << e.what() << std::endl;
        return -1;
    }
}

std::vector<OrderCancelVO> OrderCancelDAO::getCancelAppliesByCanteen(sql::Connection *conn, int canteen_id) {
    std::vector<OrderCancelVO> list;
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(R"(
                SELECT oc.cancel_id, oc.order_id, oc.cancel_time, oc.cancel_reason, oc.status, 
                       oc.reject_reason, oc.handle_time, u.username AS order_for_user_name, 
                       c.name AS canteen_name, o.total_price, o.order_time
                FROM order_cancel oc
                JOIN orders o ON oc.order_id = o.order_id
                JOIN canteen c ON o.canteen_id = c.canteen_id
                JOIN users u ON o.order_for_user_id = u.user_id
                WHERE o.canteen_id = ?
                ORDER BY oc.status ASC, oc.cancel_time DESC
            )")
        );
        stmt->setInt(1, canteen_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        while (res->next()) {
            OrderCancelVO vo;
            vo.setCancelId(res->getInt("cancel_id"));
            vo.setOrderId(res->getInt("order_id"));
            vo.setCancelTime(res->getString("cancel_time"));
            vo.setCancelReason(res->getString("cancel_reason"));
            vo.setStatus(res->getInt("status"));
            if (!res->isNull("reject_reason")) {
                vo.setRejectReason(res->getString("reject_reason"));
            }
            if (!res->isNull("handle_time")) {
                vo.setHandleTime(res->getString("handle_time"));
            }
            vo.setOrderForUserName(res->getString("order_for_user_name"));
            vo.setCanteenName(res->getString("canteen_name"));
            vo.setTotalPrice(std::stod(res->getString("total_price")));
            vo.setOrderTime(res->getString("order_time"));
            list.push_back(vo);
        }
    } catch (const std::exception& e) {
        std::cerr << "获取取消申请失败: " << e.what() << std::endl;
    }
    return list;
}

std::shared_ptr<OrderCancelVO> OrderCancelDAO::getCancelApplyByOrderId(sql::Connection *conn, int order_id) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(R"(
                SELECT oc.cancel_id, oc.order_id, oc.cancel_time, oc.cancel_reason, oc.status, 
                       oc.reject_reason, oc.handle_time
                FROM order_cancel oc
                WHERE oc.order_id = ?
                ORDER BY oc.cancel_time DESC
                LIMIT 1
            )")
        );
        stmt->setInt(1, order_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        if (res->next()) {
            auto vo = std::make_shared<OrderCancelVO>();
            vo->setCancelId(res->getInt("cancel_id"));
            vo->setOrderId(res->getInt("order_id"));
            vo->setCancelTime(res->getString("cancel_time"));
            vo->setCancelReason(res->getString("cancel_reason"));
            vo->setStatus(res->getInt("status"));
            if (!res->isNull("reject_reason")) {
                vo->setRejectReason(res->getString("reject_reason"));
            }
            if (!res->isNull("handle_time")) {
                vo->setHandleTime(res->getString("handle_time"));
            }
            return vo;
        }
    } catch (const std::exception& e) {
        std::cerr << "获取取消申请失败: " << e.what() << std::endl;
    }
    return nullptr;
}

std::shared_ptr<OrderCancelVO> OrderCancelDAO::getCancelApplyByCancelId(sql::Connection *conn, int cancel_id) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(R"(
                SELECT oc.cancel_id, oc.order_id, oc.cancel_time, oc.cancel_reason, oc.status, 
                       oc.reject_reason, oc.handle_time
                FROM order_cancel oc
                WHERE oc.cancel_id = ?
            )")
        );
        stmt->setInt(1, cancel_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        if (res->next()) {
            auto vo = std::make_shared<OrderCancelVO>();
            vo->setCancelId(res->getInt("cancel_id"));
            vo->setOrderId(res->getInt("order_id"));
            vo->setCancelTime(res->getString("cancel_time"));
            vo->setCancelReason(res->getString("cancel_reason"));
            vo->setStatus(res->getInt("status"));
            if (!res->isNull("reject_reason")) {
                vo->setRejectReason(res->getString("reject_reason"));
            }
            if (!res->isNull("handle_time")) {
                vo->setHandleTime(res->getString("handle_time"));
            }
            return vo;
        }
    } catch (const std::exception& e) {
        std::cerr << "获取取消申请失败: " << e.what() << std::endl;
    }
    return nullptr;
}

bool OrderCancelDAO::updateCancelStatus(sql::Connection *conn, int cancel_id, int status, const std::string& reject_reason) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("UPDATE order_cancel SET status = ?, reject_reason = ?, handle_time = NOW() WHERE cancel_id = ?")
        );
        stmt->setInt(1, status);
        stmt->setString(2, reject_reason);
        stmt->setInt(3, cancel_id);
        return stmt->executeUpdate() > 0;
    } catch (const std::exception& e) {
        std::cerr << "更新取消申请状态失败: " << e.what() << std::endl;
        return false;
    }
}

double CanteenDAO::getTodayExpense(sql::Connection *conn, int canteen_id) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT COALESCE(SUM(amount), 0) FROM purchase_bill WHERE canteen_id = ? AND DATE(purchase_date) = DATE(NOW())"
            )
        );

        stmt->setInt(1, canteen_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (res->next()) {
            return res->getDouble(1);
        }
    } catch (const std::exception& e) {
        std::cerr << "[CanteenDAO::getTodayExpense] Error: " << e.what() << std::endl;
    }

    return 0.0;
}

double CanteenDAO::getIncomeByTimeDimension(sql::Connection *conn, int canteen_id, const std::string& time_dimension, const std::string& date_str) {
    try {
        std::string sql;
        if (time_dimension == "day") {
            sql = "SELECT COALESCE(SUM(total_price), 0) FROM orders WHERE canteen_id = ? AND status = 1 AND DATE(order_time) = ?";
        } else if (time_dimension == "month") {
            sql = "SELECT COALESCE(SUM(total_price), 0) FROM orders WHERE canteen_id = ? AND status = 1 AND DATE_FORMAT(order_time, '%Y-%m') = ?";
        } else if (time_dimension == "year") {
            sql = "SELECT COALESCE(SUM(total_price), 0) FROM orders WHERE canteen_id = ? AND status = 1 AND YEAR(order_time) = ?";
        } else {
            return 0.0;
        }

        auto stmt = std::unique_ptr<sql::PreparedStatement>(conn->prepareStatement(sql));
        stmt->setInt(1, canteen_id);
        stmt->setString(2, date_str);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (res->next()) {
            return res->getDouble(1);
        }
    } catch (const std::exception& e) {
        std::cerr << "[CanteenDAO::getExpenseByTimeDimension] Error: " << e.what() << std::endl;
    }

    return 0.0;
}

double CanteenDAO::getExpenseByTimeDimension(sql::Connection *conn, int canteen_id, const std::string& time_dimension, const std::string& date_str) {
    try {
        std::string sql;
        if (time_dimension == "day") {
            sql = "SELECT COALESCE(SUM(amount), 0) FROM purchase_bill WHERE canteen_id = ? AND DATE(purchase_date) = ?";
        } else if (time_dimension == "month") {
            sql = "SELECT COALESCE(SUM(amount), 0) FROM purchase_bill WHERE canteen_id = ? AND DATE_FORMAT(purchase_date, '%Y-%m') = ?";
        } else if (time_dimension == "year") {
            sql = "SELECT COALESCE(SUM(amount), 0) FROM purchase_bill WHERE canteen_id = ? AND YEAR(purchase_date) = ?";
        } else {
            return 0.0;
        }

        auto stmt = std::unique_ptr<sql::PreparedStatement>(conn->prepareStatement(sql));
        stmt->setInt(1, canteen_id);
        stmt->setString(2, date_str);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        if (res->next()) {
            return res->getDouble(1);
        }
    } catch (const std::exception& e) {
        std::cerr << "[CanteenDAO::getIncomeByTimeDimension] Error: " << e.what() << std::endl;
    }

    return 0.0;
}

/***************************************************************************************
 * HistoryMenuDao
 ***************************************************************************************/
int HistoryMenuDAO::saveHistoryMenu(sql::Connection *conn, const HistoryMenu& historyMenu) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO history_menu(menu_id, canteen_id, meal_type, start_time, end_time) "
                "VALUES (?, ?, ?, NOW(), NULL)"
            )
        );
        
        stmt->setInt(1, historyMenu.getMenuId());
        stmt->setInt(2, historyMenu.getCanteenId());
        stmt->setString(3, historyMenu.getMealType());
        
        if (stmt->executeUpdate() == 0) {
            return -1;
        }
        
        // 获取自增ID
        std::unique_ptr<sql::Statement> cstmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> rs(
            cstmt->executeQuery("SELECT LAST_INSERT_ID()")
        );
        
        int history_menu_id = -1;
        if (rs->next()) {
            history_menu_id = rs->getInt(1);
        }
        
        return history_menu_id;
    } catch (const std::exception& e) {
        std::cerr << "[HistoryMenuDAO::saveHistoryMenu] Error: " << e.what() << std::endl;
        return -1;
    }
}

bool HistoryMenuDAO::saveHistoryMenuDishes(sql::Connection *conn, int historyMenuId, const std::vector<int>& dishIds) {
    try {
        for (int dish_id : dishIds) {
            auto stmt = std::unique_ptr<sql::PreparedStatement>(
                conn->prepareStatement(
                    "INSERT INTO history_menu_dish(history_menu_id, dish_id) "
                    "VALUES (?, ?)"
                )
            );
            
            stmt->setInt(1, historyMenuId);
            stmt->setInt(2, dish_id);
            stmt->executeUpdate();
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[HistoryMenuDAO::saveHistoryMenuDishes] Error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<HistoryMenu> HistoryMenuDAO::getHistoryMenusByCanteen(sql::Connection *conn, int canteen_id) {
    std::vector<HistoryMenu> list;
    
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT * FROM history_menu WHERE canteen_id = ? ORDER BY start_time DESC"
            )
        );
        
        stmt->setInt(1, canteen_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        
        while (res->next()) {
            HistoryMenu hm;
            hm.setHistoryMenuId(res->getInt("history_menu_id"));
            hm.setMenuId(res->getInt("menu_id"));
            hm.setCanteenId(res->getInt("canteen_id"));
            hm.setMealType(res->getString("meal_type"));
            hm.setStartTime(res->getString("start_time"));
            hm.setEndTime(res->getString("end_time"));
            list.push_back(hm);
        }
    } catch (sql::SQLException& e){
        std::cout << "获取历史餐单失败: " << e.what() << std::endl;
    }
   
    return list;
}

std::vector<Dish> HistoryMenuDAO::getHistoryMenuDishes(sql::Connection *conn, int history_menu_id) {
    std::vector<Dish> list;
    
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT d.* FROM dish d "
                "JOIN history_menu_dish hmd ON d.dish_id = hmd.dish_id "
                "WHERE hmd.history_menu_id = ?"
            )
        );
        
        stmt->setInt(1, history_menu_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        
        while (res->next()) {
            Dish d;
            d.setId(res->getInt("dish_id"));
            d.setCanteenId(res->getInt("canteen_id"));
            d.setName(res->getString("name"));
            d.setType(res->getString("type"));
            d.setPrice(res->getDouble("price"));
            d.setCalories(res->getInt("calories"));
            d.setNutritionInfo(res->getString("nutrition_info"));
            d.setStatus(res->getInt("status"));
            list.push_back(d);
        }
    } catch (const std::exception& e) {
        std::cerr << "[HistoryMenuDAO::getHistoryMenuDishes] Error: " << e.what() << std::endl;
    }
    
    return list;
}

int HistoryMenuDAO::getHistoryMenuIdByCanteenIdAndMealType(sql::Connection *conn, int canteen_id, const std::string& meal_type) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT history_menu_id FROM history_menu WHERE canteen_id = ? AND meal_type = ? AND `end_time` IS NULL"
            )
        );
        
        stmt->setInt(1, canteen_id);
        stmt->setString(2, meal_type);  
        
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        
        if (res->next()) {
            return res->getInt("history_menu_id");
        }
        
        return -1;
    } catch (const std::exception& e) {
        std::cerr << "[HistoryMenuDAO::getHistoryMenuIdByCanteenIdAndMealType] Error: " << e.what() << std::endl;
        return -1;
    }
}

bool HistoryMenuDAO::updateHistoryMenuEndTime(sql::Connection *conn, int history_menu_id, int canteen_id, const std::string& meal_type) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "UPDATE history_menu SET end_time = NOW() "
                "WHERE history_menu_id = ? AND canteen_id = ? AND meal_type = ? AND end_time IS NULL"
            )
        );
        
        stmt->setInt(1, history_menu_id);
        stmt->setInt(2, canteen_id);
        stmt->setString(3, meal_type);
        
        return stmt->executeUpdate() > 0;
    } catch (sql::SQLException &e) {
        std::cout << "SQL Error: " << e.what() << std::endl;
        return false;
    }
}

/***************************************************************************************
 * TagDAO
 ***************************************************************************************/
std::vector<Tag> TagDAO::getAllTags(sql::Connection *conn) {
    std::vector<Tag> list;

    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT tag_id, tag_name FROM tag")
        );

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            Tag tag;
            tag.setId(res->getInt("tag_id"));
            tag.setName(res->getString("tag_name"));
            list.push_back(tag);
        }
    } catch (const std::exception& e) {
        std::cerr << "[TagDAO::getAllTags] Error: " << e.what() << std::endl;
    }

    return list;
}

std::vector<Tag> TagDAO::getTagsByDishId(sql::Connection *conn, int dish_id) {
    std::vector<Tag> list;

    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT t.tag_id, t.tag_name FROM tag t "
                "JOIN dish_tag dt ON t.tag_id = dt.tag_id "
                "WHERE dt.dish_id = ?"
            )
        );

        stmt->setInt(1, dish_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            Tag tag;
            tag.setId(res->getInt("tag_id"));
            tag.setName(res->getString("tag_name"));
            list.push_back(tag);
        }
    } catch (const std::exception& e) {
        std::cerr << "[TagDAO::getTagsByDishId] Error: " << e.what() << std::endl;
    }

    return list;
}

/***************************************************************************************
 * DishTagDAO
 ***************************************************************************************/
bool DishTagDAO::insertDishTag(sql::Connection *conn, int dish_id, int tag_id) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "INSERT INTO dish_tag(dish_id, tag_id) VALUES (?, ?)"
            )
        );

        stmt->setInt(1, dish_id);
        stmt->setInt(2, tag_id);
        stmt->executeUpdate();

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[DishTagDAO::insertDishTag] Error: " << e.what() << std::endl;
        return false;
    }
}

bool DishTagDAO::deleteDishTag(sql::Connection *conn, int dish_id, int tag_id) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "DELETE FROM dish_tag WHERE dish_id = ? AND tag_id = ?"
            )
        );

        stmt->setInt(1, dish_id);
        stmt->setInt(2, tag_id);
        stmt->executeUpdate();

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[DishTagDAO::deleteDishTag] Error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<int> DishTagDAO::getTagIdsByDishId(sql::Connection *conn, int dish_id) {
    std::vector<int> list;

    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "SELECT tag_id FROM dish_tag WHERE dish_id = ?"
            )
        );

        stmt->setInt(1, dish_id);
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());

        while (res->next()) {
            list.push_back(res->getInt("tag_id"));
        }
    } catch (const std::exception& e) {
        std::cerr << "[DishTagDAO::getTagIdsByDishId] Error: " << e.what() << std::endl;
    }

    return list;
}
