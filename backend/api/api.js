import request from "./request";

// ============================ 用户相关 ============================
// 登录
export function login(data) {
  return request({
    url: "/login",
    method: "post",
    data
  });
}

// 注册
export function register(data) {
  return request({
    url: "/register",
    method: "post",
    data
  });
}

//============================ 食堂相关 ============================
// 获取食堂列表
export function getCanteens() {
  return request({
    url: "/canteens",
    method: "get"
  });
}

//============================ 订单相关 ============================
// 下单
export function createOrder(data) {
  return request({
    url: "/orders",
    method: "post",
    data
  });
}

// 获取订单
export function getOrders() {
  return request({
    url: "/orders",
    method: "get"
  });
}


//============================ 评价相关 ============================
// 提交评价
export function createRating(data) {
  return request({
    url: "/rating",
    method: "post",
    data
  });
}


//============================ 举报相关 ============================
// 举报
export function createReport(data) {
  return request({
    url: "/report",
    method: "post",
    data
  });
}