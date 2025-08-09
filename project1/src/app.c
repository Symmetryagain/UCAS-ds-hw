#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app.h"
#include "debug.h"
#include "global.h"
#include "modify.h"
#include "plane.h"
#include "train.h"
#include "debug.h"

#define TRAIN_DATA_PATH "data/train.txt"
#define FLIGHT_DATA_PATH "data/flight.txt"

extern Stations g_stations;
extern Graph g_graph;
extern Citys g_citys;
extern PlaneGraph g_plane_graph;
extern int debug_on;

int app_init() {
    if (!g_init()) {
        printf("Global initialization failed!\n");
        return 0;
    }
    printf("Loading train data...\n");
    load_trains_from_txt(TRAIN_DATA_PATH);
    printf("Loading flight data...\n");
    if (load_plane_data(&g_plane_graph, FLIGHT_DATA_PATH) != 1) {
        printf("Failed to load flight data!\n");

    }
    printf("Initialization complete.\n");
    return 1;
}

static void print_menu() {
    printf("\n==== National Transport System ====\n");
    printf("1. Add train station\n");
    printf("2. Delete train station\n");
    printf("3. Add train\n");
    printf("4. Delete train\n");
    printf("5. Add flight\n");
    printf("6. Delete flight\n");
    printf("7. Query train (city to city)\n");
    printf("8. Query train (station to station)\n");
    printf("9. Query flight (city to city)\n");
    printf("0. Exit\n");
    printf("==================================\n");
    printf("Enter your choice: ");
}

static void add_station() {
    char s_name[64], c_name[64];
    printf("Enter station name: ");
    scanf("%s", s_name);
    printf("Enter city name: ");
    scanf("%s", c_name);
    int res = insertStation(s_name, c_name);
    if (res > 0) printf("Station added successfully.\n");
    else if (res == 0) printf("Station already exists.\n");
    else printf("Failed to add station.\n");
}

static void delete_station() {
    char s_name[64];
    printf("Enter station name to delete: ");
    scanf("%s", s_name);
    int res = deleteStation(s_name);
    if (res > 0) printf("Station deleted successfully.\n");
    else if (res == 0) printf("Station is already deleted.\n");
    else printf("Station has never been inserted.\n");
}

static void add_train() {
    char train_id[64];
    int n;
    printf("Enter train ID: ");
    scanf("%s", train_id);
    printf("Enter number of stations: ");
    scanf("%d", &n);
    if (n < 2) { 
        printf("A train must have at least 2 stations.\n"); 
        return;
    }
    char **st_names = (char **)malloc(n * sizeof(char *));
    TimeType *t_arr = (TimeType *)malloc(n * sizeof(TimeType));
    TimeType *t_dep = (TimeType *)malloc(n * sizeof(TimeType));
    double *price = (double *)malloc((n - 1) * sizeof(double));
    printf("Enter %d station names (separated by space):\n", n);
    for (int i = 0; i < n; ++i) {
        char tmp[64];
        scanf("%s", tmp);
        st_names[i] = strdup(tmp);
        if (findStation(st_names[i]) <= 0) {
            int res = insertStation(st_names[i], st_names[i]);
            if (res > 0) {
                debug_printf("[app.c][add_train][INFO]: Successfully added station %s, id = %d\n", st_names[i], res);
            } else if (res == 0) {
                debug_printf("[app.c][add_train][Error]: Station %s already exists, and should not reach here\n", st_names[i]);
            } else {
                debug_printf("[app.c][add_train][Error]: Error executing insertStation()\n");
            }
        }
    }
    printf("Enter %d segments (depart_time(HH:MM) arrive_time(HH:MM) price):\n", n - 1);
    for (int i = 0; i < n - 1; ++i) {
        int dh, dm, ah, am;
        double pval;
        scanf("%d:%d %d:%d %lf", &dh, &dm, &ah, &am, &pval);
        t_dep[i].hh = dh; t_dep[i].mm = dm;
        t_arr[i + 1].hh = ah; t_arr[i + 1].mm = am;
        price[i] = pval;
    }
    t_arr[0] = t_dep[0];
    t_dep[n - 1] = t_arr[n - 1];
    int res = insertTrain(train_id, n, st_names, t_arr, t_dep, price);
    if (res > 0) printf("Train added successfully.\n");
    else if (res == 0) printf("Train already exists.\n");
    else printf("Failed to add train.\n");
    for (int i = 0; i < n; ++i) free(st_names[i]);
    free(st_names); free(t_arr); free(t_dep); free(price);
}

static void delete_train() {
    char train_id[64];
    printf("Enter train ID to delete: ");
    scanf("%s", train_id);
    int res = deleteTrain(train_id);
    if (res > 0) printf("Train deleted successfully.\n");
    else if (res == 0) printf("Train is already deleted.\n");
    else printf("Train has never been inserted.\n");
}

static void add_flight() {
    char flight_no[64], from_city[64], to_city[64];
    int dh, dm, ah, am, price;
    printf("Enter flight_no: ");
    scanf("%s", flight_no);
    printf("Enter from city: ");
    scanf("%s", from_city);
    printf("Enter to city: ");
    scanf("%s", to_city);
    printf("Enter depart time (HH:MM): ");
    scanf("%d:%d", &dh, &dm);
    printf("Enter arrive time (HH:MM): ");
    scanf("%d:%d", &ah, &am);
    printf("Enter price (yuan, integer): ");
    scanf("%d", &price);
    int from_id = findCity(from_city);
    if (from_id <= 0) from_id = insertCity(from_city);
    int to_id = findCity(to_city);
    if (to_id <= 0) to_id = insertCity(to_city);
    int res = add_plane(&g_plane_graph, flight_no, from_id, to_id, dh*60+dm, ah*60+am, price*100);
    if (res == 1) printf("Flight added successfully.\n");
    else if (res == 0) printf("Flight already exists.\n");
    else printf("Failed to add flight.\n");
}

