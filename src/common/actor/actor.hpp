#ifndef DEF_ACTOR_HPP
#define DEF_ACTOR_HPP

#include <glm/glm.hpp>
#include <json/json.hpp>

enum class actor_type
{
    unit,
    building,
    ressource,

    MAX_ACTOR_TYPE
};

namespace glm {
    void to_json(nlohmann::json &j, const vec2 &v);
    void from_json(const nlohmann::json &j, vec2 &v);
    void to_json(nlohmann::json &j, const vec3 &v);
    void from_json(const nlohmann::json &j, vec3 &v);
}

class actor
{
    glm::vec3 position;
    bool is_visible;
    bool is_active;
    actor_type type;
public:

    explicit actor(actor_type type = actor_type::MAX_ACTOR_TYPE)
    : actor({}, true, true, type) {

    }

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

    const glm::vec3& get_position() const {
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

    friend void from_json(const nlohmann::json& j, actor& u);
    friend void to_json(nlohmann::json& j, const actor& u);
};



#endif