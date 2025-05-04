#include <SFML/Network.hpp>
#include <iostream>
#include <array>
#include <thread>


bool setHosting() {
    char number;
    while (true) {
        std::cout << "Press 1 to host or 0 to join and press enter: ";
        std::cin >> number;

        if (number == '1') return true;
        if (number == '0') return false;

        std::cout << "error: not a valid entry\n";
    }
}


int main() {
    bool hosting = setHosting(); 
    const unsigned short port = 53000;
    const sf::IpAddress serverIp(192,168,0,145);
    
     if (hosting) {
        // --- Server Code ---
        sf::TcpListener listener;
        if (listener.listen(port) != sf::Socket::Status::Done) {
            std::cerr << "Failed to bind listener\n";
            return 1;
        }
        listener.setBlocking(false);
        std::cout << "Server listening on port " << port << "...\n";

        sf::TcpSocket client;
        client.setBlocking(false);
        bool connected = false;

        while (!connected) {
            if (listener.accept(client) == sf::Socket::Status::Done) {
                connected = true;
                std::cout << "Client connected!\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
	    std::cout << "waiting on client\n";
        }

        std::string msg = "hello";
        if (client.send(msg.c_str(), msg.size()) != sf::Socket::Status::Done) {
            std::cerr << "Failed to send message\n";
        } else {
            std::cout << "Sent: " << msg << "\n";
        }

    } else {
        // --- Client Code ---
        sf::TcpSocket socket;
        socket.setBlocking(false);

        sf::Socket::Status status = socket.connect(serverIp, port);
        if (status != sf::Socket::Status::Done) {
            std::cout << "Waiting for connection...\n";
        }

        // Try connecting until successful
        while (socket.getRemoteAddress() != serverIp) {
            socket.connect(serverIp, port);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::array<char, 100> buffer;
        std::size_t received;
        while (true) {
            sf::Socket::Status result = socket.receive(buffer.data(), buffer.size(), received);
            if (result == sf::Socket::Status::Done) {
                std::string msg(buffer.data(), received);
                std::cout << "Received: " << msg << "\n";
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    return 0;
}

