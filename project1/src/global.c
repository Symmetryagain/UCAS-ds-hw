#include "graph.h"
#include "plane.h"
#include <string.h>

Stations g_stations;
Graph g_graph;
Citys g_citys;
PlaneGraph g_plane_graph;
int trainID_expire[MAX_TRAIN_ID];
int trainID_vis[MAX_TRAIN_ID];

int g_init() {
    if (initStations(&g_stations) == 0)
        return 0;
    if (initCitys(&g_citys) == 0)
        return 0;
    if (init_graph(&g_graph) == 0)
        return 0;
    memset(trainID_expire, 0, sizeof trainID_expire);
    init_plane_graph(&g_plane_graph);
    return 1;
}
