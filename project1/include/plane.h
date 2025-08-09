#ifndef _PLANE_H
#define _PLANE_H

#include "graph.h"

#define MAX_FLIGHT 10000
#define MAX_CITY_NAME 64

// 航班信息结构
typedef struct PlaneEdge {
    int from_city; // 城市id
    int to_city;   // 城市id
    int depart_min; // 起飞时间（分钟）
    int arrive_min; // 到达时间（分钟）
    int price;      // 价格（元*100）
    char flight_no[MAX_CITY_NAME];
    struct PlaneEdge *next;
} PlaneEdge;

// 航班图结构
typedef struct {
    PlaneEdge *edges[MAX_CITYS]; // 每个城市出发的航班链表
} PlaneGraph;

// 初始化航班图
typedef enum { PLANE_TIME_FIRST, PLANE_PRICE_FIRST } PlaneQueryType;

void init_plane_graph(PlaneGraph *g);

/**
 * @brief 查询两城市间的最优航班方案
 * @param g 航班图
 * @param from_city 起点城市id
 * @param to_city 终点城市id
 * @param query_type 0时间优先 1价格优先
 * @param allow_transfer 是否允许中转（0/1）
 * @param result_flight_no 返回航班号数组（需外部分配空间）
 * @param result_count 返回航段数
 * @param result_total_time 返回总时间（分钟）
 * @param result_total_price 返回总价格（分）
 * @return 1成功，0无解，-1参数错误
 */
int plane_query(const PlaneGraph *g, int from_city, int to_city, int query_type, int allow_transfer,
               char result_flight_no[][MAX_CITY_NAME], int *result_count, int *result_total_time, int *result_total_price);

/**
 * @brief 添加航班
 * @param g 航班图
 * @param flight_no 航班号
 * @param from_city 起点城市id
 * @param to_city 终点城市id
 * @param depart_min 起飞时间（分钟）
 * @param arrive_min 到达时间（分钟）
 * @param price 价格（分）
 * @return 1成功，0已存在，-1失败
 */
int add_plane(PlaneGraph *g, const char *flight_no, int from_city, int to_city, int depart_min, int arrive_min, int price);

/**
 * @brief 删除航班
 * @param g 航班图
 * @param flight_no 航班号
 * @return 1成功，0不存在
 */
int delete_plane(PlaneGraph *g, const char *flight_no);

/**
 * @brief Load flight data from file (e.g. data/flight.txt)
 * @param g PlaneGraph pointer
 * @param filepath file path
 * @return 1 success, -1 fail
 */
int load_plane_data(PlaneGraph *g, const char *filepath);

#endif
