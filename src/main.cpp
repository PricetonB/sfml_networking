#include <SFML/Network.hpp>
#include <iostream>
#include <array>
#include <thread>

int main() {
    bool hosting = true; // Set to false for client
    const unsigned short port = 53000;
    const sf::IpAddress serverIp = "127.0.0.1";

    if (hosting) {
        // --- Server Code ---
        sf::TcpListener listener;
        if (listener.listen(port) != sf::Socket::Done) {
            std::cerr << "Failed to bind listener\n";
            return 1;
        }
        listener.setBlocking(false);
        std::cout << "Server listening on port " << port << "...\n";

        sf::TcpSocket client;
        client.setBlocking(false);
        bool connected = false;

        while (!connected) {
            if (listener.accept(client) == sf::Socket::Done) {
                connected = true;
                std::cout << "Client connected!\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::string msg = "hello";
        if (client.send(msg.c_str(), msg.size()) != sf::Socket::Done) {
            std::cerr << "Failed to send message\n";
        } else {
            std::cout << "Sent: " << msg << "\n";
        }

    } else {
        // --- Client Code ---
        sf::TcpSocket socket;
        socket.setBlocking(false);

        sf::Socket::Status status = socket.connect(serverIp, port);
        if (status != sf::Socket::Done) {
            std::cout << "Waiting for connection...\n";
        }

        // Try connecting until successful
        while (socket.getRemoteAddress() == sf::IpAddress::None) {
            socket.connect(serverIp, port);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::array<char, 100> buffer;
        std::size_t received;
        while (true) {
            sf::Socket::Status result = socket.receive(buffer.data(), buffer.size(), received);
            if (result == sf::Socket::Done) {
                std::string msg(buffer.data(), received);
                std::cout << "Received: " << msg << "\n";
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    return 0;
}

