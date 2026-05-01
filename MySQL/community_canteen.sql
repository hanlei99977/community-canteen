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


-- ##################### 基础表 #####################
-- ================================
--  地区  
-- ================================
CREATE TABLE region (
    region_id INT AUTO_INCREMENT PRIMARY KEY,
    region_name VARCHAR(50) NOT NULL,
    region_level ENUM('市级','区级') NOT NULL, 
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

-- 标签表
CREATE TABLE tag (
    tag_id INT PRIMARY KEY AUTO_INCREMENT,
    tag_name VARCHAR(50) UNIQUE
);

-- ##################### 用户表 #####################
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
    family_id int DEFAULT 1,
    region_id INT,
    disease_history varchar(100),  -- 疾病史
    taste_preference varchar(100),  -- 口味偏好
    FOREIGN KEY (region_id) REFERENCES region(region_id),
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    FOREIGN KEY (family_id) REFERENCES family(family_id)
);

-- 食堂管理者表
CREATE TABLE canteen_manager (
    user_id INT PRIMARY KEY,
    canteen_id INT,
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id)
);
-- ##################### 权限申请表 #####################
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

-- 食堂管理者申请表
CREATE TABLE canteen_manager_apply (
    apply_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    canteen_name VARCHAR(50) NOT NULL,
    region_id INT NOT NULL,
    status INT DEFAULT 0,  -- 0待审核 1通过 2拒绝
    apply_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    review_time DATETIME,
    reviewer_id INT,       -- 审核人（系统管理员）

    FOREIGN KEY (user_id) REFERENCES users(user_id),
    FOREIGN KEY (region_id) REFERENCES region(region_id)
);

-- ##################### 内容消息表 #####################
-- 公告表
CREATE TABLE announcement (
    announce_id INT PRIMARY KEY AUTO_INCREMENT,
    title VARCHAR(50),
    content TEXT,
    publish_time DATETIME DEFAULT CURRENT_TIMESTAMP,  -- 发布时间
    publisher_id INT,  -- 发布人（管理员用户ID）
    FOREIGN KEY (publisher_id) REFERENCES admin(user_id)
);

-- 消息表
CREATE TABLE message (
    message_id INT AUTO_INCREMENT PRIMARY KEY COMMENT '消息ID',
    sender_id INT NOT NULL COMMENT '发送人ID',
    receiver_id INT NOT NULL COMMENT '接收人ID',
    content TEXT NOT NULL COMMENT '发送内容',
    status INT DEFAULT 0 COMMENT '状态：0未读，1已读',
    create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '发送时间',
    FOREIGN KEY (sender_id) REFERENCES users(user_id),
    FOREIGN KEY (receiver_id) REFERENCES users(user_id)
) COMMENT '消息表';

-- ##################### 食堂运营相关表 #####################
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
    status INT DEFAULT 1,
    FOREIGN KEY (region_id) REFERENCES region(region_id)
);

-- 菜品表
CREATE TABLE dish (
    dish_id INT PRIMARY KEY AUTO_INCREMENT,
    canteen_id INT,
    name VARCHAR(50),
    type VARCHAR(10),     -- 荤 / 素
    price DECIMAL(5,2),  -- 价格
    calories INT,  -- 热量
    nutrition_info VARCHAR(100),  -- 营养成分信息
    status INT DEFAULT 1,
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id)
);

-- 菜品-标签关系表
CREATE TABLE dish_tag (
    dish_id INT,
    tag_id INT,
    PRIMARY KEY (dish_id, tag_id),
    FOREIGN KEY (dish_id) REFERENCES dish(dish_id),
    FOREIGN KEY (tag_id) REFERENCES tag(tag_id)
);

-- 每日餐单表
CREATE TABLE daily_menu (
    menu_id INT PRIMARY KEY AUTO_INCREMENT,
    canteen_id INT,
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

-- 历史餐单表
CREATE TABLE history_menu (
    history_menu_id INT AUTO_INCREMENT PRIMARY KEY,
    menu_id INT,
    canteen_id INT,
    meal_type VARCHAR(10),  -- 早餐 / 午餐 / 晚餐
    start_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    end_time DATETIME NULL,
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id)
);

