//
// Created by matheus on 1/27/26.
//

#include "ui.h"

#include <cstring>
#include <stdlib.h>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include "ftxui/component/component_options.hpp"
#include <ftxui/component/screen_interactive.hpp>

#include "common_lib/common_lib.h"
#include "ftxui/component/captured_mouse.hpp"

using namespace ftxui;

ChatUI::ChatUI(std::string prefsPath) : pref(prefsPath) {

    username = pref.GetString("username");
    ipAddress = pref.GetString("ip_address");

}

void ChatUI::push_message(const std::string &msg) {
    JS::ParseContext context(msg);
    common_lib::message obj;
    context.parseTo(obj);

    std::string formattedMsg;

    if (obj.message_code == 201) {
        if (obj.content == "Connected") {
            formattedMsg = "SERVER MESSAGE: " + obj.author + " joined the room!";
        } else if (obj.content == "Disconnected") {
            formattedMsg = "SERVER MESSAGE: " + obj.author + " left the room.";
        } else {
            formattedMsg = "SERVER MESSAGE: " + obj.content;
        }
    } else {
        formattedMsg = obj.author + " -> " + obj.content;
    }

    messages.insert(messages.begin(), formattedMsg);
}

void ChatUI::PrepareConnection(std::string &tempPort, ScreenInteractive &screen) {
    if (username.empty()) {
        return;
    }

    if (room.empty()) {
        srand(time(0));
        int roomCode = rand() % 1000;
        room = std::to_string(roomCode);
    }

    if (ipAddress.empty()) {
        ipAddress = "127.0.0.1";
    }

    if (tempPort.empty()) {
        port = 8080;
    } else {
        try {
            unsigned long value = std::stoul(tempPort);
            if (value <= 65535) {
                port = static_cast<uint16_t>(value);
            } else {
                return;
            }
        } catch (const std::exception &e) {
            return;
        }
    }

    pref.SetString("username", username);
    pref.SetString("ip_address", ipAddress);

    if (on_join) {
        on_join(username, room, ipAddress, port);
    }

    state = UIState::Chat;

    // Restart UI to switch tabs
    screen.ExitLoopClosure()();
    run();
}

void ChatUI::Quit(ScreenInteractive &screen) {
    if (on_quit) {
        on_quit(screen);
    }
}

void ChatUI::Disconnect(ftxui::ScreenInteractive &screen) {
    if (on_disconnect) {
        on_disconnect();
    }

    messages.clear();

    room = "";
    port = NULL;

    state = UIState::Join;
    screen.ExitLoopClosure()();
    run();
}

void ChatUI::Send(std::string &msg) {
    chat_input = "";

    if (on_send) {
        on_send(msg);
    }
}

