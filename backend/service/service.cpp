#include "service.h"
#include <iostream>
#include <map>
#include <algorithm>
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
 **********************************************/
bool UserService::validateDistrictRegion(int region_id) {
    if (region_id <= 0) {
        return false;
    }
    DBConnectionGuard guard;
    auto* conn = guard.get();
    RegionDAO regionDAO;
    return regionDAO.isDistrictLevel(conn, region_id);
}

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
                if (region_id <= 0 || !validateDistrictRegion(region_id)) {
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
    catch (const std::exception& e) {
        std::cerr << "[UserService::registerUser] Error: " << e.what() << std::endl;
        return false;
    }
}

bool UserService::isUsernameTaken(const std::string& username) {
    if (username.empty()) {
        return false;
    }
    UserDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.existsByUsername(conn, username);
}

std::shared_ptr<User> UserService::login(
    std::string& username,
    std::string& password
) {
    UserDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    // 去空格（很重要）
    username.erase(0, username.find_first_not_of(" "));
    username.erase(username.find_last_not_of(" ") + 1);

    password.erase(0, password.find_first_not_of(" "));
    password.erase(password.find_last_not_of(" ") + 1);

    auto user = dao.getUserByUsernameAndPassword(conn, username, password);
    if (user && user->getStatus() == 1) {
        return user;
    }
    return nullptr;
}

std::shared_ptr<LoginResultVO> UserService::completeLogin(std::string& username, std::string& password) {
    // 调用基础登录方法验证用户名密码
    auto user = login(username, password);
    if (!user) {
        return nullptr;
    }

    int user_id = user->getId();
    std::string role = getUserRole(user_id);

    // 构建登录结果
    auto result = std::make_shared<LoginResultVO>();
    result->setUserId(user_id);
    result->setUsername(user->getUsername());
    result->setRole(role);

    // 食堂管理者：获取食堂ID
    int canteen_id = -1;
    if (role == "canteen_manager") {
        CanteenService canteenService;
        canteen_id = canteenService.getCanteenIdByUserId(user_id);
    }
    result->setCanteenId(canteen_id);

    // 管理员：获取级别和区域信息
    int admin_level = 0;
    int region_id = 0;
    std::string region_name = "";

    if (role == "admin" || role == "system_admin") {
        DBConnectionGuard guard;
        auto* conn = guard.get();
        
        AdminDAO adminDAO;
        RegionDAO regionDAO;
        
        auto admin = adminDAO.getAdminByUserId(conn, user_id);
        if (admin) {
            admin_level = admin->getLevelId();
            region_id = admin->getRegionId();
            
            auto region = regionDAO.getRegionById(conn, region_id);
            if (region) {
                region_name = region->getName();
            }
        }
    }

    result->setAdminLevel(admin_level);
    result->setRegionId(region_id);
    result->setRegionName(region_name);

    return result;
}


std::shared_ptr<UserCenterVO> UserService::getUserCenterByUserId(int user_id) {
    DBConnectionGuard guard;
    auto* conn = guard.get();

    UserDAO userDAO;
    std::string role = userDAO.getUserRole(conn, user_id);

    auto result = std::make_shared<UserCenterVO>();
    result->setUserId(user_id);
    result->setRole(role);

    if (role == "diner") {
        DinerDAO dinerDAO;
        auto diner = dinerDAO.getDinerCenterByUserId(conn, user_id);
        if (diner) {
            result->setUsername(diner->getUsername());
            result->setAge(diner->getAge());
            result->setPhone(diner->getPhone());
            result->setIdCard(diner->getIdCard());
            result->setRegionId(diner->getRegionId());
            result->setRegionName(diner->getRegionName());
            result->setFamilyId(diner->getFamilyId());
            result->setFamilyName(diner->getFamilyName());
            result->setDiseaseHistory(diner->getDiseaseHistory());
            result->setTastePreference(diner->getTastePreference());
        }
    } else if (role == "canteen_manager") {
        auto user = userDAO.getUserById(conn, user_id);
        if (user) {
            result->setUsername(user->getUsername());
            result->setAge(user->getAge());
            result->setPhone(user->getPhone());
            result->setIdCard(user->getIdCard());
        }

        CanteenDAO canteenDAO;
        int canteen_id = canteenDAO.getCanteenIdByUserId(conn, user_id);
        if (canteen_id > 0) {
            auto canteen = canteenDAO.getCanteenById(conn, canteen_id);
            if (canteen) {
                result->setCanteenId(canteen->getId());
                result->setCanteenName(canteen->getName());
            }
        }
    } else if (role == "admin" || role == "system_admin") {
        auto user = userDAO.getUserById(conn, user_id);
        if (user) {
            result->setUsername(user->getUsername());
            result->setAge(user->getAge());
            result->setPhone(user->getPhone());
            result->setIdCard(user->getIdCard());
        }

        AdminDAO adminDAO;
        auto admin = adminDAO.getAdminByUserId(conn, user_id);
        if (admin) {
            result->setAdminLevel(admin->getLevelId());
            RegionDAO regionDAO;
            auto region = regionDAO.getRegionById(conn, admin->getRegionId());
            if (region) {
                result->setAdminRegion(region->getName());
            }
        }
    } else {
        return nullptr;
    }

    return result;
}

bool UserService::updateUserCenter(const UserCenterVO& diner) {
    DBConnectionGuard guard;
    auto* conn = guard.get();

    std::string role = diner.getRole();
    if (role == "diner") {
        if (!validateDistrictRegion(diner.getRegionId())) {
            std::cout << "用餐者所在区域必须是区级" << std::endl;
            return false;
        }
    }

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
    DBConnectionGuard guard;
    auto* conn = guard.get();
    if (user_id <= 0) {
        return "unknown";
    }
    return dao.getUserRole(conn, user_id);
}   

bool UserService::updateStatus(const User& user)
{
    UserDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.updateStatus(conn, user);
}


