#ifndef MMAP_DEMO_CHUNK_RENDERER_H
#define MMAP_DEMO_CHUNK_RENDERER_H

#include "../opengl/opengl.hpp"
#include "mesh.hpp"
#include "../../common/world/world_chunk.hpp"
#include "rendering_manager.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <map>

// This class will render a chunk on screen
namespace rendering {

class chunk_renderer {
    const world_chunk &chunk;
    rendering::mesh floor_mesh;
    rendering::mesh sites_mesh;

    static std::map<int, glm::vec3> make_biome_colors();

    void build_floor_mesh() noexcept;

    void build_site_meshes() noexcept;

public:
    static constexpr float SQUARE_SIZE = 32.f;

    explicit chunk_renderer(const world_chunk& chunk) noexcept;
    explicit chunk_renderer(const world_chunk& chunk, bool) noexcept;

    void build() noexcept;

    void render(mesh_rendering_system &renderer, glm::mat4 parent_model) const noexcept;
    const world_chunk& displayed_chunk() const;
};

}

#endif //MMAP_DEMO_CHUNK_RENDERER_H
