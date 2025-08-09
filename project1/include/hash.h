#ifndef _HASH_H
#define _HASH_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Insert a city name into the hash map.
 * @param c_str The city name as a C-style string.
 * @param c_val The value of this city name, namely its index in array.
 * @retval >0 ID of the inserted city.
 * @retval 0 if the city already exists or insertion failed.
 */
int insert_city_name(char* c_str, int c_val);

/**
 * @brief Insert a station name into the hash map.
 * @param s_str The station name as a C-style string.
 * @param s_val The value of this station name, namely its index in array.
 * @retval >0 ID of the inserted station.
 * @retval 0 if the station already exists or insertion failed.
 */
int insert_station_name(char* s_str, int s_val);

/**
 * @brief Find a city name in the hash map.
 * @param c_str The city name as a C-style string.
 * @retval >0 ID of the found city.
 * @retval 0 if the city is not found.
 */
int find_city_name(char* c_str);

/**
 * @brief Find a station name in the hash map.
 * @param s_str The station name as a C-style string.
 * @retval >0 ID of the found station.
 * @retval 0 if the station is not found.
 */
int find_station_name(char* s_str);

#ifdef __cplusplus
}
#endif

#endif
