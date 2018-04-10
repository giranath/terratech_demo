#include "client.hpp"
#include "../common/world/world.hpp"

client::client(networking::network_manager::socket_handle socket, uint8_t id)
: socket(socket)
, id(id)
, map_visibility(world::CHUNK_WIDTH * 20, world::CHUNK_DEPTH * 20) {

}

bool client::operator==(const client& other) const noexcept {
    return id == other.id;
}