#include "train.h"
#include "modify.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern Citys g_citys;
extern Stations g_stations;
extern Graph g_graph;
extern ValueType dist[MAX_NODE];
extern int prev[MAX_NODE];
extern int debug_on;

static inline int storeInterval(Interval *in, int start_node) {
    int now = start_node;
    in->train_id = g_graph.node[now].train_id;
    in->ed_station_id = g_graph.node[now].station_id;
    in->ne = NULL;
    in->arrive_time = g_graph.node[now].train_arrive_time;

    in->cost = dist[now].p_cost;

    int travel_time = dist[now].p_time;
    in->extra_day = travel_time / MIN_PER_DAY;
    while (prev[now] != -1 && 
        g_graph.node[now].train_id == g_graph.node[prev[now]].train_id
    ) { // trace back
        now = prev[now];
    }

    in->cost -= dist[now].p_cost;

    in->depart_time = g_graph.node[now].train_depart_time;
    in->st_station_id = g_graph.node[now].station_id;
    // in->arrive_time = getTime(&in->depart_time, travel_time);
    return now;
}

int query_train(
    int start_city, int start_sta, 
    int end_city, int end_sta, int prior, 
    TimeType depart_time, int ac_change, int change_minute, Interval **result
) {
    *result = NULL;
    if (start_sta == -1) { // city
        debug_printf("[train.c][query_train][INFO]: Querying from city %d\n", start_city);
        if (start_city >= g_citys.city_count) {
            debug_printf("[train.c][query_train][Error]: Invalid start city %d\n", start_city);
            return -1;
        }
        City *now = &g_citys.citys[start_city];
        int len = now->st_count;
        int *arr = (int *)malloc(len * sizeof(int));
        if (arr == NULL) {
            debug_printf("[train.c][query_train][INFO]: Fail to allocate memory for arr");
            return -1;
        }
        for (int i = 0; i < len; ++i) {
            arr[i] = now->st_ids[i];
        }
        if (dijkstra(&g_graph, len, arr, prior, &depart_time, change_minute) < 0) {
            debug_printf("[train.c][query_train][Error]: Dijkstra failed for city %d\n", start_city);
            free(arr);
            return -1;
        }
        debug_printf("[train.c][query_train][INFO]: Dijkstra completed for city %d\n", start_city);
        debug_printf("[train.c][query_train][INFO]: Start station IDs (total = %d): ", len);
        for (int i = 0; i < len; ++i) {
            debug_printf("%d ", arr[i]);
        }
        debug_printf("\n");
        free(arr);
    }
    else if (start_city == -1) { // station
        if (start_sta >= g_stations.station_count) {
            debug_printf("[train.c][query_train][Error]: Invalid start station %d\n", start_sta);
            return -1;
        }
        if (dijkstra(&g_graph, 1, &start_sta, prior, &depart_time, change_minute) < 0) {
            debug_printf("[train.c][query_train][Error]: Dijkstra failed for station %d\n", start_sta);
            return -1;
        }
        debug_printf("[train.c][query_train][INFO]: Dijkstra completed for station %d\n", start_sta);
    }
    else {
        debug_printf("[train.c][query_train][Error]: Invalid input for start station and city\n");
        return -1;
    }

    int minp = INF, minp_node = -1;
    if (end_sta == -1) { // city
        if (end_city >= g_citys.city_count) {
            debug_printf("[train.c][query_train][Error]: Invalid end city %d\n", end_city);
            return -1;
        }
        debug_printf("[train.c][query_train][INFO]: Querying end city %d\n", end_city);
        City *now = &g_citys.citys[end_city];
        int len = now->st_count;
        debug_printf("[train.c][query_train][INFO]: End station IDs (total = %d): ", len);
        for (int i = 0; i < len; ++i) {
            debug_printf("%d ", now->st_ids[i]);
        }
        debug_printf("\n");
        for (int i = 0; i < len; ++i) {
            Station *now_stat = &g_stations.st[now->st_ids[i]];
            NodeList *cur = now_stat->nodes;
            debug_printf("[train.c][query_train][INFO]: Processing station %d with nodes: ", now->st_ids[i]);
            while (cur) {
                int now_node = cur->node_id;
                debug_printf("%d ", now_node);
                if (ac_change)
                    now_node += DELTA;
                int value = prior ? dist[now_node].p_cost : dist[now_node].p_time;
                if (minp > value) {
                    minp = value;
                    minp_node = now_node;
                }
                cur = cur->next;
            }
            debug_printf("\n");
        }
    } else if (end_city == -1) { // station
        if (end_sta >= g_stations.station_count) {
            debug_printf("[train.c][query_train][Error]: Invalid end station %d\n", end_sta);
            return -1;
        }
        debug_printf("[train.c][query_train][INFO]: Querying to station %d\n", end_sta);
        Station *now_stat = &g_stations.st[end_sta];
        NodeList *cur = now_stat->nodes;
        while (cur) {
            int now_node = cur->node_id;
            if (ac_change)
                now_node += DELTA;
            int value = prior ? dist[now_node].p_cost : dist[now_node].p_time;
            if (minp > value) {
                minp = value;
                minp_node = now_node;
            }
            cur = cur->next;
        }
    } else {
        debug_printf("[train.c][query_train][Error]: Invalid end parameters: city %d, station %d\n", end_city, end_sta);
        return -1;
    }

    if (minp == INF || minp_node == -1) {
        debug_printf("[train.c][query_train][INFO]: No Solution\n");
        return 0;
    }
    int now = minp_node;
    Interval *in = (Interval *)malloc(sizeof(Interval));
    if (in == NULL) {
        debug_printf("[train.c][query_train][Error]: Memory allocation failed for Interval\n");
        return -1;
    }
    if (ac_change) {
        printf("%d %d\n", now, prev[now]);
        now = storeInterval(in, now);
        printf("%d %d\n", now, prev[now]);
        if (prev[now] == -1) {
            debug_printf("[train.c][query_train][Error]: Path must contain at least 2 nodes\n");
            return -1;
        }
        now = prev[now];
        Interval *in_front = (Interval *)malloc(sizeof(Interval));
        if (in_front == NULL) {
            debug_printf("[train.c][query_train][Error]: Memory allocation failed for Interval in_front\n");
            return -1;
        }
        now = storeInterval(in_front, now);
        in_front->ne = in;
        *result = in_front;
        debug_printf("[train.c][query_train][INFO]: Found a path with change, first from %d to %d, second from %d to %d\n", in_front->st_station_id, in_front->ed_station_id, in->st_station_id, in->ed_station_id);
        return 2;
    } else {
        now = storeInterval(in, now);
        if (prev[now] != -1) {
            debug_printf("[train.c][query_train][Error]: Path isn't over\n");
            return -1;
        }
        *result = in;
        debug_printf("[train.c][query_train][INFO]: Found a direct path with train_id %d from station %d to station %d\n", in->train_id, in->st_station_id, in->ed_station_id);
        return 1;
    }
    debug_printf("[train.c][query_train][Error]: Unexpected Error\n");
    return -1;
}

