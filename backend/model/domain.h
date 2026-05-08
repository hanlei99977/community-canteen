#include "model.h"

// ================================
// DishScore
// 用于菜品推荐算法中的评分计算
// ================================
class DishScore {
private:
    Dish dish;
    double preferenceScore;
    double popularityScore;
    double healthScore;
    double totalScore;
    std::string reason;

public:
    // 默认构造函数
    DishScore() : preferenceScore(0.0), popularityScore(0.0), healthScore(0.0), totalScore(0.0) {}

    // 带参数构造函数
    DishScore(const Dish& d, double prefScore, double popScore, double healthScr, double totalScr, const std::string& rs)
        : dish(d), preferenceScore(prefScore), popularityScore(popScore), 
          healthScore(healthScr), totalScore(totalScr), reason(rs) {}

    // Getters
    const Dish& getDish() const { return dish; }
    double getPreferenceScore() const { return preferenceScore; }
    double getPopularityScore() const { return popularityScore; }
    double getHealthScore() const { return healthScore; }
    double getTotalScore() const { return totalScore; }
    const std::string& getReason() const { return reason; }

    // Setters
    void setDish(const Dish& value) { dish = value; }
    void setPreferenceScore(double value) { preferenceScore = value; }
    void setPopularityScore(double value) { popularityScore = value; }
    void setHealthScore(double value) { healthScore = value; }
    void setTotalScore(double value) { totalScore = value; }
    void setReason(const std::string& value) { reason = value; }
};
