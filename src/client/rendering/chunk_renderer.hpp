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

    rendering::mesh_builder build_floor();
    void build_floor_mesh() noexcept;
    void rebuild_floor_mesh() noexcept;

    rendering::mesh_builder build_sites();
    void build_site_meshes() noexcept;
    void rebuild_site_meshes() noexcept;

public:
    static constexpr float SQUARE_SIZE = 32.f;

    explicit chunk_renderer(const world_chunk& chunk) noexcept;
    chunk_renderer(const world_chunk& chunk, gl::buffer&& vertices, gl::buffer&& colors, gl::buffer&& uvs,
                                             gl::buffer&& svertices, gl::buffer&& scolors, gl::buffer&& suvs) noexcept;

    void build() noexcept;

    void render(mesh_rendering_system &renderer, glm::mat4 parent_model) const noexcept;
    const world_chunk& displayed_chunk() const;
};

}

#endif //MMAP_DEMO_CHUNK_RENDERER_H
