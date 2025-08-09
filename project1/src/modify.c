#include "modify.h"
#include "debug.h"
#include <string.h>
#include <stdlib.h>

extern Stations g_stations;
extern Citys g_citys;
extern Graph g_graph;
extern int trainID_expire[MAX_TRAIN_ID];
extern int trainID_vis[MAX_TRAIN_ID];
extern int debug_on;

int findCity(char *c_name) {
    if (c_name == NULL) {
        debug_printf("[modify.c][findCity][Error]: c_name is NULL\n");
        return -1;
    }
    int res = find_city_name(c_name);
    if (res > 0) {
        debug_printf("[modify.c][findCity][INFO]: find id = %d of city name %s\n", res, c_name);
        return res; // If the city is found, return its ID
    }
    debug_printf("[modify.c][findCity][INFO]: id of city name %s not found\n", c_name);
    return 0; // not found
}

int findStation(char *s_name) {
    if (s_name == NULL) {
        debug_printf("[modify.c][findStation][Error]: s_name is NULL\n");
        return -1;
    }
    int res = find_station_name(s_name);
    if (res > 0) {
        debug_printf("[modify.c][findStation][INFO]: find id = %d of station name %s\n", res, s_name);
        return res; // If the station is found, return its ID
    }
    debug_printf("[modify.c][findStation][INFO]: id of station name %s not found\n", s_name);
    return 0; // not found
}

int insertStation(char *s_name, char *c_name) {
    if (s_name == NULL || c_name == NULL) {
        debug_printf("[modify.c][insertStation][Error]: s_name or c_name is NULL\n");
        return -1;
    }

    int city_id = findCity(c_name);
    if (city_id < 0) {
        debug_printf("[modify.c][insertStation][Error]: Error executing findCity()\n");
        return -1;
    } else if (city_id == 0) {
        debug_printf("[modify.c][insertStation][INFO]: City '%s' not found, creating...\n", c_name);
        int res = insertCity(c_name);
        if (res > 0) {
            debug_printf("[modify.c][insertStation][INFO]: City %s successfully inserted, id = %d\n", c_name, res);
            city_id = res;
        } else if (res == 0) {
            debug_printf("[modify.c][insertStation][Error]: City %s already exists, should not reach here\n", c_name);
            return -1;
        } else {
            debug_printf("[modify.c][insertStation][Error]: Error executing insertCity()\n");
            return -1;
        }
    } else {
        debug_printf("[modify.c][insertStation][INFO]: find City id %d of city %s\n", city_id, c_name);
    }

    int station_id = findStation(s_name);
    if (station_id > 0) {
        if (g_stations.st[station_id].expire) {
            // If the station is expired, we can reuse it
            g_stations.st[station_id].expire = 0; // mark as not expired
            debug_printf("[modify.c][insertStation][INFO]: Reusing expired station '%s' with ID %d\n", s_name, station_id);
            return station_id; // return the existing station ID
        } else {
            debug_printf("[modify.c][insertStation][INFO]: Station '%s' already exists and is not expired\n", s_name);
            return 0; // already exists and not expired
        }
    }
    if (station_id == -1) {
        debug_printf("[modify.c][insertStation][Error]: Error executing findStation\n");
        return -1;
    }
    // not found
    debug_printf("[modify.c][insertStation][INFO]: Station name '%s' not found, inserting into database...\n", s_name);
    int index = g_stations.station_count;
    station_id = insert_station_name(s_name, index);
    if (station_id <= 0) {
        debug_printf("[modify.c][insertStation][Error]: Failed to insert station '%s'\n", s_name);
        return -1; // insertion failed
    }

    debug_printf("[modify.c][insertStation][INFO]: Inserting station '%s' with ID %d in city '%s'\n", s_name, station_id, c_name);
    // Create a new station
    if (index >= MAX_STATIONS) {
        debug_printf("[modify.c][insertStation][Error]: No space for new station '%s'\n", s_name);
        return -1; // no space for new station
    }
    Station *new_station = &g_stations.st[index];
    new_station->expire = 0; // not expired
    new_station->train_count = 0; // no trains yet
    new_station->nodes = NULL; // no nodes yet
    ++g_stations.station_count;
    debug_printf("[modify.c][insertStation][INFO]: New station '%s' created with ID %d in city '%s'\n", s_name, station_id, c_name);

    g_citys.citys[city_id].st_ids[g_citys.citys[city_id].st_count++] = station_id; // add station ID to city's list

    return station_id; // success, new station created
}

