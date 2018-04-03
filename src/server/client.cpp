#include "client.hpp"
#include "../common/world/world.hpp"

uint8_t client::next_id = 0;

client::client(networking::network_manager::socket_handle socket)
: socket(socket)
, id(++next_id)
, map_visibility(world::CHUNK_WIDTH * 20, world::CHUNK_DEPTH * 20) {

}

bool client::operator==(const client& other) const noexcept {
    return id == other.id;
}