bool UserService::changePassword(int user_id, const std::string& old_password, const std::string& new_password) {
    if (user_id <= 0 || old_password.empty() || new_password.empty()) {
        return false;
    }
    
    // 验证新密码长度
    if (new_password.length() < 6) {
        std::cout << "新密码长度不足" << std::endl;
        return false;
    }
    
    DBConnectionGuard guard;
    auto* conn = guard.get();
    
    try {
        TransactionGuard tx(conn);
        
        UserDAO userDAO;
        // 验证旧密码是否正确
        auto user = userDAO.getUserById(conn, user_id);
        if (!user || user->getPassword() != old_password) {
            std::cout << "旧密码错误" << std::endl;
            return false;
        }
        
        // 更新密码
        if (!userDAO.updatePassword(conn, user_id, new_password)) {
            std::cout << "更新密码失败" << std::endl;
            return false;
        }
        
        // 写入消息中心通知
        MessageCenterService messageService;
        MessageNotification message;
        message.setSenderId(1); // 系统发送
        message.setReceiverId(user_id);
        message.setContent("您的账户密码已成功修改。");
        message.setStatus(0); // 未读
        messageService.createMessage(message);
        
        tx.commit();
        std::cout << "密码修改成功" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[UserService::changePassword] Error: " << e.what() << std::endl;
        return false;
    }
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
    } catch (const std::exception& e) {
        std::cerr << "[AdminService::submitAdminApply] Error: " << e.what() << std::endl;
        return false;
    }
}


std::vector<AdminInformation> AdminService::getAdminList(int viewer_id) {

    DBConnectionGuard guard;
    auto* conn = guard.get();

    AdminDAO adminDAO;
    try{
        
        auto allAdmins = adminDAO.getAdminList(conn);

        // 获取查看者的管理员信息
        auto viewer = adminDAO.getAdminByUserId(conn, viewer_id);
        if (!viewer) {
            throw std::runtime_error("无权限"); // 如果不是管理员，终止并抛出异常
        }

        int viewer_level_id = viewer->getLevelId();
        int viewer_region_id = viewer->getRegionId();

        // 系统管理员（level_id=1）可以查看所有管理员
        if (viewer_level_id == 1) {
            return allAdmins;
        }

        RegionService regionService;
        // 市级管理员（level_id=2）可以查看辖区内的区级管理员
        if (viewer_level_id == 2) {
            std::vector<AdminInformation> filteredAdmins;
            for (const auto& admin : allAdmins) {
                // 市级管理员可以查看自己和辖区内的区级管理员
                if (admin.getLevelId() == 3 && regionService.isRegionInScope(conn, admin.getRegionId(), viewer_region_id)) {
                    filteredAdmins.push_back(admin);
                }
            }
            return filteredAdmins;
        }

        // 区级管理员（level_id=3）只能查看自己
        if (viewer_level_id == 3) {
            std::vector<AdminInformation> filteredAdmins;
            for (const auto& admin : allAdmins) {
                if (admin.getUserId() == viewer_id) {
                    filteredAdmins.push_back(admin);
                    break;
                }
            }
            return filteredAdmins;
        }

        return {};
    } catch (const std::exception& e) {
        std::cerr << "[AdminService::getAdminList] Error: " << e.what() << std::endl;
        return {};
    }
}

std::vector<AdminApplyVO> AdminService::getAdminApplyList(int reviewer_id) {
    DBConnectionGuard guard;
    auto* conn = guard.get();

    AdminApplyDAO dao;
    try{
        auto allApplies = dao.getApplyList(conn);

        // 根据审核者权限过滤申请
        std::vector<AdminApplyVO> filteredApplies;
        for (const auto& apply : allApplies) {
            if (hasAdminApplyReviewPermission(reviewer_id, apply.getRegionId(), apply.getLevelId())) {
                filteredApplies.push_back(apply);
            }
        }

        return filteredApplies;
    } catch (const std::exception& e) {
        std::cerr << "[AdminService::getAdminApplyList] Error: " << e.what() << std::endl;
        return {};
    }
}

std::shared_ptr<Admin> AdminService::getAdminInfo(int user_id) {
    if (user_id <= 0) {
        return nullptr;
    }
    DBConnectionGuard guard;
    auto* conn = guard.get();
    AdminDAO adminDAO;
    return adminDAO.getAdminByUserId(conn, user_id);
}

bool AdminService::hasAdminApplyReviewPermission(int reviewer_id, int apply_region_id, int apply_level_id) {
    if (reviewer_id <= 0 || apply_region_id <= 0 || apply_level_id <= 0) {
        return false;
    }

    DBConnectionGuard guard;
    auto* conn = guard.get();

    AdminDAO adminDAO;
    auto reviewer = adminDAO.getAdminByUserId(conn, reviewer_id);
    if (!reviewer) {
        return false;
    }

    int reviewer_level_id = reviewer->getLevelId();
    int reviewer_region_id = reviewer->getRegionId();

    // 系统管理员（level_id=1）可以审核所有申请
    if (reviewer_level_id == 1) {
        return true;
    }

    // 市级管理员（level_id=2）可以审核辖区内的区级管理员申请（level_id=3）
    if (reviewer_level_id == 2 && apply_level_id == 3) {
        RegionService regionService;
        return regionService.isRegionInScope(conn, apply_region_id, reviewer_region_id);
    }

    // 区级管理员（level_id=3）不能审核管理员申请
    return false;
}

bool AdminService::hasManagerApplyReviewPermission(int reviewer_id, int apply_region_id) {
    if (reviewer_id <= 0 || apply_region_id <= 0) {
        return false;
    }

    DBConnectionGuard guard;
    auto* conn = guard.get();

    AdminDAO adminDAO;
    auto reviewer = adminDAO.getAdminByUserId(conn, reviewer_id);
    if (!reviewer) {
        return false;
    }

    int reviewer_level_id = reviewer->getLevelId();
    int reviewer_region_id = reviewer->getRegionId();

    // 系统管理员（level_id=1）可以审核所有申请
    if (reviewer_level_id == 1) {
        return true;
    }

    RegionService regionService;
    
    // 市/区级管理员可以审核辖区内所有食堂管理者申请
    if (reviewer_level_id == 2  || reviewer_level_id == 3) {
        return regionService.isRegionInScope(conn, apply_region_id, reviewer_region_id);
    }

    return false;
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

        // 检查审核权限
        if (!hasAdminApplyReviewPermission(reviewer_id, apply->getRegionId(), apply->getLevelId())) {
            std::cerr << "审核者 " << reviewer_id << " 没有权限审核管理员申请 " << apply_id << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[AdminService::reviewAdminApply] Error: " << e.what() << std::endl;
        return false;
    }
}

// ================================
// 食堂管理者服务
// ================================
bool ManagerService::validateDistrictRegion(int region_id) {
    if (region_id <= 0) {
        return false;
    }
    DBConnectionGuard guard;
    auto* conn = guard.get();
    RegionDAO regionDAO;
    return regionDAO.isDistrictLevel(conn, region_id);
}

