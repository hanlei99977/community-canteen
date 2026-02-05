-- ================================
-- 社区长者食堂管理系统数据库
-- 数据库名：community_canteen
-- ================================

-- 创建数据库
CREATE DATABASE IF NOT EXISTS community_canteen
CHARACTER SET utf8mb4
COLLATE utf8mb4_general_ci;

USE community_canteen;

-- ================================
-- 1. 用户与角色管理
-- ================================

-- 用户表
CREATE TABLE users (
    user_id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) NOT NULL,
    password VARCHAR(100) NOT NULL,
    phone VARCHAR(20),
    id_card VARCHAR(18),
    address VARCHAR(100),
    register_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    status INT DEFAULT 1
);

-- 角色表
CREATE TABLE role (
    role_id INT PRIMARY KEY,
    role_name VARCHAR(50) NOT NULL
);

-- 用户-角色关系表
CREATE TABLE user_role (
    user_id INT,
    role_id INT,
    PRIMARY KEY (user_id, role_id),
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    FOREIGN KEY (role_id) REFERENCES role(role_id)
);

-- ================================
-- 2. 管理员与公告
-- ================================

-- 管理区域表
CREATE TABLE admin_region (
    admin_id INT,
    level VARCHAR(20),
    region_name VARCHAR(50),
    FOREIGN KEY (admin_id) REFERENCES user(user_id)
);

-- 公告表
CREATE TABLE announcement (
    announce_id INT PRIMARY KEY AUTO_INCREMENT,
    title VARCHAR(100),
    content TEXT,
    publish_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    publisher_id INT,
    FOREIGN KEY (publisher_id) REFERENCES user(user_id)
);

-- ================================
-- 3. 社区食堂相关
-- ================================

-- 食堂表
CREATE TABLE canteen (
    canteen_id INT PRIMARY KEY AUTO_INCREMENT,
    code VARCHAR(20) UNIQUE,
    name VARCHAR(50),
    address VARCHAR(100),
    community VARCHAR(50),
    manager_id INT,
    status INT DEFAULT 1,
    FOREIGN KEY (manager_id) REFERENCES user(user_id)
);

-- 食堂工作人员表
CREATE TABLE canteen_staff (
    staff_id INT PRIMARY KEY AUTO_INCREMENT,
    canteen_id INT,
    name VARCHAR(50),
    role VARCHAR(30),
    phone VARCHAR(20),
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id)
);

-- ================================
-- 4. 菜品与餐单
-- ================================

-- 菜品表
CREATE TABLE dish (
    dish_id INT PRIMARY KEY AUTO_INCREMENT,
    canteen_id INT,
    name VARCHAR(50),
    type VARCHAR(10),     -- 荤 / 素
    price DECIMAL(5,2),
    calories INT,
    nutrition_info VARCHAR(100),
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id)
);

-- 每日餐单表
CREATE TABLE daily_menu (
    menu_id INT PRIMARY KEY AUTO_INCREMENT,
    canteen_id INT,
    date DATE,
    meal_type VARCHAR(10),  -- 早餐 / 午餐 / 晚餐
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id)
);

-- 餐单-菜品关系表
CREATE TABLE menu_dish (
    menu_id INT,
    dish_id INT,
    PRIMARY KEY (menu_id, dish_id),
    FOREIGN KEY (menu_id) REFERENCES daily_menu(menu_id),
    FOREIGN KEY (dish_id) REFERENCES dish(dish_id)
);

-- ================================
-- 5. 点餐与消费
-- ================================

-- 订单表
CREATE TABLE orders (
    order_id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT,
    canteen_id INT,
    total_price DECIMAL(6,2),
    order_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    status VARCHAR(20),
    FOREIGN KEY (user_id) REFERENCES user(user_id),
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id)
);

-- 订单详情表
CREATE TABLE order_item (
    order_id INT,
    dish_id INT,
    quantity INT,
    PRIMARY KEY (order_id, dish_id),
    FOREIGN KEY (order_id) REFERENCES orders(order_id),
    FOREIGN KEY (dish_id) REFERENCES dish(dish_id)
);

-- ================================
-- 6. 评价与监督
-- ================================

-- 评价表
CREATE TABLE rating (
    user_id INT,
    canteen_id INT,
    score INT,
    comment VARCHAR(200),
    time DATETIME DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (user_id, canteen_id),
    FOREIGN KEY (user_id) REFERENCES user(user_id),
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id)
);

-- ================================
-- 7. 财务与统计
-- ================================

-- 食材进货表
CREATE TABLE ingredient_purchase (
    purchase_id INT PRIMARY KEY AUTO_INCREMENT,
    canteen_id INT,
    amount DECIMAL(6,2),
    date DATE,
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id)
);

-- ================================
-- 8. 初始数据
-- ================================

-- 角色初始化
INSERT INTO role VALUES
(1,'系统管理员'),
(2,'市级管理员'),
(3,'区级管理员'),
(4,'食堂管理员'),
(5,'社区监督者'),
(6,'用餐者');

-- 默认管理员用户
INSERT INTO users(username,password,phone)
VALUES ('admin','123456','13800000000');

-- 管理员角色绑定
INSERT INTO user_role VALUES (1,1);

-- 示例社区食堂
INSERT INTO canteen(code,name,address,community,manager_id)
VALUES ('CT001','幸福社区食堂','幸福路1号','幸福社区',1);
