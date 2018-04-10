#ifndef DEF_ARENA_HPP
#define DEF_ARENA_HPP

#include <array>
#include <bitset>
#include <iterator>
#include <memory>

template <class OBJECT, size_t NB_OBJECT>
class arena
{
    char* pool;
    std::bitset<NB_OBJECT> available;
    size_t last_added;

    size_t next(size_t n)
    {
        return (n + 1) % NB_OBJECT;
    }

    bool is_full()
    {
        bool full = false;
        size_t unavailable = 0;
        for (size_t i = 0; i < available.size(); ++i)
        {
            if (available.test(i))
            {
                ++unavailable;
            }
        }
        return unavailable == available.size();
    }

    std::pair<size_t, bool> next_available()
    {
        
        size_t current_test = next(last_added);
        for(; current_test != last_added; current_test = next(current_test))
        {
            if (!available.test(current_test))
            {
                return { current_test, true };
            }
        }
        return { current_test, false };
    }    
    
public :
    arena() :
        pool{ new char[sizeof(OBJECT) * NB_OBJECT] },
        last_added{} {
    }

    OBJECT* add()
    {
        auto next_av = next_available();
        if (next_av.second)
        {
            OBJECT* obj = new (pool + (next_av.first * sizeof(OBJECT))) OBJECT{10};
            available.set(next_av.first);
            last_added = next_av.first;
            return obj;
        }
        throw std::bad_alloc{};
        return nullptr;
    }

    void destroy(const void* to_remove)
    {
        auto v = static_cast<const char*>(to_remove) - pool;
        size_t position_debut = v / sizeof(OBJECT);
        available.reset(position_debut);
    }
};
#endif