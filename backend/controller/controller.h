#pragma once
#include "../../third_party/httplib.h"
#include "../service/service.h"

class Controller {
public:
    static void initRoutes(httplib::Server& server);

private:
    static void registerUserRoutes(httplib::Server& server);
    static void registerOrderRoutes(httplib::Server& server);
    static void registerCanteenRoutes(httplib::Server& server);
    static void registerUserCenterRoutes(httplib::Server& server);

    //用户注册登录
    static void handleLogin(const httplib::Request& req, httplib::Response& res);
    static void handleRegister(const httplib::Request& req, httplib::Response& res);
    //食堂相关
    static void handleCanteens(const httplib::Request& req, httplib::Response& res);
        // 餐单
    static void handleMenu(const httplib::Request& req, httplib::Response& res);
    static void handleGetCanteenMenus(const httplib::Request& req, httplib::Response& res);
    static void handleUpdateMenu(const httplib::Request& req, httplib::Response& res);
        // 菜品
    static void handleGetDishes(const httplib::Request& req, httplib::Response& res);
    static void handleCreateDish(const httplib::Request& req, httplib::Response& res);
    static void handleDisableDish(const httplib::Request& req, httplib::Response& res);
    static void handleEnableDish(const httplib::Request& req, httplib::Response& res);
    
    //订单相关
    static void handlePlaceOrder(const httplib::Request& req, httplib::Response& res);
    static void handleOrderTargets(const httplib::Request& req, httplib::Response& res);
    static void handleGetOrders(const httplib::Request& req, httplib::Response& res);
    static void handleOrderDetails(const httplib::Request& req, httplib::Response& res);
    static void handleRecentOrder(const httplib::Request& req, httplib::Response& res);

    static void handleRating(const httplib::Request& req, httplib::Response& res);
    static void handleReport(const httplib::Request& req, httplib::Response& res);
    // 用餐偏好
    static void handleDiningPreference(const httplib::Request& req, httplib::Response& res);
    //个人中心相关
    static void handleUserCenter(const httplib::Request& req, httplib::Response& res);
    static void handleUserCenterUpdate(const httplib::Request& req, httplib::Response& res);
    static void handleFamilyList(const httplib::Request& req, httplib::Response& res);
    static void handleCreateFamily(const httplib::Request& req, httplib::Response& res);
    static void handleRegionList(const httplib::Request& req, httplib::Response& res);
    //管理员
    static void handleAdminList(const httplib::Request& req, httplib::Response& res);
    static void handleAdminApplyList(const httplib::Request& req, httplib::Response& res);
    static void handleAdminApplyReview(const httplib::Request& req, httplib::Response& res);
    static void handleManagerApplyList(const httplib::Request& req, httplib::Response& res);
    static void handleManagerApplyReview(const httplib::Request& req, httplib::Response& res);
    static void handleDinerList(const httplib::Request& req, httplib::Response& res);
    static void handleUpdateStatus(const httplib::Request& req, httplib::Response& res);
    //公告
    static void handleAnnouncementList(const httplib::Request& req, httplib::Response& res);
    static void handleAnnouncementPublish(const httplib::Request& req, httplib::Response& res);
    static void handleAnnouncementDelete(const httplib::Request& req, httplib::Response& res);
    // 投诉处理
    static void handleReportList(const httplib::Request& req, httplib::Response& res);
    static void handleReportHandle(const httplib::Request& req, httplib::Response& res);
    
    // 食堂管理
    static void handleMyCanteen(const httplib::Request& req, httplib::Response& res);
    static void handleUpdateCanteenAddress(const httplib::Request& req, httplib::Response& res);
    static void handleCanteenList(const httplib::Request& req, httplib::Response& res);
    static void handleUpdateCanteenStatus(const httplib::Request& req, httplib::Response& res);
    static void handlePurchaseList(const httplib::Request& req, httplib::Response& res);
    static void handleCreatePurchase(const httplib::Request& req, httplib::Response& res);
    // 财务统计
    static void handleFinancialStatistics(const httplib::Request& req, httplib::Response& res);
    // 留言板
    static void handleCreateMessage(const httplib::Request& req, httplib::Response& res);
    static void handleGetUserMessages(const httplib::Request& req, httplib::Response& res);
    static void handleGetCanteenMessages(const httplib::Request& req, httplib::Response& res);
    static void handleReplyMessage(const httplib::Request& req, httplib::Response& res);
};
