#pragma once

#include <string>

// ================================
// PasswordUtil
// 密码安全存储工具类
// 使用bcrypt算法实现密码哈希
// ================================
class PasswordUtil {
public:
    /**
     * @brief 对密码进行bcrypt哈希
     * @param password 原始密码
     * @return bcrypt哈希值（包含salt）
     *         格式: $2b$12$xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
     */
    static std::string hashPassword(const std::string& password);

    /**
     * @brief 验证密码是否匹配哈希值
     * @param password 用户输入的密码
     * @param hash 数据库中存储的bcrypt哈希值
     * @return true表示密码匹配，false表示不匹配
     */
    static bool verifyPassword(const std::string& password, const std::string& hash);

private:
    // 禁止实例化
    PasswordUtil() = delete;
    ~PasswordUtil() = delete;
    PasswordUtil(const PasswordUtil&) = delete;
    PasswordUtil& operator=(const PasswordUtil&) = delete;
};
