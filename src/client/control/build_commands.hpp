#ifndef MMAP_DEMO_BUILD_COMMANDS_HPP
#define MMAP_DEMO_BUILD_COMMANDS_HPP

#include "command.hpp"

#include <memory>

namespace input {
template<int Mode, typename Predicate>
class change_click_mode : public command {
    int& current_mode;
    Predicate pred;
public:
    explicit change_click_mode(int& mode, Predicate pred)
    : current_mode(mode)
    , pred(pred) {

    }

    void execute() override {
        if(pred()) {
            current_mode = Mode;
        }
    }
};

template<int Mode, typename Predicate>
std::unique_ptr<change_click_mode<Mode, Predicate> make_change_click_mode_command(int& mode, Predicate pred) {
    return std::make_unique<change_click_mode<Mode, Predicate>>(mode, pred);
}

}

#endif //MMAP_DEMO_BUILD_COMMANDS_HPP
