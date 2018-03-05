#ifndef DEF_ALL_COMMANDS_HPP
#define DEF_ALL_COMMANDS_HPP
#include "command.hpp"
#include "../camera.hpp"

class look_left_command : public command
{
    camera* cam;
public:
    look_left_command(camera* cam) : cam{ cam } {}
    virtual void execute()
    {
        cam->translate(cam->right() * 10.f);
    }
};

class look_right_command : public command
{
    camera* cam;
public:
    look_right_command(camera* cam) : cam{ cam } {}
    virtual void execute()
    {
        cam->translate(cam->right() * -10.f);
    }
};

class look_up_command : public command
{
    camera* cam;
public:
    look_up_command(camera* cam) : cam{ cam } {}
    virtual void execute()
    {
        cam->translate(cam->forward() * 10.f);
    }
};

class look_down_command : public command
{
    camera* cam;
public:
    look_down_command(camera* cam) : cam{ cam } {}
    virtual void execute()
    {
        cam->translate(cam->forward() * -10.f);
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
};

#endif