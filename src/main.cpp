#include "opengl/opengl.hpp"
#include "sdl/sdl.hpp"
#include "game.hpp"
#include "camera.hpp"
#include "debug/profiler.hpp"
#include "debug/profiler_administrator.hpp"
#include "control/input_handler.hpp"
#include "world/world.hpp"
#include "world/world_generator.hpp"
#include "chunk_renderer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <terratech/terratech.h>
#include <fstream>

template<typename Shader>
Shader load_shader(const std::string& name) {
    std::ifstream file("asset/shader/" + name);
    Shader shader;

    auto res = shader.compile(file);
    if(!res.good()) {
        std::cerr << res.message() << std::endl;
        shader = Shader{};
    }
    else if(!res.message().empty()) {
        std::cout << res.message() << std::endl;
    }

    return shader;
}

gl::program load_program(const std::string& name) {
    auto ver = load_shader<gl::vertex_shader>(name + ".vert");
    auto frag = load_shader<gl::fragment_shader>(name + ".frag");

    gl::program prog{};
    prog.attach(ver);
    prog.attach(frag);

    auto res = prog.link();
    if(!res.good()) {
        prog = gl::program{};
        std::cerr << res.message() << std::endl;
    }

    return prog;
}

std::ostream& print_opengl_severity(std::ostream& os, GLenum severity) {
    switch(severity) {
        case GL_DEBUG_SEVERITY_HIGH_ARB:
            os << "high";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB:
            os << "medium";
            break;
        case GL_DEBUG_SEVERITY_LOW_ARB:
            os << "low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            os << "debug";
            break;
        default:
            os << "unknown";
            break;
    }

    return os;
}

std::ostream& print_opengl_type(std::ostream& os, GLenum type) {
    switch(type) {
        case GL_DEBUG_TYPE_ERROR_ARB:
            os << "error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
            os << "deprecated";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
            os << "undefined behavior";
            break;
        case GL_DEBUG_TYPE_PORTABILITY_ARB:
            os << "portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE_ARB:
            os << "performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            os << "marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            os << "push group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            os << "pop group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            os << "other";
            break;
        default:
            os << "unknown";
            break;
    }
    return os;
}

std::ostream& print_opengl_source(std::ostream& os, GLenum source) {
    switch(source) {
        case GL_DEBUG_SOURCE_API:
            os << "api";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
            os << "window";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
            os << "compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
            os << "third-party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION_ARB:
            os << "application";
            break;
        case GL_DEBUG_SOURCE_OTHER_ARB:
            os << "other";
            break;
        default:
            os << "unknown";
            break;
    }
    return os;
}

void opengl_message_cb(GLenum source, GLenum type, GLuint id, GLenum severity,
                       GLsizei len, const GLchar* message, const void* user_params) {
    std::cerr << "[GL:" << id << "]";

    std::cerr << "[";
    print_opengl_source(std::cerr, source);
    std::cerr << "]";

    std::cerr << "[";
    print_opengl_severity(std::cerr, severity);
    std::cerr << "]";

    std::cerr << "[";
    print_opengl_type(std::cerr, type);
    std::cerr << "] ";

    std::cerr << message << std::endl;
}

void setup_opengl_debug_msg() {
    // We check if glDebugMessageCallback exists
    GL3WglProc debug_msg_callback_extension = gl3wGetProcAddress("glDebugMessageCallbackARB");
    if(debug_msg_callback_extension) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

        typedef GLvoid (APIENTRYP DebugMessageCallbackARBPROC )(GLDEBUGPROCARB callback, GLvoid* userParam);
        DebugMessageCallbackARBPROC glDebugMessageCallbackARB = reinterpret_cast<DebugMessageCallbackARBPROC>(debug_msg_callback_extension);

        glDebugMessageCallbackARB((GLDEBUGPROCARB) opengl_message_cb, nullptr);
    }
}

void setup_opengl() {
    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    setup_opengl_debug_msg();

    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
}

void set_opengl_version(int major, int minor) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
}

int main(int argc, char* argv[]) {

    world game_world(static_cast<uint32_t>(std::time(nullptr)));
    auto chunk = game_world.generate_at(0, 0);
    auto chunk2 = game_world.generate_at(1, 0);

    sdl::context<>& sdl = sdl::context<>::instance();
    if(!sdl.good()) {
        std::cerr << "cannot initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Setup OpenGL attributes
    set_opengl_version(3, 3);

    // Setup the window

    sdl::window window("RTS v." GAME_VERSION " (terratech v." TERRA_VERSION_STR ")", 800, 600);
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

    setup_opengl();

    gl::vertex_array vao = gl::vertex_array::make();
    gl::bind(vao);

    gl::program prog = load_program("standard");

    auto model_matrix_uniform = prog.find_uniform<glm::mat4>("model_matrix");
    auto camera_matrix_uniform = prog.find_uniform<glm::mat4>("camera_matrix");

    chunk_renderer chunk_ren{chunk};
    chunk_renderer chunk2_ren{chunk2};



    camera god_cam(-400.f, 400.f, -300.f, 300.f, -1000.f, 1000.f);
    god_cam.reset({200.f, 200.f, 200.f});

    const float CAMERA_SPEED = 250.f; // 250 pixels per seconds

    input_handler input(god_cam);
    // Game loop
    bool is_running = true;
    bool is_scrolling = false;
    while(is_running) {
        const float LAST_FRAME_DURATION = std::chrono::duration_cast<std::chrono::milliseconds>(last_frame_duration).count() / 1000.f;

        const auto start_of_frame = game::clock::now();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gl::bind(prog);

        glm::mat4 model_matrix{1.f};
        model_matrix_uniform.set(model_matrix);
        camera_matrix_uniform.set(god_cam.matrix());

        game_state.render();

        // Render first chunk
        chunk_ren.render();

        // Render second chunk
        model_matrix = glm::translate(model_matrix, {world::CHUNK_WIDTH * chunk_renderer::SQUARE_SIZE, 0.f, 0.f});
        model_matrix_uniform.set(model_matrix);
        chunk2_ren.render();

        window.gl_swap();


        constexpr float CAM_SPEED = 10.f;

        // Handle events from user here
        for(auto event : sdl.poll_events()) {
            profiler<std::chrono::nanoseconds> p("test");
            
            if(event.type == SDL_QUIT) {
                is_running = false;
            }
            else if(event.type == SDL_MOUSEBUTTONDOWN) {
                if(event.button.button == SDL_BUTTON_MIDDLE) {
                    is_scrolling = true;
                }
            }
            else if(event.type == SDL_MOUSEBUTTONUP) {
                if(event.button.button == SDL_BUTTON_MIDDLE) {
                    is_scrolling = false;
                }
            }
            else if(event.type == SDL_MOUSEMOTION) {
                if(is_scrolling) {
                    god_cam.translate({event.motion.xrel, 0.f, event.motion.yrel});
                }
            }
            else if (event.type == SDL_KEYDOWN)
            {
                input.is_pressed(event.key.keysym.sym);
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