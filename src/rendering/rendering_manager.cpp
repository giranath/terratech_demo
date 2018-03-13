#include "rendering_manager.hpp"

#include <algorithm>
#include <iterator>

mesh_renderer::mesh_renderer(const mesh* m, glm::mat4 model, texture_handle texture, program_handle prog)
: rendering_mesh{m}
, model{model}
, texture{texture}
, program{prog} {

}

mesh_renderer::sort_key mesh_renderer::key() const noexcept {
    return (program << 8) | texture;
}

bool mesh_renderer::operator<(const mesh_renderer& other) const noexcept {
    return key() < other.key();
}

void mesh_rendering_system::push(const mesh_renderer& renderer) {
    meshes.push_back(renderer);
}

void mesh_rendering_system::push(mesh_renderer&& renderer) {
    meshes.push_back(std::move(renderer));
}

void mesh_rendering_system::emplace(const mesh* m, glm::mat4 model, mesh_renderer::texture_handle texture, mesh_renderer::program_handle prog) {
    meshes.emplace_back(m, model, texture, prog);
}

void mesh_rendering_system::render() {
    if(meshes.size() > 0) {

        // First we sort meshes by program than texture
        std::sort(std::begin(meshes), std::end(meshes));

        mesh_renderer::program_handle last_program = -1;
        mesh_renderer::texture_handle last_texture = -1;

        // TODO: Setup camera related uniform

        for (const mesh_renderer &renderer : meshes) {
            if(renderer.program != last_program) {
                // TODO: Bind new program
            }

            if(renderer.texture != last_texture) {
                // TODO: Bind new texture
            }

            // TODO: Setup model uniform
            renderer.rendering_mesh->render();

            last_program = renderer.program;
            last_texture = renderer.texture;
        }
    }
    // TODO: Iterate on every meshes
}