-- 历史餐单-菜品关系表
CREATE TABLE history_menu_dish (
    history_menu_id INT,
    dish_id INT,
    PRIMARY KEY (history_menu_id, dish_id),
    FOREIGN KEY (history_menu_id) REFERENCES history_menu(history_menu_id),
    FOREIGN KEY (dish_id) REFERENCES dish(dish_id)
);

-- ##################### 点餐订单相关表 #####################
-- 订单表
CREATE TABLE orders (
    order_id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT,  -- 谁下单
    order_for_user_id INT,  -- 为谁点餐（本人/家庭成员）
    canteen_id INT,
    total_price DECIMAL(6,2),
    order_time DATETIME DEFAULT CURRENT_TIMESTAMP, -- 下单时间
    status INT DEFAULT 0, -- 0待处理 1已处理 2已取消

    discount_rate DECIMAL(5,2) DEFAULT 1.00 COMMENT '订单折扣系数',
    original_total DECIMAL(10,2) COMMENT '原总价',
    saved_amount DECIMAL(10,2) COMMENT '优惠金额',

    FOREIGN KEY (user_id) REFERENCES diner(user_id),
    FOREIGN KEY (order_for_user_id) REFERENCES diner(user_id),
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id)
);

-- 订单详情表
CREATE TABLE order_item (
    order_id INT,
    dish_id INT,
    quantity INT,  -- 点餐数量

    unit_price DECIMAL(8,2) NOT NULL COMMENT '下单时原价',
    discount_price DECIMAL(8,2) NOT NULL COMMENT '优惠后单价',
    subtotal DECIMAL(8,2) NOT NULL COMMENT '该项小计',

    PRIMARY KEY (order_id, dish_id),
    FOREIGN KEY (order_id) REFERENCES orders(order_id),
    FOREIGN KEY (dish_id) REFERENCES dish(dish_id)
);

-- 创建订单取消表
CREATE TABLE order_cancel (
    cancel_id INT AUTO_INCREMENT PRIMARY KEY,
    order_id INT NOT NULL,
    cancel_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    cancel_reason TEXT NOT NULL,
    status TINYINT NOT NULL DEFAULT 0 COMMENT '0: 申请取消, 1: 已取消, 2: 拒绝取消',
    reject_reason TEXT COMMENT '拒绝取消的原因',
    handle_time DATETIME COMMENT '处理时间',
    FOREIGN KEY (order_id) REFERENCES orders(order_id) ON DELETE CASCADE
);


-- ##################### 互动反馈相关表 #####################
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

--  投诉与反馈
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

--  留言板
CREATE TABLE messageboard (
    message_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '主键',

    canteen_id INT NOT NULL COMMENT '食堂ID',
    user_id INT NOT NULL COMMENT '留言用户ID',

    create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '留言时间',
    reply_time DATETIME NULL COMMENT '回复时间',

    content TEXT NOT NULL COMMENT '留言内容',
    reply_content TEXT NULL COMMENT '回复内容',

    status TINYINT DEFAULT 0 COMMENT '0待回复 1已回复',

    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id),
    FOREIGN KEY (user_id) REFERENCES users(user_id)
);

-- ##################### 财务与统计相关表 #####################
-- 进货账单表
CREATE TABLE purchase_bill (
    bill_id INT PRIMARY KEY AUTO_INCREMENT,
    canteen_id INT NOT NULL,
    amount DECIMAL(8,2) NOT NULL,   -- 进货金额
    purchase_date DATE NOT NULL,    -- 进货日期
    remark TEXT,            -- 备注
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id)
);

--  用户用餐偏好表
CREATE TABLE diner_preference (
    user_id INT,
    tag_id INT,
    score INT DEFAULT 0,
    PRIMARY KEY (user_id, tag_id),
    FOREIGN KEY (user_id) REFERENCES diner(user_id),
    FOREIGN KEY (tag_id) REFERENCES tag(tag_id)
);
--##################### 添加索引 #####################
CREATE INDEX idx_order_id ON order_cancel(order_id);
CREATE INDEX idx_status ON order_cancel(status);

-- ================================
--  索引优化
-- ================================
-- users
CREATE UNIQUE INDEX idx_users_username ON users(username);
CREATE INDEX idx_users_phone ON users(phone);
CREATE INDEX idx_users_status ON users(status);

-- admin
CREATE INDEX idx_admin_level ON admin(level_id);
CREATE INDEX idx_admin_region ON admin(region_id);

