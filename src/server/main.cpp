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

namespace {
    volatile std::sig_atomic_t g_signal_status = 0;
}

extern "C" void sign_handler(int signo) {
    if(signo == SIGTERM) {
        g_signal_status = 1;
    }
}

int main(int argc, const char** argv) {
    std::string message = "Hello world!";
    crypto::aes::key key(16, 1024);

    std::vector<uint8_t> raw_bytes(message.size(), 0);
    std::transform(std::begin(message), std::end(message), std::begin(raw_bytes), [](char letter) {
       return static_cast<uint8_t>(letter);
    });

    auto cipher_bytes = crypto::aes::encrypt(raw_bytes, key);

    std::string cipher;
    std::transform(std::begin(cipher_bytes), std::end(cipher_bytes), std::back_inserter(cipher), [](uint8_t byte) {
        return static_cast<char>(byte);
    });

    auto decrypted = crypto::aes::decrypt(cipher_bytes, key);

    std::string decrypted_text;
    std::transform(std::begin(decrypted), std::end(decrypted), std::back_inserter(decrypted_text), [](uint8_t byte) {
        return static_cast<char>(byte);
    });

    std::cout << message << " to " << cipher << " to " << decrypted_text << std::endl;


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

    // TODO: Add a way to close the server gracefully
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