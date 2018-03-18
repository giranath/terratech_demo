#ifndef DEF_ALL_COMMANDS_HPP
#define DEF_ALL_COMMANDS_HPP

#include "command.hpp"
#include "camera_commands.hpp"
#include "change_unit_command.hpp"
#include "generic_command.hpp"


#include "../opengl/opengl.hpp"

namespace input {

class wireframe_command : public command {
    bool wire = false;
public:
    wireframe_command() = default;

    void execute() override {
        wire = !wire;
        glPolygonMode(GL_FRONT_AND_BACK, wire ? GL_LINE : GL_FILL);
    }
};

}

#endif