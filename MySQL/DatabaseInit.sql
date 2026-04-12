-- ================================
-- 社区长者食堂管理系统数据库初始化
-- ================================

USE community_canteen;

SET FOREIGN_KEY_CHECKS = 0;

START TRANSACTION;

-- ===============================
-- 1. 地区初始化（市 + 区）
-- ===============================

-- 市级
INSERT INTO region(region_name,region_level,parent_id) VALUES
('沈阳市','市级',NULL),   -- id = 1
('大连市','市级',NULL);   -- id = 2

-- 沈阳市区
INSERT INTO region(region_name,region_level,parent_id) VALUES
('和平区','区级',1),
('沈河区','区级',1),
('大东区','区级',1),
('皇姑区','区级',1),
('铁西区','区级',1),
('苏家屯区','区级',1),
('浑南区','区级',1),
('沈北新区','区级',1),
('于洪区','区级',1),
('辽中区','区级',1);

-- 大连市区
INSERT INTO region(region_name,region_level,parent_id) VALUES
('中山区','区级',2),
('西岗区','区级',2),
('沙河口区','区级',2),
('甘井子区','区级',2),
('旅顺口区','区级',2),
('金州区','区级',2),
('普兰店区','区级',2);

-- ===============================
-- 2. 管理员级别
-- ===============================
INSERT INTO level(level_id,level_name) VALUES
(1,'系统管理员'),
(2,'市级管理员'),
(3,'区级管理员');

-- ===============================
-- 3. 家庭初始化
-- ===============================
INSERT INTO family(family_id,family_name) VALUES
(1,'未设置家庭');

-- ===============================
-- 4. 用户初始化
-- ===============================

-- 系统管理员
INSERT INTO users(username,age,password,phone) VALUES
('admin',35,'admin123','13800000000');

-- 市级管理员（沈阳）
INSERT INTO users(username,age,password,phone) VALUES
('sy_admin',40,'123456','13800000001');

-- 区级管理员（和平区）
INSERT INTO users(username,age,password,phone) VALUES
('hp_admin',38,'123456','13800000002');

-- 用餐者（和平区）
INSERT INTO users(username,age,password,phone) VALUES
('zhangsan',70,'123456','13800000003'),
('lisi',65,'123456','13800000004');

-- ===============================
-- 5. 管理员初始化
-- ===============================

-- 系统管理员（无区域）
INSERT INTO admin(user_id,level_id,region_id) VALUES
(1,1,NULL);

-- 市级管理员（沈阳市）
INSERT INTO admin(user_id,level_id,region_id) VALUES
(2,2,1);

-- 区级管理员（和平区）
INSERT INTO admin(user_id,level_id,region_id) VALUES
(3,3,3);

-- ===============================
-- 6. 用餐者初始化
-- ===============================

INSERT INTO diner(user_id,family_id,region_id,disease_history,taste_preference) VALUES
(4,1,3,'高血压','清淡'),
(5,1,3,'糖尿病','低糖');

-- ===============================
-- 7. 食堂初始化
-- ===============================

-- 和平区食堂
INSERT INTO canteen(code,name,address,region_id,status) VALUES
('HP001','和平区第一食堂','和平区中心街道',3,1),
('HP002','和平区第二食堂','和平区南市场',3,1);

-- ===============================
-- 8. 菜品初始化
-- ===============================
INSERT INTO dish(canteen_id,name,type,price,calories,nutrition_info) VALUES
(1,'红烧肉','荤',12.00,450,'高蛋白'),
(1,'清炒白菜','素',5.00,120,'高纤维'),
(2,'西红柿炒蛋','素',6.00,200,'维生素');

-- ===============================
-- 9. 餐单初始化
-- ===============================
INSERT INTO daily_menu(canteen_id,date,meal_type) VALUES
(1,CURDATE(),'午餐'),
(2,CURDATE(),'午餐');

-- ===============================
-- 10. 餐单菜品
-- ===============================
INSERT INTO menu_dish(menu_id,dish_id) VALUES
(1,1),
(1,2),
(2,3);

-- ===============================
-- 11. 订单初始化
-- ===============================
INSERT INTO orders(user_id,order_for_user_id,canteen_id,total_price,status) VALUES
(4,4,1,17.00,'已完成');

-- ===============================
-- 12. 订单详情
-- ===============================
INSERT INTO order_item(order_id,dish_id,quantity) VALUES
(1,1,1),
(1,2,1);

-- ===============================
-- 13. 公告
-- ===============================
INSERT INTO announcement(title,content,publisher_id) VALUES
('系统上线','社区食堂系统正式运行',1);

COMMIT;
SET FOREIGN_KEY_CHECKS = 1;