//
// Created by matheus on 1/27/26.
//

#include <iostream>
#include <string>

#include "client.h"
#include "ui.h"
#include "json_struct/json_struct.h"

int main() {

    std::string windows_settings_path = "%APPDATA%/ChatPlusPlus/settings.json";
    std::string linux_settings_path = "~/.config/YourApp/settings.json";
    std::string macos_settings_path = "~/Library/Application Support/YourApp/settings.json";

    ChatUI ui("settings.json");
    Client client;

    // Set Join Callback: Init, connect, and start listener
    ui.set_join_callback([&](const std::string& username, const std::string& room, const std::string& ip, const uint16_t& port) {
        client.Init(username, room);
        client.connect_client(ip, port);
        client.start_listener();
    });

    // Set Send Callback: Send message
    ui.set_send_callback([&](const std::string& msg) {
        client.send_msg(msg);
    });

    // Set receive callback: Push message to ui
    client.set_message_callback([&](const std::string& msg) {
        ui.push_message(msg);
    });

    // Set quit callback: stop listener, Disconnect, stop UI
    ui.set_quit_callback([&](ftxui::ScreenInteractive& screen) {
        std::thread([&] {
            client.disconnect_client();
        }).detach();

        screen.ExitLoopClosure()();
    });

    // Set disconnect callback: stop listener, Disconnect
    ui.set_disconnect_callback([&]() {
        std::thread([&] {
            client.disconnect_client();
        }).detach();
    });

    ui.run();

}
