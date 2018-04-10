#ifndef DEF_ARRAY_MAP_HPP
#define DEF_ARRAY_MAP_HPP

#include "../memory/arena.hpp"
#include "unit.hpp"

#include <algorithm>
#include <iterator>



template <class T, size_t NB_OBJ>
class array_map 
{
    std::array<size_t, NB_OBJ> keys;
    std::array<T, NB_OBJ> values;
    size_t position;

    size_t find_key_position(size_t key) const 
    {
        auto it = std::find(keys.begin(), keys.end(), key);
        if (it != keys.end())
        {
            return std::distance(keys.begin(), it);
        }
        return NB_OBJ + 1;
    }

public:
    class iterator;
    friend iterator;
    
    class iterator : std::iterator<std::forward_iterator_tag, std::pair<size_t, T>>
    {
        std::pair<size_t, T> pair{};
        size_t index{};
        array_map* owner{};
    public:
        iterator(array_map* x, size_t index) :
            owner(x),
            index{ index } {
            if (owner)
            {
                pair.first = owner->keys[index];
                pair.second = owner->values[index];
            }
        }
        iterator() = default;

        iterator(const iterator& mit) :
            owner{ mit.owner },
            index(mit.index)
        {
            if (owner)
            {
                pair.first = owner->keys[index];
                pair.second = owner->values[index];
            }
        }

        iterator& operator++()
        {
            ++index;
            pair.first = owner->keys[index];
            pair.second = owner->values[index];
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp(*this);
            operator++();
            return tmp;
        }

        bool operator==(const iterator& rhs) const
        {
            return index == rhs.index;
        }

        bool operator!=(const iterator& rhs) const
        {
            return index != rhs.index;
        }

        std::pair<size_t, T>& operator*()
        {
            return pair;
        }

        std::pair<size_t, T>* operator->()
        {
            return &pair;
        }
    };
    T& operator[](size_t key)
    {
        auto it = std::find(keys.begin(), keys.end(), key);
        if (it != keys.end())
        {
            return values[std::distance(keys.begin(), it)];
        }
        assert(false, "key noy found");
    }

    const T& operator[](size_t key) const
    {
        auto it = std::find(keys.begin(), keys.end(), key);
        if (it != keys.end())
        {
            return values[std::distance(keys.begin(), it)];
        }
        assert(false);
    }

    void add(size_t key, const T& value)
    {
        assert(position < NB_OBJ);
        keys[position] = key;
        values[position] = value;
        ++position;
    }


    void remove(size_t key)
    {
        assert(position > 0);

        auto pos = find_key_position(key);
        if (pos > NB_OBJ)
        {
            return;
        }

        std::swap(keys[pos], keys[position - 1]);
        std::swap(values[pos], values[position - 1]);

        values[position - 1].~T();

        --position;
    }
    size_t size() const
    {
        return position - 1;
    }
    bool contains(uint32_t key) const 
    {
        auto it = std::find(keys.begin(), keys.end(), key);
        return it != keys.end();
    }

    iterator begin()
    {
        return iterator{ this, 0 };
    }

    iterator end()
    {
        return iterator{ this, position };
    }


};
#endif
