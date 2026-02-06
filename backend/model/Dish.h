#ifndef DISH_H
#define DISH_H

#include <string>

class Dish {
public:
    int dishId;
    int canteenId;
    std::string name;
    std::string type;        // 荤 / 素
    double price;
    int calories;
    std::string nutritionInfo;

    Dish()
        : dishId(0), canteenId(0), price(0.0), calories(0) {}
};

#endif
