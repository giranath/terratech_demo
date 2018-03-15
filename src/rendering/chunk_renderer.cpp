#include "chunk_renderer.hpp"
#include "../world/world.hpp"
#include "../world/world_generator.hpp"
#include "../bounding_box.hpp"
#include "../debug/profiler.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iterator>
#include <random>

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

std::map<int, std::vector<bounding_box<float>>> make_biome_textures() {
    std::map<int, std::vector<bounding_box<float>>> texture_rects;

    // left, bottom, right, up
    texture_rects[BIOME_SNOW] =   {bounding_box<float>{ 0.50f, 0.00f, 0.75f, 0.25f }};
    texture_rects[BIOME_ROCK] =   {bounding_box<float>{ 0.25f, 0.00f, 0.50f, 0.25f }, bounding_box<float>{ 0.25f, 0.25f, 0.50f, 0.50f}};
    texture_rects[BIOME_GRASS] =  {bounding_box<float>{ 0.00f, 0.00f, 0.25f, 0.25f }};
    texture_rects[BIOME_DESERT] = {bounding_box<float>{ 0.75f, 0.00f, 1.00f, 0.25f }};
    texture_rects[BIOME_WATER] =  {bounding_box<float>{ 0.00f, 0.25f, 0.25f, 0.50f }};

    return texture_rects;
}

void chunk_renderer::build_floor_mesh() noexcept {
    auto biome_colors = make_biome_colors();
    auto biome_textures = make_biome_textures();

    std::default_random_engine engine(std::time(NULL));

    mesh_builder floor_mesh_builder;
    for(std::size_t x = 0; x < world::CHUNK_WIDTH; ++x) {
        for(std::size_t z = 0; z < world::CHUNK_DEPTH; ++z) {
            const int CURRENT_BIOME = chunk.biome_at(x, 0, z);

            const float LEFT = x * SQUARE_SIZE;
            const float BOTTOM = z * SQUARE_SIZE;
            const float RIGHT = LEFT + SQUARE_SIZE;
            const float TOP = BOTTOM + SQUARE_SIZE;
            const glm::vec3 TILE_COLOR = biome_colors[CURRENT_BIOME];

            std::uniform_int_distribution<std::size_t> texture_index_distrib(0, biome_textures[CURRENT_BIOME].size() - 1);
            const bounding_box<float> TILE_TEXTURE = biome_textures[CURRENT_BIOME][texture_index_distrib(engine)];

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
    const float SITE_SIZE = SQUARE_SIZE * 0.5f;
    mesh_builder sites_builder;
    for(std::size_t x = 0; x < world::CHUNK_WIDTH; ++x) {
        for (std::size_t z = 0; z < world::CHUNK_DEPTH; ++z) {
            auto sites = chunk.sites_at(x, 0, z);

            if(sites.size() > 0 && sites.front()->type() != SITE_NOTHING) {
                const glm::vec3 SITE_COLOR = get_site_color(sites.front()->type());
                make_cube(sites_builder, SITE_SIZE, SITE_COLOR, glm::vec3{x * SQUARE_SIZE + (SQUARE_SIZE / 4.f), 0.f, z * SQUARE_SIZE + (SQUARE_SIZE / 4.f)});
            }
        }
    }

    sites_mesh = sites_builder.build();
}

void chunk_renderer::build() noexcept {
    build_floor_mesh();
    build_site_meshes();
}

void chunk_renderer::render(mesh_rendering_system& renderer, glm::mat4 parent_model) const noexcept {
    profiler_us p("chunk_renderer");
    renderer.emplace(&floor_mesh, parent_model, 0, 0);
    renderer.emplace(&sites_mesh, parent_model, 1, 0);
}