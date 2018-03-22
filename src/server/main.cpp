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
#include "../common/crypto/aes.hpp"
#include "../common/memory/heap_allocator.hpp"
#include "../common/memory/malloc_allocator.hpp"

namespace {
    volatile std::sig_atomic_t g_signal_status = 0;
}

extern "C" void sign_handler(int signo) {
    if(signo == SIGTERM) {
        g_signal_status = 1;
    }
}

int main(int argc, char* argv[]) {
    memory::malloc_allocator backbone_allocator;
    memory::raw_memory_ptr heap_memory = backbone_allocator.allocate(1024);

    memory::heap_allocator heap_allocator(heap_memory, 1024);
    uint64_t* temp = static_cast<uint64_t*>(heap_allocator.allocate(sizeof(temp)));
    uint64_t* temp2 = static_cast<uint64_t*>(heap_allocator.allocate(sizeof(temp2)));

    //heap_allocator.free(reinterpret_cast<void*>(temp2), sizeof(uint64_t));
    heap_allocator.free(reinterpret_cast<void*>(temp), sizeof(uint64_t));

    backbone_allocator.free(heap_memory, 1024);


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

    if(std::signal(SIGTERM, sign_handler) == SIG_ERR) {
        std::cerr << "can't catch SIGTERM" << std::endl;
    }

    game_time::highres_clock frame_time;
    while(game.is_running() && g_signal_status == 0) {
        game.update(frame_time.elapsed_time<gameplay::base_game::frame_duration>());
        frame_time.restart();
    }
    // If exiting game loop because of signal, call stop
    if(game.is_running()) {
        game.stop();
    }

    game.release();

    SDLNet_Quit();
    SDL_Quit();
}