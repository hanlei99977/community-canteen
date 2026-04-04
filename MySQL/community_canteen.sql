-- sudo mysql -u root -p < community_canteen.sql
-- sudo mysql -u root -p < DatabaseInit.sql

-- ================================
-- 社区长者食堂管理系统数据库
-- 数据库名：community_canteen
-- ================================

-- 创建数据库
DROP DATABASE IF EXISTS community_canteen;
CREATE DATABASE community_canteen
CHARACTER SET utf8mb4
COLLATE utf8mb4_general_ci;

USE community_canteen;

-- ================================
--  地区  
-- ================================
CREATE TABLE region (
    region_id INT AUTO_INCREMENT PRIMARY KEY,
    region_name VARCHAR(50) NOT NULL,
    region_level ENUM('市级','区级','社区级') NOT NULL, 
    parent_id INT NULL,  -- 上级地区ID，NULL表示顶级地区
    FOREIGN KEY (parent_id) REFERENCES region(region_id)
);

-- ================================
--  管理员级别
-- ================================
CREATE TABLE level (
    level_id INT PRIMARY KEY,
    level_name ENUM('系统管理员', '市级管理员', '区级管理员') NOT NULL
);

-- ================================
--  家庭表
-- ================================
CREATE TABLE family (
    family_id INT PRIMARY KEY AUTO_INCREMENT,
    family_name VARCHAR(50)
);

-- ===============================
-- 用户信息
-- ===============================
-- 用户表
CREATE TABLE users (
    user_id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) NOT NULL,
    age INT,
    password VARCHAR(50) NOT NULL,
    phone VARCHAR(11),  -- 联系电话
    id_card VARCHAR(18),  -- 身份证号码
    address VARCHAR(100),  -- 地址
    register_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    status INT DEFAULT 1 -- 1: 正常, 0: 禁用
);

-- 管理员表
CREATE TABLE admin(
    user_id int primary key,
    level_id int,
    region_id int,
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    FOREIGN KEY (level_id) REFERENCES level(level_id),
    FOREIGN KEY (region_id) REFERENCES region(region_id)
);

-- 用餐者表
CREATE TABLE diner(
    user_id int primary key,
    family_id int,
    disease_history varchar(100),  -- 疾病史
    taste_preference varchar(100),  -- 口味偏好
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    FOREIGN KEY (family_id) REFERENCES family(family_id)
);

-- 管理员申请表
CREATE TABLE admin_apply (
    apply_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    level_id INT,          -- 申请的管理员级别
    region_id INT,         -- 申请管理的区域
    status INT DEFAULT 0,  -- 0待审核 1通过 2拒绝
    apply_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    review_time DATETIME,
    reviewer_id INT,       -- 审核人（系统管理员）
    
    FOREIGN KEY (user_id) REFERENCES users(user_id)
);

-- 公告表
CREATE TABLE announcement (
    announce_id INT PRIMARY KEY AUTO_INCREMENT,
    title VARCHAR(50),
    content TEXT,
    publish_time DATETIME DEFAULT CURRENT_TIMESTAMP,  -- 发布时间
    publisher_id INT,  -- 发布人（管理员用户ID）
    FOREIGN KEY (publisher_id) REFERENCES admin(user_id)
);


-- ================================
--  社区食堂相关
-- ================================
-- 食堂表
CREATE TABLE canteen (
    canteen_id INT PRIMARY KEY AUTO_INCREMENT,
    code VARCHAR(20) UNIQUE,  -- 食堂编号
    name VARCHAR(50),
    address VARCHAR(100),
    region_id INT,  -- 所属地区ID
    manager_id INT,  -- 食堂管理员（用户ID）
    status INT DEFAULT 1,
    FOREIGN KEY (manager_id) REFERENCES admin(user_id),
    FOREIGN KEY (region_id) REFERENCES region(region_id)
);

CREATE TABLE canteen_manager (
    user_id INT PRIMARY KEY,
    canteen_id INT,
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id)
);
-- ================================
--  菜品与餐单
-- ================================

-- 菜品表
CREATE TABLE dish (
    dish_id INT PRIMARY KEY AUTO_INCREMENT,
    canteen_id INT,
    name VARCHAR(50),
    type VARCHAR(10),     -- 荤 / 素
    price DECIMAL(5,2),  -- 价格
    calories INT,  -- 热量
    nutrition_info VARCHAR(100),  -- 营养成分信息
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
--  点餐与消费
-- ================================

-- 订单表
CREATE TABLE orders (
    order_id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT,  -- 谁下单
    order_for_user_id INT,  -- 为谁点餐（本人/家庭成员）
    canteen_id INT,
    total_price DECIMAL(6,2),
    order_time DATETIME DEFAULT CURRENT_TIMESTAMP, -- 下单时间
    status VARCHAR(20),
    FOREIGN KEY (user_id) REFERENCES diner(user_id),
    FOREIGN KEY (order_for_user_id) REFERENCES diner(user_id),
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id)
);

-- 订单详情表
CREATE TABLE order_item (
    order_id INT,
    dish_id INT,
    quantity INT,  -- 点餐数量
    PRIMARY KEY (order_id, dish_id),
    FOREIGN KEY (order_id) REFERENCES orders(order_id),
    FOREIGN KEY (dish_id) REFERENCES dish(dish_id)
);

-- ================================
--  评价与监督
-- ================================
-- 评价表
CREATE TABLE rating (
    user_id INT,
    canteen_id INT,
    order_id INT,
    score INT,  -- 评分（1-5）
    comment TEXT,  -- 评价内容
    time DATETIME DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (user_id, order_id),
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id),
    FOREIGN KEY (order_id) REFERENCES orders(order_id)
);

-- ================================
--  财务与统计
-- ================================

-- 进货账单表
CREATE TABLE purchase_bill (
    bill_id INT PRIMARY KEY AUTO_INCREMENT,
    canteen_id INT NOT NULL,
    amount DECIMAL(8,2) NOT NULL,   -- 进货金额
    purchase_date DATE NOT NULL,    -- 进货日期
    remark TEXT,            -- 备注
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id)
);

-- ================================
--  投诉与反馈
-- ================================
CREATE TABLE report (
    report_id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT NOT NULL,
    canteen_id INT NOT NULL,
    type INT,  -- 投诉类型：1-食品安全问题，2-服务态度问题，3-环境卫生问题，4-其他
    content TEXT,  -- 投诉内容
    status INT DEFAULT 0,  -- 0: 待处理, 1: 已处理, 2: 不予处理
    create_time DATETIME DEFAULT CURRENT_TIMESTAMP,  -- 投诉时间
    handle_time DATETIME,  -- 处理时间
    handler_id INT,  -- 处理人（管理员用户ID）
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id),
    FOREIGN KEY (handler_id) REFERENCES users(user_id)
);

-- ================================
--  索引优化
-- ================================
CREATE INDEX idx_orders_user ON orders(user_id);
CREATE INDEX idx_orders_canteen ON orders(canteen_id);
CREATE INDEX idx_dish_canteen ON dish(canteen_id);
CREATE INDEX idx_menu_canteen ON daily_menu(canteen_id);
CREATE INDEX idx_report_canteen ON report(canteen_id);


-- ================================
--  插入基本的数据
-- ================================
INSERT INTO users(username, age, password, phone, address)
VALUES ('admin', 35, 'admin123', '13800000000', '系统');

INSERT INTO admin(user_id, level_id, region_id)
VALUES (LAST_INSERT_ID(), 1, NULL);

INSERT INTO family(family_id, family_name) VALUES
(1,'未设置家庭');

