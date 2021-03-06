#include "../common/time/clock.hpp"

#include "authoritative_game.hpp"

#ifdef WIN32
#include <SDL.h>
#include <SDL_net.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#endif

#include <iostream>
#include <csignal>

namespace {
    volatile std::sig_atomic_t g_signal_status = 0;
}

extern "C" void sign_handler(int signo) {
    if(signo == SIGTERM) {
        g_signal_status = 1;
    }
#ifdef WIN32
    else if(signo == SIGBREAK) {
        g_signal_status = 1;
    }
#endif
}

int main(int argc, char* argv[]) {
    if(SDL_Init(0) == -1) {
        std::cerr << "cannot initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    if(SDLNet_Init() == -1) {
        std::cerr << "cannot initialize SDLnet: " << SDLNet_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    map_choice chosen_map = map_choice::PLAIN_MAP;
    if (argc >= 2 && argv[1] != NULL)
    {
        std::string map_string = argv[1];

       if (map_string == "plain")
       {
           chosen_map = map_choice::PLAIN_MAP;
       }
       else if (map_string == "island")
       {
           chosen_map = map_choice::ISLAND_MAP;
       }
       else if (map_string == "lake")
       {
           chosen_map = map_choice::LAKE_MAP;
       }
       else if (map_string == "river")
       {
           chosen_map = map_choice::RIVER_MAP;
       }
    }
    authoritative_game game(chosen_map);
    game.init();

    if(std::signal(SIGTERM, sign_handler) == SIG_ERR) {
        std::cerr << "can't catch SIGTERM" << std::endl;
    }

#ifdef WIN32
    if(std::signal(SIGBREAK, sign_handler) == SIG_ERR) {
        std::cerr << "can't catch SIGBREAK" << std::endl;
    }
#endif

    game_time::highres_clock frame_time;
    while(game.is_running() && g_signal_status == 0) {
        const auto last_frame_duration = frame_time.elapsed_time<gameplay::base_game::frame_duration>();
        frame_time.restart();
        game.update(last_frame_duration);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    // If exiting game loop because of signal, call stop
    if(game.is_running()) {
        game.stop();
    }

    game.release();

    SDLNet_Quit();
    SDL_Quit();
}