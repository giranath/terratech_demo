#include "chunk_renderer.hpp"
#include "../world/world.hpp"
#include "../world/world_generator.hpp"

#include <algorithm>
#include <iterator>

chunk_renderer::chunk_renderer(const world_chunk &chunk) noexcept
: chunk{chunk}
, floor_mesh{} {
    build();
}

std::map<int, glm::vec3> chunk_renderer::make_biome_colors() {
    // Here we asign a color to a biome
    // see https://i.stack.imgur.com/vlvQQ.png for color code
    std::map<int, glm::vec3> biome_color;
    biome_color[BIOME_RAIN_FOREST] = glm::vec3(0, 0, 255.f / 255.f);
    biome_color[BIOME_SWAMP] = glm::vec3(63 / 255.f, 64 / 255.f, 255 / 255.f);
    biome_color[BIOME_SEASONAL_FOREST] = glm::vec3(170.f / 255.f, 0, 255 / 255.f);
    biome_color[BIOME_FOREST] = glm::vec3(191 / 255.f, 64 / 255.f, 255 / 255.f);
    biome_color[BIOME_TAIGA] = glm::vec3(255 / 255.f, 128 / 255.f, 255 / 255.f);
    biome_color[BIOME_WOODS] = glm::vec3(255 / 255.f, 64 / 255.f, 191 / 255.f);
    biome_color[BIOME_SAVANNA] = glm::vec3(255 / 255.f, 0, 170 / 255.f);
    biome_color[BIOME_DESERT] = glm::vec3(255 / 255.f, 0, 0);
    biome_color[BIOME_GRASS_DESERT] = glm::vec3(255 / 255.f, 97 / 255.f, 97 / 255.f);
    biome_color[BIOME_TUNDRA] = glm::vec3(255 / 255.f, 191 / 255.f, 212 / 255.f);
    biome_color[BIOME_WATER] = glm::vec3(0, 255 / 255.f, 230 / 255.f);
    biome_color[BIOME_DEEP_WATER] = glm::vec3(0, 0, 255 / 255.f);

    return biome_color;
}

void chunk_renderer::build_floor_mesh() noexcept {
    auto biome_colors = make_biome_colors();

    mesh_builder floor_mesh_builder;
    for(std::size_t x = 0; x < world::CHUNK_WIDTH; ++x) {
        for(std::size_t z = 0; z < world::CHUNK_DEPTH; ++z) {
            const int CURRENT_BIOME = chunk.biome_at(x, 0, z);

            const float LEFT = x * SQUARE_SIZE;
            const float BOTTOM = z * SQUARE_SIZE;
            const float RIGHT = LEFT + SQUARE_SIZE;
            const float TOP = BOTTOM + SQUARE_SIZE;
            const glm::vec3 TILE_COLOR = biome_colors[CURRENT_BIOME];

            floor_mesh_builder.add_vertex({LEFT, 0.f, BOTTOM}, TILE_COLOR);
            floor_mesh_builder.add_vertex({LEFT, 0.f, TOP},    TILE_COLOR);
            floor_mesh_builder.add_vertex({RIGHT, 0.f, TOP},   TILE_COLOR);
            floor_mesh_builder.add_vertex({LEFT, 0.f, BOTTOM}, TILE_COLOR);
            floor_mesh_builder.add_vertex({RIGHT, 0.f, TOP},    TILE_COLOR);
            floor_mesh_builder.add_vertex({RIGHT, 0.f, BOTTOM},   TILE_COLOR);
        }
    }

    floor_mesh = floor_mesh_builder.build();
}

void chunk_renderer::build() noexcept {
    build_floor_mesh();
}

void chunk_renderer::render(gl::program& program) const noexcept {
    floor_mesh.render();

    // TODO: Render sites
}