int insertCity(char *c_name) {
    if (c_name == NULL) {
        debug_printf("[modify.c][insertCity][Error]: c_name is NULL\n");
        return -1;
    }
    int city_id = findCity(c_name);
    if (city_id > 0) {
        debug_printf("[modify.c][insertCity][INFO]: City '%s' already exists with ID %d\n", c_name, city_id);
        return 0; // already exists
    }
    int index = g_citys.city_count;
    city_id = insert_city_name(c_name, index);
    if (city_id <= 0) {
        debug_printf("[modify.c][insertCity][Error]: Failed to insert city '%s'\n", c_name);
        return -1; // insertion failed
    }
    if (index >= MAX_CITYS) {
        debug_printf("[modify.c][insertCity][Error]: No space for new city '%s'\n", c_name);
        return -1; // no space for new city
    }
    City *new_city = &g_citys.citys[index];
    new_city->st_count = 0; // no stations yet
    ++g_citys.city_count;

    debug_printf("[modify.c][insertCity][INFO]: New city '%s' created with ID %d\n", c_name, city_id);
    return city_id; // success, new city created
}


/*
增加火车车次，先字符串查找车次（train_id）是否已经存在。如果存在且 `expire == 1`，则将该车次的 `expire` 赋为 0；如果 `expire == 0` 返回增加失败，信息为已经存在。
如果不存在，要求按格式输入：
第一行车站数 n，接下来一行 n 个字符串表示车站名，
必须都已经存在，接下来 n-1 行每行依次输入，
第 i 站到第 i+1 站这段的 出发时间(HH:MM)，到达时间(HH:MM)，票价（两位小数）。
然后根据输入的信息插入。
下面这个函数是专为插入情况调用。
将字符串 train_id 转化为整数 train_id 的方法：
如果纯四位数字，直接化为五位整数
其余均为字母开头，格式为 [character][number]，
其中如果number 不足四位，前面补零。
字母变成数字规则如下：c->1 d->2 g->3 k->4 t-> 5 y->6 z->7

如 C123 -> 10123, D1012 -> 21012
G3 ->30003, T33->50033。

字母只会出现这几种。
变成数字之后，
在extern int trainID_expire 数组中查询是否过期
在extern int trainID_vis 数组中查询是否已经存在。
如果要插入的车次存在且已经过期，则将其 `expire` 赋为 0。
如果要插入的车次存在且没有过期，则返回 0，表示已经存在。
如果要插入的车次不存在，则将 vis 赋为1, expire 赋为 0。
函数实现：
*/
static int train_str_to_id(const char *train_id) {
    if (!train_id) {
        debug_printf("[modify.c][train_str_to_id][Error]: train_id is NULL\n");
        return -1;
    }
    int len = strlen(train_id);
    int id = 0;
    if (len == 4 && isdigit(train_id[0]) && isdigit(train_id[1]) && isdigit(train_id[2]) && isdigit(train_id[3])) {
        id = atoi(train_id);
    } else if (isalpha(train_id[0])) {
        int prefix = 0;
        switch (tolower(train_id[0])) {
            case 'c': prefix = 1; break;
            case 'd': prefix = 2; break;
            case 'g': prefix = 3; break;
            case 'k': prefix = 4; break;
            case 't': prefix = 5; break;
            case 'y': prefix = 6; break;
            case 'z': prefix = 7; break;
            default: return -1;
        }
        char numbuf[8] = {0};
        int numlen = len - 1;
        if (numlen > 4) return -1;
        strncpy(numbuf, train_id + 1, numlen);
        for (int i = 0; i < numlen; ++i) {
            if (!isdigit(numbuf[i])) return -1;
        }
        char numstr[5] = {0};
        int fill = 4 - numlen;
        for (int i = 0; i < fill; ++i) numstr[i] = '0';
        strcpy(numstr + fill, numbuf);
        id = prefix * 10000 + atoi(numstr);
    } else {
        debug_printf("[modify.c][train_str_to_id][Error]: Invalid train_id format '%s'\n", train_id);
        return -1;
    }
    if (id <= 0 || id >= MAX_TRAIN_ID) {
        debug_printf("[modify.c][train_str_to_id][Error]: train_id out of range: %d\n", id);
        return -1;
    }
    debug_printf("[modify.c][train_str_to_id][INFO]: train number %s changed into id = %d\n", train_id, id);
    return id;
}

