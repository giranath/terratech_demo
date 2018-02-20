#include "poll_event_iterator.hpp"

namespace sdl {

poll_event_iterator::poll_event_iterator()
: event()
, is_last(true) {
}

poll_event_iterator::poll_event_iterator(SDL_Event event)
: event(event)
, is_last(false) {

}

poll_event_iterator poll_event_iterator::operator++(int) noexcept {
    auto temp = *this;

    (*this)++;

    return temp;
}

poll_event_iterator& poll_event_iterator::operator++() noexcept {
    is_last = SDL_PollEvent(&event) == 0;

    return *this;
}

bool poll_event_iterator::operator==(const poll_event_iterator& other) const noexcept {
    return is_last == other.is_last;
}

bool poll_event_iterator::operator!=(const poll_event_iterator& other) const noexcept {
    return is_last != other.is_last;
}

poll_event_iterator::reference poll_event_iterator::operator*() const noexcept {
    return event;
}

poll_event_iterator::pointer poll_event_iterator::operator->() const noexcept {
    return &event;
}

}