bool ManagerService::submitManagerApply(const User& user, const std::string& canteen_name, int region_id)
{
    if (user.getUsername().empty() || user.getPassword().empty() || canteen_name.empty() || region_id <= 0) {
        return false;
    }

    if (!validateDistrictRegion(region_id)) {
        std::cout << "食堂管理者所在区域必须是区级" << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[ManagerService::submitManagerApply] Error: " << e.what() << std::endl;
        return false;
    }
}


std::vector<CanteenManagerApplyVO> ManagerService::getManagerApplyList(int reviewer_id) {

    DBConnectionGuard guard;
    auto* conn = guard.get();

    CanteenManagerApplyDAO dao;
    auto allApplies = dao.getApplyList(conn);

    // 根据审核者权限过滤申请
    AdminService adminService;
    std::vector<CanteenManagerApplyVO> filteredApplies;
    for (const auto& apply : allApplies) {
        if (adminService.hasManagerApplyReviewPermission(reviewer_id, apply.getRegionId())) {
            filteredApplies.push_back(apply);
        }
    }

    return filteredApplies;
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

        // 检查审核权限
        AdminService adminService;
        if (!adminService.hasManagerApplyReviewPermission(reviewer_id, apply->getRegionId())) {
            std::cerr << "审核者 " << reviewer_id << " 没有权限审核食堂管理者申请 " << apply_id << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "[ManagerService::reviewManagerApply] Error: " << e.what() << std::endl;
        return false;
    }
}

// ================================
// 用餐者服务
// ================================
std::vector<DinerInformation> DinerService::getDinerList(int viewer_id) {

    DBConnectionGuard guard;
    auto* conn = guard.get();

    try{
        // 获取查看者的管理员信息
        AdminDAO adminDAO;
        auto viewer = adminDAO.getAdminByUserId(conn, viewer_id);
        if (!viewer) {
            throw std::runtime_error("无权限"); // 如果不是管理员，终止并抛出异常
        }

        // 获取所有用餐者
        DinerDAO dinerDAO;
        auto allDiners = dinerDAO.getDinerList(conn);

        int viewer_level_id = viewer->getLevelId();
        int viewer_region_id = viewer->getRegionId();

        // 系统管理员（level_id=1）可以查看所有用餐者
        if (viewer_level_id == 1) {
            return allDiners;
        }

        RegionService regionService;

        // 市级管理员（level_id=2）可以查看辖区内所有用餐者
        // 区级管理员（level_id=3）可以查看本区域用餐者
        if (viewer_level_id == 2 || viewer_level_id == 3) {
            std::vector<DinerInformation> filteredDiners;
            for (const auto& diner : allDiners) {
                if (regionService.isRegionInScope(conn, diner.getRegionId(), viewer_region_id)) {
                    filteredDiners.push_back(diner);
                }
            }
            return filteredDiners;
        }

        return {};
    }catch (const std::exception& e) {
        std::cerr << "[DinerService::getDinerList] Error: " << e.what() << std::endl;
        return {};
    }
}

int CanteenService::getCanteenIdByUserId(int user_id) {
    CanteenDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getCanteenIdByUserId(conn, user_id);
}


/**********************************************
 *RegionService
 *********************************************/
std::vector<Region> RegionService::getRegionList() {
    RegionDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getRegionList(conn);
}

std::vector<Region> RegionService::getDistrictRegionList() {
    RegionDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getDistrictRegions(conn);
}

bool RegionService::isRegionInScope(sql::Connection *conn, int region_id, int parent_region_id) {
    if (region_id <= 0 || parent_region_id <= 0) {
        return false;
    }
    
    // 如果是同一区域，直接返回true
    if (region_id == parent_region_id) {
        return true;
    }
  
    RegionDAO regionDAO;
    
    try {
        // 检查parent_region_id是否是市级区域
        bool isCity = regionDAO.isCityLevel(conn, parent_region_id);
        
        if (isCity) {
            // 市级区域可以管理其下所有区级区域
            auto districts = regionDAO.getDistrictRegionsByCity(conn, parent_region_id);
            for (const auto& district : districts) {
                if (district.getId() == region_id) {
                    return true;
                }
            }
        } else {
            // 区级区域只能管理自己
            return region_id == parent_region_id;
        }
    } catch (const std::exception& e) {
        std::cerr << "[RegionService::isRegionInScope] Error: " << e.what() << std::endl;
    }
    return false;
}

/**********************************************
 * FamilyService
 *********************************************/
Family FamilyService::getFamilyByUserId(int user_id) {
    FamilyDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getFamilyByUserId(conn, user_id);
}

std::vector<Family> FamilyService::getFamilyList() {
    FamilyDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getFamilyList(conn);
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
        int family_id = familyDAO.insertFamily(conn, family);
        if (family_id == -1) {
            std::cout << "插入家庭失败" << std::endl;
            return false;
        }
        std::cout << "插入家庭成功，family_id=" << family_id << std::endl;

        // 更新用餐者的家庭ID
        DinerDAO dinerDAO;
        auto diner = dinerDAO.getDinerByUserId(conn, user_id);
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
        std::cerr << "[FamilyService::createFamily] Error: " << e.what() << std::endl;
        return false;
    }
}

/**********************************************
 * CanteenService
 **********************************************/
std::vector<Canteen> CanteenService::getAllCanteens() {
    CanteenDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getAllCanteens(conn);
}

std::shared_ptr<CanteenVO> CanteenService::getCanteenById(int id) {
    CanteenDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getCanteenById(conn, id);
}

bool CanteenService::updateCanteenAddress(int canteen_id, const std::string& address) {
    if (canteen_id <= 0 || address.empty()) {
        return false;
    }
    CanteenDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.updateCanteenAddress(conn, canteen_id, address);
}

bool CanteenService::updateCanteenStatus(int canteen_id, int status) {
    if (canteen_id <= 0 || (status != 0 && status != 1)) {
        return false;
    }
    CanteenDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.updateCanteenStatus(conn, canteen_id, status);
}

std::shared_ptr<CanteenVO> CanteenService::getCanteenDetails(int canteen_id) {
    if (canteen_id <= 0) {
        return nullptr;
    }
    CanteenDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getCanteenById(conn, canteen_id);
}

std::vector<CanteenManagerVO> CanteenService::getCanteensWithManagers() {
    CanteenDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getCanteensWithManagers(conn);
}

