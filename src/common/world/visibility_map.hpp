#ifndef MMAP_DEMO_VISIBILITY_MAP_HPP
#define MMAP_DEMO_VISIBILITY_MAP_HPP

#include <cstdint>
#include <vector>

enum class visibility : uint8_t {
    unexplored,
    explored,
    visible
};

class visibility_map {
    std::vector<std::vector<visibility>> tile_visibility;
    std::size_t width_ = 0, height_ = 0;
public:
    visibility_map() = delete;
    visibility_map(std::size_t width, std::size_t height);

    void clear(bool complete = false) noexcept;
    visibility at(std::size_t x, std::size_t y) const noexcept;
    void set(std::size_t x, std::size_t y, visibility value) noexcept;

    std::size_t width() const noexcept;
    std::size_t height() const noexcept;
};


#endif //MMAP_DEMO_VISIBILITY_MAP_HPP