int insertTrain(char *train_id, int n, char **st_names, TimeType *t_arr, TimeType *t_depart, double *price) {
    if (!train_id || n <= 1 || !st_names || !t_arr || !t_depart || !price) {
        debug_printf("[modify.c][insertTrain][Error]: Invalid parameters\n");
        return -1;
    }
    int id = train_str_to_id(train_id);
    if (id == -1) {
        debug_printf("[modify.c][insertTrain][Error]: Invalid train_id '%s'\n", train_id);
        return -1;
    }

    if (trainID_vis[id]) {
        if (trainID_expire[id]) {
            debug_printf("[modify.c][insertTrain][INFO]: Reusing expired train_id %d\n", id);
            trainID_expire[id] = 0;
            return id;
        } else {
            debug_printf("[modify.c][insertTrain][INFO]: Train_id %d already exists and is not expired\n", id);
            return 0;
        }
    } else {
        trainID_vis[id] = 1;
        trainID_expire[id] = 0;
        int *node_ids = (int *)malloc(n * sizeof(int));
        if (node_ids == NULL) {
            debug_printf("[modify.c][insertTrain][Error]: Fail to allocate memory for node_ids array\n");
            return -1;
        }

        for (int i = 0; i < n; ++i) {
            int s_id = findStation(st_names[i]);
            if (s_id == -1) {
                debug_printf("[modify.c][insertTrain][Error]: Error executing findStation, station name = %s\n", st_names[i]);
            } else if (s_id == 0) {
                debug_printf("[modify.c][insertTrain][Error]: Station '%s' not found\n", st_names[i]);
                return -1;
            }
            TimeType arr = t_arr[i];
            TimeType dep = t_depart[i];
            if (i == 0) arr = dep; // start station
            if (i == n - 1) dep = arr; // terminal station
            int node_id = add_node(&g_graph, id, s_id, &arr, &dep);
            if (node_id < 0) {
                debug_printf("[modify.c][insertTrain][Error]: Failed to add node for train_id %d at station '%s'\n", id, st_names[i]);
                return -1;
            }
            debug_printf("[modify.c][insertTrain][INFO]: Added node %d for train_id %d at station '%s'\n", node_id, id, st_names[i]);
            node_ids[i] = node_id;
            // Add to list of the station
            NodeList *new_node = (NodeList*)malloc(sizeof(NodeList));
            if (!new_node) {
                debug_printf("[modify.c][insertTrain][Error]: Memory allocation failed for new_node\n");
                return -1;
            }
            debug_printf("[modify.c][insertTrain][INFO]: Adding node %d to station %d\n", node_id, s_id);
            new_node->node_id = node_id;
            new_node->next = g_stations.st[s_id].nodes;
            g_stations.st[s_id].nodes = new_node;
            g_stations.st[s_id].train_count++;
        }
        // 1. IN Edge
        for (int i = 0; i < n - 1; ++i) {
            ValueType w;
            w.p_time = getTimeInterval(&t_depart[i], &t_arr[i + 1]);
            w.p_cost = (int)(price[i] * 100 + 0.5);
            add_edge(&g_graph, node_ids[i], node_ids[i + 1], IN, w);
            debug_printf("[modify.c][insertTrain][INFO]: Added edge from node %d to node %d with time %d and cost %.2f\n", node_ids[i], node_ids[i + 1], w.p_time, price[i]);
            add_edge(&g_graph, node_ids[i] + DELTA, node_ids[i + 1] + DELTA, IN, w);
            debug_printf("[modify.c][insertTrain][INFO]: Added edge from node %d to node %d on layer 2 with time %d and cost %.2f\n", node_ids[i] + DELTA, node_ids[i + 1] + DELTA, w.p_time, price[i]);
        }
        // 2. CROSS Edge
        for (int i = 1; i < n; ++i) {
            int s_id = findStation(st_names[i]);
            int u = node_ids[i];
            NodeList *cur = g_stations.st[s_id].nodes;
            while (cur) {
                int v = cur->node_id;
                if (v != u) {
                    ValueType w1;
                    w1.p_cost = 0;
                    w1.p_time = getTimeInterval(&g_graph.node[v].train_arrive_time, &g_graph.node[u].train_arrive_time);
                    add_edge(&g_graph, u, v+DELTA, CROSS, w1);
                    debug_printf("[modify.c][insertTrain][INFO]: Added cross edge from node %d to node %d with time %d\n", u, v + DELTA, w1.p_time);
                    ValueType w2;
                    w2.p_cost = 0;
                    w2.p_time = getTimeInterval(&g_graph.node[u].train_arrive_time, &g_graph.node[v].train_arrive_time);
                    add_edge(&g_graph, v, u+DELTA, CROSS, w2);
                    debug_printf("[modify.c][insertTrain][INFO]: Added cross edge from node %d to node %d with time %d\n", v, u + DELTA, w2.p_time);
                }
                cur = cur->next;
            }
        }
        free(node_ids);
        debug_printf("[modify.c][insertTrain][INFO]: Train_id %d inserted successfully with %d stations\n", id, n);
        return id;
    }
    debug_printf("[modify.c][insertTrain][Error]: Unexpected Error\n");
    return -1; // should not reach here
}

