#ifndef MMAP_DEMO_VISIBILITY_MAP_HPP
#define MMAP_DEMO_VISIBILITY_MAP_HPP

#include <cstdint>
#include <vector>

namespace world {

enum class visibility : uint8_t {
    unexplored,
    explored,
    visible
};

class visibility_map {
    std::vector<std::vector<visibility>> tile_visibility;
public:
    visibility_map() = default;
    visibility_map(std::size_t width, std::size_t height);

    void clear(bool complete = false) noexcept;
    visibility at(std::size_t x, std::size_t y) const noexcept;
    void set(std::size_t x, std::size_t y, visibility value) noexcept;
};

}


#endif //MMAP_DEMO_VISIBILITY_MAP_HPP
