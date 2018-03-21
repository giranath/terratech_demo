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
    float height_;
    float width_;
    int construction_time;
    float speed;
    uint16_t damage;
    int16_t armor;
    float range;
    uint8_t tranport_unit_capacity;
    uint8_t population_cost;
    bool transportable;
    std::string texture_handle;

public:
    unit_flyweight() = default;
    explicit unit_flyweight(nlohmann::json& data) {
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

	float get_speed()
	{
		return speed;
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
            if (name == "Grass") return biome_type::grass;
            if (name == "Rock") return biome_type::rock;
            if (name == "Desert") return biome_type::desert;
            if (name == "Snow") return biome_type::snow;
            if (name == "Water") return biome_type::water;
            return biome_type::unknown;
        });
        transportable = json["Transportable"];
        population_cost = json["PopulationCost"];
        height_ = json["Height"];
        width_ = json["Width"];
        construction_time = json["ConstructionTime"];
        texture_handle = json["Texture"];
    }

    static std::vector<std::string> ressource_enum_to_string(std::vector<ressource_type> v)
    {
        std::vector<std::string> str_vec;
        str_vec.reserve(v.size());

        std::transform(std::begin(v), std::end(v), std::back_inserter(str_vec), [](const ressource_type name)
        {
            if (name == ressource_type::food ) return"Food";
            if (name == ressource_type::gold) return"Gold";
            if (name == ressource_type::stone) return"Stone";
            if (name == ressource_type::wood) return"Wood";
            if (name == ressource_type::magic_essence) return"MagicEssence";
            return "Unknown";
        });
        return str_vec;
    }

    static std::vector<std::string> biome_enum_to_string(std::vector<biome_type> v)
    {
        std::vector<std::string> str_vec;
        str_vec.reserve(v.size());

        std::transform(std::begin(v), std::end(v), std::back_inserter(str_vec), [](const biome_type name)
        {
            if (name == biome_type::grass) return"Grass";
            if (name == biome_type::rock) return"Rock";
            if (name == biome_type::snow) return"Snow";
            if (name == biome_type::water) return"Water";
            if (name == biome_type::desert) return"Desert";
            return "Unknown";
        });
        return str_vec;
    }

    friend void to_json(nlohmann::json& j, const unit_flyweight& uf)
    {
        j = {
            {"id", uf.unit_id},
            {"Name", uf.name},
            {"Food",uf.unit_cost.food},
            {"Wood",uf.unit_cost.wood},
            {"Stone",uf.unit_cost.stone},
            {"Gold", uf.unit_cost.gold},
            {"Essence", uf.unit_cost.magic_essence},
            {"Life", uf.max_health},
            {"Armor", uf.armor},
            {"AttackSpeed", uf.attack_speed},
            {"WalkSpeed", uf.speed},
            {"Range", uf.range},
            {"Damage", uf.damage},
            {"TransportCapacity", uf.tranport_unit_capacity},
            {"BuildableUnit", uf.buildable_unit_id_list},
            {"RessourceGatheringType", ressource_enum_to_string(uf.ressource_gathering_type)},
            {"RessourceDropOff", ressource_enum_to_string(uf.ressource_drop_off)},
            {"WalkableType", biome_enum_to_string(uf.walkable_biome) },
            {"Transportable", uf.transportable},
            {"PopulationCost", uf.population_cost },
            {"Height", uf.height_},
            {"Width", uf.width_},
            {"ConstructionTime", uf.construction_time},
            {"Texture", uf.texture_handle}
        };
    }

    void from_json(const nlohmann::json& j, unit_flyweight& uf)
    {
        uf.load_unit_from_json(j);
    }

    const std::string& texture() const noexcept {
        return texture_handle;
    }

    float width() const noexcept {
        return width_;
    }

    float height() const noexcept {
        return height_;
    }

    int id() const noexcept {
        return unit_id;
    }
};
#endif
