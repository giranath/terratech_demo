#include "../common/networking/tcp_socket.hpp"

#include <iostream>
#include <vector>

int main(int argc, const char** argv) {
    SDLNet_Init();

    IPaddress address;

    if(SDLNet_ResolveHost(&address, nullptr, 1647) == -1) {
        std::cerr << "cannot resolve address: " << SDLNet_GetError() << std::endl;
        SDLNet_Quit();
        return 1;
    }

    SDLNet_SocketSet all_sockets = SDLNet_AllocSocketSet(12);
    TCPsocket server_socket = SDLNet_TCP_Open(&address);

    SDLNet_TCP_AddSocket(all_sockets, server_socket);

    std::cout << "waiting for connection..." << std::endl;
    TCPsocket connected_client = nullptr;

    std::vector<networking::tcp_socket> client_sockets;

    bool is_running = true;
    while(is_running) {
        int num_ready = SDLNet_CheckSockets(all_sockets, 1000);
        if(num_ready > 0) {
            if(SDLNet_SocketReady(server_socket)) {
                std::cout << "new connection" << std::endl;
                TCPsocket connected_client = SDLNet_TCP_Accept(server_socket);
                SDLNet_TCP_AddSocket(all_sockets, connected_client);
                client_sockets.emplace_back(connected_client);
            }
            else {
                std::for_each(std::begin(client_sockets), std::end(client_sockets), [&](networking::tcp_socket& socket) {
                    TCPsocket s = static_cast<TCPsocket>(socket);
                    if(SDLNet_SocketReady(s)) {
                        char RECV_BUFFER[7];

                        socket.receive(reinterpret_cast<uint8_t*>(RECV_BUFFER), 7);
                        std::cout << "received: " << RECV_BUFFER << std::endl;

                        std::string MSG = "THANK!";
                        socket.send(reinterpret_cast<const uint8_t*>(MSG.c_str()), 7);

                        is_running = false;
                        //SDLNet_TCP_DelSocket(all_sockets, socket);
                        //SDLNet_TCP_Close(socket);
                    }
                });
            }
        }
    }

    // Close le serveur
    SDLNet_TCP_Close(server_socket);
    SDLNet_FreeSocketSet(all_sockets);

    SDLNet_Quit();
}