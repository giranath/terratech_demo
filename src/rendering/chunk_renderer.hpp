#ifndef MMAP_DEMO_CHUNK_RENDERER_H
#define MMAP_DEMO_CHUNK_RENDERER_H

#include "../opengl/opengl.hpp"
#include "mesh.hpp"
#include "../world/world_chunk.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <map>

// This class will render a chunk on screen
class chunk_renderer {
    const world_chunk& chunk;
    mesh floor_mesh;

    static std::map<int, glm::vec3> make_biome_colors();
    void build_floor_mesh() noexcept;
public:
    static constexpr float SQUARE_SIZE = 40.f;

    explicit chunk_renderer(const world_chunk& chunk) noexcept;

    void build() noexcept;
    void render(gl::program& program) const noexcept;
};


#endif //MMAP_DEMO_CHUNK_RENDERER_H