std::vector<PurchaseBill> CanteenService::getPurchaseBills(int canteen_id) {
    if (canteen_id <= 0) {
        return std::vector<PurchaseBill>();
    }
    CanteenDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getPurchaseBillsByCanteen(conn, canteen_id);
}

int CanteenService::createPurchaseBill(const PurchaseBill& bill) {
    if (bill.getCanteenId() <= 0 || bill.getAmount() <= 0 || bill.getPurchaseDate().empty() || bill.getRemark().empty()) {
        return -1;
    }
    CanteenDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.createPurchaseBill(conn, bill);
}

bool CanteenService::updatePurchaseBill(const PurchaseBill& bill) {
    if (bill.getId() <= 0 || bill.getAmount() <= 0 || bill.getPurchaseDate().empty()) {
        return false;
    }
    CanteenDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.updatePurchaseBill(conn, bill);
}

bool CanteenService::deletePurchaseBill(int bill_id) {
    if (bill_id <= 0) {
        return false;
    }
    CanteenDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.deletePurchaseBill(conn, bill_id);
}

// 财务统计 - 获取今日财务数据
TodayFinancialData CanteenService::getTodayFinancialData(int canteen_id) {
    TodayFinancialData data;
    
    try {
        CanteenDAO dao;
        DBConnectionGuard guard;
        auto* conn = guard.get();
        double income = dao.getTodayIncome(conn, canteen_id);
        double expense = dao.getTodayExpense(conn, canteen_id);
        data.setIncome(income);
        data.setExpense(expense);
        data.setProfit(income - expense);
    } catch (const std::exception& e) {
        std::cout << "获取今日财务数据失败: " << e.what() << std::endl;
        data.setIncome(0.0);
        data.setExpense(0.0);
        data.setProfit(0.0);
    }
    
    return data;
}

// 财务统计 - 获取指定时间维度的财务数据
FinancialData CanteenService::getFinancialData(int canteen_id, const std::string& time_dimension, const std::string& stats_type) {
    FinancialData data;
    
    try {
        CanteenDAO dao;
        DBConnectionGuard guard;
        auto* conn = guard.get();
        
        if (time_dimension == "day") {
            // 最近30天
            for (int i = 29; i >= 0; i--) {
                auto date = std::chrono::system_clock::now() - std::chrono::hours(24 * i);
                std::time_t date_c = std::chrono::system_clock::to_time_t(date);
                std::tm* date_tm = std::localtime(&date_c);
                char date_buf[11];
                std::strftime(date_buf, sizeof(date_buf), "%Y-%m-%d", date_tm);
                std::string date_str(date_buf);
                data.addLabel(date_str);
                
                double value = 0.0;
                if (stats_type == "income") {
                    value = dao.getIncomeByTimeDimension(conn, canteen_id, time_dimension, date_str);
                } else if (stats_type == "expense") {
                    value = dao.getExpenseByTimeDimension(conn, canteen_id, time_dimension, date_str);
                } else if (stats_type == "profit") {
                    double income = dao.getIncomeByTimeDimension(conn, canteen_id, time_dimension, date_str);
                    double expense = dao.getExpenseByTimeDimension(conn, canteen_id, time_dimension, date_str);
                    value = income - expense;
                }
                
                data.addValue(value);
            }
        } else if (time_dimension == "month") {
            // 最近12个月
            for (int i = 11; i >= 0; i--) {
                // 使用 std::chrono::duration 替代 std::chrono::months
                auto date = std::chrono::system_clock::now() - std::chrono::duration<int, std::ratio<2592000>>{i}; // 一个月的秒数
                std::time_t date_c = std::chrono::system_clock::to_time_t(date);
                std::tm* date_tm = std::localtime(&date_c);
                char date_buf[8];
                std::strftime(date_buf, sizeof(date_buf), "%Y-%m", date_tm);
                std::string date_str(date_buf);
                data.addLabel(date_str);
                
                double value = 0.0;
                if (stats_type == "income") {
                    value = dao.getIncomeByTimeDimension(conn, canteen_id, time_dimension, date_str);
                } else if (stats_type == "expense") {
                    value = dao.getExpenseByTimeDimension(conn, canteen_id, time_dimension, date_str);
                } else if (stats_type == "profit") {
                    double income = dao.getIncomeByTimeDimension(conn, canteen_id, time_dimension, date_str);
                    double expense = dao.getExpenseByTimeDimension(conn, canteen_id, time_dimension, date_str);
                    value = income - expense;
                }
                
                data.addValue(value);
            }
        } else if (time_dimension == "year") {
            // 最近10年
            for (int i = 9; i >= 0; i--) {
                // 使用 std::chrono::duration 替代 std::chrono::years
                auto date = std::chrono::system_clock::now() - std::chrono::duration<int, std::ratio<31536000>>{i}; // 一年的秒数
                std::time_t date_c = std::chrono::system_clock::to_time_t(date);
                std::tm* date_tm = std::localtime(&date_c);
                char date_buf[5];
                std::strftime(date_buf, sizeof(date_buf), "%Y", date_tm);
                std::string date_str(date_buf);
                data.addLabel(date_str);
                
                double value = 0.0;
                if (stats_type == "income") {
                    value = dao.getIncomeByTimeDimension(conn, canteen_id, time_dimension, date_str);
                } else if (stats_type == "expense") {
                    value = dao.getExpenseByTimeDimension(conn, canteen_id, time_dimension, date_str);
                } else if (stats_type == "profit") {
                    double income = dao.getIncomeByTimeDimension(conn, canteen_id, time_dimension, date_str);
                    double expense = dao.getExpenseByTimeDimension(conn, canteen_id, time_dimension, date_str);
                    value = income - expense;
                }
                
                data.addValue(value);
            }
        }
    } catch (const std::exception& e) {
        std::cout << "获取财务数据失败: " << e.what() << std::endl;
    }
    
    return data;
}

/**********************************************
 * MenuService
 *********************************************/
std::vector<Dish> MenuService::getMenuByMealType(int canteen_id, const std::string& meal_type) {
    MenuDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getMenuByMealType(conn, canteen_id, meal_type);
}

std::vector<CanteenMenuVO> MenuService::getMenuByCanteen(int canteen_id) {
    MenuDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getMenuByCanteen(conn, canteen_id);
}

