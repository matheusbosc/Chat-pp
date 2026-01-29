//
// Created by matheus on 1/15/26.
//

#include "client.h"

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <csignal>
#include <cstdlib>
#include <thread>
#include <common_lib/common_lib.h>
#include <json_struct/json_struct.h>
#include <arpa/inet.h>

void Client::Init(std::string _usr, std::string _room) {
    username = _usr;
    room = _room;

    // Create Join MSG
    common_lib::message joinmsg = common_lib::message("Connected", username, room, 200, "server_action");
    join_msg = JS::serializeStruct(joinmsg);

    // Create Leave MSG
    common_lib::message leavemsg = common_lib::message("Disconnected", username, room, 200, "server_action");
    leave_msg = JS::serializeStruct(leavemsg);
}

// connect_client(string, uint16_t) -- Connects client to the provided address
int Client::connect_client(const std::string &ipAddr, uint16_t port) {
    sockaddr_in _serverAddress;
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_port = htons(port);

    // Convert ip to binary IPv4
    if (inet_pton(AF_INET, ipAddr.c_str(), &_serverAddress.sin_addr) != 1) {
        return -1;
    }

    if (clientSocket >= 0) {
        close(clientSocket);  // close old socket
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0); // Defines the socket that the server connects to

    connect(clientSocket, (struct sockaddr *) &_serverAddress,
            sizeof(_serverAddress)); // Connects to the server

    send_raw(join_msg);

    return 0;
}


// disconnect_client() -- Disconnects the client from the current server
void Client::disconnect_client() {

    if (clientSocket > 0) {
        send_raw(leave_msg);

        std::cout << "Leave message sent" << std::endl;

        shutdown(clientSocket, SHUT_RDWR);

        close(clientSocket); // Close connection to server

        std::cout << "Socket Closed" << std::endl;
    }

    if (listenerCreated) {
        stop_listener();
    }

    std::cout << "Disconnected" << std::endl;
}

// send_msg(string, string) -- Sends a message to the server
// If you are using a preset message (ex. leave_msg, join_msg), you can set the preset_msg with that value
void Client::send_msg(const std::string &message) {
    try {
        // Prepare Message for server if preset message isn't used
        auto formattedMsg = common_lib::message(message, username, room, 200, "communication");
        std::string jsonMsg = JS::serializeStruct(formattedMsg);

        // Send Message
        const char *msg = jsonMsg.c_str();
        send(clientSocket, msg, strlen(msg), 0);
    } catch (std::exception &e) {
        std::cerr << "Server not running! Error: " << e.what() << std::endl;
    }
}

void Client::send_raw(const std::string &json) {
    try {
        const char *msg = json.c_str();
        send(clientSocket, msg, strlen(msg), 0);
    } catch (std::exception &e) {
        std::cerr << "Server not running! Error: " << e.what() << std::endl;
    }
}

void Client::start_listener() {
    running = true;
    pthread_create(&listenerThread, nullptr, &Client::listener_entry, this);
    listenerCreated = true;
}

void Client::set_message_callback(MessageCallback cb) {
    messageCallback = std::move(cb);
}

void Client::listener_loop() {
    try {
        while (running) {
            char buffer[1024];
            int bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

            if (bytes <= 0) {
                running = false;
                break;
            }

            buffer[bytes] = '\0';

            if (messageCallback) {
                messageCallback(buffer);
            }
        }
    } catch (...) {
        // Never let exceptions escape a thread
        running = false;
    }
}

void Client::stop_listener() {
    running = false;

    pthread_join(listenerThread, nullptr);

    listenerCreated = false;
}

Client::~Client() {
    //stop_listener();
}
