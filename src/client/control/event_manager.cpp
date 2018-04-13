#include "event_manager.hpp"

namespace input {

event_manager::context::context()
: parent(nullptr) {

}

event_manager::context::context(context* c)
: parent(c) {

}

void event_manager::context::register_key_action(int key, std::unique_ptr<command> c) {
    key_handler.register_action(key, std::move(c));
}

void event_manager::context::register_key_action(int key, int modifiers, std::unique_ptr<command> c) {
    key_handler.register_action(key, modifiers, std::move(c));
}

void event_manager::context::register_key_state(int key, std::unique_ptr<command> c) {
    key_handler.register_state(key, std::move(c));
}

void event_manager::context::register_mouse_drag(int button, drag_event_handler handler) {
    mouse_handler.register_drag(button, std::move(handler));
}

void event_manager::context::register_mouse_click(int button, click_event_handler handler) {
    mouse_handler.register_click(button, std::move(handler));
}

void event_manager::context::handle(SDL_Event event) {
    switch(event.type) {
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEWHEEL:
        case SDL_MOUSEMOTION:
            if(!mouse_handler.handle(event) && parent) {
                parent->mouse_handler.handle(event);
            }
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            if(!key_handler.handle(event) && parent) {
                parent->key_handler.handle(event);
            }
            break;
    }
}

void event_manager::context::dispatch() {
    key_handler.dispatch();
    mouse_handler.dispatch();

    if(parent) {
        parent->dispatch();
    }
}

event_manager::event_manager()
: contexes(1)
, current_context(0) {
}

event_manager::context_handle event_manager::make_context() {
    context_handle hdl = contexes.size();

    contexes.emplace_back();

    return hdl;
}

event_manager::context_handle event_manager::extend_context(context_handle to_extend) {
    assert(to_extend < contexes.size());

    context_handle hdl = contexes.size();

    contexes.push_back(context(&get(to_extend)));

    return hdl;
}

event_manager::context& event_manager::get(context_handle handle) {
    assert(handle < contexes.size());
    return contexes[handle];
}

void event_manager::change_current(context_handle new_current) {
    assert(new_current < contexes.size());

    current_context = new_current;
}

void event_manager::handle(SDL_Event event) {
    contexes[current_context].handle(event);
}

void event_manager::dispatch() {
    contexes[current_context].dispatch();
}

}