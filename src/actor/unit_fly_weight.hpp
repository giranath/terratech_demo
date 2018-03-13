#ifndef DEF_UNIT_FLY_WEIGHT_HPP
#define DEF_UNIT_FLY_WEIGHT_HPP

#include "ressource_value.hpp"

#include <vector>
#include <string>
#include <cstdint>

class unit_fly_weight
{
    int max_health;
    ressource_value unit_cost;

    uint16_t speed;
    int16_t armor;
    int16_t range;
    int8_t tranport_unit_capacity;
    float attack_speed;
    uint16_t damage;
    std::vector<ressource_type> ressource_gathering_type;
    std::vector<int> buildable_unit_id_list;
    int unit_id;
    bool transportable;
    std::string name;
    uint8_t population_cost;
    int height;
    int width;
    int construction_time;

    void load_unit_from_json(std::string json_file)
    {
        //todo load from json
    }

    //texture_handle
};
#endif