bool MenuService::updateMenu(const MenuCreateDTO& menu) {
    DBConnectionGuard guard;
    auto* conn = guard.get();

    try {
        TransactionGuard tx(conn);

        HistoryMenuDAO historyMenuDAO;
        MenuDAO menuDAO;

        // 获取当前餐单的menu_id
        int menu_id = menuDAO.getMenuIdByCanteenAndMealType(conn, menu.getCanteenId(), menu.getMealType());
        if (menu_id == -1) {
            std::cout << "获取当前餐单ID失败" << "canteen_id:" << menu.getCanteenId() << " meal_type:" << menu.getMealType() << std::endl;
            return false;
        }

        std::vector<int> currentDishIds = menuDAO.getMenuDishIds(conn, menu_id);
        std::vector<int> newDishIds = menu.getDishIds();

        if (!currentDishIds.empty()) {
            // 找到当前餐厅的对应餐单的上一个历史餐单ID
            int history_menu_id = historyMenuDAO.getHistoryMenuIdByCanteenIdAndMealType(conn, menu.getCanteenId(), menu.getMealType());
            if (history_menu_id == -1) {
                // 如果没有历史餐单，直接创建新的历史餐单
                std::cout << "获取历史餐单ID失败" << "canteen_id:" << menu.getCanteenId() << " meal_type:" << menu.getMealType() << std::endl;
            } else if (!historyMenuDAO.updateHistoryMenuEndTime(conn, history_menu_id, menu.getCanteenId(), menu.getMealType())) {
                std::cout << "更新历史餐单结束时间失败" << "history_menu_id:" << history_menu_id << " canteen_id:" << menu.getCanteenId() << " meal_type:" << menu.getMealType() << std::endl;
                return false;
            }

            HistoryMenu historyMenu;
            historyMenu.setMenuId(menu_id);
            historyMenu.setCanteenId(menu.getCanteenId());
            historyMenu.setMealType(menu.getMealType());

            // 保存历史餐单
            int historyMenuId = historyMenuDAO.saveHistoryMenu(conn, historyMenu);
            if (historyMenuId == -1) {
                return false;
            }

            if (!historyMenuDAO.saveHistoryMenuDishes(conn, historyMenuId, currentDishIds)) {
                return false;
            }
        }

        // 对比新旧菜品列表，确定需要添加和删除的菜品ID
        std::vector<int> to_add;
        std::vector<int> to_remove;

        for (int dish_id : newDishIds) {
            if (std::find(currentDishIds.begin(), currentDishIds.end(), dish_id) == currentDishIds.end()) {
                to_add.push_back(dish_id);
            }
        }

        for (int dish_id : currentDishIds) {
            if (std::find(newDishIds.begin(), newDishIds.end(), dish_id) == newDishIds.end()) {
                to_remove.push_back(dish_id);
            }
        }

        // 更新menu_dish表
        for (int dish_id : to_remove) {
            menuDAO.deleteMenuDish(conn, menu_id, dish_id);
        }
        
        for (int dish_id : to_add) {
            menuDAO.insertMenuDish(conn, menu_id, dish_id);
        }

        tx.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[MenuService::updateMenu] Error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<HistoryMenu> MenuService::getHistoryMenusByCanteen(int canteen_id) {
    DBConnectionGuard guard;
    auto* conn = guard.get();
    
    try {     
        // 获取历史餐单列表
        HistoryMenuDAO historyMenuDAO;
        return historyMenuDAO.getHistoryMenusByCanteen(conn, canteen_id);
    } catch (const std::exception& e) {
        std::cerr << "[MenuService::getHistoryMenusByCanteen] Error: " << e.what() << std::endl;
        return std::vector<HistoryMenu>();
    }
}

std::vector<Dish> MenuService::getHistoryMenuDishes(int history_menu_id) {
    DBConnectionGuard guard;
    auto* conn = guard.get();
    
    try {
        HistoryMenuDAO historyMenuDAO;
        return historyMenuDAO.getHistoryMenuDishes(conn, history_menu_id);
    } catch (const std::exception& e) {
        std::cerr << "[MenuService::getHistoryMenuDishesByHistoryMenuId] Error: " << e.what() << std::endl;
        return std::vector<Dish>();
    }
}

/**********************************************
 * DishService
 *********************************************/
std::vector<Dish> DishService::getDishsByCanteen(int canteen_id) {
    DishDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getDishesByCanteen(conn, canteen_id);
}

int DishService::insertDish(const Dish& dish, const std::vector<int>& tag_ids) {
    DishDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();

    TransactionGuard tx(conn);

    int dish_id = dao.insertDish(conn, dish);
    if (dish_id <= 0) {
        return -1;
    }

    if (!tag_ids.empty()) {
        DishTagService dishTagService;
        dishTagService.updateDishTags(conn, dish_id, tag_ids);
    }

    tx.commit();
    return dish_id;
}

bool DishService::updateDish(int dish_id, double price, int calories, const std::string& nutrition_info, const std::vector<int>& tag_ids) {
    DishDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    TransactionGuard tx(conn);

    if (dao.updateDish(conn, dish_id, price, calories, nutrition_info) == -1) {
        std::cout << "更新菜品失败" << std::endl;
        return false;
    }

    DishTagService dishTagService;
    if (!dishTagService.updateDishTags(conn, dish_id, tag_ids)) {
        std::cout << "更新菜品标签失败" << std::endl;
        return false;
    }
    tx.commit();
    return true;
}

bool DishService::disableDishByDishId(const int dish_id) {
    // 检查菜品是否在餐单中
    MenuDAO menu_dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    
    if (menu_dao.isDishInMenu(conn, dish_id)) {
        std::cout<< " 菜品在餐单中，不能下架" << std::endl;
        return false; // 菜品在餐单中，不能下架
    }
    
    std::cout<< " 菜品不在餐单中，可以下架" << std::endl;
    DishDAO dao;
    return dao.disableDishByDishId(conn, dish_id);
}

bool DishService::enableDishByDishId(int dish_id) {
    DishDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.enableDishByDishId(conn, dish_id);
}

/**********************************************
 * OrderService
 *********************************************/
bool OrderService::placeOrder(int user_id,
                              int canteen_id,
                              int order_for_user_id,
                              const std::vector<OrderItem>& items) {

    std::cout << "开始下单：user_id=" << user_id << ", order_for_user_id=" << order_for_user_id << ", canteen_id=" << canteen_id << std::endl;

    auto user_mutex = getUserOrderMutex(user_id);
    std::lock_guard<std::mutex> user_lock(*user_mutex);

    DBConnectionGuard guard;
    auto* conn = guard.get();

    try{
        std::cout << "获取数据库连接成功" << std::endl;
        TransactionGuard tx(conn);
        std::cout << "开始事务" << std::endl;
        
        if (items.empty()) {
            std::cout << "订单为空" << std::endl;
            return false;
        }

        DishDAO dishDAO;
        OrderDAO orderDAO;
        UserDAO userDAO;

        double total = 0.0;

        auto dishes = dishDAO.getDishesByCanteen(conn, canteen_id);// 获取食堂菜品（可以优化成批量查询）
        std::cout << "获取到" << dishes.size() << "个菜品" << std::endl;
        // 1️⃣ 计算总价（业务逻辑）
        std::vector<OrderItem> orderItems;
        
        for (const auto& item : items) {
            std::cout << "菜品ID：" << item.getDishId() << "，数量：" << item.getQuantity() << std::endl;
            for (const auto& d : dishes) {
                if (d.getId() == item.getDishId()) {
                    std::cout << "找到菜品：" << d.getName() << "，价格：" << d.getPrice() << std::endl;
                    total += d.getPrice() * item.getQuantity();
                    
                    // 构造订单项，设置新的字段
                    OrderItem orderItem;
                    orderItem.setOrderId(0); // 订单ID稍后设置
                    orderItem.setDishId(item.getDishId());
                    orderItem.setQuantity(item.getQuantity());
                    orderItem.setUnitPrice(d.getPrice());
                    orderItems.push_back(orderItem);
                }
            }
        }
        std::cout << "原始总价：" << total << std::endl;

        // 获取用餐者信息，计算折扣
        auto orderForUser = userDAO.getUserById(conn, order_for_user_id);
        double discount = 1.0;
        if (orderForUser) {
            int age = orderForUser->getAge();
            std::cout << "用餐者年龄：" << age << std::endl;
            if (age >= 60 && age <= 69) {
                discount = 0.9; // 9折
                std::cout << "应用9折优惠" << std::endl;
            } else if (age >= 70 && age <= 79) {
                discount = 0.7; // 7折
                std::cout << "应用7折优惠" << std::endl;
            } else if (age >= 80 && age <= 89) {
                discount = 0.5; // 5折
                std::cout << "应用5折优惠" << std::endl;
            } else if (age >= 90) {
                discount = 0.0; // 免费
                std::cout << "应用免费优惠" << std::endl;
            } else {
                std::cout << "无优惠" << std::endl;
            }
        } else {
            std::cout << "未找到用餐者信息" << std::endl;
        }

        // 应用折扣
        double discountedTotal = total * discount;
        double savedAmount = total - discountedTotal;
        std::cout << "折扣后总价：" << discountedTotal << std::endl;
        std::cout << "优惠金额：" << savedAmount << std::endl;

        // 设置订单项的折扣价格和小计
        for (auto& orderItem : orderItems) {
            double unitPrice = orderItem.getUnitPrice();
            double discountPrice = unitPrice * discount;
            double subtotal = discountPrice * orderItem.getQuantity();
            orderItem.setDiscountPrice(discountPrice);
            orderItem.setSubtotal(subtotal);
        }

        DinerDAO dinerDAO;
        bool canOrderForTarget = false;
        canOrderForTarget = true;
        
        /* 菜单列表界面的为谁点餐列表中显示的用餐者已经是经过筛选的家庭成员，不用再进行确认，故注释
        // 如果是为自己点餐，直接允许
        if (user_id == order_for_user_id) {
            canOrderForTarget = true;
            std::cout << "为自己点餐，允许下单" << std::endl;
        } else {
            // 否则检查是否为家庭成员
            auto familyMembers = dinerDAO.getFamilyMembersByUserId(conn, user_id);
            std::cout << "获取到" << familyMembers.size() << "个家庭成员" << std::endl;
            for (const auto& member : familyMembers) {
                std::cout << "家庭成员ID：" << member.getUserId() << "，姓名：" << member.getUsername() << std::endl;
                if (member.getUserId() == order_for_user_id) {
                    canOrderForTarget = true;
                    std::cout << "可以为该用户点餐" << std::endl;
                    break;
                }
            }
        }
        */
        
        if (!canOrderForTarget) {
            std::cout << "不能为该用户点餐" << std::endl;
            return false;
        }

        // 2️⃣ 构造订单
        Order order;
        order.setUserId(user_id);
        order.setOrderForUserId(order_for_user_id);
        order.setCanteenId(canteen_id);
        order.setTotalPrice(discountedTotal);
        order.setStatus(0);
        order.setDiscountRate(discount);
        order.setOriginalTotal(total);
        order.setSavedAmount(savedAmount);
        std::cout << "订单构造完成" << std::endl;

        // 3️⃣ 创建订单（事务）
        std::cout << "开始创建订单" << std::endl;
        int order_id = orderDAO.insertOrder(conn, order);
        if (order_id == -1) {
            std::cout << "创建订单失败" << std::endl;
            return false;
        }
        std::cout << "创建订单成功，订单ID：" << order_id << std::endl;

        // 设置订单项的订单ID
        for (auto& orderItem : orderItems) {
            orderItem.setOrderId(order_id);
        }

        OrderItemDAO orderItemDAO;
        std::cout << "开始插入订单项" << std::endl;
        if (!orderItemDAO.insertOrderItems(conn, order_id, orderItems)) {
            std::cout << "插入订单项失败" << std::endl;
            return false;
        }
        std::cout << "插入订单项成功" << std::endl;

        tx.commit(); // 提交事务
        std::cout << "事务提交成功" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[OrderService::placeOrder] Error: " << e.what() << std::endl;
        return false;
        }
}

std::vector<FamilyMemberVO> OrderService::getOrderTargetsByUser(int user_id)
{
    DinerDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getFamilyMembersByUserId(conn, user_id);
}

std::vector<OrderVO> OrderService::getOrdersByUser(int user_id)
{
    OrderDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getOrdersByUser(conn, user_id);
}

std::vector<OrderVO> OrderService::getOrdersByCanteen(int canteen_id)
{
    OrderDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getOrdersByCanteen(conn, canteen_id);
}

bool OrderService::updateOrderStatus(int order_id, int status) {
    OrderDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.updateOrderStatus(conn, order_id, status);
}

std::vector<OrderDetailVO> OrderService::getOrdersDetailsByOrderId(int order_id) {
    OrderDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getOrdersDetailsByOrderId(conn, order_id);
}

// 用餐偏好相关方法
DiningPreference OrderService::getDiningPreference(int user_id, const std::string& time_dimension) {
    DiningPreference preference;
    try {
        OrderDAO dao;
        DBConnectionGuard guard;
        auto* conn = guard.get();
        
        // 获取用餐偏好摘要
        auto summary = dao.getDiningPreferenceSummary(conn, user_id, time_dimension);
        DiningPreferenceSummary summary_vo;
        summary_vo.setTotalAmount(summary.getTotalAmount());
        summary_vo.setOrderCount(summary.getOrderCount());
        summary_vo.setCanteenCount(summary.getCanteenCount());
        preference.setSummary(summary_vo);
        
        // 获取餐厅消费次数
        auto canteen_consumption = dao.getCanteenConsumptionCount(conn, user_id, time_dimension);
        for (const auto& item : canteen_consumption) {
            ConsumptionItem consumption_item;
            consumption_item.setName(item.first);
            consumption_item.setCount(item.second);
            preference.addCanteenConsumption(consumption_item);
        }
        
        // 获取菜品消费次数
        auto dish_consumption = dao.getDishConsumptionCount(conn, user_id, time_dimension);
        for (const auto& item : dish_consumption) {
            ConsumptionItem consumption_item;
            consumption_item.setName(item.first);
            consumption_item.setCount(item.second);
            preference.addDishConsumption(consumption_item);
        }
    } catch (const std::exception& e) {
        std::cerr << "获取用餐偏好失败: " << e.what() << std::endl;
    }
    return preference;
}

std::shared_ptr<RecentOrderVO> OrderService::getRecentOrder(int user_id, int order_for_user_id, int canteen_id) {
    try {
        OrderDAO dao;
        DBConnectionGuard guard;
        auto* conn = guard.get();
        return dao.getRecentOrder(conn, user_id, order_for_user_id, canteen_id);
    } catch (const std::exception& e) {
        std::cerr << "获取最近订单失败: " << e.what() << std::endl;
        return nullptr;
    }
}

/**********************************************
 * RatingService
 *********************************************/
bool RatingService::submitRating(const Rating& rating) {
    RatingDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();

    // 校验评分
    if (rating.getScore() < 1 || rating.getScore() > 5) {
        return false;
    }

    return dao.insertRating(conn, rating);
}

std::vector<Rating> RatingService::getRatings(int canteen_id) {
    RatingDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getRatingsByCanteen(conn, canteen_id);
}

std::vector<CanteenRatingVO> RatingService::getCanteenRatingDetails(int canteen_id) {
    RatingDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getCanteenRatingDetails(conn, canteen_id);
}

/**********************************************
 * ReportService
 *********************************************/
bool ReportService::addReport(const Report& report) {
    ReportDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();

    if (report.getContent().empty()) return false;

    return dao.insertReport(conn, report);
}

std::vector<Report> ReportService::getReportsByCanteen(int canteen_id) {
    ReportDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getReportsByCanteen(conn, canteen_id);
}

std::vector<ReportVO> ReportService::getAllReports() {
    ReportDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getAllReports(conn);
}

bool ReportService::updateReportStatus(int report_id, int status, int handler_id) {
    if (report_id <= 0 || handler_id <= 0) {
        return false;
    }
    if (status != 1 && status != 2) {
        return false;
    }
    ReportDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.updateReportStatus(conn, report_id, status, handler_id);
}

/**********************************************
 * AnnouncementService
 *********************************************/
bool AnnouncementService::publishAnnouncement(const Announcement& announcement) {
    if (announcement.getTitle().empty() || announcement.getContent().empty() || announcement.getPublisherId() <= 0) {
        return false;
    }

    AnnouncementDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.insertAnnouncement(conn, announcement);
}

std::vector<AnnouncementVO> AnnouncementService::getAnnouncementList() {
    AnnouncementDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getAnnouncementList(conn);
}

bool AnnouncementService::deleteAnnouncement(int announce_id, int publisher_id) {
    if (announce_id <= 0 || publisher_id <= 0) {
        return false;
    }
    AnnouncementDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.deleteAnnouncement(conn, announce_id, publisher_id);
}

/**********************************************
 * MessageService
 *********************************************/
bool MessageService::createMessage(const Message& message) {
    if (message.getCanteenId() <= 0 || message.getUserId() <= 0 || message.getContent().empty()) {
        return false;
    }
    MessageDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.insertMessage(conn, message);
}

std::vector<Message> MessageService::getMessagesByCanteen(int canteen_id) {
    if (canteen_id <= 0) {
        return {};
    }
    MessageDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getMessagesByCanteen(conn, canteen_id);
}

std::vector<Message> MessageService::getMessagesByUser(int user_id, int canteen_id) {
    if (user_id <= 0 || canteen_id <= 0) {
        return {};
    }
    MessageDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getMessagesByUser(conn, user_id, canteen_id);
}

bool MessageService::replyMessage(const Message& message) {
    if (message.getId() <= 0 || message.getReplyContent().empty()) {
        return false;
    }
    MessageDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.replyMessage(conn, message);
}

/***************************************************************************************
 * MessageCenterService
 ***************************************************************************************/
int MessageCenterService::createMessage(const MessageNotification& message) {
    if (message.getSenderId() <= 0 || message.getReceiverId() <= 0 || message.getContent().empty()) {
        return -1;
    }
    MessageCenterDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.createMessage(conn, message);
}

std::vector<MessageNotification> MessageCenterService::getMessagesByReceiver(int receiver_id) {
    if (receiver_id <= 0) {
        return {};
    }
    MessageCenterDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getMessagesByReceiver(conn, receiver_id);
}

bool MessageCenterService::updateMessageStatus(int message_id, int status) {
    if (message_id <= 0 || (status != 0 && status != 1)) {
        return false;
    }
    MessageCenterDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.updateMessageStatus(conn, message_id, status);
}

/**********************************************
 * OrderCancelService
 *********************************************/
bool OrderCancelService::createCancelApply(int order_id, const std::string& cancel_reason) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();
        
        //已取消申请，不允许重复申请
        auto cancel_apply = getCancelApplyByOrderId(order_id);
        if (cancel_apply) {
            return false;
        }

        OrderCancelDAO dao;
        int cancel_id = dao.insertCancelApply(conn, order_id, cancel_reason);
        return cancel_id > 0;
    } catch (const std::exception& e) {
        std::cerr << "创建取消申请失败: " << e.what() << std::endl;
        return false;
    }
}

