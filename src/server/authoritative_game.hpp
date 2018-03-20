#ifndef MMAP_DEMO_AUTHORITATIVE_GAME_HPP
#define MMAP_DEMO_AUTHORITATIVE_GAME_HPP

#include "../common/game/base_game.hpp"

class authoritative_game : public gameplay::base_game {
public:
    authoritative_game();

    void on_update(frame_duration last_frame) override;
};


#endif //MMAP_DEMO_AUTHORITATIVE_GAME_HPP
