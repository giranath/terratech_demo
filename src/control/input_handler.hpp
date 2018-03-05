#ifndef DEF_INPUT_HANDLER_HPP
#define DEF_INPUT_HANDLER_HPP

#include "../camera.hpp"
#include "command.hpp"
#include "all_commands.hpp"
#include "../sdl/sdl.hpp"

#include <memory>
#include <map>

class input_handler
{

    camera* cam;
    std::map<int, command*> binding;

    std::unique_ptr<look_left_command> look_left;
    std::unique_ptr<look_right_command> look_right;
    std::unique_ptr<look_up_command> look_up;
    std::unique_ptr<look_down_command> look_down;
    std::unique_ptr<wireframe_command> wireframe;

public:
    void bind_key_to_command(const int& key, command* command)
    {
        binding[key] = command;
    }
    input_handler(camera& cam) : cam{ &cam },
        look_left(std::make_unique<look_left_command>(&cam)),
        look_right(std::make_unique<look_right_command>(&cam)),
        look_up(std::make_unique<look_up_command>(&cam)),
        look_down(std::make_unique<look_down_command>(&cam)),
        wireframe(std::make_unique<wireframe_command>())

    {
        bind_key_to_command(SDLK_LEFT, look_left.get());
        bind_key_to_command(SDLK_RIGHT, look_right.get());
        bind_key_to_command(SDLK_UP, look_up.get());
        bind_key_to_command(SDLK_DOWN, look_down.get());
        bind_key_to_command(SDLK_w, look_up.get());
        bind_key_to_command(SDLK_a, look_left.get());
        bind_key_to_command(SDLK_d, look_right.get());
        bind_key_to_command(SDLK_s, look_down.get());
        bind_key_to_command(SDLK_m, wireframe.get());
    }

    void is_pressed(const int& key)
    {
        if (binding[key])
        {
            binding[key]->execute();
        }
    }
};
#endif
