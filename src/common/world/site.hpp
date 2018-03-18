#ifndef MMAP_DEMO_SITE_HPP
#define MMAP_DEMO_SITE_HPP

#include <cstdint>

class site {
public:
    using quantity = uint32_t;
    using id = int;
private:
    id identifier;
    quantity current_amount;
public:
    site(id type, quantity qty);

    site::id type() const noexcept;
    quantity amount() const noexcept;
    bool is_depleted() const noexcept;

    void collect(quantity qty_to_collect) noexcept;
};


#endif //MMAP_DEMO_SITE_HPP