void ChatUI::run() {
    ScreenInteractive screen = ScreenInteractive::Fullscreen();

    //uiScreen = &screen;

    std::string tempPort;
    std::string msgInp;

    Component input_message = Input(&msgInp, "Type your message here...");
    Component input_username = Input(&username, "name");
    Component input_room_code = Input(&room, "leave empty for random");
    Component input_address = Input(&ipAddress, "leave empty for default");

    Component input_port = Input(&tempPort, "leave empty for default");
    input_port |= CatchEvent([&](Event event) {
        return event.is_character() && !std::isdigit(event.character()[0]);
    });

    auto spacer = Renderer([] { return text("") | flex; });

    auto message_area = Renderer([&] {
        std::string all_messages;
        for (const auto &msg: messages) {
            all_messages += msg + "\n";
        }

        return paragraph(all_messages);
    });

    Component container = state == UIState::Join
                              ?

                              // Connect Page
                              Container::Vertical({

                                  // Header
                                  Container::Horizontal({
                                      spacer | flex,
                                      Button("Quit", [&] { Quit(screen); }, ButtonOption::Ascii())
                                  }) | border,

                                  // Content
                                  Container::Vertical({
                                      Container::Horizontal({
                                          spacer | flex,
                                      }) | flex,
                                      Container::Horizontal({
                                          spacer | flex,
                                          Container::Vertical({

                                              // Title
                                              Container::Horizontal({
                                                  spacer | flex,
                                                  Renderer([] { return text("Chat++ by Matheus"); }),
                                                  spacer | flex,
                                              }),

                                              Renderer([] { return separator(); }),

                                              spacer | size(HEIGHT, EQUAL, Dimension::Full().dimy * 0.025f),

                                              // Input Fields
                                              Container::Horizontal({
                                                  Renderer([] { return text("Username  : "); }),
                                                  input_username,
                                              }),
                                              Container::Horizontal({
                                                  Renderer([] { return text("Room Code : "); }),
                                                  input_room_code,
                                              }),
                                              Container::Horizontal({
                                                  Renderer([] { return text("Address   : "); }),
                                                  input_address,
                                              }),
                                              Container::Horizontal({
                                                  Renderer([] { return text("Port      : "); }),
                                                  input_port,
                                              }),

                                              spacer | size(HEIGHT, EQUAL, Dimension::Full().dimy * 0.025f),

                                              Renderer([] { return separatorCharacter("*"); }),

                                              spacer | size(HEIGHT, EQUAL, Dimension::Full().dimy * 0.025f),


                                              // Connect Button
                                              Container::Horizontal({
                                                  spacer | flex,
                                                  Button("Connect", [&] { PrepareConnection(tempPort, screen); },
                                                         ButtonOption::Ascii()),
                                                  spacer | flex,
                                              }),

                                          }) | size(WIDTH, EQUAL, Dimension::Full().dimx * 0.36f) | border,
                                          spacer | flex,
                                      }) | xflex,
                                      Container::Horizontal({
                                          spacer | flex,
                                      }) | flex,

                                  }) | border | flex,

                                  // Footer
                                  Container::Horizontal({
                                      spacer | flex,
                                      Renderer([] {
                                          return text("Made by Matheus Boscariol | Licensed under GNU AGPL 3.0");
                                      }),
                                      spacer | flex,

                                  }) | border,
                              }) | flex

                              :

                              // Chat Page
                              Container::Vertical({

                                  // Header
                                  Container::Horizontal({
                                      spacer | flex,
                                      Renderer([&] { return text(ipAddress + ":" + std::to_string(port)); }),
                                      spacer | flex,
                                      Renderer([&] { return text("Username: " + username); }),
                                      spacer | flex,
                                      Renderer([&] { return text("Room: " + room); }),
                                      spacer | flex,
                                      Button("Disconnect", [&] { Disconnect(screen); }, ButtonOption::Ascii()),
                                      Button("Quit", [&] { Quit(screen); }, ButtonOption::Ascii()),
                                  }) | border,

                                  // Content
                                  Container::Vertical({
                                      Container::Horizontal({
                                          Container::Vertical({

                                              // Title
                                              Container::Horizontal({
                                                  spacer | flex,
                                                  Renderer([] { return text("Chat++ by Matheus"); }),
                                                  spacer | flex,
                                              }),

                                              Renderer([] { return separator(); }),

                                              spacer | size(HEIGHT, EQUAL, Dimension::Full().dimy * 0.025f),

                                              message_area | flex,

                                              spacer | size(HEIGHT, EQUAL, Dimension::Full().dimy * 0.025f),

                                          }) | border | flex,
                                      }) | flex,

                                      // Input Field
                                      Container::Horizontal({
                                          Renderer([] { return text("Message : "); }),
                                          input_message,
                                          Button("Send Message", [&] {
                                              Send(msgInp);
                                              msgInp = "";
                                          }, ButtonOption::Ascii()),
                                      }) | border | xflex,

                                  }) | border | flex,

                                  // Footer
                                  Container::Horizontal({
                                      spacer | flex,
                                      Renderer([] {
                                          return text("Made by Matheus Boscariol | Licensed under GNU AGPL 3.0");
                                      }),
                                      spacer | flex,

                                  }) | border,
                              }) | flex;


    screen.Loop(container);

    //uiScreen = nullptr;
}
