#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <random>
#include <stdexcept>

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

const int BCRYPT_COST = 12;
const char* BCRYPT_PREFIX = "$2b$";

const char base64_chars[] = "./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

bool isBcryptHash(const std::string& password) {
    if (password.length() < 7) {
        return false;
    }
    return (password.substr(0, 4) == "$2a$" ||
            password.substr(0, 4) == "$2b$" ||
            password.substr(0, 4) == "$2y$");
}

static std::string generateSalt() {
    unsigned char salt_bytes[16];

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    for (int i = 0; i < 16; i++) {
        salt_bytes[i] = static_cast<unsigned char>(dis(gen));
    }

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

    char salt[30];
    snprintf(salt, sizeof(salt), "$2b$%02d$%s", BCRYPT_COST, result.substr(0, 22).c_str());

    return std::string(salt);
}

std::string hashPassword(const std::string& password) {
    std::string salt = generateSalt();

    char* hash = crypt(password.c_str(), salt.c_str());

    if (hash == nullptr) {
        throw std::runtime_error("Password hashing failed");
    }

    return std::string(hash);
}

class User {
public:
    int user_id;
    std::string username;
    std::string password;

    int getId() const { return user_id; }
    std::string getUsername() const { return username; }
    std::string getPassword() const { return password; }
};

std::vector<User> getAllUsers(sql::Connection* conn) {
    std::vector<User> users;
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT user_id, username, password FROM users")
        );
        auto res = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
        while (res->next()) {
            User u;
            u.user_id = res->getInt("user_id");
            u.username = res->getString("username");
            u.password = res->getString("password");
            users.push_back(u);
        }
    } catch (const std::exception& e) {
        std::cerr << "[getAllUsers] Error: " << e.what() << std::endl;
    }
    return users;
}

bool updatePassword(sql::Connection* conn, int user_id, const std::string& new_password) {
    try {
        auto stmt = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("UPDATE users SET password=? WHERE user_id=?")
        );
        stmt->setString(1, new_password);
        stmt->setInt(2, user_id);
        return stmt->executeUpdate() > 0;
    } catch (const std::exception& e) {
        std::cerr << "[updatePassword] Error: " << e.what() << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "   用户密码迁移工具 (明文 -> bcrypt)" << std::endl;
    std::cout << "========================================" << std::endl;

    std::string db_host = "tcp://127.0.0.1:3306";
    std::string db_user = "user";
    std::string db_password = "Hl@123456";
    std::string db_name = "community_canteen";

    try {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        std::unique_ptr<sql::Connection> conn(driver->connect(db_host, db_user, db_password));
        conn->setSchema(db_name);

        std::cout << "\n[INFO] 连接数据库成功" << std::endl;

        std::vector<User> users = getAllUsers(conn.get());

        std::cout << "[INFO] 共找到 " << users.size() << " 个用户" << std::endl;

        int migratedCount = 0;
        int skipCount = 0;
        int errorCount = 0;

        std::cout << "\n[INFO] 开始密码迁移..." << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        for (const auto& user : users) {
            std::string currentPassword = user.getPassword();

            if (isBcryptHash(currentPassword)) {
                std::cout << "[SKIP] 用户: " << user.username
                          << " - 密码已是bcrypt格式" << std::endl;
                skipCount++;
                continue;
            }

            std::cout << "[MIGRATE] 用户: " << user.username
                      << " - 正在迁移明文密码..." << std::endl;

            try {
                std::string hashedPassword = hashPassword(currentPassword);

                if (updatePassword(conn.get(), user.user_id, hashedPassword)) {
                    std::cout << "[SUCCESS] 用户: " << user.username
                              << " - 密码迁移成功" << std::endl;
                    migratedCount++;
                } else {
                    std::cout << "[ERROR] 用户: " << user.username
                              << " - 密码更新失败" << std::endl;
                    errorCount++;
                }
            } catch (const std::exception& e) {
                std::cout << "[ERROR] 用户: " << user.username
                          << " - 哈希失败: " << e.what() << std::endl;
                errorCount++;
            }
        }

        std::cout << "----------------------------------------" << std::endl;
        std::cout << "\n[RESULT] 密码迁移完成!" << std::endl;
        std::cout << "  - 成功迁移: " << migratedCount << " 个" << std::endl;
        std::cout << "  - 已是加密: " << skipCount << " 个" << std::endl;
        std::cout << "  - 迁移失败: " << errorCount << " 个" << std::endl;
        std::cout << "========================================" << std::endl;

        return (migratedCount > 0) ? 0 : 1;

    } catch (sql::SQLException& e) {
        std::cerr << "[ERROR] 数据库错误: " << e.what() << std::endl;
        std::cerr << "[ERROR] SQLState: " << e.getSQLState() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] 程序错误: " << e.what() << std::endl;
        return 1;
    }
}