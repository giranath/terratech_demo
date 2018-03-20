#include "../common/networking/tcp_socket.hpp"
#include "../common/networking/socket_set.hpp"

#include <iostream>
#include <vector>

int main(int argc, const char** argv) {
    SDLNet_Init();

    networking::tcp_listener listener;
    listener.try_bind(1647);

    networking::socket_set all_sockets(12);
    all_sockets.add(listener);

    std::vector<networking::tcp_socket> client_sockets;

    bool is_running = true;
    while(is_running) {
        int num_ready = all_sockets.check(std::chrono::seconds(1));
        if(num_ready > 0) {
            if(all_sockets.is_ready(listener)) {
                std::cout << "new connection" << std::endl;
                networking::tcp_socket connected_client = listener.accept();
                all_sockets.add(connected_client);
                client_sockets.emplace_back(std::move(connected_client));
            }
            else {
                std::for_each(std::begin(client_sockets), std::end(client_sockets), [&](networking::tcp_socket& socket) {
                    if(all_sockets.is_ready(socket)) {
                        char RECV_BUFFER[7];

                        socket.receive(reinterpret_cast<uint8_t*>(RECV_BUFFER), 7);
                        std::cout << "received: " << RECV_BUFFER << std::endl;

                        std::string MSG = "THANK!";
                        socket.send(reinterpret_cast<const uint8_t*>(MSG.c_str()), 7);

                        is_running = false;
                        all_sockets.remove(socket);
                    }
                });
            }
        }
    }

    SDLNet_Quit();
}