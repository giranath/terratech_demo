#ifndef DEF_UNIT_FLY_WEIGHT_HPP
#define DEF_UNIT_FLY_WEIGHT_HPP

#include "ressource_value.hpp"
#include "ressource_type.hpp"
#include <vector>
#include <string>
#include <cstdint>

class unit_fly_weight
{
private:
    std::vector<ressource_type> ressource_gathering_type;
    std::vector<int> buildable_unit_id_list;
    ressource_value unit_cost;
    std::string name;
    float attack_speed;
    int max_health;
    int unit_id;
    int height;
    int width;
    int construction_time;
    uint16_t speed;
    uint16_t damage;
    int16_t armor;
    int16_t range;
    uint8_t tranport_unit_capacity;
    uint8_t population_cost;
    bool transportable;

public:

    int get_max_health()
    {
        return max_health;
    }

    uint8_t get_tranport_unit_capacity()
    {
        return tranport_unit_capacity;
    }
    
    void load_unit_from_json(std::string json_file)
    {
        //todo load from json
    }

    //texture_handle
};
#endif
