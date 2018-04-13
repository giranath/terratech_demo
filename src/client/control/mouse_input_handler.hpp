#ifndef MMAP_DEMO_MOUSE_INPUT_HANDLER_HPP
#define MMAP_DEMO_MOUSE_INPUT_HANDLER_HPP

#include "../sdl/sdl.hpp"
#include "command.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <array>
#include <functional>

namespace input {

struct drag_event {
    enum class state {
        starting,
        dragging,
        ending
    };

    const state current_state;
    const glm::vec2 start;
    const glm::vec2 current;
    const glm::vec2 rel;

    drag_event(state s, glm::vec2 start, glm::vec2 end, glm::vec2 rel);
};

struct click_event {
    const int count;
    const glm::vec2 position;

    click_event(int count, glm::vec2 pos);
};

using drag_event_handler = std::function<void(drag_event)>;
using click_event_handler = std::function<void(click_event)>;

class base_mouse_drag_handler {
private:
    bool is_dragging;
    glm::vec2 start_pos;
    drag_event_handler handler;
public:
    explicit base_mouse_drag_handler(drag_event_handler handler);
    virtual void on_moved(const glm::vec2& current, const glm::vec2& delta);
    virtual void on_released(const glm::vec2& current);
};

class mouse_input_handler {
    std::unordered_map<int, click_event_handler> click_handlers;
    std::unordered_map<int, bool> button_states;
    std::unordered_map<int, base_mouse_drag_handler> drag_handlers;
public:
    mouse_input_handler();
    void register_click(int button, click_event_handler handler);
    void register_drag(int button, drag_event_handler handler);

    bool handle(SDL_Event event);
    void dispatch();
};

}


#endif //MMAP_DEMO_MOUSE_INPUT_HANDLER_HPP
