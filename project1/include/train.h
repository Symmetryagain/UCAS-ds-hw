#ifndef _TRAIN_H
#define _TRAIN_H

#include "graph.h"

typedef struct Interval {
    int train_id;
    int st_station_id;
    int ed_station_id;
    int cost;
    TimeType depart_time;
    TimeType arrive_time;
    int extra_day;
    struct Interval *ne;
} Interval;

/**
 * @param start_city start city, if not a city, value = -1
 * @param start_sta start station, if not a station, value = -1
 * @param end_city end city, if not a city, value = -1
 * @param end_sta end station, if not a station, value = -1
 * @param prior 0 time first, 1 cost first
 * @param depart_time depart time, HH:MM
 * @param ac_change whether accpet change train, at most once
 * @param change_minute least time between two part
 * @param result pointer of the pointer of the first result
 * @retval >0 number of result intervals
 * @retval =0 no result
 * @retval <0 fail
 */
int query_train(int start_city, int start_sta, 
    int end_city, int end_sta, int prior,
    TimeType depart_time, int ac_change, int change_minute, Interval **result);

void load_trains_from_txt(const char *filepath); 

#endif
