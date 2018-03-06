#ifndef MMAP_DEMO_CHUNK_RENDERER_H
#define MMAP_DEMO_CHUNK_RENDERER_H

#include "../opengl/opengl.hpp"
#include "../world/world_chunk.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <map>

// This class will render a chunk on screen
class chunk_renderer {
    gl::buffer vertices;
    gl::buffer colors;
    const world_chunk& chunk;

    static std::vector<glm::vec3> make_vertices();
    std::vector<glm::vec3> make_vertice_colors();
    static std::map<int, glm::vec3> make_biome_colors();
public:
    static constexpr float SQUARE_SIZE = 40.f;

    explicit chunk_renderer(const world_chunk& chunk) noexcept;

    void build() noexcept;
    void render() const noexcept;
};


#endif //MMAP_DEMO_CHUNK_RENDERER_H
