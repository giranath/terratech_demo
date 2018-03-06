#include "chunk_renderer.hpp"
#include "../world/world.hpp"
#include "../world/world_generator.hpp"

#include <glm/gtc/matrix_transform.hpp>
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

glm::vec3 get_rgb(uint8_t r, uint8_t g, uint8_t b) {
    return {r / 255.f, g / 255.f, b / 255.f};
}

glm::vec3 get_site_color(int type) {
    switch(type) {
        case SITE_HORSES:
            return get_rgb(191, 35, 30);
        case SITE_IRON:
            return get_rgb(229, 23, 74);
        case SITE_COAL:
            return get_rgb(76, 71, 53);
        case SITE_OIL:
            return get_rgb(0, 0, 0);
        case SITE_ALUMINUM:
            return get_rgb(23, 177, 76);
        case SITE_URANIUM:
            return get_rgb(0, 178, 127);
        case SITE_BANANA:
            return get_rgb(178, 136, 0);
        case SITE_CATTLE:
            return get_rgb(47, 105, 188);
        case SITE_DEER:
            return get_rgb(76, 74, 11);
        case SITE_SHEEP:
            return get_rgb(229, 228, 195);
        case SITE_WHEAT:
            return get_rgb(188, 57, 47);
        case SITE_STONE:
            return get_rgb(178, 174, 160);
        case SITE_PEARLS:
            return get_rgb(247, 255, 2);
        case SITE_FISH:
            return get_rgb(191, 0, 84);
        default:
            return get_rgb(0, 0, 0);
    }
}

void chunk_renderer::build_site_meshes() noexcept {
    site_meshes.clear();
    site_positions.clear();

    for(std::size_t x = 0; x < world::CHUNK_WIDTH; ++x) {
        for (std::size_t z = 0; z < world::CHUNK_DEPTH; ++z) {
            auto sites = chunk.sites_at(x, 0, z);

            if(sites.size() > 0 && sites.front()->type() != SITE_NOTHING) {
                site_meshes.push_back(make_cube(SQUARE_SIZE * 0.5f, get_site_color(sites.front()->type())));
                site_positions.emplace_back(x, 0.f, z);
            }
        }
    }
}

void chunk_renderer::build() noexcept {
    build_floor_mesh();
    build_site_meshes();
}

void chunk_renderer::render(gl::program& program, glm::mat4 parent_model) const noexcept {
    floor_mesh.render();

    // TODO: Render sites
    for(std::size_t i = 0; i < site_meshes.size(); ++i) {
        glm::vec3 pos = site_positions[i] * SQUARE_SIZE;
        pos.x += SQUARE_SIZE / 4.f;
        pos.z += SQUARE_SIZE / 4.f;

        glm::mat4 site_matrix = glm::translate(glm::mat4{1.f}, pos);
        auto model_uniform = program.find_uniform<glm::mat4>("model_matrix");

        model_uniform.set(parent_model * site_matrix);
        site_meshes[i].render();
    }
}