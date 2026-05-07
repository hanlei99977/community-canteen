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

    /*************************************用户通用**************************************/
    // 登录注册
    static void handleLogin(const httplib::Request& req, httplib::Response& res);
    static void handleRegister(const httplib::Request& req, httplib::Response& res);
    // 个人中心相关
    static void handleUserCenter(const httplib::Request& req, httplib::Response& res);
    static void handleUserCenterUpdate(const httplib::Request& req, httplib::Response& res);
    // 所在区域
    static void handleRegionList(const httplib::Request& req, httplib::Response& res);
    static void handleDistrictRegionList(const httplib::Request& req, httplib::Response& res);
    static void handleCityRegionList(const httplib::Request& req, httplib::Response& res);
    static void handleGetDistrictsByCity(const httplib::Request& req, httplib::Response& res);
    // 密码修改
    static void handleChangePassword(const httplib::Request& req, httplib::Response& res);

    /*************************************用餐者**************************************/
    // 用餐偏好
    static void handleDiningPreference(const httplib::Request& req, httplib::Response& res);
    static void handleGetDishPurchaseRanking(const httplib::Request& req, httplib::Response& res);
    static void handleGetDishDetail(const httplib::Request& req, httplib::Response& res);
    // 疾病
    static void handleGetAllDiseases(const httplib::Request& req, httplib::Response& res);
    static void handleGetUserDiseases(const httplib::Request& req, httplib::Response& res);
    static void handleUpdateUserDiseases(const httplib::Request& req, httplib::Response& res);
    // 疾病管理
    static void handleGetAllDiseaseDetails(const httplib::Request& req, httplib::Response& res);
    static void handleCreateDisease(const httplib::Request& req, httplib::Response& res);
    static void handleUpdateDisease(const httplib::Request& req, httplib::Response& res);
    // 家庭
    static void handleFamilyList(const httplib::Request& req, httplib::Response& res);
    static void handleCreateFamily(const httplib::Request& req, httplib::Response& res);
    // 消息中心
    static void handleGetMessages(const httplib::Request& req, httplib::Response& res);
    static void handleMarkMessageAsRead(const httplib::Request& req, httplib::Response& res);
    /*************************************食堂**************************************/
    // 食堂管理
    static void handleCanteens(const httplib::Request& req, httplib::Response& res);
    // 我的食堂
    static void handleMyCanteen(const httplib::Request& req, httplib::Response& res);
    static void handleUpdateCanteenAddress(const httplib::Request& req, httplib::Response& res);
    // 食堂列表
    static void handleCanteenList(const httplib::Request& req, httplib::Response& res);
    // 更新食堂状态
    static void handleUpdateCanteenStatus(const httplib::Request& req, httplib::Response& res);
    // 日常采购
    static void handleCreatePurchase(const httplib::Request& req, httplib::Response& res);
    static void handlePurchaseList(const httplib::Request& req, httplib::Response& res);
    static void handleUpdatePurchase(const httplib::Request& req, httplib::Response& res);
    static void handleDeletePurchase(const httplib::Request& req, httplib::Response& res);
    // 餐单发布
    static void handleMenu(const httplib::Request& req, httplib::Response& res);
    static void handleGetCanteenMenus(const httplib::Request& req, httplib::Response& res);
    static void handleUpdateMenu(const httplib::Request& req, httplib::Response& res);
    static void handleGetHistoryMenus(const httplib::Request& req, httplib::Response& res);
    // 菜品管理
    static void handleGetDishes(const httplib::Request& req, httplib::Response& res);
    static void handleCreateDish(const httplib::Request& req, httplib::Response& res);
    static void handleDisableDish(const httplib::Request& req, httplib::Response& res);
    static void handleEnableDish(const httplib::Request& req, httplib::Response& res);
    static void handleDishSales(const httplib::Request& req, httplib::Response& res);
    // 留言板
    static void handleCreateMessage(const httplib::Request& req, httplib::Response& res);
    static void handleGetUserMessages(const httplib::Request& req, httplib::Response& res);
    static void handleGetCanteenMessages(const httplib::Request& req, httplib::Response& res);
    static void handleReplyMessage(const httplib::Request& req, httplib::Response& res);
    /*************************************管理员**************************************/
    //管理员
    static void handleAdminList(const httplib::Request& req, httplib::Response& res);
    static void handleAdminApplyList(const httplib::Request& req, httplib::Response& res);
    static void handleAdminApplyReview(const httplib::Request& req, httplib::Response& res);
    static void handleManagerApplyList(const httplib::Request& req, httplib::Response& res);
    static void handleManagerApplyReview(const httplib::Request& req, httplib::Response& res);
    static void handleDinerList(const httplib::Request& req, httplib::Response& res);
    static void handleUpdateStatus(const httplib::Request& req, httplib::Response& res);
    //公告发布
    static void handleAnnouncementList(const httplib::Request& req, httplib::Response& res);
    static void handleAnnouncementPublish(const httplib::Request& req, httplib::Response& res);
    static void handleAnnouncementDelete(const httplib::Request& req, httplib::Response& res);
    // 投诉处理
    static void handleReportList(const httplib::Request& req, httplib::Response& res);
    static void handleReportHandle(const httplib::Request& req, httplib::Response& res);
    static void handleReportStatistics(const httplib::Request& req, httplib::Response& res);
    static void handleReportTrend(const httplib::Request& req, httplib::Response& res);
    static void handleReportTypeDistribution(const httplib::Request& req, httplib::Response& res);
    static void handleTopCanteenReports(const httplib::Request& req, httplib::Response& res);
    static void handleFilteredReports(const httplib::Request& req, httplib::Response& res);
    static void handleCanteenReviews(const httplib::Request& req, httplib::Response& res);
    static void handleCanteenComplaints(const httplib::Request& req, httplib::Response& res);
    /*************************************订单相关**************************************/
    // 下单
    static void handlePlaceOrder(const httplib::Request& req, httplib::Response& res);
    static void handleOrderTargets(const httplib::Request& req, httplib::Response& res);
    // 获取订单
    static void handleGetOrders(const httplib::Request& req, httplib::Response& res);
    static void handleOrderDetails(const httplib::Request& req, httplib::Response& res);
    static void handleRecentOrder(const httplib::Request& req, httplib::Response& res);
    static void handleCanteenOrders(const httplib::Request& req, httplib::Response& res);
    // 更新订单状态
    static void handleUpdateOrderStatus(const httplib::Request& req, httplib::Response& res);
    // 取消订单
    static void handleCreateCancelApply(const httplib::Request& req, httplib::Response& res);
    static void handleGetCancelApplies(const httplib::Request& req, httplib::Response& res);
    static void handleHandleCancelApply(const httplib::Request& req, httplib::Response& res);
    static void handleGetCancelApplyByOrderId(const httplib::Request& req, httplib::Response& res);
    
    /*************************************评价**************************************/
    static void handleRating(const httplib::Request& req, httplib::Response& res);
    static void handleGetRating(const httplib::Request& req, httplib::Response& res);
    static void handleReport(const httplib::Request& req, httplib::Response& res);
    /*************************************数据统计**************************************/
    static void handleFinancialStatistics(const httplib::Request& req, httplib::Response& res);
    /*************************************标签相关**************************************/
    static void handleGetAllTags(const httplib::Request& req, httplib::Response& res);
    static void handleGetDishTags(const httplib::Request& req, httplib::Response& res);
    static void handleUpdateDish(const httplib::Request& req, httplib::Response& res);
    /*************************************收藏相关**************************************/
    static void handleAddFavorite(const httplib::Request& req, httplib::Response& res);
    static void handleRemoveFavorite(const httplib::Request& req, httplib::Response& res);
    static void handleGetFavorites(const httplib::Request& req, httplib::Response& res);
    static void handleCheckFavorite(const httplib::Request& req, httplib::Response& res);
};
