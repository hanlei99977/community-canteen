#include "service.h"
#include <iostream>
#include <map>
#include "../../MySQL/ConnectionPool.h"
#include "../model/vo.h"

// 下单操作加锁
std::mutex OrderService::userOrderMapMutex;
std::unordered_map<int, std::shared_ptr<std::mutex>> OrderService::userOrderMutexes;

std::shared_ptr<std::mutex> OrderService::getUserOrderMutex(int user_id) {
    std::lock_guard<std::mutex> lock(userOrderMapMutex);
    auto it = userOrderMutexes.find(user_id);
    if (it != userOrderMutexes.end()) {
        return it->second;
    }

    auto mutex_ptr = std::make_shared<std::mutex>();
    userOrderMutexes[user_id] = mutex_ptr;
    return mutex_ptr;
}

/**********************************************
 * UserService
 *********************************************/
bool UserService::registerUser(const User& user, int role, int region_id) {
    UserDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    bool res = true;

    try {
        TransactionGuard tx(conn);

        // 简单校验（可以扩展）
        if (user.getUsername().empty() || user.getPassword().empty()) {
            return false;
        }

        int user_id = dao.insertUser(conn, user);
        if (user_id == -1) {
            return false;
        }

        switch (role) {
            case 1: // diner
                if (region_id <= 0) {
                    return false;
                }
                DinerDAO dinerDAO;
                res = dinerDAO.insertDiner(conn, user_id, region_id);
                break;
            case 2: // admin
                AdminDAO adminDAO;
                res = adminDAO.insertAdmin(conn, user_id);
                break;
            case 3: // manager
                ManagerDAO managerDAO;
                res = managerDAO.insertManager(conn, user_id);
                break;
            default:
                return false;
        }

        if (!res) {
            return false;
        }
        tx.commit(); // 提交事务
        return true;
    }
    catch (...) {
        return false;
    }
}

bool UserService::isUsernameTaken(const std::string& username) {
    if (username.empty()) {
        return false;
    }
    UserDAO dao;
    return dao.existsByUsername(username);
}

std::shared_ptr<User> UserService::login(
    std::string& username,
    std::string& password
) {
    UserDAO dao;
     // 去空格（很重要）
    username.erase(0, username.find_first_not_of(" "));
    username.erase(username.find_last_not_of(" ") + 1);

    password.erase(0, password.find_first_not_of(" "));
    password.erase(password.find_last_not_of(" ") + 1);

    auto user = dao.getUserByUsernameAndPassword(username, password);
    if (user && user->getStatus() == 1) {
        return user;
    }
    return nullptr;
}

std::shared_ptr<DinerCenterVO> UserService::getDinerCenterByUserId(int user_id) {
    DinerDAO dao;
    return dao.getDinerCenterByUserId(user_id);
}

bool UserService::updateDinerCenter(const DinerCenterVO& diner) {
    DBConnectionGuard guard;
    auto* conn = guard.get();

   try{
        TransactionGuard tx(conn);

        UserDAO userDAO;
        DinerDAO dinerdao;
        if (!userDAO.updateUser(conn, diner))
        {
            std::cout << "更新 user 信息失败" << std::endl;
            return false;
        }
        if (!dinerdao.updateDiner(conn, diner))
        {
            std::cout << "更新 diner 信息失败" << std::endl;
            return false;
        }

        tx.commit();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        return false;
    }
}

std::string UserService::getUserRole(int user_id) {
    UserDAO dao;
    if (user_id <= 0) {
        return "unknown";
    }
    return dao.getUserRole(user_id);
}   

bool UserService::updateStatus(const User& user)
{
    UserDAO dao;
    return dao.updateStatus(user);
}

