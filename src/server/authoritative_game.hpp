#ifndef MMAP_DEMO_AUTHORITATIVE_GAME_HPP
#define MMAP_DEMO_AUTHORITATIVE_GAME_HPP

#include "../common/game/base_game.hpp"

class authoritative_game : public gameplay::base_game {

    void load_flyweights();

    void load_assets();

public:
    authoritative_game();

    void on_init() override;
    void on_update(frame_duration last_frame) override;
    void on_release() override;
};


#endif //MMAP_DEMO_AUTHORITATIVE_GAME_HPP
