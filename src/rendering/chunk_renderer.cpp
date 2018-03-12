#include "chunk_renderer.hpp"
#include "../world/world.hpp"
#include "../world/world_generator.hpp"
#include "../bounding_box.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iterator>

glm::vec3 get_rgb(uint8_t r, uint8_t g, uint8_t b) {
    return {r / 255.f, g / 255.f, b / 255.f};
}

chunk_renderer::chunk_renderer(const world_chunk &chunk) noexcept
: chunk{chunk}
, floor_mesh{} {
    build();
}

std::map<int, glm::vec3> chunk_renderer::make_biome_colors() {
    // Here we asign a color to a biome
    // see https://i.stack.imgur.com/vlvQQ.png for color code
    std::map<int, glm::vec3> biome_color;
    biome_color[BIOME_SNOW] = get_rgb(255, 255, 255);
    biome_color[BIOME_ROCK] = get_rgb(6, 232, 182);
    biome_color[BIOME_GRASS] = get_rgb(153, 255, 20);
    biome_color[BIOME_DESERT] = get_rgb(232, 161, 6);
    biome_color[BIOME_WATER] = get_rgb(19, 33, 255);

    return biome_color;
}

std::map<int, bounding_box<float>> make_biome_textures() {
    std::map<int, bounding_box<float>> texture_rects;

    return texture_rects;
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

            floor_mesh_builder.add_vertex({LEFT, 0.f, BOTTOM},  {}, TILE_COLOR);
            floor_mesh_builder.add_vertex({LEFT, 0.f, TOP},     {}, TILE_COLOR);
            floor_mesh_builder.add_vertex({RIGHT, 0.f, TOP},    {}, TILE_COLOR);
            floor_mesh_builder.add_vertex({LEFT, 0.f, BOTTOM},  {}, TILE_COLOR);
            floor_mesh_builder.add_vertex({RIGHT, 0.f, TOP},    {}, TILE_COLOR);
            floor_mesh_builder.add_vertex({RIGHT, 0.f, BOTTOM}, {}, TILE_COLOR);
        }
    }

    floor_mesh = floor_mesh_builder.build();
}

glm::vec3 get_site_color(int type) {
    switch(type) {
        case SITE_MAGIC_ESSENCE:
            return get_rgb(191, 35, 30);
        case SITE_BERRY:
            return get_rgb(229, 23, 74);
        case SITE_TREE:
            return get_rgb(76, 71, 53);
        case SITE_DEER:
            return get_rgb(76, 74, 11);
        case SITE_STONE:
            return get_rgb(178, 174, 160);
        case SITE_GOLD:
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