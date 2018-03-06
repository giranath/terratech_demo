#include "chunk_renderer.hpp"
#include "../world/world.hpp"
#include "../world/world_generator.hpp"

#include <algorithm>
#include <iterator>

chunk_renderer::chunk_renderer(const world_chunk &chunk) noexcept
: vertices{ gl::buffer::make() }
, colors{ gl::buffer::make() }
, chunk{chunk} {
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

std::vector<glm::vec3> chunk_renderer::make_vertices() {
    std::vector<glm::vec3> vertices;
    vertices.reserve(world::CHUNK_WIDTH * world::CHUNK_DEPTH * 6); // WIDTH * DEPTH squares, each square is composed of 6 vertices

    // Iterate over each square
    for(std::size_t x = 0; x < world::CHUNK_WIDTH; ++x) {
        for(std::size_t z = 0; z < world::CHUNK_DEPTH; ++z) {
            const float LEFT = x * SQUARE_SIZE;
            const float BOTTOM = z * SQUARE_SIZE;
            const float RIGHT = LEFT + SQUARE_SIZE;
            const float TOP = BOTTOM + SQUARE_SIZE;

            // First triangle
            vertices.emplace_back(LEFT, 0.f, BOTTOM);
            vertices.emplace_back(LEFT, 0.f, TOP);
            vertices.emplace_back(RIGHT, 0.f, TOP);

            // Second triangle
            vertices.emplace_back(LEFT, 0.f, BOTTOM);
            vertices.emplace_back(RIGHT, 0.f, TOP);
            vertices.emplace_back(RIGHT, 0.f, BOTTOM);
        }
    }

    return vertices;
}

std::vector<glm::vec3> chunk_renderer::make_vertice_colors() {
    auto biome_colors = make_biome_colors();
    std::vector<glm::vec3> colors_vec;
    for(std::size_t x = 0; x < world::CHUNK_WIDTH; ++x) {
        for(std::size_t z = 0; z < world::CHUNK_DEPTH; ++z) {
            const float LEFT = x * SQUARE_SIZE;
            const float BOTTOM = z * SQUARE_SIZE;
            const float RIGHT = LEFT + SQUARE_SIZE;
            const float TOP = BOTTOM + SQUARE_SIZE;

            const glm::vec3 biome_color = biome_colors[chunk.biome_at(x, 0, z)];
            for(int i = 0; i < 6; ++i) {
                colors_vec.push_back(biome_color);
            }
        }
    }

    return colors_vec;
}

void chunk_renderer::build() noexcept {
    // TODO: use mesh instead
    auto vertice_vec = make_vertices();
    auto colors_vec = make_vertice_colors();

    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(vertices));
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertice_vec.size(), &vertice_vec.front(), GL_STATIC_DRAW);

    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(colors));
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colors_vec.size(), &colors_vec.front(), GL_STATIC_DRAW);
}

void chunk_renderer::render() const noexcept {
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(vertices));
    glVertexAttribPointer(
            0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            nullptr            // array buffer offset
    );

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(colors));
    glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            nullptr                          // array buffer offset
    );

    // Draw floor
    glDrawArrays(GL_TRIANGLES, 0, world::CHUNK_WIDTH * world::CHUNK_DEPTH * 6);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    // TODO: Draw sites
}