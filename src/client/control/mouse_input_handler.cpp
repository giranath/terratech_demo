#include "mouse_input_handler.hpp"

#include <iterator>
#include <algorithm>

namespace input {

drag_event::drag_event(state s, glm::vec2 start, glm::vec2 end, glm::vec2 rel)
: current_state(s)
, start(start)
, current(end)
, rel(rel) {

}

click_event::click_event(int count, glm::vec2 pos)
: count(count)
, position(pos) {

}

base_mouse_drag_handler::base_mouse_drag_handler(drag_event_handler handler)
: is_dragging(false)
, start_pos()
, last_pos()
, handler(std::move(handler)) {

}

void base_mouse_drag_handler::on_moved(const glm::vec2& current, const glm::vec2& delta) {
    if(is_dragging) {
		last_pos = current;
    }
    else {
		start_pos = current;
		last_pos = current;
        is_dragging = true;
    }

	handler(drag_event(drag_event::state::starting, start_pos, last_pos, delta));
}

void base_mouse_drag_handler::on_idle()
{
	if (is_dragging){
		handler(drag_event(drag_event::state::idle, start_pos, last_pos, {0,0}));
	}
}

void base_mouse_drag_handler::on_released(const glm::vec2& current) {
    if(is_dragging) {
        handler(drag_event(drag_event::state::ending, start_pos, current, glm::vec2{}));
		is_dragging = false;
    }
}

mouse_input_handler::mouse_input_handler() {
    // Initial button state
    button_states[SDL_BUTTON_LEFT] = false;
    button_states[SDL_BUTTON_RIGHT] = false;
    button_states[SDL_BUTTON_MIDDLE] = false;
    button_states[SDL_BUTTON_X1] = false;
    button_states[SDL_BUTTON_X2] = false;
}

mouse_input_handler::mouse_input_handler(mouse_input_handler&& other) 
: click_handlers(std::move(other.click_handlers))
, button_states(std::move(other.button_states))
, drag_handlers(std::move(other.drag_handlers)) {
}

void mouse_input_handler::register_click(int button, click_event_handler handler) {
    click_handlers[button] = std::move(handler);
}

void mouse_input_handler::register_drag(int button, drag_event_handler handler) {
	drag_handlers.emplace(button, std::move(handler));
}

bool mouse_input_handler::handle(SDL_Event event) {
    bool handled = false;
    switch(event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            auto click_it = click_handlers.find(event.button.button);

            if(click_it != std::end(click_handlers)) {
                click_it->second(click_event(event.button.clicks, glm::vec2(event.button.x, event.button.y)));
                handled = true;
            }

			for (auto it = std::begin(drag_handlers); it != std::end(drag_handlers); ++it) {
				if (button_states[it->first]) {
					it->second.on_idle();
					handled = true;
				}
			}

            button_states[event.button.button] = true;
        } break;
        case SDL_MOUSEBUTTONUP: {
           
			auto draging_it = drag_handlers.find(event.button.button);
            if(draging_it != std::end(drag_handlers)) {
                draging_it->second.on_released(glm::vec2(event.button.x, event.button.y));
                handled = true;
            }

            button_states[event.button.button] = false;
        } break;
        case SDL_MOUSEWHEEL:
            // TODO: Handle mouse wheel
            break;
        case SDL_MOUSEMOTION: {
            //std::cout << "mouse moving" << std::endl;
            for(auto it = std::begin(drag_handlers); it != std::end(drag_handlers); ++it) {
                if(button_states[it->first]) {
                    it->second.on_moved(glm::vec2(event.motion.x, event.motion.y), glm::vec2(event.motion.xrel, event.motion.yrel));
                    handled = true;
                }
            }
			break;
		}   
    }

    return handled;
}

void mouse_input_handler::dispatch() {
	for (auto it = std::begin(drag_handlers); it != std::end(drag_handlers); ++it) {
		if (button_states[it->first]) {
			it->second.on_idle();
		}
	}
}

}