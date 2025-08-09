#include "graph.h"
#include <stdlib.h>

int cmpTime(TimeType *a, TimeType *b) {
    return a->hh < b->hh || (a->hh == b->hh && a->mm < b->mm);
}

int getTimeInterval(TimeType *l, TimeType *r) {
    int l_min = l->hh * MIN_PER_HOUR + l->mm;
    int r_min = r->hh * MIN_PER_HOUR + r->mm;
    if (l_min <= r_min)
        return r_min - l_min;
    else
        return MIN_PER_DAY + l_min - r_min;
}

TimeType getTime(TimeType *l, int len) {
    int min_sum = l->hh * MIN_PER_HOUR + l->mm;
    min_sum = (min_sum + len) % MIN_PER_DAY;
    return (TimeType){
        min_sum / MIN_PER_HOUR,
        min_sum % MIN_PER_HOUR
    };
}

int init_graph(Graph *g) {
    if (g == NULL)
        return 0;
    g->node = (Node *)malloc(MAX_NODE * sizeof(Node));
    if (g->node == NULL)
        return 0;
    g->node_count = g->edge_count = 0;
    return 1;
}

int add_node(Graph *g, int t_id, int s_id, TimeType *t_arr, TimeType *t_depart) {
    if (g == NULL || g->node_count == DELTA)
        return -1;
    int now = g->node_count;
    ++g->node_count; // real node count, on layer 1
    g->node[now].train_id = t_id;
    g->node[now].station_id = s_id;
    g->node[now].head = NULL;
    g->node[now].train_arrive_time = *t_arr;
    g->node[now].train_depart_time = *t_depart;
    g->node[now + DELTA].train_id = t_id; // add node on layer 2
    g->node[now + DELTA].station_id = s_id;
    g->node[now + DELTA].head = NULL;
    g->node[now + DELTA].train_arrive_time = *t_arr;
    g->node[now + DELTA].train_depart_time = *t_depart;
    return now;
}

int add_edge(Graph *g, int u, int v, EdgeType t, ValueType w) {
    if (g == NULL || g->edge_count == MAX_EDGE)
        return 0;
    Edge *nEdge = (Edge *)malloc(sizeof(Edge));
    if (nEdge == NULL)
        return 0;
    nEdge->to = v;
    nEdge->e_type = t;
    nEdge->weight = w;
    nEdge->ne = g->node[u].head;
    g->node[u].head = nEdge;
    ++g->edge_count;
    return 1;
}

int free_graph(Graph *g) {
    if (g == NULL)
        return 0;
    for (int i = 0; i < g->node_count; ++i) {
        Edge *now = g->node[i].head;
        while (now != NULL) {
            Edge *nxt = now->ne;
            free(now);
            now = nxt;
        }
    }
    free(g->node);
    return 1;
}

int initStations(Stations *s) {
    s->st = (Station *)malloc(MAX_STATIONS * sizeof(Station));
    if (s->st == NULL)
        return 0;
    s->station_count = 1;
    return 1;
}

int initCitys(Citys *c) {
    c->citys = (City *)malloc(MAX_CITYS * sizeof(City));
    if (c->citys == NULL)
        return 0;
    c->city_count = 1;
    return 1;
}
