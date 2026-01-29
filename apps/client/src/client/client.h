//
// Created by matheus on 1/27/26.
//

#ifndef CHATPP_CLIENT_H
#define CHATPP_CLIENT_H

#pragma once

#include <string>
#include <thread>
#include <functional>

using MessageCallback = std::function<void(const std::string&)>;

class Client {
public:
    void Init(std::string username, std::string room);

    int connect_client(const std::string& ip, uint16_t port);
    void disconnect_client();
    void send_msg(const std::string& message);

    void start_listener();
    void stop_listener();

    void set_message_callback(MessageCallback cb);
    /*
     *  Call this in the UI to set the callback
     *
     *  client.set_message_callback([](const std::string& msg){
     *      // code in here
     *  });
     *
     */

    ~Client();

private:
    // networking
    int clientSocket = -1;
    pthread_t listenerThread;

    // identity
    std::string username;
    std::string room;

    // preset messages
    std::string join_msg;
    std::string leave_msg;

    MessageCallback messageCallback;
    std::atomic<bool> running{false};
    bool listenerCreated{false};

    void listener_loop();
    static void* listener_entry(void* arg) {
        Client* self = static_cast<Client*>(arg);
        self->listener_loop();
        return nullptr;
    }


    // internal helpers
    void send_raw(const std::string& json);
};


#endif //CHATPP_CLIENT_H