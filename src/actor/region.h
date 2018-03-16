#ifndef DEF_REGION_H
#define DEF_REGION_H

#include <vector>
#include <cstdint>

class Region
{
    std::vector<uint16_t> sites;
public:
    void add_site(uint16_t site_id)
    {
        sites.push_back(site_id);
    }

    bool has_site(uint16_t site_id)
    {
        return std::find(sites.begin(), sites.end(), site_id) != sites.end();
    }
};

#endif
