#ifndef MMAP_DEMO_RENDERING_MANAGER_HPP
#define MMAP_DEMO_RENDERING_MANAGER_HPP

#include "mesh.hpp"

#include <glm/glm.hpp>
#include <cstdint>

// TODO: Render meshes
// Sort meshes by program, texture

struct mesh_renderer {
    using texture_handle = uint8_t;
    using program_handle = uint8_t;
    using sort_key = uint16_t;

    const mesh* rendering_mesh;
    texture_handle texture;
    program_handle program;
    glm::mat4 model;

    mesh_renderer(const mesh* m, glm::mat4 model, texture_handle texture, program_handle prog);
    sort_key key() const noexcept;
    bool operator<(const mesh_renderer& other) const noexcept;
};

class mesh_rendering_system {
    std::vector<mesh_renderer> meshes;
public:
    void push(const mesh_renderer& renderer);
    void push(mesh_renderer&& renderer);
    void emplace(const mesh* m, glm::mat4 model, mesh_renderer::texture_handle texture, mesh_renderer::program_handle prog);
    void render();
};


#endif //MMAP_DEMO_RENDERING_MANAGER_HPP
