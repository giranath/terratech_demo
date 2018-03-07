#ifndef DEF_ALL_COMMANDS_HPP
#define DEF_ALL_COMMANDS_HPP
#include "command.hpp"
#include "../rendering/camera.hpp"

class look_left_command : public command
{
    camera* cam;
public:
    look_left_command(camera* cam) : cam{ cam } {}
    virtual void execute()
    {
        cam->translate({ -1.f, 0.f, 0.f });
    }
    bool is_repeatable()
    {
        return true;
    }
};

class look_right_command : public command
{
    camera* cam;
public:
    look_right_command(camera* cam) : cam{ cam } {}
    virtual void execute()
    {
        cam->translate({ 1.f, 0.f, 0.f });
    }
    bool is_repeatable()
    {
        return true;
    }
};

class look_up_command : public command
{
    camera* cam;
public:
    look_up_command(camera* cam) : cam{ cam } {}
    virtual void execute()
    {
        cam->translate({ 0.f, 1.f, 0.f });
    }
    bool is_repeatable()
    {
        return true;
    }
};

class look_down_command : public command
{
    camera* cam;
public:
    look_down_command(camera* cam) : cam{ cam } {}
    virtual void execute()
    {
        cam->translate({ 0.f, -1.f, 0.f });
    }
    bool is_repeatable()
    {
        return true;
    }
};

class wireframe_command : public command
{
    bool wire = false;

    virtual void execute()
    {
        wire = !wire;
        glPolygonMode(GL_FRONT_AND_BACK, wire ? GL_LINE : GL_FILL);
    }
    bool is_repeatable()
    {
        return false;
    }
};

#endif