std::vector<OrderCancelVO> OrderCancelService::getCancelAppliesByCanteen(int canteen_id) {
    OrderCancelDAO dao;
    DBConnectionGuard guard;
    auto* conn = guard.get();
    return dao.getCancelAppliesByCanteen(conn, canteen_id);
}

bool OrderCancelService::handleCancelApply(int cancel_id, int status, const std::string& reject_reason) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();
        TransactionGuard tx(conn);

        OrderCancelDAO dao;
        if (dao.updateCancelStatus(conn, cancel_id, status, reject_reason)) {
            // 如果同意取消，更新订单状态为已取消
            if (status == 1) {
                OrderDAO order_dao;
                OrderCancelDAO cancel_dao;
                // 获取订单ID
                auto cancel_apply = cancel_dao.getCancelApplyByCancelId(conn, cancel_id);
                if (cancel_apply) {
                    int order_id = cancel_apply->getOrderId();
                    // 更新订单状态为已取消
                    if (!order_dao.updateOrderStatus(conn, order_id, 2)) {
                        std::cout<<"updateOrderStatus failed"<<std::endl;
                        return false;
                    }
                }
            } else if (status == 2) { // 拒绝取消申请，发送消息给用户
                OrderDAO order_dao;
                OrderCancelDAO cancel_dao;
                // 获取取消申请信息
                auto cancel_apply = cancel_dao.getCancelApplyByCancelId(conn, cancel_id);
                if (cancel_apply) {
                    int order_id = cancel_apply->getOrderId();
                    // 获取订单信息
                    auto order = order_dao.getOrderById(conn, order_id);
                    if (order) {
                        int user_id = order->getUserId();
                        // 构建消息内容
                        std::string content = "您申请取消的订单（订单号：" + std::to_string(order_id) + "）已被食堂拒绝。原因：" + reject_reason + "。";
                        // 创建消息
                        MessageNotification message;
                        message.setSenderId(1); // 系统发送
                        message.setReceiverId(user_id);
                        message.setContent(content);
                        message.setStatus(0); // 未读
                        // 发送消息
                        MessageCenterDAO message_dao;
                        if (!message_dao.createMessage(conn, message)) {
                            std::cout<<"createMessage failed"<<std::endl;
                            return false;
                        }
                    }
                }
                else {
                    std::cout<<"cancel_apply is null"<<std::endl;
                    return false;
                }
            }
            tx.commit();
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        std::cerr << "处理取消申请失败: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<OrderCancelVO> OrderCancelService::getCancelApplyByOrderId(int order_id) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();
        OrderCancelDAO dao;
        return dao.getCancelApplyByOrderId(conn, order_id);
    } catch (const std::exception& e) {
        std::cerr << "获取取消申请失败: " << e.what() << std::endl;
        return nullptr;
    }
}

