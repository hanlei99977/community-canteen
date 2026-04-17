#include "dao.h"
#include <cppconn/prepared_statement.h>

/***************************************************************************************
 * RegionDao
 ***************************************************************************************/

std::vector<Region> RegionDAO::getRegionList() {
    std::vector<Region> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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
    } catch (...) {}

    return list;
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
    } catch (...) { return -1; }
}

bool UserDAO::existsByUsername(const std::string& username)
{
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT 1 FROM users WHERE username = ? LIMIT 1")
        );
        stmt->setString(1, username);

        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        return res->next();
    } catch (...) {
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
    } catch (...) { return false; }
}

bool UserDAO::updateStatus(const User& user)
{
    try {
        DBConnectionGuard guard;
        auto conn = guard.get();

        return updateStatus(conn, user.getId(), user.getStatus());
    } catch (...) { return false; }
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

std::vector<AdminInformation> AdminDAO::getAdminList()
{
    std::vector<AdminInformation> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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
    } catch (...) {}

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
    } catch (...) { return false; }
}

std::vector<AdminApplyVO> AdminApplyDAO::getApplyList()
{
    std::vector<AdminApplyVO> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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
    } catch (...) {}

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
    } catch (...) {}

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
    } catch (...) { return false; }
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
    } catch (...) { return false; }
}

std::vector<CanteenManagerApplyVO> CanteenManagerApplyDAO::getApplyList()
{
    std::vector<CanteenManagerApplyVO> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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
    } catch (...) {}

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
    } catch (...) {}
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

std::shared_ptr<DinerCenterVO> DinerDAO::getDinerCenterByUserId(int user_id)
{
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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
            auto vo = std::make_shared<DinerCenterVO>();
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
    } catch (...) {}
    return nullptr;
}

std::vector<FamilyMemberVO> DinerDAO::getFamilyMembersByUserId(int user_id)
{
    std::vector<FamilyMemberVO> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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
    } catch (...) {}

    return list;
}

bool DinerDAO::updateDiner(sql::Connection *conn, const DinerCenterVO& diner) {
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
    } catch (...) { return false; }
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
    } catch (...) { return false; }
}

std::vector<DinerInformation> DinerDAO::getDinerList()
{
    std::vector<DinerInformation> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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
    } catch (...) {}

    return list;
}
/***************************************************************************************
 * FamilyDao
***************************************************************************************/
int FamilyDAO::insertFamily(const Family& family) {
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
    } catch (...) { return -1; }
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
            c->setAddress(res->getString("address"));
            c->setRegionId(res->getInt("region_id"));
            c->setStatus(res->getInt("status"));
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
        if (rs->next()) {
            return rs->getInt(1);
        }
    } catch (...) {}
    return -1;
}

bool CanteenDAO::updateCanteenAddress(int canteen_id, const std::string& address) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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
    } catch (...) { return false; }
}

bool CanteenDAO::updateCanteenStatus(int canteen_id, int status) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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

        return true;
    } catch (...) { return false; }
}

std::vector<CanteenManagerVO> CanteenDAO::getCanteensWithManagers() {
    std::vector<CanteenManagerVO> canteens;
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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
    } catch (...) {
        // 异常处理
    }
    return canteens;
}

std::vector<PurchaseBill> CanteenDAO::getPurchaseBillsByCanteen(int canteen_id) {
    std::vector<PurchaseBill> bills;
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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
    } catch (...) {
        // 异常处理
    }
    return bills;
}

int CanteenDAO::createPurchaseBill(const PurchaseBill& bill) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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
    } catch (...) {
        // 异常处理
    }
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
        std::cerr << "下架菜品失败: " << e.what() << std::endl;
        return false;
    }

}

bool DishDAO::enableDishByDishId(const int dish_id){
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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
        stmt->setString(5, order.getStatus());
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
                o.canteen_id AS canteen_id,
                u.username AS order_for_user_name,
                c.name AS canteen_name,
                o.total_price,
                o.discount_rate,
                o.original_total,
                o.saved_amount,
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
            o.setCreateTime(res->getString("order_time"));
            o.setHasRating(!res->isNull("rating_score"));
            if (!res->isNull("rating_score")) {
                o.setRatingScore(res->getInt("rating_score"));
                o.setRatingComment(res->isNull("rating_comment") ? "" : res->getString("rating_comment"));
                o.setRatingTime(res->isNull("rating_time") ? "" : res->getString("rating_time"));
            }

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
                    oi.unit_price,
                    oi.discount_price,
                    oi.quantity,
                    oi.subtotal
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

std::vector<CanteenRatingVO> RatingDAO::getCanteenRatingDetails(int canteen_id)
{
    std::vector<CanteenRatingVO> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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

std::vector<ReportVO> ReportDAO::getAllReports()
{
    std::vector<ReportVO> list;

    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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
    } catch (...) {}

    return list;
}

bool ReportDAO::updateReportStatus(int report_id, int status, int handler_id)
{
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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
    } catch (...) { return false; }
}

/***************************************************************************************
 * AnnouncementDao
***************************************************************************************/
bool AnnouncementDAO::insertAnnouncement(const Announcement& announcement) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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
    } catch (...) { return false; }
}

std::vector<AnnouncementVO> AnnouncementDAO::getAnnouncementList() {
    std::vector<AnnouncementVO> list;
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

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
    } catch (...) {}

    return list;
}

bool AnnouncementDAO::deleteAnnouncement(int announce_id, int publisher_id) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();

        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement(
                "DELETE FROM announcement WHERE announce_id = ? AND publisher_id = ?"
            )
        );

        stmt->setInt(1, announce_id);
        stmt->setInt(2, publisher_id);
        return stmt->executeUpdate() > 0;
    } catch (...) { return false; }
}
