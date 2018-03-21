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

int main(int argc, const char** argv) {
    if(SDL_Init(0) == -1) {
        std::cerr << "cannot initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    if(SDLNet_Init() == -1) {
        std::cerr << "cannot initialize SDLnet: " << SDLNet_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    authoritative_game game;
    game.init();

    // TODO: Add a way to close the server gracefully
    game_time::highres_clock frame_time;
    while(game.is_running()) {
        game.update(frame_time.elapsed_time<gameplay::base_game::frame_duration>());
        frame_time.restart();
    }

    game.release();

    SDLNet_Quit();
    SDL_Quit();
}