#ifndef DEF_UNIT_MANAGER_HPP
#define DEF_UNIT_MANAGER_HPP

#include "base_unit.hpp"
#include "target_handle.hpp"

#include <vector>
#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <iterator>

struct unit_id
{
    uint8_t player_id;
    uint8_t unit_type;
    uint16_t counter;
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
    using unit_collection = std::unordered_map<uint32_t, unit_ptr>;

    struct unit_pair {
        uint32_t id;
        base_unit* unit;
    };

    class unit_iterator : public std::iterator<std::forward_iterator_tag, unit_pair> {
        friend unit_manager;
        unit_pair current_element;
        typename unit_collection::iterator raw_iterator;

        unit_iterator(typename unit_collection::iterator it)
        : raw_iterator(it) {

        }
    public:
        unit_iterator& operator++() {
            ++raw_iterator;
            return *this;
        }

        unit_iterator operator++(int) {
            unit_iterator temp(*this);
            operator++();
            return temp;
        }

        bool operator==(const unit_iterator& other) const {
            return raw_iterator == other.raw_iterator;
        }

        bool operator!=(const unit_iterator& other) const {
            return raw_iterator != other.raw_iterator;
        }

        reference operator*() {
            current_element.id = raw_iterator->first;
            current_element.unit = raw_iterator->second.get();

            return current_element;
        }

        pointer operator->() {
            current_element.id = raw_iterator->first;
            current_element.unit = raw_iterator->second.get();

            return &current_element;
        }
    };
private:
    std::vector<std::unordered_map<uint32_t, unit_ptr>> units;

public:
    unit_manager();

    uint32_t get_unit_type(uint32_t id);

    uint32_t actor_type_to_uint32_t(base_unit* unit);

    base_unit* get(uint32_t id);

    target_handle add(unit_ptr unit, uint32_t id);

    void remove(uint32_t id);

    unit_iterator begin_of_units();
    unit_iterator end_of_units();

};
#endif
