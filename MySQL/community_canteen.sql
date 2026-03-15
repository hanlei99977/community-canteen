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
-- 1. 用户与角色管理
-- ================================

-- 用户表
CREATE TABLE users (
    user_id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) NOT NULL,
    password VARCHAR(100) NOT NULL,
    phone VARCHAR(20),  -- 联系电话
    id_card VARCHAR(18),  -- 身份证号码
    address VARCHAR(100),  -- 地址
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
    level VARCHAR(20), -- 市级 / 区级
    region_name VARCHAR(50),
    FOREIGN KEY (admin_id) REFERENCES users(user_id)
);

-- 公告表
CREATE TABLE announcement (
    announce_id INT PRIMARY KEY AUTO_INCREMENT,
    title VARCHAR(100),
    content TEXT,
    publish_time DATETIME DEFAULT CURRENT_TIMESTAMP,  -- 发布时间
    publisher_id INT,  -- 发布人（管理员用户ID）
    FOREIGN KEY (publisher_id) REFERENCES users(user_id)
);

-- ================================
-- 3. 社区食堂相关
-- ================================

-- 食堂表
CREATE TABLE canteen (
    canteen_id INT PRIMARY KEY AUTO_INCREMENT,
    code VARCHAR(20) UNIQUE,  -- 食堂编号
    name VARCHAR(50),
    address VARCHAR(100),
    community VARCHAR(50),  -- 所属社区
    manager_id INT,  -- 食堂管理员（用户ID）
    status INT DEFAULT 1,
    FOREIGN KEY (manager_id) REFERENCES users(user_id)
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
-- 5. 点餐与消费
-- ================================

-- 订单表
CREATE TABLE orders (
    order_id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT,  -- 谁下单
    order_for_user_id INT,  -- 为谁点餐（本人/家庭成员）
    canteen_id INT,
    total_price DECIMAL(6,2),
    order_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    status VARCHAR(20),
    FOREIGN KEY (user_id) REFERENCES users(user_id),
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
-- 6. 评价与监督
-- ================================

-- 评价表
CREATE TABLE rating (
    user_id INT,
    canteen_id INT,
    score INT,  -- 评分（1-5）
    comment VARCHAR(200),  -- 评价内容
    time DATETIME DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (user_id, canteen_id),
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id)
);

-- ================================
-- 7. 财务与统计
-- ================================

-- 食材进货表
CREATE TABLE ingredient_purchase (
    purchase_id INT PRIMARY KEY AUTO_INCREMENT,  -- 进货记录ID
    canteen_id INT,
    amount DECIMAL(6,2),  -- 进货金额
    date DATE,
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id)
);

-- ================================
-- 8. 家庭
-- ================================
-- 家庭表
CREATE TABLE family (
    family_id INT PRIMARY KEY AUTO_INCREMENT,
    family_name VARCHAR(50)
);

-- 家庭成员表
CREATE TABLE family_member (
    family_id INT,
    user_id INT,
    relation VARCHAR(20),   -- 本人 / 父亲 / 母亲 / 配偶 / 子女
    PRIMARY KEY (family_id, user_id),
    FOREIGN KEY (family_id) REFERENCES family(family_id),
    FOREIGN KEY (user_id) REFERENCES users(user_id)
);

-- ================================
-- 9. 举报与反馈
-- ================================
CREATE TABLE report (
    report_id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT NOT NULL,
    canteen_id INT NOT NULL,
    type INT,  -- 举报类型：1-食品安全问题，2-服务态度问题，3-环境卫生问题，4-其他
    content TEXT,  -- 举报内容
    status INT DEFAULT 0,  -- 0: 待处理, 1: 已处理, 2: 不予处理
    create_time DATETIME,  -- 举报时间
    handle_time DATETIME,  -- 处理时间
    hander_id INT,  -- 处理人（管理员用户ID）
    FOREIGN KEY (user_id) REFERENCES users(user_id),
    FOREIGN KEY (canteen_id) REFERENCES canteen(canteen_id),
    FOREIGN KEY (hander_id) REFERENCES users(user_id)
);

-- ================================
-- 10. 索引优化
-- ================================
CREATE INDEX idx_orders_user ON orders(user_id);
CREATE INDEX idx_orders_canteen ON orders(canteen_id);
CREATE INDEX idx_dish_canteen ON dish(canteen_id);