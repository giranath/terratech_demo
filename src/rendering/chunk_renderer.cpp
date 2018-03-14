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
    biome_color[BIOME_ROCK] = get_rgb(255, 255, 255);
    biome_color[BIOME_GRASS] = get_rgb(255, 255, 255);
    biome_color[BIOME_DESERT] = get_rgb(255, 255, 255);
    biome_color[BIOME_WATER] = get_rgb(255, 255, 255);

    return biome_color;
}

std::map<int, bounding_box<float>> make_biome_textures() {
    std::map<int, bounding_box<float>> texture_rects;

    // left, bottom, right, up
    texture_rects[BIOME_SNOW] =   { 0.50f, 0.00f, 0.75f, 0.25f };
    texture_rects[BIOME_ROCK] =   { 0.25f, 0.00f, 0.50f, 0.25f };
    texture_rects[BIOME_GRASS] =  { 0.00f, 0.00f, 0.25f, 0.25f };
    texture_rects[BIOME_DESERT] = { 0.75f, 0.00f, 1.00f, 0.25f };
    texture_rects[BIOME_WATER] =  { 0.00f, 0.25f, 0.25f, 0.50f };

    return texture_rects;
}

void chunk_renderer::build_floor_mesh() noexcept {
    auto biome_colors = make_biome_colors();
    auto biome_textures = make_biome_textures();

    mesh_builder floor_mesh_builder;
    for(std::size_t x = 0; x < world::CHUNK_WIDTH; ++x) {
        for(std::size_t z = 0; z < world::CHUNK_DEPTH; ++z) {
            const int CURRENT_BIOME = chunk.biome_at(x, 0, z);

            const float LEFT = x * SQUARE_SIZE;
            const float BOTTOM = z * SQUARE_SIZE;
            const float RIGHT = LEFT + SQUARE_SIZE;
            const float TOP = BOTTOM + SQUARE_SIZE;
            const glm::vec3 TILE_COLOR = biome_colors[CURRENT_BIOME];
            const bounding_box<float> TILE_TEXTURE = biome_textures[CURRENT_BIOME];

            floor_mesh_builder.add_vertex({LEFT, 0.f, BOTTOM},  { TILE_TEXTURE.left(),  TILE_TEXTURE.bottom() },  TILE_COLOR);
            floor_mesh_builder.add_vertex({LEFT, 0.f, TOP},     { TILE_TEXTURE.left(),  TILE_TEXTURE.top() },     TILE_COLOR);
            floor_mesh_builder.add_vertex({RIGHT, 0.f, TOP},    { TILE_TEXTURE.right(), TILE_TEXTURE.top() },     TILE_COLOR);
            floor_mesh_builder.add_vertex({LEFT, 0.f, BOTTOM},  { TILE_TEXTURE.left(),  TILE_TEXTURE.bottom() },  TILE_COLOR);
            floor_mesh_builder.add_vertex({RIGHT, 0.f, TOP},    { TILE_TEXTURE.right(), TILE_TEXTURE.top() },     TILE_COLOR);
            floor_mesh_builder.add_vertex({RIGHT, 0.f, BOTTOM}, { TILE_TEXTURE.right(), TILE_TEXTURE.bottom() },  TILE_COLOR);
        }
    }

    floor_mesh = floor_mesh_builder.build();
}

glm::vec3 get_site_color(int type) {
    switch(type) {
        case SITE_MAGIC_ESSENCE:
            return get_rgb(59, 41, 89);
        case SITE_BERRY:
            return get_rgb(194, 127, 169);
        case SITE_TREE:
            return get_rgb(97, 58, 21);
        case SITE_DEER:
            return get_rgb(255, 96, 28);
        case SITE_STONE:
            return get_rgb(40, 15, 50);
        case SITE_GOLD:
            return get_rgb(246, 181, 6);
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

void chunk_renderer::render(mesh_rendering_system& renderer, glm::mat4 parent_model) const noexcept {
    renderer.emplace(&floor_mesh, parent_model, 0, 0);

    for(std::size_t i = 0; i < site_meshes.size(); ++i) {
        glm::vec3 pos = site_positions[i] * SQUARE_SIZE;
        pos.x += SQUARE_SIZE / 4.f;
        pos.z += SQUARE_SIZE / 4.f;

        glm::mat4 site_matrix = glm::translate(glm::mat4{1.f}, pos);
        renderer.emplace(&site_meshes[i], parent_model * site_matrix, 1, 0);
    }
}