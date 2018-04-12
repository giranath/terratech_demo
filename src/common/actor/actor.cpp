#include "actor.hpp"

namespace glm {
    void to_json(nlohmann::json &j, const vec2 &v) {
        j["x"] = v.x;
        j["y"] = v.y;
    }

    void from_json(const nlohmann::json &j, vec2 &v) {
        v.x = j["x"];
        v.y = j["y"];
    }

    void to_json(nlohmann::json &j, const vec3 &v) {
        j["x"] = v.x;
        j["y"] = v.y;
        j["z"] = v.z;
    }

    void from_json(const nlohmann::json &j, vec3 &v) {
        v.x = j["x"];
        v.y = j["y"];
        v.z = j["z"];
    }
}

constexpr const char* actor_type_to_str(actor_type type)  {
    switch(type) {
        case actor_type::unit:
            return "unit";
        case actor_type::building:
            return "building";
        case actor_type::ressource:
            return "resource";
        default:
            return "unkownn";
    }
}

actor_type str_to_actor_type(const char* t) {
    if(std::strcmp(t, "unit") == 0) {
        return actor_type::unit;
    }
    else if(std::strcmp(t, "building") == 0) {
        return actor_type::building;
    }
    else if(std::strcmp(t, "resource") == 0) {
        return actor_type::ressource;
    }

    return actor_type::MAX_ACTOR_TYPE;
}

void to_json(nlohmann::json& j, const actor& u) {
    j["position"] = u.position;
    j["is_visible"] = u.is_visible_;
    j["is_active"] = u.is_active;
    j["type"] = actor_type_to_str(u.type);
}

void from_json(const nlohmann::json& j, actor& u) {
    u.position = j["position"].get<glm::vec3>();
    u.is_visible_ = j["is_visible"];
    u.is_active = j["is_active"];
    u.type = str_to_actor_type(j["type"].get<std::string>().c_str());
}