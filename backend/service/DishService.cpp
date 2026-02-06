#include "DishService.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <memory>
#include <iostream>
#include "../../MySQL/Database.h"

using namespace std;

bool DishService::addDish(const Dish& dish) {
    try {
        auto* conn = Database::getInstance().getConnection();
        unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement(
                "INSERT INTO dish(canteen_id, name, type, price, calories, nutrition_info) "
                "VALUES (?, ?, ?, ?, ?, ?)"
            )
        );
        pstmt->setInt(1, dish.canteenId);
        pstmt->setString(2, dish.name);
        pstmt->setString(3, dish.type);
        pstmt->setDouble(4, dish.price);
        pstmt->setInt(5, dish.calories);
        pstmt->setString(6, dish.nutritionInfo);
        pstmt->executeUpdate();
        return true;
    } catch (sql::SQLException& e) {
        cerr << "[Add Dish Error] " << e.what() << endl;
        return false;
    }
}

bool DishService::updateDish(const Dish& dish) {
    try {
        auto* conn = Database::getInstance().getConnection();
        unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement(
                "UPDATE dish SET name=?, type=?, price=?, calories=?, nutrition_info=? "
                "WHERE dish_id=?"
            )
        );
        pstmt->setString(1, dish.name);
        pstmt->setString(2, dish.type);
        pstmt->setDouble(3, dish.price);
        pstmt->setInt(4, dish.calories);
        pstmt->setString(5, dish.nutritionInfo);
        pstmt->setInt(6, dish.dishId);
        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException& e) {
        cerr << "[Update Dish Error] " << e.what() << endl;
        return false;
    }
}

bool DishService::deleteDish(int dishId) {
    try {
        auto* conn = Database::getInstance().getConnection();
        unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement(
                "DELETE FROM dish WHERE dish_id=?"
            )
        );
        pstmt->setInt(1, dishId);
        return pstmt->executeUpdate() > 0;
    } catch (sql::SQLException& e) {
        cerr << "[Delete Dish Error] " << e.what() << endl;
        return false;
    }
}

vector<Dish> DishService::listDishesByCanteen(int canteenId) {
    vector<Dish> list;
    try {
        auto* conn = Database::getInstance().getConnection();
        unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement(
                "SELECT * FROM dish WHERE canteen_id=?"
            )
        );
        pstmt->setInt(1, canteenId);
        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            Dish d;
            d.dishId = res->getInt("dish_id");
            d.canteenId = res->getInt("canteen_id");
            d.name = res->getString("name");
            d.type = res->getString("type");
            d.price = res->getDouble("price");
            d.calories = res->getInt("calories");
            d.nutritionInfo = res->getString("nutrition_info");
            list.push_back(d);
        }
    } catch (sql::SQLException& e) {
        cerr << "[List Dish Error] " << e.what() << endl;
    }
    return list;
}
