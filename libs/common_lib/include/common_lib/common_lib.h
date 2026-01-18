#ifndef CHATPP_COMMON_LIB_H
#define CHATPP_COMMON_LIB_H

#include <string>
#include <json_struct/json_struct.h>

namespace common_lib {


    struct message {
        std::string content;
        std::string author;
        std::string room_code;
        int message_code; // 200
        std::string type; // communication, server_action, error, server_response

        JS_OBJ(content, author, room_code, message_code, type);
    };

}

#endif // CHATPP_COMMON_LIB_H
