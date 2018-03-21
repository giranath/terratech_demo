#ifndef MMAP_DEMO_AUTHORITATIVE_GAME_HPP
#define MMAP_DEMO_AUTHORITATIVE_GAME_HPP

#include "../common/game/base_game.hpp"
#include "../common/world/world.hpp"

class authoritative_game : public gameplay::base_game {
    infinite_world world;

    void load_flyweights();

    void load_assets();

    void generate_world();

public:
    authoritative_game();

    void on_init() override;
    void on_update(frame_duration last_frame) override;
    void on_release() override;
};

#endif //MMAP_DEMO_AUTHORITATIVE_GAME_HPP
