cmake_minimum_required(VERSION 3.10)
project(password_migration)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/../MySQL
)

find_package(Threads REQUIRED)

add_executable(password_migration
    password_migration.cpp
)

target_link_libraries(password_migration
    mysqlcppconn
    Threads::Threads
    crypt
)