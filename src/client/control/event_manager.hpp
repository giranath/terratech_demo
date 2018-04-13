#ifndef MMAP_DEMO_EVENT_MANAGER_HPP
#define MMAP_DEMO_EVENT_MANAGER_HPP

#include "key_input_handler.hpp"
#include "mouse_input_handler.hpp"
#include <vector>

namespace input {


class event_manager {
public:
    class context {
        friend event_manager;

        context* parent;
        mouse_input_handler mouse_handler;
        key_input_handler key_handler;

        context(context* c);
        void handle(SDL_Event event);
        void dispatch();
    public:
        context();

        void register_key_action(int key, std::unique_ptr<command> c);
        void register_key_action(int key, int modifiers, std::unique_ptr<command> c);
        void register_key_state(int key, std::unique_ptr<command> c);
        void register_mouse_drag(int button, drag_event_handler handler);
        void register_mouse_click(int button, click_event_handler handler);


    };
    using context_handle = std::size_t;
    static const context_handle root = 0;
private:
    std::vector<context> contexes;
    context_handle current_context;
public:
    event_manager();
    context_handle make_context();
    context_handle extend_context(context_handle to_extend);

    context& get(context_handle handle);

    void change_current(context_handle new_current);

    void handle(SDL_Event event);
    void dispatch();
};

}


#endif //MMAP_DEMO_EVENT_MANAGER_HPP
