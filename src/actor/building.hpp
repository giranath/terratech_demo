#ifndef DEF_BUILDING_HPP
#define DEF_BUILDING_HPP

#include "actor.hpp"
#include "unit.hpp"
#include "unit_flyweight.hpp"
#include <glm/glm.hpp>
class building : actor
{
    glm::vec3 waypoint;

    std::vector<int> building_queue;
    std::vector<unit*> transported_unit;

    void load_unit(unit* u)
    {
        transported_unit.push_back(u);
    }
    void build_unit(const int& unit_id)
    {
        building_queue.push_back(unit_id);
    }
};
#endif

