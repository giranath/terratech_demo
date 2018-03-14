#include "input_handler.hpp"

#include <iostream>

namespace input {

key_input_handler::state_key_handler::state_key_handler(std::unique_ptr<command>&& c)
: command_to_execute{std::move(c)}
, is_pressed(false) {

}

void key_input_handler::state_key_handler::on_pressed() {
    is_pressed = true;
}
void key_input_handler::state_key_handler::on_released() {
    is_pressed = false;
}

void key_input_handler::state_key_handler::execute() {
    if(is_pressed) {
        command_to_execute->execute();
    }
}

key_input_handler::action_key_handler::action_key_handler(std::unique_ptr<command>&& c)
: command_to_execute(std::move(c))
, should_execute(false)
, can_be_repeated(true){

}

void key_input_handler::action_key_handler::on_pressed() {
    if(can_be_repeated) {
        should_execute = true;
        can_be_repeated = false;
    }
}

void key_input_handler::action_key_handler::on_released() {
    can_be_repeated = true;
}

void key_input_handler::action_key_handler::execute() {
    if(should_execute) {
        command_to_execute->execute();
        should_execute = false;
    }
}

void key_input_handler::register_state(int key, std::unique_ptr<command> c) {
    handlers[key] = std::make_unique<state_key_handler>(std::move(c));
}

void key_input_handler::register_action(int key, std::unique_ptr<command> c) {
    handlers[key] = std::make_unique<action_key_handler>(std::move(c));
}

void key_input_handler::register_action(int key, int modifiers, std::unique_ptr<command> c) {
    // TODO: Register key with modifiers
    register_action(key, std::move(c));
}

void key_input_handler::unregister(int key) {
    handlers.erase(key);
}

bool key_input_handler::is_registered(int key) const noexcept {
    auto it = handlers.find(key);

    return it != handlers.end();
}

void key_input_handler::handle(SDL_Event event) {
    auto it = handlers.find(event.key.keysym.sym);

    if(it != handlers.end()) {
        if(event.key.state == SDL_PRESSED) {
            it->second->on_pressed();
        }
        else if(event.key.state == SDL_RELEASED) {
            it->second->on_released();
        }
    }
}

void key_input_handler::dispatch() {
    for(auto it = handlers.begin(); it != handlers.end(); ++it) {
        it->second->execute();
    }
}

}