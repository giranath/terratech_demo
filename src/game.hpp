#ifndef MMAP_DEMO_GAME_HPP
#define MMAP_DEMO_GAME_HPP

#include <chrono>

class game {
public:
    using clock = std::chrono::high_resolution_clock;
    using frame_duration = clock::duration;

    /**
     * Updates the game state
     * @param last_frame_duration As the name implies, it's the duration of the last processed frame
     */
    void update(frame_duration last_frame_duration);

    /**
     * Render the current state on screen
     */
    void render();
};


#endif //MMAP_DEMO_GAME_HPP
