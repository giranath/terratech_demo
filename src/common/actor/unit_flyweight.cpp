#include "unit_flyweight.hpp"

void to_json(nlohmann::json& j, const unit_flyweight& uf)
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
            {"RessourceGatheringType", unit_flyweight::ressource_enum_to_string(uf.ressource_gathering_type)},
            {"RessourceDropOff", unit_flyweight::ressource_enum_to_string(uf.ressource_drop_off)},
            {"WalkableType", unit_flyweight::biome_enum_to_string(uf.walkable_biome) },
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