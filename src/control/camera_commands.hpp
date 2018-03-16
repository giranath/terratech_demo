#ifndef MMAP_DEMO_CAMERA_COMMANDS_HPP
#define MMAP_DEMO_CAMERA_COMMANDS_HPP

#include "command.hpp"
#include "../rendering/camera.hpp"

namespace input {

class camera_command : public command {
    rendering::camera& cam;
    float speed;
public:
    explicit camera_command(rendering::camera& cam, float speed)
    : cam(cam)
    , speed(speed) {

    }

    void translate(glm::vec3 direction) {
        cam.translate(direction * speed);
    }

    const rendering::camera& current_camera() const {
        return cam;
    }
};

class look_left_command : public camera_command {
public:
    explicit look_left_command(rendering::camera& cam, float speed)
    : camera_command(cam, speed) {

    }

    void execute() override
    {
        translate(current_camera().right() * -1.f);
    }
};

class look_right_command : public camera_command  {
public:
    explicit look_right_command(rendering::camera& cam, float speed)
    : camera_command{ cam, speed } {

    }

    void execute() override
    {
        translate(current_camera().right());
    }
};

class look_up_command : public camera_command {
public:
    explicit look_up_command(rendering::camera& cam, float speed)
    : camera_command{ cam, speed } {

    }

    void execute() override
    {
        translate(current_camera().forward());
    }
};

class look_down_command : public camera_command {
public:
    explicit look_down_command(rendering::camera& cam, float speed)
    : camera_command{ cam, speed } {

    }

    void execute() override
    {
        translate(current_camera().forward() * -1.f);
    }
};

}

#endif //MMAP_DEMO_CAMERA_COMMANDS_HPP
