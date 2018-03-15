#ifndef DEF_ACTOR_HPP
#define DEF_ACTOR_HPP

#include <glm/glm.hpp>

enum class actor_type
{
    unit,
    building,
    ressource,

    MAX_ACTOR_TYPE
};

class actor
{
    glm::vec3 position;
    bool is_visible;
    bool is_active;
    actor_type type;
public:

    actor(glm::vec3 position, bool is_visible, bool is_active, actor_type type) :
        position{ position },
        is_visible{ is_visible },
        is_active{ is_active },
        type{type}
    {
    }

    void set_position(glm::vec3 _position)
    {
        position = _position;
    }

    glm::vec3& get_position()
    {
        return position;
    }

    void set_visibility(bool _is_visible)
    {
        is_visible = _is_visible;
    }

    void set_active(bool _is_active)
    {
        is_active = _is_active;
    }

    actor_type get_type()
    {
        return type;
    }
};
#endif