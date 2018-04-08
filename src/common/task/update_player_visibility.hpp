#ifndef MMAP_DEMO_UPDATE_PLAYER_VISIBILITY_HPP
#define MMAP_DEMO_UPDATE_PLAYER_VISIBILITY_HPP

#include "../async/task.hpp"
#include "../world/visibility_map.hpp"
#include "../actor/unit_manager.hpp"

namespace task {
class update_player_visibility : public async::base_task {
    uint8_t player_id;
    visibility_map visibility_;
    unit_manager& units_;
public:
    update_player_visibility(uint8_t player, const visibility_map& v, unit_manager& units);

    void execute() override;
    uint8_t get_player() const noexcept;
    const visibility_map& visibility() const noexcept;
};
}

#endif //MMAP_DEMO_UPDATE_PLAYER_VISIBILITY_HPP
