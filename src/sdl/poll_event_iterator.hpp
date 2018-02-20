#ifndef MMAP_DEMO_POLL_EVENT_ITERATOR_HPP
#define MMAP_DEMO_POLL_EVENT_ITERATOR_HPP

#include <SDL.h>
#include <iterator>

namespace sdl {

class poll_event_iterator {
    SDL_Event event;
    bool is_last;
public:
    using difference_type = void;
    using value_type = SDL_Event;
    using pointer = const value_type*;
    using reference = const value_type&;
    using iterator_category = std::input_iterator_tag;

    poll_event_iterator();
    explicit poll_event_iterator(SDL_Event event);

    poll_event_iterator operator++(int) noexcept;
    poll_event_iterator& operator++() noexcept;

    bool operator==(const poll_event_iterator& other) const noexcept;
    bool operator!=(const poll_event_iterator& other) const noexcept;

    reference operator*() const noexcept;
    pointer operator->() const noexcept;
};

}

#endif
