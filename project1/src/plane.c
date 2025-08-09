#include "plane.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hash.h"

extern Citys g_citys;

void init_plane_graph(PlaneGraph *g) {
    for (int i = 0; i < MAX_CITYS; ++i) 
        g->edges[i] = NULL;
}

int add_plane(PlaneGraph *g, const char *flight_no, int from_city, int to_city, int depart_min, int arrive_min, int price) {
    if (!g || !flight_no || from_city < 0 || to_city < 0) return -1;
    PlaneEdge *cur = g->edges[from_city];
    while (cur) {
        if (strcmp(cur->flight_no, flight_no) == 0 && cur->to_city == to_city) return 0;
        cur = cur->next;
    }
    PlaneEdge *e = (PlaneEdge*)malloc(sizeof(PlaneEdge));
    if (!e) return -1;
    strcpy(e->flight_no, flight_no);
    e->from_city = from_city;
    e->to_city = to_city;
    e->depart_min = depart_min;
    e->arrive_min = arrive_min;
    e->price = price;
    e->next = g->edges[from_city];
    g->edges[from_city] = e;
    return 1;
}

int delete_plane(PlaneGraph *g, const char *flight_no) {
    if (!g || !flight_no) return -1;
    for (int i = 0; i < MAX_CITYS; ++i) {
        PlaneEdge **pp = &g->edges[i];
        while (*pp) {
            if (strcmp((*pp)->flight_no, flight_no) == 0) {
                PlaneEdge *to_del = *pp;
                *pp = to_del->next;
                free(to_del);
                return 1;
            }
            pp = &((*pp)->next);
        }
    }
    return 0;
}

typedef struct {
    int city, time, price, pre, edge_idx;
} PQNode;

#include <limits.h>
#define PLANE_INF 0x3f3f3f3f

int plane_query(const PlaneGraph *g, int from_city, int to_city, int query_type, int allow_transfer,
               char result_flight_no[][MAX_CITY_NAME], int *result_count, int *result_total_time, int *result_total_price) {
    if (!g || from_city < 0 || to_city < 0 || !result_flight_no || !result_count || !result_total_time || !result_total_price)
        return -1;
    int dist[MAX_CITYS], cost[MAX_CITYS], pre[MAX_CITYS], edge_idx[MAX_CITYS];
    for (int i = 0; i < MAX_CITYS; ++i) dist[i] = cost[i] = PLANE_INF, pre[i] = -1, edge_idx[i] = -1;
    dist[from_city] = 0; cost[from_city] = 0;

    for (int step = 0; step < (allow_transfer ? 2 : 1); ++step) {
        for (int u = 0; u < MAX_CITYS; ++u) {
            if ((step == 0 && u != from_city) || dist[u] == PLANE_INF) continue;
            for (PlaneEdge *e = g->edges[u]; e; e = e->next) {
                int v = e->to_city;
                int wait = 0;
                if (step == 1) {
                    wait = e->depart_min - dist[u];
                    if (wait < 0) wait += MIN_PER_DAY;
                }
                int new_time = dist[u] + wait + (e->arrive_min - e->depart_min + MIN_PER_DAY) % MIN_PER_DAY;
                int new_cost = cost[u] + e->price;
                if ((query_type == PLANE_TIME_FIRST && new_time < dist[v]) ||
                    (query_type == PLANE_PRICE_FIRST && new_cost < cost[v])) {
                    dist[v] = new_time;
                    cost[v] = new_cost;
                    pre[v] = u;
                    edge_idx[v] = u;
                }
            }
        }
    }
    if ((query_type == PLANE_TIME_FIRST && dist[to_city] == PLANE_INF) ||
        (query_type == PLANE_PRICE_FIRST && cost[to_city] == PLANE_INF))
        return 0;
    // 回溯路径
    int path[3], cnt = 0, cur = to_city;
    while (cur != from_city && cur != -1 && cnt < 3) {
        path[cnt++] = cur;
        cur = pre[cur];
    }
    if (cur != from_city) return 0;
    *result_count = cnt;
    *result_total_time = dist[to_city];
    *result_total_price = cost[to_city];
    // 填充航班号
    cur = to_city;
    for (int i = cnt-1; i >= 0; --i) {
        int u = pre[cur];
        PlaneEdge *e = g->edges[u];
        while (e) {
            if (e->to_city == cur) {
                strcpy(result_flight_no[cnt-1-i], e->flight_no);
                break;
            }
            e = e->next;
        }
        cur = u;
    }
    return 1;
}

int load_plane_data(PlaneGraph *g, const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) return -1;
    char flight_no[MAX_CITY_NAME], from_city_name[MAX_CITY_NAME], to_city_name[MAX_CITY_NAME];
    int depart_h, depart_m, arrive_h, arrive_m, price;
    while (fscanf(fp, "%s %d:%d %s %d:%d %s %d", flight_no, &depart_h, &depart_m, from_city_name, &arrive_h, &arrive_m, to_city_name, &price) == 8) {
        int from_city = find_city_name(from_city_name);
        int to_city = find_city_name(to_city_name);
        if (from_city <= 0) {
            int index = g_citys.city_count;
            from_city = insert_city_name(from_city_name, index);
            ++g_citys.city_count;
        }
        if (to_city <= 0) {
            int index = g_citys.city_count;
            to_city = insert_city_name(to_city_name, index);
            ++g_citys.city_count;
        }
        if (from_city <= 0 || to_city <= 0) continue;
        int depart_min = depart_h * 60 + depart_m;
        int arrive_min = arrive_h * 60 + arrive_m;
        add_plane(g, flight_no, from_city, to_city, depart_min, arrive_min, price * 100);
    }
    fclose(fp);
    return 1;
}
