#ifndef _APP_H
#define _APP_H

/**
 * @brief Initialize the application: global variables, load train and flight data.
 * @return 1 on success, 0 on failure.
 */
int app_init();

/**
 * @brief Run the main user interaction loop in the console.
 *        Supports adding/deleting stations, trains, flights, and querying optimal routes.
 */
void app_run();

#endif
