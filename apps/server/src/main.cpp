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
    thread clientThread;
    string clientName;
    int clientSocket;
};

void ClientActions(int clientSocket) {
    while (true) {
        // recieving data
        char buffer[1024] = { 0 };
        ssize_t bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytes == 0) {
            return;
        }
        if (bytes < 0) {
            return;
        }

        cout << "Message from client: " << buffer
                  << endl;

    }
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

            clients.emplace_back(std::thread(ClientActions, clientSocket), "client", clientSocket);
        }

    } catch (int e) {
        // closing the socket.

        TerminateProgram(clients, serverSocket);
        return e;
    }

    TerminateProgram(clients, serverSocket);
    return 0;
}

