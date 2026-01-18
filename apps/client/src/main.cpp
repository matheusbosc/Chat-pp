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
#include <thread>
#include <common_lib/common_lib.h>
#include <json_struct/json_struct.h>


using namespace std;

void listener(int clientSocket) {

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

        JS::ParseContext context(buffer);
        common_lib::message obj;
        context.parseTo(obj);

        cout << obj.content << obj.type << endl;
    }

}


int main()
{

        string username;
    string room;
    cout << "Please enter your username [text]: ";
    getline(std::cin, username);

    cout << "What room would you like to use [number]: ";
    getline(std::cin, room);


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

    common_lib::message joinmsg = common_lib::message("Connected", username, room, 200, "server_action");

    string json_msg = JS::serializeStruct(joinmsg);

    const char* msg = json_msg.c_str();
    send(clientSocket, msg, strlen(msg), 0);

    std::thread message_listener = std::thread(listener, clientSocket);


    try {

        auto throwFunction = [](int e) {
            throw e;
        };

        signal(SIGINT, throwFunction);

        while(true) {

            string input;
            cout << "enter input: ";
            if (!getline(std::cin, input)) break;

            common_lib::message chatmsg = common_lib::message(input, username, room, 200, "communication");

            string json_chat_msg = JS::serializeStruct(chatmsg);

            // sending data
            const char* message = json_chat_msg.c_str();
            send(clientSocket, message, strlen(message), 0);
        }

    } catch (int e) {
        // closing the socket.

        message_listener.join();

        common_lib::message leave_msg = common_lib::message("Disconnected", username, room, 200, "server_action");

        string json_leave_msg = JS::serializeStruct(leave_msg);

        // sending data
        const char* message = json_leave_msg.c_str();
        send(clientSocket, message, strlen(message), 0);


        close(clientSocket);

        return 0;

    }

        message_listener.join();

    common_lib::message leave_msg = common_lib::message("Disconnected", username, room, 200, "server_action");

    string json_leave_msg = JS::serializeStruct(leave_msg);

    // sending data
    const char* message = json_leave_msg.c_str();
    send(clientSocket, message, strlen(message), 0);



    close(clientSocket);



    return 0;
}