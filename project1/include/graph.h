#ifndef _GRAPH_H
#define _GRAPH_H

#include "heap.h"

/**
 * @note layer 1: 0 ~ 599999
 * @note layer 2: 600000 ~ 1199999
 */
#define MAX_NODE 1200000
#define MAX_EDGE 2500000
#define MAX_TRAIN_ID 100000
#define MAX_CITYS 3000
#define MAX_STATIONS 3000
#define INF 0x3f3f3f3f
#define DELTA 600000
#define MIN_PER_HOUR 60
#define MIN_PER_DAY (24 * 60)

typedef struct {
    int hh;
    int mm;
} TimeType;

/**
 * @param a time 1
 * @param b time 2
 * @retval 1 if a < b
 * @retval 0 if a >= b
 */
int cmpTime(TimeType *a, TimeType *b);

/**
 * @param l left point of time interval
 * @param r right point of time interval
 * @retval time length (minutes)
 */
int getTimeInterval(TimeType *l, TimeType *r);

/**
 * @param l start point of time
 * @param len time length (minutes)
 * @retval end point of time
 */
TimeType getTime(TimeType *l, int len);

typedef enum {
    IN,
    CROSS
} EdgeType;

typedef struct Edge {
    int to;
    EdgeType e_type;
    ValueType weight;
    struct Edge *ne;
} Edge;

typedef struct Node {
    int train_id;
    int station_id;
    Edge *head;
    TimeType train_arrive_time;
    TimeType train_depart_time;
} Node;

typedef struct Graph {
    int node_count;
    int edge_count;
    Node *node;
} Graph;

/**
 * @param g graph to init
 * @retval >0 success, node capacity of this graph
 * @retval 0 fail
 */
int init_graph(Graph *g);

/**
 * @param g graph to add node
 * @param t_id train id at this node
 * @param s_id station id at this node
 * @param t_arr arrive time, if at start station, it's equal to depart time
 * @param t_depart depart time, if at terminal station, it's equal to arrive time
 * @retval >=0 success, id of this node
 * @retval -1 fail
 */
int add_node(Graph *g, int t_id, int s_id, TimeType *t_arr, TimeType *t_depart);

/**
 * @param g graph to add edge
 * @param u start node of edge to add
 * @param v end node of edge to add
 * @param t type of this edge (IN / CROSS)
 * @param w value of this edge
 * @retval 1 success
 * @retval 0 fail
 */
int add_edge(Graph *g, int u, int v, EdgeType t, ValueType w);

/**
 * @param g graph to free
 * @retval 1 success
 * @retval 0 fail
 */
int free_graph(Graph *g);

/**
 * @param g graph to run dijkstra
 * @param station_num number of stations
 * @param st array of stations
 * @param prior 0 time first, 1 cost first
 * @param depart_time depart time
 * @param change_time minimal change time between two train
 * @retval >=0 success, 0 means no source node in this query
 * @retval -1 fail
 * @note preprocess the sssp array
 */
int dijkstra(Graph *g, int station_num, int *st, int prior, TimeType *depart_time, int change_time);

typedef struct NodeList {
    int node_id;
    struct NodeList *next;
} NodeList;

typedef struct Station {
    int expire;
    int train_count;
    NodeList *nodes;
} Station;

typedef struct Stations {
    int station_count;
    Station *st;
} Stations;

/**
 * @param s pointer of stations to init
 * @retval 1 success
 * @retval 0 fail
 */
int initStations(Stations *s);

typedef struct City {
    int st_count;
    int st_ids[20];
} City;

typedef struct Citys {
    int city_count;
    City *citys;
} Citys;

/**
 * @param c pointer of Citys to init
 * @retval 1 success
 * @retval 0 fail
 */
int initCitys(Citys *c);


#endif
