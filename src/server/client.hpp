#ifndef MMAP_DEMO_CLIENT_HPP
#define MMAP_DEMO_CLIENT_HPP

#include "../common/networking/network_manager.hpp"
#include "../common/util/vec_hash.hpp"
#include "../common/world/visibility_map.hpp"

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <glm/glm.hpp>

struct client {
    networking::network_manager::socket_handle socket;
    uint8_t id;

    // The units this player knows about
    std::unordered_set<uint32_t> known_units;

    // Holds this player visibility
    visibility_map map_visibility;

    client(networking::network_manager::socket_handle socket, uint8_t id);

	client() = delete;
    bool operator==(const client& other) const noexcept;
};

#endif //MMAP_DEMO_CLIENT_HPP
