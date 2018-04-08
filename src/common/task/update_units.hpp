#ifndef MMAP_DEMO_UPDATE_UNITS_HPP
#define MMAP_DEMO_UPDATE_UNITS_HPP

#include "../async/task.hpp"
#include "../actor/unit_manager.hpp"
#include "../world/world.hpp"

namespace task {
class update_units : public async::base_task {
    unit_manager& units;
    world& w;
    float elapsed_seconds;

    static bool can_move(base_unit* unit, glm::vec3 position, world& w) noexcept;
public:
    update_units(unit_manager& units, world& w, float elapsed_seconds);
    void execute() override;

};

}


#endif //MMAP_DEMO_UPDATE_UNITS_HPP
