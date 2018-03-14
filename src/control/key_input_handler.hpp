#ifndef DEF_INPUT_HANDLER_HPP
#define DEF_INPUT_HANDLER_HPP

#include "all_commands.hpp"
#include "../sdl/sdl.hpp"

#include <map>
#include <unordered_map>
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

    class multiple_key_handler {
        using iterator = std::unordered_map<uint16_t, std::unique_ptr<base_key_handler>>::iterator;
        std::unordered_map<uint16_t, std::unique_ptr<base_key_handler>> handlers;

        iterator find_modifier(uint16_t mod);

    public:
        bool add(std::unique_ptr<base_key_handler>&& handler);
        bool add(uint16_t mod, std::unique_ptr<base_key_handler>&& handler);

        void press(uint16_t mod);
        void release(uint16_t mod);

        void execute();
    };

    std::unordered_map<int, multiple_key_handler> handlers;
public:
    bool register_state(int key, std::unique_ptr<command> c);
    bool register_action(int key, std::unique_ptr<command> c);
    bool register_action(int key, int modifiers, std::unique_ptr<command> c);
    void unregister(int key);
    bool is_registered(int key) const noexcept;

    void handle(SDL_Event event);

    void dispatch();
};

}
#endif
