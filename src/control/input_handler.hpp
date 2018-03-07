#ifndef DEF_INPUT_HANDLER_HPP
#define DEF_INPUT_HANDLER_HPP

#include "../rendering/camera.hpp"
#include "command.hpp"
#include "all_commands.hpp"
#include "../sdl/sdl.hpp"

#include <memory>
#include <map>
#include <vector>
#include <algorithm>
#include <tuple>
class input_handler
{
    std::map<int, bool> key_pressed;
    std::map<int, std::vector<std::pair<std::unique_ptr<command>, bool>*>> key_to_command;
    camera* cam;
    sdl::context<>* sdl;
    std::pair<std::unique_ptr<command>, bool> ctrl_m;
    std::pair<std::unique_ptr<command>, bool> arrow_left;
    std::pair<std::unique_ptr<command>, bool> arrow_right;
    std::pair<std::unique_ptr<command>, bool> arrow_up;
    std::pair<std::unique_ptr<command>, bool> arrow_down;

    void pressed(const int& key)
    {
        key_pressed[key] = true;
    }

    void released(const int& key)
    {
        key_pressed[key] = false;
        for (auto& v : key_to_command[key])
        {
            v->second = true;
        }
    }

    void current_input()
    {
        for (auto event : sdl->poll_events()) {
            if (event.type == SDL_KEYDOWN)
            {
                pressed(event.key.keysym.sym);
            }
            else if (event.type == SDL_KEYUP)
            {
                released(event.key.keysym.sym);
            }
        }
    }

public:
    input_handler(camera& cam, sdl::context<>& sdl) : 
        cam{ &cam },
        sdl{&sdl},
        arrow_left(std::make_unique<look_left_command>(&cam), true),
        arrow_right(std::make_unique<look_right_command>(&cam), true),
        arrow_up(std::make_unique<look_up_command>(&cam), true),
        arrow_down(std::make_unique<look_down_command>(&cam), true),
        ctrl_m(std::make_unique<wireframe_command>(), true)

    {
        key_to_command[SDLK_m].push_back(&ctrl_m);
        key_to_command[SDLK_LCTRL].push_back(&ctrl_m);

        key_to_command[SDLK_LEFT].push_back(&arrow_left);
        key_to_command[SDLK_RIGHT].push_back(&arrow_right);
        key_to_command[SDLK_UP].push_back(&arrow_up);
        key_to_command[SDLK_DOWN].push_back(&arrow_down);

    }

    
    void handle_input()
    {
        current_input();

        if (key_pressed[SDLK_LCTRL] && key_pressed[SDLK_m])
        {
            if (ctrl_m.second)
            {
                ctrl_m.first->execute();
                if (!ctrl_m.first->is_repeatable())
                {
                    ctrl_m.second = false;
                }
            }
        }
        else if (key_pressed[SDLK_m])
        {
            int i = 0;
        }


        if (key_pressed[SDLK_LEFT])
        {
            arrow_left.first->execute();
        }

        if (key_pressed[SDLK_RIGHT])
        {
            arrow_right.first->execute();
        }
        if (key_pressed[SDLK_UP])
        {
            arrow_up.first->execute();
        }
        if (key_pressed[SDLK_DOWN])
        {
            arrow_down.first->execute();
        }
    }

};
#endif
