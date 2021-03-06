#ifndef DEF_UNIT_MANAGER_HPP
#define DEF_UNIT_MANAGER_HPP

#include "base_unit.hpp"
#include "unit.hpp"
#include "building.hpp"
#include "target_handle.hpp"
#include "../collision/circle_shape.hpp"
#include "../collision/collision_detector.hpp"
#include "../memory/arena.hpp"
#include "../actor/actor_arena.hpp"

#include <vector>
#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <array>
#include <mutex>

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
    static const size_t MAX_UNIT = 400;
public:
    using unit_ptr = std::unique_ptr<base_unit>;
    using unit_iterator = array_map<unit, MAX_UNIT>::iterator;
    using building_iterator = array_map<building, MAX_UNIT>::iterator;
private:
    mutable std::mutex units_mutex;
    mutable std::mutex buildings_mutex;
    std::array<std::unordered_map<uint32_t, unit_ptr>, 3> manager_data;
    
    array_map<unit, MAX_UNIT> units{};
    array_map<building, MAX_UNIT> buildings{};

public:
    unit_manager() = default;

    uint32_t get_unit_type(uint32_t id);

    uint32_t actor_type_to_uint32_t(base_unit* unit);

    base_unit* get(uint32_t id);

    target_handle add(unit _unit, uint32_t id);
    target_handle add(building _unit, uint32_t id);

    void remove(uint32_t id);

    unit_iterator begin_of_units();
    unit_iterator end_of_units();
    std::size_t count_units() const noexcept;

    building_iterator begin_of_buildings();
    building_iterator end_of_buildings();
    size_t count_buildings() const noexcept;

    template <class output_iterator>
    output_iterator units_of(uint8_t player_id, output_iterator ot) {
        std::lock_guard<std::mutex> lock(units_mutex);
        for(auto it = std::begin(units); it != std::end(units); ++it) {
            const unit_id id(it->second->get_id());

            if(id.player_id == player_id) {
                *ot = it->second;
                ++ot;
            }
        }

        return ot;
    }
    
    template<typename CollisionShape, typename OutputIterator, typename predicate>
    OutputIterator units_in(CollisionShape shape, OutputIterator ot, predicate pred) {
        std::lock_guard<std::mutex> lock(units_mutex);
        static_assert(collision::is_collision_shape<CollisionShape>::value, "you must specify a collision shape");

        for(auto it = std::begin(units); it != std::end(units); ++it) {
			static const float select_unit_to_chunk_ratio = 1 / 45.f;
            unit* u = it->second;
            if(collision::detect(collision::circle_shape(glm::vec2(u->get_position().x, u->get_position().z), select_unit_to_chunk_ratio * u->get_flyweight()->width()), shape) && pred(u)) {
                *ot = u;
                ++ot;
            }
        }

        return ot;
    }

    template <class output_iterator>
    output_iterator buildings_of(uint8_t player_id, output_iterator ot) {
        std::lock_guard<std::mutex> lock(buildings_mutex);
        for (auto it = std::begin(buildings); it != std::end(buildings); ++it) {
            const unit_id id(it->second->get_id());

            if (id.player_id == player_id) {
                *ot = it->second;
                ++ot;
            }
        }

        return ot;
    }

    template<typename CollisionShape, typename OutputIterator, typename pred>
    OutputIterator buildings_in(CollisionShape shape, OutputIterator ot, pred) {
        std::lock_guard<std::mutex> lock(buildings_mutex);
        static_assert(collision::is_collision_shape<CollisionShape>::value, "you must specify a collision shape");

        for (auto it = std::begin(buildings); it != std::end(buildings); ++it) {
            building* u = it->second;
            if (collision::detect(collision::circle_shape(glm::vec2(u->get_position().x, u->get_position().z), 1.5), shape), pred(u)) {
                *ot = u;
                ++ot;
            }
        }

        return ot;
    }

};
#endif
