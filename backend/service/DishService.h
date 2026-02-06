#ifndef DISH_SERVICE_H
#define DISH_SERVICE_H

#include "../model/Dish.h"
#include <vector>

class DishService {
public:
    bool addDish(const Dish& dish);
    bool updateDish(const Dish& dish);
    bool deleteDish(int dishId);
    std::vector<Dish> listDishesByCanteen(int canteenId);
};

#endif
