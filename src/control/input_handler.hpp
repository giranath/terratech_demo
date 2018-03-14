#ifndef DEF_INPUT_HANDLER_HPP
#define DEF_INPUT_HANDLER_HPP

#include "all_commands.hpp"
#include "../sdl/sdl.hpp"

#include <map>
#include <memory>

// Two types of inputs:
// 1 - state  (is happening NOW)
// 2 - action (only once)

namespace input {

class key_input_handler {
    class base_key_handler {
    public:
        virtual ~base_key_handler() = default;

        virtual void on_pressed() = 0;
        virtual void on_released() = 0;
        virtual void execute() = 0;
    };

    class state_key_handler : public base_key_handler {
        bool is_pressed;
        std::unique_ptr<command> command_to_execute;
    public:
        explicit state_key_handler(std::unique_ptr<command>&& c);
        void on_pressed() override;
        void on_released() override;
        void execute() override;
    };

    class action_key_handler : public base_key_handler {
        bool should_execute;
        bool can_be_repeated;
        std::unique_ptr<command> command_to_execute;
    public:
        explicit action_key_handler(std::unique_ptr<command>&& c);
        void on_pressed() override;
        void on_released() override;
        void execute() override;
    };

    std::map<int, std::unique_ptr<base_key_handler>> handlers;
public:
    void register_state(int key, std::unique_ptr<command> c);
    void register_action(int key, std::unique_ptr<command> c);
    void unregister(int key);
    bool is_registered(int key) const noexcept;

    void handle(SDL_Event event);

    void dispatch();
};

}
#endif
