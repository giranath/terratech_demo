#ifndef MMAP_DEMO_CAMERA_HPP
#define MMAP_DEMO_CAMERA_HPP

#include <glm/glm.hpp>

class camera {
    glm::vec3 pos;
    glm::mat4 projection;

public:
    /// \brief Reset the camera's position
    /// \param pos The new position
    void reset(glm::vec3 pos) noexcept;

    /// \brief Returns the view matrix
    /// \return The view matrix
    glm::mat4 matrix() const noexcept;
};


#endif //MMAP_DEMO_CAMERA_HPP
