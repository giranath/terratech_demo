#ifndef DEF_UNIT_FLY_WEIGHT_HPP
#define DEF_UNIT_FLY_WEIGHT_HPP

#include "ressource_value.hpp"
#include "ressource_type.hpp"
#include "../world/biome_type.hpp"

#include <json/json.hpp>
#include <vector>
#include <string>
#include <cstdint>

class unit_flyweight
{
private:
    std::vector<ressource_type> ressource_gathering_type;
    std::vector<ressource_type> ressource_drop_off;
    std::vector<biome_type> walkable_biome;
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
    unit_flyweight() = default;
    explicit unit_flyweight(const nlohmann::json& data) {
        load_unit_from_json(data);
    }

    int get_max_health()
    {
        return max_health;
    }

    uint8_t get_tranport_unit_capacity()
    {
        return tranport_unit_capacity;
    }
    
    void load_unit_from_json(const nlohmann::json& json)
    {
        unit_id = json["id"];
        name = json["Name"].get<std::string>();
        unit_cost.food = json["Food"];
        unit_cost.wood = json["Wood"];
        unit_cost.stone = json["Stone"];
        unit_cost.gold = json["Gold"];
        unit_cost.magic_essence = json["Essence"];
        max_health = json["Life"];
        armor = json["Armor"];
        attack_speed = json["AttackSpeed"];
        speed = json["WalkSpeed"];
        range = json["Range"];
        damage = json["Damage"];
        tranport_unit_capacity = json["TransportCapacity"];
        buildable_unit_id_list = json["BuildableUnit"].get<std::vector<int>>();

        // Converts string values to enum
        std::vector<std::string> resources = json["RessourceGatheringType"].get<std::vector<std::string>>();
        std::transform(std::begin(resources), std::end(resources), std::back_inserter(ressource_gathering_type), [](const std::string& name) {
            if(name == "Food") return ressource_type::food;
            if(name == "Stone") return ressource_type::stone;
            if(name == "Wood") return ressource_type::wood;
            if(name == "Gold") return ressource_type::gold;
            if(name == "MagicEssence") return ressource_type::magic_essence;

            return ressource_type::unknown;
        });

        std::vector<std::string> drop_off = json["RessourceDropOff"].get<std::vector<std::string>>();
        std::transform(std::begin(drop_off), std::end(drop_off), std::back_inserter(ressource_drop_off), [](const std::string& name) {
            if (name == "Food") return ressource_type::food;
            if (name == "Stone") return ressource_type::stone;
            if (name == "Wood") return ressource_type::wood;
            if (name == "Gold") return ressource_type::gold;
            if (name == "MagicEssence") return ressource_type::magic_essence;
            return ressource_type::unknown;
        });
        

        std::vector<std::string> walk_type = json["WalkableType"].get<std::vector<std::string>>();
        std::transform(std::begin(walk_type), std::end(walk_type), std::back_inserter(walkable_biome), [](const std::string& name)
        {
            if (name == "Grass") return biome_type::GRASS;
            if (name == "Rock") return biome_type::ROCK;
            if (name == "Desert") return biome_type::DESERT;
            if (name == "Snow") return biome_type::SNOW;
            if (name == "Water") return biome_type::WATER;
            return biome_type::UNKNOWN;
        });
        transportable = json["Transportable"];
        population_cost = json["PopulationCost"];
        height = json["Height"];
        width = json["Width"];
        construction_time = json["ConstructionTime"];
    }

    //texture_handle
};
#endif
