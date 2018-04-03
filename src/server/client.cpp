#include "client.hpp"

uint8_t client::next_id = 0;

client::client(networking::network_manager::socket_handle socket)
: socket(socket)
, id(++next_id) {

}

bool client::operator==(const client& other) const noexcept {
    return id == other.id;
}