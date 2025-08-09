#include "app.h"
#include "modify.h"
#include "graph.h"
#include "train.h"
#include <stdio.h>
#include <stdlib.h>

void print_interval(const Interval *res) {
    int seg = 1;
    while (res) {
        printf("  [段%d] 列车ID: %d, 起点站: %d, 终点站: %d, 出发: %02d:%02d, 到达: %02d:%02d, 票价: %.2f, 跨天: %d\n",
            seg++, res->train_id, res->st_station_id, res->ed_station_id,
            res->depart_time.hh, res->depart_time.mm,
            res->arrive_time.hh, res->arrive_time.mm,
            1.0 * res->cost / 100.0, res->extra_day);
        res = res->ne;
    }
}

void test_insert() {
    // 测试数据
    const char *cities[] = {"北京", "上海", "葫芦岛", "天津", "南京", "沈阳", "锦州"};
    const char *stations[][2] = {
        {"北京站", "北京"},
        {"上海站", "上海"},
        {"葫芦岛站", "葫芦岛"},
        {"天津站", "天津"},
        {"南京站", "南京"},
        {"沈阳站", "沈阳"},
        {"锦州站", "锦州"}
    };
    int city_cnt = 7, station_cnt = 7;
    // 插入城市
    for (int i = 0; i < city_cnt; ++i) {
        int cid = insertCity((char*)cities[i]);
        printf("insertCity('%s') = %d\n", cities[i], cid);
    }
    // 插入站点
    for (int i = 0; i < station_cnt; ++i) {
        int sid = insertStation((char*)stations[i][0], (char*)stations[i][1]);
        printf("insertStation('%s', '%s') = %d\n", stations[i][0], stations[i][1], sid);
    }
    // 插入火车1：北京-天津-葫芦岛
    char *train_id1 = "G123";
    int n1 = 3;
    char *st_names1[3] = {"北京站", "天津站", "葫芦岛站"};
    TimeType t_arr1[3] = {{8,0}, {9,0}, {10,30}};
    TimeType t_dep1[3] = {{8,0}, {9,5}, {10,30}};
    double price1[2] = {60.00, 80.00};
    int tid1 = insertTrain(train_id1, n1, st_names1, t_arr1, t_dep1, price1);
    printf("insertTrain('%s', ...) = %d\n", train_id1, tid1);
    // 插入火车2：上海-南京-北京-葫芦岛
    char *train_id2 = "D456";
    int n2 = 4;
    char *st_names2[4] = {"上海站", "南京站", "北京站", "葫芦岛站"};
    TimeType t_arr2[4] = {{7,0}, {8,30}, {12,0}, {14,30}};
    TimeType t_dep2[4] = {{7,0}, {8,35}, {12,10}, {14,30}};
    double price2[3] = {100.00, 150.00, 90.00};
    int tid2 = insertTrain(train_id2, n2, st_names2, t_arr2, t_dep2, price2);
    printf("insertTrain('%s', ...) = %d\n", train_id2, tid2);
    // 插入火车3：沈阳-锦州-葫芦岛
    char *train_id3 = "K789";
    int n3 = 3;
    char *st_names3[3] = {"沈阳站", "锦州站", "葫芦岛站"};
    TimeType t_arr3[3] = {{6,30}, {8,0}, {9,20}};
    TimeType t_dep3[3] = {{6,30}, {8,10}, {9,20}};
    double price3[2] = {55.00, 40.00};
    int tid3 = insertTrain(train_id3, n3, st_names3, t_arr3, t_dep3, price3);
    printf("insertTrain('%s', ...) = %d\n", train_id3, tid3);
}
void test_delete() {
    // 删除站点
    const char *del_stations[] = {"南京站", "葫芦岛站", "不存在站", "葫芦岛站"};
    int del_station_cnt = 4;
    for (int i = 0; i < del_station_cnt; ++i) {
        int res = deleteStation((char*)del_stations[i]);
        printf("deleteStation('%s') = %d\n", del_stations[i], res);
    }
    // 删除火车
    const char *del_trains[] = {"G123", "K789", "Z999"};
    int del_train_cnt = 3;
    for (int i = 0; i < del_train_cnt; ++i) {
        int res = deleteTrain((char*)del_trains[i]);
        printf("deleteTrain('%s') = %d\n", del_trains[i], res);
    }
}
void test_reinsert() {
    // 重新插入被删除的站点
    const char *re_stations[][2] = {
        {"南京站", "南京"},
        {"葫芦岛站", "葫芦岛"},
        {"不存在站", "不存在城"}
    };
    int re_station_cnt = 3;
    for (int i = 0; i < re_station_cnt; ++i) {
        int res = insertStation((char*)re_stations[i][0], (char*)re_stations[i][1]);
        printf("re-insertStation('%s', '%s') = %d\n", re_stations[i][0], re_stations[i][1], res);
    }
    // 重新插入被删除的火车
    char *train_id1 = "G123";
    int n1 = 3;
    char *st_names1[3] = {"北京站", "天津站", "葫芦岛站"};
    TimeType t_arr1[3] = {{11,0}, {12,0}, {13,30}};
    TimeType t_dep1[3] = {{11,0}, {12,5}, {13,30}};
    double price1[2] = {65.00, 85.00};
    int tid1 = insertTrain(train_id1, n1, st_names1, t_arr1, t_dep1, price1);
    printf("re-insertTrain('%s', ...) = %d\n", train_id1, tid1);
    char *train_id2 = "K789";
    int n2 = 3;
    char *st_names2[3] = {"沈阳站", "锦州站", "葫芦岛站"};
    TimeType t_arr2[3] = {{15,30}, {17,0}, {18,20}};
    TimeType t_dep2[3] = {{15,30}, {17,10}, {18,20}};
    double price2[2] = {60.00, 45.00};
    int tid2 = insertTrain(train_id2, n2, st_names2, t_arr2, t_dep2, price2);
    printf("re-insertTrain('%s', ...) = %d\n", train_id2, tid2);
}

