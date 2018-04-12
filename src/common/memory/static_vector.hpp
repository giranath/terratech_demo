#ifndef DEF_STATIC_VECTOR_HPP
#define DEF_STATIC_VECTOR_HPP

#include <array>
#include <memory>
#include <iterator>
#include <utility>
template <class T, size_t NB>
class static_vector 
{
    std::unique_ptr<std::array<T, NB>> values;
    size_t position;

public:
    static_vector() : 
        values(std::make_unique<std::array<T, NB>>()),
        position{0}
    {}

    void push_back(T& value)
    {
        (*values)[position] = value;
    }

    template <class ... args>
    void emplace_back(args&&... a)
    {
        (*values)[position] = T(std::forward<args>(a) ...);
    }

    T& operator[](size_t pos)
    {
        assert(pos < position);

        return (*values)[pos];
    }

    const T& operator[](size_t pos) const
    {
        assert(pos < position);

        return (*values)[pos];
    }

    auto begin()
    {
        return values->begin();
    }

    auto end()
    {
        return values->begin() +  position;
    }

    const auto begin() const
    {
        return values->begin();
    }

    const auto end() const
    {
        return values->begin() + position;
    }
};

#endif