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

// 1. Drag events
// 2. Click events

/*
class drag_event_handler {
public:
    ~drag_event_handler() = default;

    // Called when the dragging started
    virtual void on_start(const glm::vec2& start) = 0;

    // Called every frame
    virtual void on_dragging(const glm::vec2& start, const glm::vec2& current) = 0;

    // Called when the dragging ended
    virtual void on_end(const glm::vec2& start, const glm::vec2& end) = 0;
};
 */

/*
class base_mouse_event_handler {
public:
    virtual ~base_mouse_event_handler() = default;
    virtual void on_pressed() = 0;
    virtual void execute() = 0;
    virtual void on_moved() = 0;
    virtual void on_released() = 0;
};

class click_mouse_event_handler : public base_mouse_event_handler {
    std::unique_ptr<command> command_to_execute;
    bool should_execute;
public:
    click_mouse_event_handler(std::unique_ptr<command> c);
    void on_pressed() override;
    void execute() override;
    void on_moved() override;
    void on_released() override;
};
*/

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

using drag_event_handler = std::function<void(drag_event)>;


class base_mouse_drag_handler {
private:
    bool is_dragging;
    glm::vec2 start_pos;
    drag_event_handler handler;
public:
    base_mouse_drag_handler(drag_event_handler handler);
    virtual void on_moved(const glm::vec2& current, const glm::vec2& delta);
    virtual void on_released(const glm::vec2& current);
};

class mouse_input_handler {
    std::unordered_map<int, std::unique_ptr<command>> click_handlers;
    std::unordered_map<int, bool> button_states;
    std::unordered_map<int, std::unique_ptr<base_mouse_drag_handler>> drag_handlers;
public:
    mouse_input_handler();
    void register_click(int button, std::unique_ptr<command> c);
    void register_drag(int button, drag_event_handler handler);

    bool handle(SDL_Event event);
    void dispatch();
};

}


#endif //MMAP_DEMO_MOUSE_INPUT_HANDLER_HPP
