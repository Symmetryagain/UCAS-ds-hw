#include "graph.h"
#include "debug.h"
#include "util.h"
#include <string.h>

static Heap heap;
static int vis[MAX_NODE];
ValueType dist[MAX_NODE];
int prev[MAX_NODE];
extern Stations g_stations;
extern int trainID_expire[MAX_NODE];
extern int debug_on;

static inline void dijkstra_run(Graph *g, Heap *h, int change_time) {
    while (getsize(h)) {
        int x = top(h).id;
        pop(h);
        if (vis[x]) continue;
        vis[x] = 1;
        for (Edge *ie = g->node[x].head; ie != NULL; ie = ie->ne) {
            int y = ie->to;
            // make sure y is available
            // station of y is expired || line pass y is expired
            if (g_stations.st[g->node[y].station_id].expire || trainID_expire[g->node[y].train_id])
                continue;
            ValueType w = ie->weight;
            if (ie->e_type == CROSS) {
                Node *nn = &g->node[y];
                int stay = getTimeInterval(&nn->train_arrive_time, &nn->train_depart_time);
                if (w.p_time + stay < change_time) { // cross edge: available change time shorter than interval
                    continue;
                }
            }
            ValueType w_ = add(dist[x], w);
            if (heap.cf(&w_, &dist[y])) {
                dist[y] = w_;
                prev[y] = x;
                insert(h, (DataType){dist[y], y});
            }
        }
    }
}

int dijkstra(Graph *g, int station_num, int *st, int prior, TimeType *depart_time, int change_time) {
    if (g == NULL || st == NULL || depart_time == NULL)
        return -1;
    if (init_heap(&heap, MAX_NODE, prior ? cmp_cost_time : cmp_time_cost) == 0)
        return -1;
    int delta = -1;

    for (int i = 0; i < station_num; ++i) {
        Station *now = &g_stations.st[st[i]];
        if (now->expire) {// make sure station now is not expired
            debug_printf("[sssp.c][dijkstra][Error]: Station %d is expired\n", st[i]);
            continue;
        }
        debug_printf("[sssp.c][dijkstra][INFO]: Processing station %d, Node: ", st[i]);
        NodeList *cur = now->nodes;
        while (cur != NULL) {
            Node *nn = &g->node[cur->node_id];
            debug_printf("%d ", cur->node_id);
            if (trainID_expire[nn->train_id]) { 
                debug_printf("[sssp.c][dijkstra][INFO]: Train %d is expired\n", nn->train_id);
                cur = cur->next; 
                continue; 
            }
            if (cmpTime(&nn->train_depart_time, depart_time)) { 
                debug_printf("[sssp.c][dijkstra][INFO]: Train %d departs too early\n", nn->train_id);
                cur = cur->next; 
                continue; 
            }
            int stay = getTimeInterval(&nn->train_arrive_time, &nn->train_depart_time);
            delta = max(delta, stay);
            cur = cur->next;
        }
        debug_printf("\n");
    }
    for (int i = 0; i < g->node_count; ++i)
        dist[i] = dist[i + DELTA] = (ValueType){INF, INF};
    memset(vis, 0, sizeof vis);
    memset(prev, -1, sizeof prev);
    if (delta == -1)
        return 0;
    for (int i = 0; i < station_num; ++i) {
        Station *now = &g_stations.st[st[i]];
        if (now->expire) // make sure station now is not expired
            continue;
        NodeList *cur = now->nodes;
        while (cur != NULL) {
            Node *nn = &g->node[cur->node_id];
            if (trainID_expire[nn->train_id]) { // this line is expired
                cur = cur->next;
                continue;
            }
            if (cmpTime(&nn->train_depart_time, depart_time)) { // depart too early
                cur = cur->next;
                continue;
            }
            int stay = getTimeInterval(&nn->train_arrive_time, &nn->train_depart_time);
            dist[cur->node_id] = (ValueType){delta - stay, 0};
            insert(&heap, (DataType){dist[cur->node_id], cur->node_id});
            cur = cur->next;
        }
    }
    dijkstra_run(g, &heap, change_time);
    if (free_heap(&heap) == 0)
        return -1;
    for (int i = 0; i < g->node_count; ++i)
        dist[i].p_cost -= delta;
    return 1;
}
