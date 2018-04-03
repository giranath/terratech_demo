#include "visibility_map.hpp"

#include <algorithm>
#include <iterator>

namespace world {

visibility_map::visibility_map(std::size_t width, std::size_t height) {
    // At first each tile is unexplored
    tile_visibility.resize(height);
    for(std::size_t y = 0; y < height; ++y) {
        tile_visibility[y].resize(width, visibility::unexplored);
    }
}

void visibility_map::clear(bool complete) noexcept {
    std::for_each(std::begin(tile_visibility), std::end(tile_visibility), [complete](std::vector<visibility>& row) {
        if(complete) {
            std::transform(std::begin(row), std::end(row), std::begin(row), [](visibility v) {
                return visibility::unexplored;
            });
        }
        else {
            std::transform(std::begin(row), std::end(row), std::begin(row), [](visibility v) {
                if(v == visibility::visible) {
                    return visibility::explored;
                }

                return v;
            });
        }
    });
}

visibility visibility_map::at(std::size_t x, std::size_t y) const noexcept {
    return tile_visibility[y][x];
}

void visibility_map::set(std::size_t x, std::size_t y, visibility value) noexcept {
    tile_visibility[y][x] = value;
}

}