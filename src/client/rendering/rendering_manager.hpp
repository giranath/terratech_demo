#ifndef MMAP_DEMO_RENDERING_MANAGER_HPP
#define MMAP_DEMO_RENDERING_MANAGER_HPP

#include "camera.hpp"
#include "mesh.hpp"
#include "../opengl/opengl.hpp"
#include "virtual_texture.hpp"

#include <unordered_map>
#include <glm/glm.hpp>
#include <cstdint>

namespace rendering {

struct mesh_renderer {
    using texture_handle = uint8_t;
    using program_handle = uint8_t;
    using sort_key = uint32_t;

    const rendering::mesh *rendering_mesh;
    texture_handle texture;
    program_handle program;
    glm::mat4 model;
    uint8_t transparent_order;

    mesh_renderer(const rendering::mesh *m, glm::mat4 model, texture_handle texture, program_handle prog, uint8_t transparent_order = 0);

    sort_key key() const noexcept;

    bool operator<(const mesh_renderer &other) const noexcept;
};

class mesh_rendering_system {
    gl::vertex_array vao;
    std::vector<mesh_renderer> meshes;
    std::unordered_map<mesh_renderer::texture_handle, virtual_texture> textures;
    std::unordered_map<mesh_renderer::program_handle, gl::program> programs;
    camera *current_camera{};
public:
    mesh_rendering_system();

    void set_camera(camera *camera) noexcept;

    void set_texture(mesh_renderer::texture_handle handle, virtual_texture&& texture);
    const gl::texture* texture(mesh_renderer::texture_handle handle);

    void set_program(mesh_renderer::program_handle handle, gl::program&& program);
    gl::program* program(mesh_renderer::program_handle handle);

    void push(const mesh_renderer &renderer);

    void push(mesh_renderer &&renderer);

    template<typename It>
    void push(It begin, It end) {
        meshes.insert(meshes.end(), begin, end);
    }

    void emplace(const rendering::mesh *m, glm::mat4 model, mesh_renderer::texture_handle texture,
                 mesh_renderer::program_handle prog, uint8_t transparency);

    void render();
};

}

#endif //MMAP_DEMO_RENDERING_MANAGER_HPP
