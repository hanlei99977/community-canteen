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

// 疾病标签关联DTO
class DiseaseTagDTO {
private:
    int tag_id;
    int rule_type;
public:
    int getTagId() const { return tag_id; }
    void setTagId(int value) { tag_id = value; }

    int getRuleType() const { return rule_type; }
    void setRuleType(int value) { rule_type = value; }
};

// 疾病创建DTO
class DiseaseCreateDTO {
private:
    std::string disease_name;
    std::vector<DiseaseTagDTO> tags;
public:
    std::string getDiseaseName() const { return disease_name; }
    void setDiseaseName(const std::string& value) { disease_name = value; }

    std::vector<DiseaseTagDTO> getTags() const { return tags; }
    void setTags(const std::vector<DiseaseTagDTO>& value) { tags = value; }
};

// 疾病更新DTO
class DiseaseUpdateDTO {
private:
    int disease_id;
    std::string disease_name;
    std::vector<DiseaseTagDTO> tags;
public:
    int getDiseaseId() const { return disease_id; }
    void setDiseaseId(int value) { disease_id = value; }

    std::string getDiseaseName() const { return disease_name; }
    void setDiseaseName(const std::string& value) { disease_name = value; }

    std::vector<DiseaseTagDTO> getTags() const { return tags; }
    void setTags(const std::vector<DiseaseTagDTO>& value) { tags = value; }
};