-- diner
CREATE INDEX idx_diner_family ON diner(family_id);
CREATE INDEX idx_diner_region ON diner(region_id);

-- admin_apply
CREATE INDEX idx_admin_apply_user ON admin_apply(user_id);
CREATE INDEX idx_admin_apply_status ON admin_apply(status);
CREATE INDEX idx_admin_apply_region ON admin_apply(region_id);

-- canteen_manager_apply
CREATE INDEX idx_cm_apply_user ON canteen_manager_apply(user_id);
CREATE INDEX idx_cm_apply_status ON canteen_manager_apply(status);
CREATE INDEX idx_cm_apply_region ON canteen_manager_apply(region_id);

-- announcement
CREATE INDEX idx_announcement_time ON announcement(publish_time);
CREATE INDEX idx_announcement_publisher ON announcement(publisher_id);

-- canteen
CREATE UNIQUE INDEX idx_canteen_code ON canteen(code);
CREATE INDEX idx_canteen_region ON canteen(region_id);
CREATE INDEX idx_canteen_status ON canteen(status);
CREATE INDEX idx_canteen_name ON canteen(name);

-- canteen_manager
CREATE UNIQUE INDEX idx_canteen_manager_user ON canteen_manager(user_id);
CREATE INDEX idx_canteen_manager_canteen ON canteen_manager(canteen_id);

-- dish
CREATE INDEX idx_dish_canteen ON dish(canteen_id);
CREATE INDEX idx_dish_status ON dish(status);
CREATE INDEX idx_dish_name ON dish(name);
CREATE INDEX idx_dish_type ON dish(type);

-- daily_menu
CREATE INDEX idx_menu_canteen ON daily_menu(canteen_id);
CREATE INDEX idx_menu_meal_type ON daily_menu(meal_type);
CREATE UNIQUE INDEX idx_menu_unique 
ON daily_menu(canteen_id,meal_type);

-- menu_dish
CREATE INDEX idx_menu_dish_dish ON menu_dish(dish_id);

-- history_menu
CREATE INDEX idx_history_menu_canteen ON history_menu(canteen_id);
CREATE INDEX idx_history_menu_meal_type ON history_menu(meal_type);
CREATE INDEX idx_history_menu_start_time ON history_menu(start_time);
CREATE INDEX idx_history_menu_end_time ON history_menu(end_time);

-- history_menu_dish
CREATE INDEX idx_history_menu_dish_dish ON history_menu_dish(dish_id);

-- orders
CREATE INDEX idx_orders_user ON orders(user_id);
CREATE INDEX idx_orders_for_user ON orders(order_for_user_id);
CREATE INDEX idx_orders_canteen ON orders(canteen_id);
CREATE INDEX idx_orders_status ON orders(status);
CREATE INDEX idx_orders_time ON orders(order_time);
CREATE INDEX idx_orders_user_time ON orders(user_id,order_time);

-- order_item
CREATE INDEX idx_order_item_dish ON order_item(dish_id);

-- rating
CREATE INDEX idx_rating_canteen ON rating(canteen_id);
CREATE INDEX idx_rating_score ON rating(score);
CREATE INDEX idx_rating_time ON rating(time);

-- purchase_bill
CREATE INDEX idx_purchase_canteen ON purchase_bill(canteen_id);
CREATE INDEX idx_purchase_date ON purchase_bill(purchase_date);
CREATE INDEX idx_purchase_canteen_date 
ON purchase_bill(canteen_id,purchase_date);

-- report
CREATE INDEX idx_report_user ON report(user_id);
CREATE INDEX idx_report_canteen ON report(canteen_id);
CREATE INDEX idx_report_status ON report(status);
CREATE INDEX idx_report_type ON report(type);
CREATE INDEX idx_report_time ON report(create_time);

-- region
CREATE INDEX idx_region_parent ON region(parent_id);
CREATE INDEX idx_region_level ON region(region_level);
CREATE INDEX idx_region_name ON region(region_name);

-- messageboard
CREATE INDEX idx_msg_canteen ON messageboard(canteen_id);
CREATE INDEX idx_msg_user ON messageboard(user_id);
CREATE INDEX idx_msg_status ON messageboard(status);

-- message索引
CREATE INDEX idx_message_receiver ON message(receiver_id);
CREATE INDEX idx_message_status ON message(status);
CREATE INDEX idx_message_time ON message(create_time);