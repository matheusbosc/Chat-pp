#ifndef CHATPP_COMMON_LIB_H
#define CHATPP_COMMON_LIB_H

#include <fstream>
#include <string>
#include <unordered_map>
#include <json_struct/json_struct.h>
#include <nlohmann/json.hpp>

namespace common_lib {

    struct message {
        std::string content;
        std::string author;
        std::string room_code;
        int message_code; // 200
        std::string type; // communication, server_action, error, server_response

        JS_OBJ(content, author, room_code, message_code, type);
    };

    // ======================
    // PlayerPrefs-style class
    // ======================
    class Preferences {
    public:
        Preferences(const std::string& filename = "player_prefs.json")
            : path(filename)
        {
            std::ifstream in(path);
            if (in.is_open()) {
                try {
                    in >> data;
                } catch (...) {
                    data = {};
                }
            }
        }

        // Set values
        void SetInt(const std::string& key, int value) {
            data[key] = value;
            Save();
        }

        void SetString(const std::string& key, const std::string& value) {
            data[key] = value;
            Save();
        }

        // Get values
        int GetInt(const std::string& key, int default_value = 0) {
            if (data.contains(key)) return data[key].get<int>();
            return default_value;
        }

        std::string GetString(const std::string& key, const std::string& default_value = "") {
            if (data.contains(key)) return data[key].get<std::string>();
            return default_value;
        }

        // Remove key
        void Remove(const std::string& key) {
            data.erase(key);
            Save();
        }

        // Clear all prefs
        void Clear() {
            data.clear();
            Save();
        }

    private:
        void Save() {
            std::ofstream out(path);
            if (out.is_open()) {
                out << data.dump(4); // pretty-print
            }
        }

        nlohmann::json data;
        std::string path;
    };

}

#endif // CHATPP_COMMON_LIB_H
