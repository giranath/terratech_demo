#include "opengl/opengl.hpp"
#include "sdl/sdl.hpp"
#include "game.hpp"
#include "camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <mapgen/mapgen.h>
#include <fstream>

static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
         1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
         1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
         1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
         1.0f,-1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f,-1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
         1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
         1.0f,-1.0f, 1.0f
};

static const GLfloat g_color_buffer_data[] = {
        0.583f,  0.771f,  0.014f,
        0.583f,  0.771f,  0.014f,
        0.583f,  0.771f,  0.014f,
        0.583f,  0.771f,  0.014f,
        0.583f,  0.771f,  0.014f,
        0.583f,  0.771f,  0.014f,
        0.597f,  0.770f,  0.761f,
        0.597f,  0.770f,  0.761f,
        0.597f,  0.770f,  0.761f,
        0.597f,  0.770f,  0.761f,
        0.597f,  0.770f,  0.761f,
        0.597f,  0.770f,  0.761f,
        0.014f,  0.184f,  0.576f,
        0.014f,  0.184f,  0.576f,
        0.014f,  0.184f,  0.576f,
        0.014f,  0.184f,  0.576f,
        0.014f,  0.184f,  0.576f,
        0.014f,  0.184f,  0.576f,
        0.997f,  0.513f,  0.064f,
        0.997f,  0.513f,  0.064f,
        0.997f,  0.513f,  0.064f,
        0.997f,  0.513f,  0.064f,
        0.997f,  0.513f,  0.064f,
        0.997f,  0.513f,  0.064f,
        0.055f,  0.953f,  0.042f,
        0.055f,  0.953f,  0.042f,
        0.055f,  0.953f,  0.042f,
        0.055f,  0.953f,  0.042f,
        0.055f,  0.953f,  0.042f,
        0.055f,  0.953f,  0.042f,
        0.517f,  0.713f,  0.338f,
        0.517f,  0.713f,  0.338f,
        0.517f,  0.713f,  0.338f,
        0.517f,  0.713f,  0.338f,
        0.517f,  0.713f,  0.338f,
        0.517f,  0.713f,  0.338f
};

gl::program load_program(const std::string& name) {
    gl::vertex_shader ver;
    std::ifstream vert_file("src/shader/" + name + ".vert");
    auto vert_res = ver.compile(vert_file);

    std::ostream* out = &std::cout;
    if(!vert_res.good()) {
        out = &std::cerr;
    }
    if(!vert_res.message().empty()) *out << vert_res.message() << std::endl;

    out = &std::cout;
    gl::fragment_shader frag;
    std::ifstream frag_file("src/shader/" + name + ".frag");
    auto frag_res = frag.compile(frag_file);
    if(!frag_res.good()) {
        out = &std::cerr;
    }
    if(!frag_res.message().empty()) *out << frag_res.message() << std::endl;

    gl::program prog{};

    prog.attach(ver);
    prog.attach(frag);

    prog.link();

    return prog;
}

void opengl_message_cb(GLenum source, GLenum type, GLuint id, GLenum severity,
                       GLsizei len, const GLchar* message, const void* user_params) {
    std::cerr << "opengl: "
              << (type == GL_DEBUG_TYPE_ERROR ? "ERROR" : "") << " "
              << "type: " << type << " "
              << "severity: " << severity
              << "\n"
              << "  " << message << std::endl;
}

int main() {
    sdl::context<>& sdl = sdl::context<>::instance();

    if(!sdl.good()) {
        std::cerr << "cannot initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Setup OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Setup the window
    sdl::window window("RTS v." GAME_VERSION " (mapgen v." MAPGEN_VERSION_STR ")", 800, 600);
    if(!window.good()) {
        std::cerr << "cannot create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initializes gl3w with context created in window
    if(gl3wInit() != 0) {
        std::cerr << "cannot initialize gl3w" << std::endl;
        return 1;
    }

    const auto TARGET_FRAME_DURATION = std::chrono::milliseconds(17);

    game::frame_duration last_frame_duration = TARGET_FRAME_DURATION;

    game game_state;

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    if(gl3wIsSupported(4, 3)) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback((GLDEBUGPROC) opengl_message_cb, nullptr);
    }

    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    gl::vertex_array vao = gl::vertex_array::make();
    gl::bind(vao);

    gl::program prog = load_program("standard");


    auto model_matrix_uniform = prog.find_uniform<glm::mat4>("model_matrix");
    auto camera_matrix_uniform = prog.find_uniform<glm::mat4>("camera_matrix");

    gl::buffer vbo = gl::buffer::make();
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(vbo));
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    gl::buffer cbo = gl::buffer::make();
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(cbo));
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    glm::mat4 model_matrix = glm::scale(glm::mat4{1.f}, {100.f, 100.f, 100.f});

    camera god_cam(-400.f, 400.f, -300.f, 300.f, 0.001f, 1000.f);
    god_cam.reset({200.f, 200.f, 200.f});

    const float CAMERA_SPEED = 250.f; // 3.5 pixels per seconds

    // Game loop
    bool is_running = true;
    while(is_running) {
        const float LAST_FRAME_DURATION = std::chrono::duration_cast<std::chrono::milliseconds>(last_frame_duration).count() / 1000.f;

        model_matrix = glm::rotate(model_matrix, 0.01f, {0.f, 1.f, 0.f});

        const auto start_of_frame = game::clock::now();
        game_state.render();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gl::bind(prog);

        model_matrix_uniform.set(model_matrix);
        camera_matrix_uniform.set(god_cam.matrix());

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(vbo));
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
        gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(cbo));
        glVertexAttribPointer(
                1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                nullptr                          // array buffer offset
        );

        glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);

        window.gl_swap();

        const float cam_speed = CAMERA_SPEED * LAST_FRAME_DURATION;

        // Handle events from user here
        for(auto event : sdl.poll_events()) {
            if(event.type == SDL_QUIT) {
                is_running = false;
            }
            else if(event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_LEFT:
                        god_cam.translate({-cam_speed, 0.f, 0.f});
                        break;
                    case SDLK_RIGHT:
                        god_cam.translate({cam_speed, 0.f, 0.f});
                        break;
                    case SDLK_UP:
                        god_cam.translate({0.f, cam_speed, 0.f});
                        break;
                    case SDLK_DOWN:
                        god_cam.translate({0.f, -cam_speed, 0.f});
                        break;
                }
            }
            // TODO: Dispatch the game events to the game
        }

        // Update state here
        game_state.update(last_frame_duration);

        const auto end_of_frame = game::clock::now();
        last_frame_duration = end_of_frame - start_of_frame;
    }

    return 0;
}