/***************************************************************************************
 * TagService
 ***************************************************************************************/
std::vector<Tag> TagService::getAllTags() {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();
        TagDAO dao;
        return dao.getAllTags(conn);
    } catch (const std::exception& e) {
        std::cerr << "获取所有标签失败: " << e.what() << std::endl;
        return {};
    }
}

std::vector<Tag> TagService::getTagsByDishId(int dish_id) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();
        TagDAO dao;
        return dao.getTagsByDishId(conn, dish_id);
    } catch (const std::exception& e) {
        std::cerr << "获取菜品标签失败: " << e.what() << std::endl;
        return {};
    }
}

/***************************************************************************************
 * DishTagService
 ***************************************************************************************/
bool DishTagService::updateDishTags(int dish_id, const std::vector<int>& tag_ids) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();
        TransactionGuard tx(conn);

        DishTagDAO dishTagDao;
        // 获取当前菜品的所有标签ID
        std::vector<int> old_tag_ids = dishTagDao.getTagIdsByDishId(conn, dish_id);

        std::vector<int> to_add;
        std::vector<int> to_remove;

        // 对比新旧标签ID，确定需要添加和删除的标签ID
        for (int new_id : tag_ids) {
            if (std::find(old_tag_ids.begin(), old_tag_ids.end(), new_id) == old_tag_ids.end()) {
                to_add.push_back(new_id);
            }
        }

        for (int old_id : old_tag_ids) {
            if (std::find(tag_ids.begin(), tag_ids.end(), old_id) == tag_ids.end()) {
                to_remove.push_back(old_id);
            }
        }

        // 更新菜品标签
        for (int tag_id : to_remove) {
            dishTagDao.deleteDishTag(conn, dish_id, tag_id);
        }

        for (int tag_id : to_add) {
            dishTagDao.insertDishTag(conn, dish_id, tag_id);
        }

        tx.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "更新菜品标签失败: " << e.what() << std::endl;
        return false;
    }
}

