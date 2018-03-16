#ifndef DEF_MAP_H
#define DEF_MAP_H

#include <vector>
#include <algorithm>
#include <cstdint>
#include <functional>
#include "region.h"

class Map
{
    size_t width;
    size_t heigth;
    std::vector<std::vector<Region>> regions;

public:

    Map(size_t width, size_t heigth) : regions(heigth, std::vector<Region>(width, Region())), width(width), heigth(heigth){}

    void add_site_by_noise(const double& threshold, const uint16_t& site_id, std::function<bool(double, double)> op, const std::vector<std::vector<double>>& noise)
    {
        for (size_t i = 0; i < heigth; ++i)
        {
            for (size_t j = 0; j < width; ++j)
            {
                if (op(noise[i][j], threshold))
                {
                    regions[i][j].add_site(site_id);
                }
            }
        }
    }

    Region& get_region(size_t i, size_t j)
    {
        return regions[i][j];
    }
};

#endif