void test_query_train() {
    // 测试：北京、上海、沈阳 到 葫芦岛
    const char *from_cities[] = {"北京", "上海", "沈阳"};
    const char *to_city = "葫芦岛";
    int n = 3;
    for (int i = 0; i < n; ++i) {
        int from = findCity((char*)from_cities[i]);
        int to = findCity((char*)to_city);
        if (from < 0 || to < 0) {
            printf("[test_query_train] 城市查找失败: %s -> %s\n", from_cities[i], to_city);
            continue;
        }
        TimeType dep = {6, 0}; // 06:00出发
        Interval *res = NULL;
        int ret = query_train(from, -1, to, -1, 0, dep, 0, 30, &res);
        printf("query_train('%s'->'%s') = %d\n", from_cities[i], to_city, ret);
        if (ret > 0 && res) {
            print_interval(res);
        } else {
            printf("  无可行方案\n");
        }
    }
    // 城市到车站
    const char *from_city = "北京";
    const char *to_station = "葫芦岛站";
    int from = findCity((char*)from_city);
    int to_sta = findStation((char*)to_station);
    if (from >= 0 && to_sta >= 0) {
        TimeType dep = {6, 0};
        Interval *res = NULL;
        int ret = query_train(from, -1, -1, to_sta, 0, dep, 0, 30, &res);
        printf("query_train('%s'->'%s') = %d\n", from_city, to_station, ret);
        if (ret > 0 && res) print_interval(res);
        else printf("  无可行方案\n");
    }
    // 车站到城市
    const char *from_station = "上海站";
    const char *to_city2 = "葫芦岛";
    int from_sta = findStation((char*)from_station);
    int to2 = findCity((char*)to_city2);
    if (from_sta >= 0 && to2 >= 0) {
        TimeType dep = {6, 0};
        Interval *res = NULL;
        int ret = query_train(-1, from_sta, to2, -1, 0, dep, 0, 30, &res);
        printf("query_train('%s'->'%s') = %d\n", from_station, to_city2, ret);
        if (ret > 0 && res) print_interval(res);
        else printf("  无可行方案\n");
    }
    // 车站到车站
    const char *from_station2 = "沈阳站";
    const char *to_station2 = "葫芦岛站";
    int from_sta2 = findStation((char*)from_station2);
    int to_sta2 = findStation((char*)to_station2);
    if (from_sta2 >= 0 && to_sta2 >= 0) {
        TimeType dep = {6, 0};
        Interval *res = NULL;
        int ret = query_train(-1, from_sta2, -1, to_sta2, 0, dep, 0, 30, &res);
        if (res == NULL) {
            printf("[test_query_train] 查询结果为空: %s -> %s\n", from_station2, to_station2);
            return;
        }
        printf("query_train('%s'->'%s') = %d\n", from_station2, to_station2, ret);
        if (ret > 0 && res != NULL) print_interval(res), free(res);
        else printf("  无可行方案\n");
    }
}

int main() {
    // g_init();
    // test_insert();
    // test_query_train();
    // test_delete();
    // test_query_train();
    // test_reinsert();
    // test_query_train();
    if (!app_init()) return 1;
    app_run();
    // print();
    return 0;
}
