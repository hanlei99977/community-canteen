cmake_minimum_required(VERSION 3.10)
#
#mkdir build
#cd build
#cmake ..
#make

project(CommunityCanteen)

# 使用 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 开启多线程
find_package(Threads REQUIRED)

# ==========================
# 包含头文件目录（-I）
# ==========================
include_directories(
    ./model
    ./dao
    ./service
    ./controller
    ../MySQL
    ./third_party
)

# ==========================
# 收集源文件（*.cpp）
# ==========================
file(GLOB MYSQL_SRC ../MySQL/*.cpp)
file(GLOB DAO_SRC dao/*.cpp)
file(GLOB SERVICE_SRC service/*.cpp)
file(GLOB CONTROLLER_SRC controller/*.cpp)

# 主程序
set(MAIN_SRC main.cpp)

# ==========================
# 生成可执行文件
# ==========================
add_executable(app
    ${MYSQL_SRC}
    ${DAO_SRC}
    ${SERVICE_SRC}
    ${CONTROLLER_SRC}
    ${MAIN_SRC}
)

# ==========================
# 链接库（-lxxx）
# ==========================
target_link_libraries(app
    mysqlcppconn
    Threads::Threads
)