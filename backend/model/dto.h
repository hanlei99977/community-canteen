// MenuCreateDTO.h
#pragma once
#include <string>
#include <vector>
/********************************************************
* 用于接收数据
********************************************************/
class MenuCreateDTO {
    int canteen_id;
    std::string meal_type;
    std::vector<int> dish_ids;

public:
    int getCanteenId() const { return canteen_id; }
    void setCanteenId(int id) { canteen_id = id; }

    std::string getMealType() const { return meal_type; }
    void setMealType(const std::string& m) { meal_type = m; }

    std::vector<int> getDishIds() const { return dish_ids; }
    void setDishIds(const std::vector<int>& ids) { dish_ids = ids; }
    
};