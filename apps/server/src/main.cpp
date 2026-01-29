//
// Created by matheus on 1/15/26.
//

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <thread>
#include <vector>
#include <common_lib/common_lib.h>
#include <json_struct/json_struct.h>


using namespace std;

struct client {
    string clientName;
    int clientSocket;
    string room;
    thread clientThread;
};

void ClientActions(int clientSocket, client &this_client, vector<client> &clients) {

    // START RECEIVE MESSAGES

    while (true) {
        // receiving data
        char buffer[1024] = { 0 };
        ssize_t bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytes == 0) {
            return;
        }
        if (bytes < 0) {
            return;
        }

        JS::ParseContext context(buffer);
        common_lib::message obj;
        context.parseTo(obj);

        if (obj.type == "communication") {
            cout << "ROOM " << obj.room_code << ": " << "COMMUNICATION MESSAGE: " << obj.author << ": " << obj.content
                  << endl;
        }else if (obj.type == "server_action") {
            cout << "ROOM " << obj.room_code << ": " << "SERVER ACTION: " << obj.author << ": " << obj.content
                  << endl;
        } else if (obj.type == "error") {
            cout << "ROOM " << obj.room_code << ": " << "ERROR: " << ": " << obj.author << ": " << obj.content
                  << endl;
        } else {
            cout << "ROOM " << obj.room_code << ": " << "OTHER MESSAGE: " << ": " << obj.author << ": " << obj.content
                  << endl;
        }

        this_client.room = obj.room_code;

        common_lib::message returnmsg = common_lib::message(obj.content, obj.author, obj.room_code, (obj.type == "server_action") ? 201 : 200, "server_response");

        string json_chat_msg = JS::serializeStruct(returnmsg);

        // sending data
        const char* json_returnmsg = json_chat_msg.c_str();

        for (int i = 0; i < clients.size(); i++) {

            if (obj.content == "Disconnected" && clients[i].clientSocket == clientSocket) continue;

            if (clients[i].room != this_client.room) continue;

            send(clients[i].clientSocket, json_returnmsg, strlen(json_returnmsg), 0);
        }

    }

    // END RECEIVE MESSAGES
}

void TerminateProgram(vector<client> &clients, int &serverSocket) {
    for (client &c : clients) {
        c.clientThread.join();
    }

    close(serverSocket);
}

int main()
{
    // creating socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying the address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // binding socket.
    bind(serverSocket, (struct sockaddr*)&serverAddress,
         sizeof(serverAddress));

    // listening to the assigned socket
    listen(serverSocket, 5);

    vector<client> clients;

    try {

        auto throwFunction = [](int e) {
            throw e;
        };

        signal(SIGINT, throwFunction);

        while (true) {
            // accepting connection request
            int clientSocket
                = accept(serverSocket, nullptr, nullptr);

            clients.emplace_back("client", clientSocket);
            clients.back().clientThread = std::thread(ClientActions, clientSocket, std::ref(clients.back()), std::ref(clients));
        }

    } catch (int e) {
        // closing the socket.

        TerminateProgram(clients, serverSocket);
        return e;
    }

    TerminateProgram(clients, serverSocket);
    return 0;
}

