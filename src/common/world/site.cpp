#include "site.hpp"

site::site(id type, quantity qty)
: identifier{type}
, current_amount{qty} {

}

site::id site::type() const noexcept {
    return identifier;
}

site::quantity site::amount() const noexcept {
    return current_amount;
}

bool site::is_depleted() const noexcept {
    return current_amount == 0;
}

void site::collect(quantity qty_to_collect) noexcept {
    if(qty_to_collect < current_amount) {
        current_amount -= qty_to_collect;
    }
    else {
        current_amount = 0;
    }
}