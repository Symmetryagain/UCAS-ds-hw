#include "hash.h"
#include "debug.h"
#include <string>
#include <unordered_map>

extern int debug_on;
static std::unordered_map<std::string, int> city_name_map;
static std::unordered_map<std::string, int> station_name_map;
// static int city_id_counter = 1; // Start from 1 to avoid 0 as a valid ID
// static int station_id_counter = 1; // Start from 1 to avoid 0 as a valid ID

extern "C" {
    int insert_city_name(char* c_str, int c_val) {
        std::string city_name(c_str);
        if (city_name_map.find(city_name) != city_name_map.end()) {
            debug_printf("[hash.cpp][insert_city_name][INFO]: City '%s' already exists\n", city_name.c_str());
            return 0; // City already exists
        }
        auto it = city_name_map.insert({city_name, c_val});
        if (it.second) { // If insertion was successful
            debug_printf("[hash.cpp][insert_city_name][INFO]: Inserted city '%s' with ID %d\n", city_name.c_str(), it.first->second);
            return it.first->second; // Return the ID of the new city
        }
        debug_printf("[hash.cpp][insert_city_name][Error]: Failed to insert city '%s'\n", city_name.c_str());
        return 0; // Insertion failed
    }

    int insert_station_name(char* s_str, int s_val) {
        std::string station_name(s_str);
        if (station_name_map.find(station_name) != station_name_map.end()) {
            debug_printf("[hash.cpp][insert_station_name][INFO]: Station '%s' already exists\n", station_name.c_str());
            return 0; // Station already exists
        }
        auto it = station_name_map.insert({station_name, s_val});
        if (it.second) { // If insertion was successful
            debug_printf("[hash.cpp][insert_station_name][INFO]: Inserted station '%s' with ID %d\n", station_name.c_str(), it.first->second);
            return it.first->second; // Return the ID of the new station
        }
        debug_printf("[hash.cpp][insert_station_name][Error]: Failed to insert station '%s'\n", station_name.c_str());
        return 0; // Insertion failed
    }

    int find_city_name(char* c_str) {
        std::string city_name(c_str);
        auto it = city_name_map.find(city_name);
        if (it != city_name_map.end()) {
            debug_printf("[hash.cpp][find_city_name][INFO]: Found city '%s' with ID %d\n", city_name.c_str(), it->second);
            return it->second; // Return the ID of the city
        }
        debug_printf("[hash.cpp][find_city_name][INFO]: City '%s' not found\n", city_name.c_str());
        return 0; // City not found
    }

    int find_station_name(char* s_str) {
        std::string station_name(s_str);
        auto it = station_name_map.find(station_name);
        if (it != station_name_map.end()) {
            debug_printf("[hash.cpp][find_station_name][INFO]: Found station '%s' with ID %d\n", station_name.c_str(), it->second);
            return it->second; // Return the ID of the station
        }
        debug_printf("[hash.cpp][find_station_name][INFO]: Station '%s' not found\n", station_name.c_str());
        return 0; // Station not found
    }
}
