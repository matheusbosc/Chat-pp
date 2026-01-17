//
// Created by matheus on 1/15/26.
//

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <csignal>
#include <cstdlib>
#include <common_lib/common_lib.h>
#include <json_struct/json_struct.h>


using namespace std;

int main()
{
    string username;
    string room;
    cout << "Please enter your username [text]: ";
    getline(std::cin, username);

    cout << "What room would you like to use [number]: ";
    do {
        getline(std::cin, room);
    } while (all_of(room.begin(), room.end(), ::isdigit));



    // creating socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // sending connection request
    connect(clientSocket, (struct sockaddr*)&serverAddress,
            sizeof(serverAddress));

    common_lib::message joinmsg = common_lib::message("Joined Server", username, stoi(room), 200, "server_action");

    string json_msg = JS::serializeStruct(joinmsg);

    const char* msg = json_msg.c_str();
    send(clientSocket, msg, strlen(msg), 0);


    try {

        auto throwFunction = [](int e) {
            throw e;
        };

        signal(SIGINT, throwFunction);

        while(true) {

            string input;
            cout << "enter input: ";
            if (!getline(std::cin, input)) break;

            common_lib::message joinmsg = common_lib::message(input, username, stoi(room), 200, "communication");

            string json_msg = JS::serializeStruct(joinmsg);

            // sending data
            const char* message = json_msg.c_str();
            send(clientSocket, message, strlen(message), 0);
        }

    } catch (int e) {
        // closing the socket.
        close(clientSocket);
        return 0;
    }

    // closing the socket.
    close(clientSocket);
    return 0;
}