static void delete_flight() {
    char flight_no[64];
    printf("Enter flight number to delete: ");
    scanf("%s", flight_no);
    int res = delete_plane(&g_plane_graph, flight_no);
    if (res == 1) printf("Flight deleted successfully.\n");
    else printf("Flight not found.\n");
}

static void print_train_result(Interval *result) {
    int seg = 1;
    while (result) {
        printf("Segment %d: Train %d, from station %d to %d, depart at %02d:%02d, arrive at %02d:%02d, price: %.2f\n",
            seg++, result->train_id, result->st_station_id, result->ed_station_id,
            result->depart_time.hh, result->depart_time.mm,
            result->arrive_time.hh, result->arrive_time.mm,
            result->cost / 100.0);
        result = result->ne;
    }
}

static void query_train_city() {
    char st_city[64], ed_city[64];
    int prior, ac_change, change_minute;
    char depart_time_str[16];
    printf("Enter start city: ");
    scanf("%s", st_city);
    printf("Enter end city: ");
    scanf("%s", ed_city);
    printf("Time first(0) or price first(1): ");
    scanf("%d", &prior);
    printf("Allow transfer? (0/1): ");
    scanf("%d", &ac_change);
    printf("Minimal transfer interval (minutes): ");
    scanf("%d", &change_minute);
    printf("Depart time (HH:MM): ");
    scanf("%s", depart_time_str);
    int hh, mm;
    sscanf(depart_time_str, "%d:%d", &hh, &mm);
    TimeType depart_time = {hh, mm};
    int st_id = findCity(st_city), ed_id = findCity(ed_city);
    if (st_id <= 0 || ed_id <= 0) {
        printf("City not found.\n");
        return;
    }
    Interval *result = NULL;
    int cnt = query_train(st_id, -1, ed_id, -1, prior, depart_time, ac_change, change_minute, &result);
    if (cnt > 0) {
        printf("Train solution found (%d segment(s)):\n", cnt);
        print_train_result(result);
    } else {
        printf("No solution found.\n");
    }
}

static void query_train_station() {
    char st_name[64], ed_name[64];
    int prior, ac_change, change_minute;
    char depart_time_str[16];
    printf("Enter start station: ");
    scanf("%s", st_name);
    printf("Enter end station: ");
    scanf("%s", ed_name);
    printf("Time first(0) or price first(1): ");
    scanf("%d", &prior);
    printf("Allow transfer? (0/1): ");
    scanf("%d", &ac_change);
    printf("Minimal transfer interval (minutes): ");
    scanf("%d", &change_minute);
    printf("Depart time (HH:MM): ");
    scanf("%s", depart_time_str);
    int hh, mm;
    sscanf(depart_time_str, "%d:%d", &hh, &mm);
    TimeType depart_time = {hh, mm};
    int st_id = findStation(st_name), ed_id = findStation(ed_name);
    if (st_id <= 0 || ed_id <= 0) {
        printf("Station not found.\n");
        return;
    }
    Interval *result = NULL;
    int cnt = query_train(-1, st_id, -1, ed_id, prior, depart_time, ac_change, change_minute, &result);
    if (cnt > 0) {
        printf("Train solution found (%d segment(s)):\n", cnt);
        print_train_result(result);
    } else {
        printf("No solution found.\n");
    }
}

static void query_flight_city() {
    char st_city[64], ed_city[64];
    int prior, allow_transfer;
    printf("Enter start city: ");
    scanf("%s", st_city);
    printf("Enter end city: ");
    scanf("%s", ed_city);
    printf("Time first(0) or price first(1): ");
    scanf("%d", &prior);
    printf("Allow transfer? (0/1): ");
    scanf("%d", &allow_transfer);
    int st_id = findCity(st_city), ed_id = findCity(ed_city);
    if (st_id <= 0 || ed_id <= 0) {
        printf("City not found.\n");
        return;
    }
    char result_flight_no[2][64];
    int result_count = 0, result_total_time = 0, result_total_price = 0;
    int res = plane_query(&g_plane_graph, st_id, ed_id, prior, allow_transfer, result_flight_no, &result_count, &result_total_time, &result_total_price);
    if (res == 1) {
        printf("Flight solution found (%d segment(s)):\n", result_count);
        for (int i = 0; i < result_count; ++i) {
            printf("  Flight: %s\n", result_flight_no[i]);
        }
        printf("Total time: %d min, total price: %.2f\n", result_total_time, result_total_price / 100.0);
    } else {
        printf("No solution found.\n");
    }
}

void app_run() {
    // system("clear");
    while (1) {
        print_menu();
        int choice;
        scanf("%d", &choice);
        switch (choice) {
            case 1: add_station(); break;
            case 2: delete_station(); break;
            case 3: add_train(); break;
            case 4: delete_train(); break;
            case 5: add_flight(); break;
            case 6: delete_flight(); break;
            case 7: query_train_city(); break;
            case 8: query_train_station(); break;
            case 9: query_flight_city(); break;
            case 0: printf("Bye!\n"); return;
            default: printf("Invalid choice.\n");
        }
        // system("pause");
        // system("clear");
    }
}
