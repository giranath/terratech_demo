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

void mesh_rendering_system::set_texture(mesh_renderer::texture_handle handle, gl::texture texture) {
    textures[handle] = std::move(texture);
}

void mesh_rendering_system::set_camera(camera* cam) noexcept {
    current_camera = cam;
}

void mesh_rendering_system::set_program(mesh_renderer::program_handle handle, gl::program program) {
    programs[handle] = std::move(program);
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

        gl::program* current_program = nullptr;
        gl::texture* current_texture = nullptr;

        for (const mesh_renderer &renderer : meshes) {
            if(renderer.program != last_program) {
                current_program = &programs[renderer.program];
                gl::bind(*current_program);

                // Setup camera related uniforms
                if(current_camera) {
                    auto projection_uniform = current_program->find_uniform<glm::mat4>("projection_matrix");
                    auto view_uniform = current_program->find_uniform<glm::mat4>("view_matrix");

                    projection_uniform.set(current_camera->projection());
                    view_uniform.set(current_camera->view());
                }
            }

            // Bind new texture
            if(renderer.texture != last_texture) {
                current_texture = &textures[renderer.texture];

                glActiveTexture(GL_TEXTURE0);
                gl::bind(gl::texture_bind<GL_TEXTURE_2D>(*current_texture));
            }

            auto model_uniform = current_program->find_uniform<glm::mat4>("model_matrix");
            model_uniform.set(renderer.model);

            // TODO: Setup is textured

            renderer.rendering_mesh->render();

            last_program = renderer.program;
            last_texture = renderer.texture;
        }
    }
}