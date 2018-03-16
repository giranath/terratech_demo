#ifndef DEF_RESSOURCE_HPP
#define DEF_RESSOURCE_HPP
#include "ressource_type.hpp"
#include "actor.hpp"

class ressource : public actor
{
    ressource_type type;
    int ammount;

public:
    ressource() : actor(actor_type::ressource) {

    }
};
#endif
