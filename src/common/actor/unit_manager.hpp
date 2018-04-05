#ifndef DEF_UNIT_MANAGER_HPP
#define DEF_UNIT_MANAGER_HPP

#include "base_unit.hpp"
#include "unit.hpp"
#include "target_handle.hpp"
#include "../collision/circle_shape.hpp"
#include "../collision/collision_detector.hpp"

#include <vector>
#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include <memory>

struct unit_id
{
    uint8_t player_id = 0;
    uint8_t unit_type = 0;
    uint16_t counter  = 0;

    unit_id() = default;
    unit_id(uint32_t v) {
        from_uint32_t(v);
    }

    uint32_t to_uint32_t() const
    {
        return *reinterpret_cast<const uint32_t*>(this);
    }

    void from_uint32_t(uint32_t val)
    {
        *this = *reinterpret_cast<unit_id*>(&val);
    }
};

class unit_manager
{
public:
    using unit_ptr = std::unique_ptr<base_unit>;
    using iterator = std::unordered_map<uint32_t, unit_ptr>::iterator;
private:
    std::vector<std::unordered_map<uint32_t, unit_ptr>> units;

public:
    unit_manager();

    uint32_t get_unit_type(uint32_t id);

    uint32_t actor_type_to_uint32_t(base_unit* unit);

    base_unit* get(uint32_t id);

    target_handle add(unit_ptr unit, uint32_t id);

    void remove(uint32_t id);

    iterator begin_of_units();
    iterator end_of_units();

    std::size_t count_units() const noexcept;
    std::vector<unit*> units_of(uint8_t player_id);

    template<typename CollisionShape>
    std::vector<unit*> units_in(CollisionShape shape) {
        static_assert(collision::is_collision_shape<CollisionShape>::value, "you must specify a collision shape");

        std::vector<unit*> units_ptrs;
        units_ptrs.reserve(count_units());

        std::transform(begin_of_units(), end_of_units(), std::back_inserter(units_ptrs), [](auto& p) {
            return static_cast<unit*>(p.second.get());
        });

        auto end = std::copy_if(std::begin(units_ptrs), std::end(units_ptrs), std::begin(units_ptrs), [shape](unit* u) {
            return collision::detect(collision::circle_shape(glm::vec2(u->get_position().x, u->get_position().z), 15.0),
                                     shape);
        });

        units_ptrs.resize(std::distance(std::begin(units_ptrs), end));

        return units_ptrs;
    }

};
#endif
