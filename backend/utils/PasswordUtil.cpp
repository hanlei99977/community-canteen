#include "PasswordUtil.h"
#include <crypt.h>
#include <cstring>
#include <random>
#include <stdexcept>

// bcrypt常量
const int BCRYPT_COST = 12;
const char* BCRYPT_PREFIX = "$2b$";

// 生成随机salt
static std::string generateSalt() {
    // bcrypt salt格式: $2b$XX$XXXXXXXXXXXXXXXXXXXX
    // XX是cost factor，后面22个字符是Base64编码的16字节salt
    
    const char base64_chars[] = "./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    
    // 生成16字节随机salt
    unsigned char salt_bytes[16];
    
    // 使用随机设备生成随机数
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    for (int i = 0; i < 16; i++) {
        salt_bytes[i] = static_cast<unsigned char>(dis(gen));
    }
    
    // 转换为bcrypt的Base64格式
    std::string result;
    result.reserve(22);
    
    int i = 0;
    while (i < 16) {
        unsigned int val = (salt_bytes[i] << 16) | (salt_bytes[i+1] << 8) | salt_bytes[i+2];
        result += base64_chars[(val >> 18) & 0x3F];
        result += base64_chars[(val >> 12) & 0x3F];
        result += base64_chars[(val >> 6) & 0x3F];
        result += base64_chars[val & 0x3F];
        i += 3;
    }
    
    // 构建完整的salt字符串
    // bcrypt格式: $2b$XX$XXXXXXXXXXXXXXXXXXXX (XX是cost factor，22位Base64 salt)
    char salt[30];
    snprintf(salt, sizeof(salt), "$2b$%02d$%s", BCRYPT_COST, result.substr(0, 22).c_str());
    
    return std::string(salt);
}

std::string PasswordUtil::hashPassword(const std::string& password) {
    // 生成随机salt
    std::string salt = generateSalt();
    
    // 使用crypt()函数进行bcrypt哈希
    // crypt()在现代glibc中支持bcrypt算法
    char* hash = crypt(password.c_str(), salt.c_str());
    
    if (hash == nullptr) {
        throw std::runtime_error("Password hashing failed");
    }
    
    return std::string(hash);
}

bool PasswordUtil::verifyPassword(const std::string& password, const std::string& hash) {
    if (hash.empty()) {
        return false;
    }
    
    // 使用存储的哈希作为salt重新计算哈希
    // crypt()会从hash中提取salt并使用相同的算法
    char* computed_hash = crypt(password.c_str(), hash.c_str());
    
    if (computed_hash == nullptr) {
        return false;
    }
    
    // 安全比较（防止时序攻击）
    const char* stored = hash.c_str();
    const char* computed = computed_hash;
    
    int result = 0;
    while (*stored != '\0' && *computed != '\0') {
        result |= *stored ^ *computed;
        stored++;
        computed++;
    }
    
    // 确保两个字符串长度相同
    result |= (*stored != '\0') | (*computed != '\0');
    
    return result == 0;
}