void load_trains_from_txt(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) return;
    char line[4096];
    while (fgets(line, sizeof(line), fp)) {
        if (strlen(line) < 2) continue;
        char train_id[64];
        int n;
        if (sscanf(line, "%s %d", train_id, &n) != 2) continue;
        if (!fgets(line, sizeof(line), fp)) break;
        char **st_names = (char **)malloc(n * sizeof(char *));
        int idx = 0;
        char *p = strtok(line, " \n");
        while (p && idx < n) {
            st_names[idx] = strdup(p);
            if (findStation(st_names[idx]) <= 0) {
                int res = insertStation(st_names[idx], st_names[idx]);
                if (res > 0) {
                    debug_printf("[train.c][load_trains_from_txt][INFO]: Added Station %s with id = %d\n", st_names[idx], res);
                } else if (res == 0) {
                    debug_printf("[train.c][load_trains_from_txt][Error]: Station %s already exists, should not reach here\n", st_names[idx]);
                } else {
                    debug_printf("[train.c][load_trains_from_txt][Error]: Error executing insertStation()\n");
                }
            }
            idx++;
            p = strtok(NULL, " \n");
        }
        TimeType *t_arr = (TimeType *)malloc(n * sizeof(TimeType));
        TimeType *t_dep = (TimeType *)malloc(n * sizeof(TimeType));
        double *price = (double *)malloc((n - 1) * sizeof(double));
        for (int i = 0; i < n - 1; ++i) {
            if (!fgets(line, sizeof(line), fp)) break;
            int dh, dm, ah, am;
            double pval;
            sscanf(line, "%d:%d %d:%d %lf", &dh, &dm, &ah, &am, &pval);
            t_dep[i].hh = dh, t_dep[i].mm = dm;
            t_arr[i + 1].hh = ah, t_arr[i + 1].mm = am;
            price[i] = pval;
        }
        t_arr[0] = t_dep[0];
        t_dep[n - 1] = t_arr[n - 1];
        int res = insertTrain(train_id, n, st_names, t_arr, t_dep, price);
        if (res > 0) {
            debug_printf("[train.c][load_trains_from_txt][INFO]: Added Train %s with id = %d\n", train_id, res);
        } else if (res == 0) {
            debug_printf("[train.c][load_trains_from_txt][INFO]: Train %s already exists\n", train_id);
        } else {
            debug_printf("[train.c][load_trains_from_txt][Error]: Error executing insertTrain()\n");
        }
        for (int i = 0; i < n; ++i) free(st_names[i]);
        free(st_names); free(t_arr); free(t_dep); free(price);
        fgets(line, sizeof(line), fp);
    }
    fclose(fp);
}
