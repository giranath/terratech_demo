#ifndef DEF_RESSOURCE_VALUE_HPP
#define DEF_RESSOURCE_VALUE_HPP

struct ressource_value
{
    unsigned short int wood;
    unsigned short int food;
    unsigned short int stone;
    unsigned short int gold;
    unsigned short int magic_essence;

    ressource_value():
        wood{},
        food{},
        stone{},
        gold{},
        magic_essence{}
        {}
    int total_ressources()
    {
        return wood + food + stone + gold + magic_essence;
    }


};
#endif