int deleteStation(char *s_name) {
    int s_id = findStation(s_name);
    if (s_id == -1) {
        debug_printf("[modify.c][deleteStation][Error]: Error executing findStation with name = %s\n", s_name);
        return -1;
    }
    if (s_id == 0) {
        debug_printf("[modify.c][deleteStation][Error]: Station '%s' has never been inserted\n", s_name);
        return -1;
    }
    if (g_stations.st[s_id].expire) {
        debug_printf("[modify.c][deleteStation][INFO]: Station '%s' is inserted and already expired\n", s_name);
        return 0;
    }
    g_stations.st[s_id].expire = 1;
    debug_printf("[modify.c][deleteStation][INFO]: Station '%s' with ID %d deleted successfully\n", s_name, s_id);
    return 1;
}

int deleteTrain(char *train_id) {
    int id = train_str_to_id(train_id);
    if (id == -1) {
        debug_printf("[modify.c][deleteTrain][Error]: Invalid train_id '%s'\n", train_id);
        return -1;
    }
    if (!trainID_vis[id]) {
        debug_printf("[modify.c][deleteTrain][Error]: Train_id %s (id = %d) does not exist\n", train_id, id);
        return -1; // train does not exist
    }
    if (trainID_expire[id]) {
        debug_printf("[modify.c][deleteTrain][Error]: Train_id %s (id = %d) is inserted and already expired\n", train_id, id);
        return 0; // already expired
    }
    trainID_expire[id] = 1;
    debug_printf("[modify.c][deleteTrain][INFO]: Train_id %s (id = %d) deleted successfully\n", train_id, id);
    return 1;
}