// ================================
// 管理员服务
// ================================
bool AdminService::submitAdminApply(const User& user, int level_id, int region_id)
{
    if (user.getUsername().empty() || user.getPassword().empty()) {
        return false;
    }
    if (level_id <= 0 || region_id <= 0) {
        return false;
    }

    DBConnectionGuard guard;
    auto* conn = guard.get();

    try {
        TransactionGuard tx(conn);
        UserDAO userDAO;
        AdminApplyDAO applyDAO;

        User applyUser = user;
        applyUser.setStatus(0); // 申请中不可登录

        int user_id = userDAO.insertUser(conn, applyUser);
        if (user_id == -1) {
            return false;
        }

        if (!applyDAO.insertApply(conn, user_id, level_id, region_id)) {
            return false;
        }

        tx.commit();
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<AdminInformation> AdminService::getAdminList()
{
    AdminDAO admin;
    return admin.getAdminList();
}

std::vector<AdminApplyVO> AdminService::getAdminApplyList()
{
    AdminApplyDAO dao;
    return dao.getApplyList();
}

bool AdminService::reviewAdminApply(int apply_id, int reviewer_id, int status)
{
    if (apply_id <= 0 || reviewer_id <= 0) {
        return false;
    }
    if (status != 1 && status != 2) {
        return false;
    }

    DBConnectionGuard guard;
    auto* conn = guard.get();

    try {
        TransactionGuard tx(conn);

        AdminApplyDAO applyDAO;
        auto apply = applyDAO.getApplyById(conn, apply_id);
        if (!apply || apply->getStatus() != 0) {
            return false;
        }

        if (status == 1) {
            AdminDAO adminDAO;
            UserDAO userDAO;
            if (!adminDAO.insertAdmin(conn, apply->getUserId(), apply->getLevelId(), apply->getRegionId())) {
                return false;
            }
            if (!userDAO.updateStatus(conn, apply->getUserId(), 1)) {
                return false;
            }
        }

        if (!applyDAO.reviewApply(conn, apply_id, reviewer_id, status)) {
            return false;
        }

        tx.commit();
        return true;
    } catch (...) {
        return false;
    }
}

// ================================
// 食堂管理者服务
// ================================
bool ManagerService::submitManagerApply(const User& user, const std::string& canteen_name, int region_id)
{
    if (user.getUsername().empty() || user.getPassword().empty() || canteen_name.empty() || region_id <= 0) {
        return false;
    }

    DBConnectionGuard guard;
    auto* conn = guard.get();

    try {
        TransactionGuard tx(conn);
        UserDAO userDAO;
        CanteenManagerApplyDAO applyDAO;

        User applyUser = user;
        applyUser.setStatus(0); // 申请中不可登录

        int user_id = userDAO.insertUser(conn, applyUser);
        if (user_id == -1) {
            return false;
        }

        if (!applyDAO.insertApply(conn, user_id, canteen_name, region_id)) {
            return false;
        }

        tx.commit();
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<CanteenManagerApplyVO> ManagerService::getManagerApplyList()
{
    CanteenManagerApplyDAO dao;
    return dao.getApplyList();
}

bool ManagerService::reviewManagerApply(int apply_id, int reviewer_id, int status)
{
    if (apply_id <= 0 || reviewer_id <= 0) {
        return false;
    }
    if (status != 1 && status != 2) {
        return false;
    }

    DBConnectionGuard guard;
    auto* conn = guard.get();

    try {
        TransactionGuard tx(conn);
        CanteenManagerApplyDAO applyDAO;
        auto apply = applyDAO.getApplyById(conn, apply_id);
        if (!apply || apply->getStatus() != 0) {
            return false;
        }

        if (status == 1) {
            CanteenDAO canteenDAO;
            ManagerDAO managerDAO;
            UserDAO userDAO;

            int canteen_id = canteenDAO.insertCanteen(conn, apply->getCanteenName(), apply->getRegionId());
            if (canteen_id == -1) {
                return false;
            }

            if (!managerDAO.insertManager(conn, apply->getUserId(), canteen_id)) {
                return false;
            }

            if (!userDAO.updateStatus(conn, apply->getUserId(), 1)) {
                return false;
            }
        }

        if (!applyDAO.reviewApply(conn, apply_id, reviewer_id, status)) {
            return false;
        }

        tx.commit();
        return true;
    } catch (...) {
        return false;
    }
}

// ================================
// 用餐者服务
// ================================
std::vector<DinerInformation> DinerService::getDinerList()
{
    DinerDAO diner;
    return diner.getDinerList();
}

int CanteenService::getCanteenIdByUserId(int user_id) {
    CanteenDAO dao;
    return dao.getCanteenIdByUserId(user_id);
}


/**********************************************
 *RegionService
 *********************************************/
std::vector<Region> RegionService::getRegionList()
{
    RegionDAO dao;
    return dao.getRegionList();
}

/**********************************************
 * FamilyService
 *********************************************/
Family FamilyService::getFamilyByUserId(int user_id) {
    FamilyDAO dao;
    return dao.getFamilyByUserId(user_id);
}

std::vector<Family> FamilyService::getFamilyList() {
    FamilyDAO dao;
    return dao.getFamilyList();
}

bool FamilyService::createFamily(int user_id, const std::string& family_name) {
    if (user_id <= 0 || family_name.empty()) {
        std::cout << "参数错误：user_id=" << user_id << ", family_name=" << family_name << std::endl;
        return false;
    }

    DBConnectionGuard guard;
    auto* conn = guard.get();

    try {
        TransactionGuard tx(conn);

        // 创建家庭
        Family family;
        family.setName(family_name);

        FamilyDAO familyDAO;
        int family_id = familyDAO.insertFamily(family);
        if (family_id == -1) {
            std::cout << "插入家庭失败" << std::endl;
            return false;
        }
        std::cout << "插入家庭成功，family_id=" << family_id << std::endl;

        // 更新用餐者的家庭ID
        DinerDAO dinerDAO;
        auto diner = dinerDAO.getDinerByUserId(user_id);
        if (!diner) {
            std::cout << "获取用餐者信息失败，user_id=" << user_id << std::endl;
            return false;
        }
        std::cout << "获取用餐者信息成功，user_id=" << user_id << ", family_id=" << diner->getFamilyId() << std::endl;

        // 调用DinerDAO的updateFamilyId方法更新家庭ID
        if (!dinerDAO.updateFamilyId(conn, user_id, family_id)) {
            std::cout << "更新用餐者家庭ID失败，user_id=" << user_id << std::endl;
            return false;
        }
        std::cout << "更新用餐者家庭ID成功" << std::endl;

        tx.commit();
        std::cout << "创建家庭成功，user_id=" << user_id << ", family_name=" << family_name << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "创建家庭异常：" << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cout << "创建家庭未知异常" << std::endl;
        return false;
    }
}

/**********************************************
 * CanteenService
 *********************************************/
std::vector<Canteen> CanteenService::getAllCanteens() {
    CanteenDAO dao;
    return dao.getAllCanteens();
}

std::shared_ptr<Canteen> CanteenService::getCanteenById(int id) {
    CanteenDAO dao;
    return dao.getCanteenById(id);
}

bool CanteenService::updateCanteenAddress(int canteen_id, const std::string& address) {
    if (canteen_id <= 0 || address.empty()) {
        return false;
    }
    CanteenDAO dao;
    return dao.updateCanteenAddress(canteen_id, address);
}

bool CanteenService::updateCanteenStatus(int canteen_id, int status) {
    if (canteen_id <= 0 || (status != 0 && status != 1)) {
        return false;
    }
    CanteenDAO dao;
    return dao.updateCanteenStatus(canteen_id, status);
}

std::shared_ptr<Canteen> CanteenService::getCanteenDetails(int canteen_id) {
    if (canteen_id <= 0) {
        return nullptr;
    }
    CanteenDAO dao;
    return dao.getCanteenById(canteen_id);
}

std::vector<CanteenManagerVO> CanteenService::getCanteensWithManagers() {
    CanteenDAO dao;
    return dao.getCanteensWithManagers();
}

/**********************************************
 * MenuService
 *********************************************/
std::vector<Dish> MenuService::getTodayMenu(int canteen_id, const std::string& date) {
    MenuDAO dao;
    return dao.getMenuByDate(canteen_id, date);
}

std::vector<CanteenMenuVO> MenuService::getMenuByCanteen(int canteen_id) {
    MenuDAO dao;
    return dao.getMenuByCanteen(canteen_id);
}

bool MenuService::insertMenu(const MenuCreateDTO& menu) {
    MenuDAO dao;
    return dao.insertMenu(menu);
}

bool MenuService::eraseMenu(const int menu_id)
{
    MenuDAO dao;
    return dao.eraseMenu(menu_id);
}

/**********************************************
 * DishService
 *********************************************/
std::vector<Dish> DishService::getDishsByCanteen(int canteen_id) {
    DishDAO dao;
    return dao.getDishesByCanteen(canteen_id);
}

bool DishService::insertDish(const Dish& dish) {
    DishDAO dao;
    return dao.insertDish(dish);
}

bool DishService::disableDishByDishId(int dish_id) {
    DishDAO dao;
    return dao.disableDishByDishId(dish_id);
}

bool DishService::enableDishByDishId(int dish_id) {
    DishDAO dao;
    return dao.enableDishByDishId(dish_id);
}

/**********************************************
 * OrderService
 *********************************************/
bool OrderService::placeOrder(int user_id,
                              int canteen_id,
                              int order_for_user_id,
                              const std::vector<OrderItem>& items) {

    auto user_mutex = getUserOrderMutex(user_id);
    std::lock_guard<std::mutex> user_lock(*user_mutex);

    DBConnectionGuard guard;
    auto* conn = guard.get();

    try{
        TransactionGuard tx(conn);
        
        if (items.empty()) return false;

        DishDAO dishDAO;
        OrderDAO orderDAO;

        double total = 0.0;

        auto dishes = dishDAO.getDishesByCanteen(canteen_id);// 获取食堂菜品（可以优化成批量查询）
        // 1️⃣ 计算总价（业务逻辑）
        for (const auto& item : items) {
            for (const auto& d : dishes) {
                if (d.getId() == item.getDishId()) {
                    total += d.getPrice() * item.getQuantity();
                }
            }
        }

        DinerDAO dinerDAO;
        auto familyMembers = dinerDAO.getFamilyMembersByUserId(user_id);
        bool canOrderForTarget = false;
        for (const auto& member : familyMembers) {
            if (member.getUserId() == order_for_user_id) {
                canOrderForTarget = true;
                break;
            }
        }
        if (!canOrderForTarget) {
            return false;
        }

        // 2️⃣ 构造订单
        Order order;
        order.setUserId(user_id);
        order.setOrderForUserId(order_for_user_id);
        order.setCanteenId(canteen_id);
        order.setTotalPrice(total);
        order.setStatus("pending");

        // 3️⃣ 创建订单（事务）
        int order_id = orderDAO.insertOrder(conn, order, items);
        if (order_id == -1) {
            return false;
        }

        OrderItemDAO orderItemDAO;
        if (!orderItemDAO.insertOrderItems(conn, order_id, items)) {
            return false;
        }

        tx.commit(); // 提交事务
        return true;
        
    }catch (...) {
            return false;
        }
}

std::vector<FamilyMemberVO> OrderService::getOrderTargetsByUser(int user_id)
{
    DinerDAO dao;
    return dao.getFamilyMembersByUserId(user_id);
}

std::vector<OrderVO> OrderService::getOrdersByUser(int user_id)
{
    OrderDAO dao;
    return dao.getOrdersByUser(user_id);
}

std::vector<OrderDetailVO> OrderService::getOrdersDetailsByUser(int user_id,int order_id) {
    OrderDAO dao;
    return dao.getOrdersDetailsByUser(user_id,order_id);
}

/**********************************************
 * RatingService
 *********************************************/
bool RatingService::submitRating(const Rating& rating) {
    RatingDAO dao;

    // 校验评分
    if (rating.getScore() < 1 || rating.getScore() > 5) {
        return false;
    }

    return dao.insertRating(rating);
}

std::vector<Rating> RatingService::getRatings(int canteen_id) {
    RatingDAO dao;
    return dao.getRatingsByCanteen(canteen_id);
}

std::vector<CanteenRatingVO> RatingService::getCanteenRatingDetails(int canteen_id) {
    RatingDAO dao;
    return dao.getCanteenRatingDetails(canteen_id);
}

/**********************************************
 * ReportService
 *********************************************/
bool ReportService::submitReport(const Report& report) {
    ReportDAO dao;

    if (report.getContent().empty()) return false;

    return dao.insertReport(report);
}

std::vector<Report> ReportService::getReports(int canteen_id) {
    ReportDAO dao;
    return dao.getReportsByCanteen(canteen_id);
}

std::vector<ReportVO> ReportService::getAllReports() {
    ReportDAO dao;
    return dao.getAllReports();
}

bool ReportService::handleReport(int report_id, int status, int handler_id) {
    if (report_id <= 0 || handler_id <= 0) {
        return false;
    }
    if (status != 1 && status != 2) {
        return false;
    }
    ReportDAO dao;
    return dao.updateReportStatus(report_id, status, handler_id);
}

/**********************************************
 * AnnouncementService
 *********************************************/
bool AnnouncementService::publishAnnouncement(const Announcement& announcement) {
    if (announcement.getTitle().empty() || announcement.getContent().empty() || announcement.getPublisherId() <= 0) {
        return false;
    }

    AnnouncementDAO dao;
    return dao.insertAnnouncement(announcement);
}

std::vector<AnnouncementVO> AnnouncementService::getAnnouncementList() {
    AnnouncementDAO dao;
    return dao.getAnnouncementList();
}

bool AnnouncementService::deleteAnnouncement(int announce_id, int publisher_id) {
    if (announce_id <= 0 || publisher_id <= 0) {
        return false;
    }
    AnnouncementDAO dao;
    return dao.deleteAnnouncement(announce_id, publisher_id);
}
