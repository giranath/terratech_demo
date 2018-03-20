#include "game.hpp"
#include "sdl/sdl.hpp"
#include "debug/profiler.hpp"
#include "../common/time/clock.hpp"
#include "../common/networking/tcp_socket.hpp"

#ifdef WIN32
#include <SDL_net.h>
#else
#include <SDL2/SDL_net.h>
#endif


#include <iostream>
#include <iterator>
#include <chrono>

void connect(std::string ip_addr, int port)
{
    
    SDLNet_Init();
    networking::tcp_socket sock;
    if (!sock.try_connect(ip_addr.c_str(), port))
    {
        std::cout << "error" << std::endl;
    }

    char *msg = "Hello!";
    int len = strlen(msg) + 1;
    int result = sock.send(reinterpret_cast<const uint8_t*>(msg), len);
    if (result < len)
    {
        std::cout << "error : " << SDLNet_GetError();
    }

    uint8_t msag[7];

    result = sock.receive(msag, 7);
    if (result <= 0) {
        std::cout << "error : " << SDLNet_GetError();
        // An error may have occured, but sometimes you can just ignore it
        // It may be good to disconnect sock because it is likely invalid now.
    }
    printf("Received: \"%s\"\n", msag);
}


void setup_opengl() {
    std::cout << "available extensions: " << std::endl;
    gl::get_extensions(std::ostream_iterator<const char*>(std::cout, "\n"));

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    gl::enable_debug_messages();

    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
}

void set_opengl_version(int major, int minor) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
}

int main(int argc, char* argv[]) {
    connect("192.192.192.1", 1647);
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

    SDL_SetWindowMinimumSize(window, 800, 600);

    // Initializes gl3w with context created in window
    if(gl3wInit() != 0) {
        std::cerr << "cannot initialize gl3w" << std::endl;
        return 1;
    }

    const auto TARGET_FRAME_DURATION = std::chrono::milliseconds(17);

    game::frame_duration last_frame_duration = TARGET_FRAME_DURATION;

    game game_state;
    game_state.resize(800, 600);

    setup_opengl();

    // Game loop
    game_time::highres_clock frame_time;
    while(!game_state.wants_to_die()) {
        frame_time.restart();

        if(game_state.fps() > 0) {
            std::cout << game_state.fps() << " : " << game_state.average_fps() << std::endl;
        }
        // Render last frame on screen
        {
            profiler_us p("rendering");
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            game_state.render();
            window.gl_swap();
        }

        // Handle events from user here
        {
            profiler_us p("events");
            for (auto event : sdl.poll_events()) {
                if (event.type == SDL_QUIT) {
                    game_state.kill();
                } else if (event.type == SDL_WINDOWEVENT) {
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        const int new_width = event.window.data1;
                        const int new_height = event.window.data2;

                        game_state.resize(new_width, new_height);
                    }
                } else {
                    game_state.handle_event(event);
                }
            }
        }
        
        // Update state here
        {
            profiler_us p("update");
            game_state.update(last_frame_duration);
        }

        last_frame_duration = frame_time.elapsed_time<game::frame_duration>();
    }

    return 0;
}