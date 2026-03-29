import axios from "axios";

// 创建实例
const service = axios.create({
  baseURL: "http://localhost:8080", // 后端地址
  timeout: 5000,
  headers: {
    "Content-Type": "application/json"
  }
});

// 请求拦截器
service.interceptors.request.use(
  (config) => {
    // 可加 token
    const token = localStorage.getItem("token");
    if (token) {
      config.headers["Authorization"] = token;
    }
    return config;
  },
  (error) => {
    return Promise.reject(error);
  }
);

// 响应拦截器
service.interceptors.response.use(
  (response) => {
    const res = response.data;

    // 统一后端返回格式
    if (res.code !== 0) {
      alert(res.msg || "请求失败");
      return Promise.reject(res);
    }

    return res.data;
  },
  (error) => {
    console.error("网络错误:", error);
    alert("服务器异常");
    return Promise.reject(error);
  }
);

export default service;