bool DishTagService::updateDishTags(sql::Connection *conn, int dish_id, const std::vector<int>& tag_ids) {
    try {
        DishTagDAO dishTagDao;
        // 获取当前菜品的所有标签ID
        std::vector<int> old_tag_ids = dishTagDao.getTagIdsByDishId(conn, dish_id);

        std::vector<int> to_add;
        std::vector<int> to_remove;

        // 对比新旧标签ID，确定需要添加和删除的标签ID
        for (int new_id : tag_ids) {
            if (std::find(old_tag_ids.begin(), old_tag_ids.end(), new_id) == old_tag_ids.end()) {
                to_add.push_back(new_id);
            }
        }

        for (int old_id : old_tag_ids) {
            if (std::find(tag_ids.begin(), tag_ids.end(), old_id) == tag_ids.end()) {
                to_remove.push_back(old_id);
            }
        }

        // 更新菜品标签
        for (int tag_id : to_remove) {
            dishTagDao.deleteDishTag(conn, dish_id, tag_id);
        }

        for (int tag_id : to_add) {
            dishTagDao.insertDishTag(conn, dish_id, tag_id);
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "更新菜品标签失败: " << e.what() << std::endl;
        return false;
    }
}

std::vector<int> DishTagService::getTagIdsByDishId(int dish_id) {
    try {
        DBConnectionGuard guard;
        auto* conn = guard.get();
        DishTagDAO dao;
        return dao.getTagIdsByDishId(conn, dish_id);
    } catch (const std::exception& e) {
        std::cerr << "获取菜品标签ID失败: " << e.what() << std::endl;
        return {};
    }
}
