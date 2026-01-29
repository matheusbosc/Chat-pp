//
// Created by matheus on 1/27/26.
//

#ifndef CHATPP_UI_H
#define CHATPP_UI_H

#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <string>
#include <functional>

#include "common_lib/common_lib.h"

enum class UIState { Join, Chat };

class ChatUI {
public:
    using JoinCallback = std::function<void(const std::string&, const std::string&, const std::string&, const uint16_t&)>;
    using QuitCallback = std::function<void(ftxui::ScreenInteractive&)>;
    using SendCallback = std::function<void(const std::string&)>;
    using DisconnectCallback = std::function<void()>;

    void set_send_callback(SendCallback cb) { on_send = cb; }
    void set_disconnect_callback(DisconnectCallback cb) { on_disconnect = cb; }
    void set_join_callback(JoinCallback cb) { on_join = cb; }
    void set_quit_callback(QuitCallback cb) { on_quit = cb; }

    void push_message(const std::string& msg);

    void run();

    ChatUI(std::string prefsPath);

private:
    UIState state = UIState::Join;

    std::string username;
    std::string room;
    std::string ipAddress;
    uint16_t port;

    JoinCallback on_join;
    QuitCallback on_quit;
    SendCallback on_send;
    DisconnectCallback on_disconnect;

    // Chat state
    std::vector<std::string> messages;
    std::string chat_input;

    ftxui::ScreenInteractive* uiScreen;

    void PrepareConnection(std::string& tempPort, ftxui::ScreenInteractive& screen);
    void Quit(ftxui::ScreenInteractive& screen);
    void Disconnect(ftxui::ScreenInteractive& screen);
    void Send(std::string& msg);

    common_lib::Preferences pref;


};


#endif //CHATPP_UI_H