#ifndef _MODIFY_H
#define _MODIFY_H

#include <ctype.h>
#include "graph.h"
#include "hash.h"

/**
 * @param c_name name of city to find
 * @retval >0 id of the city
 * @retval 0 not found
 * @retval -1 fail
 */
int findCity(char *c_name);

/**
 * @param s_name name of station to find, regardless of checking if it's expired
 * @retval >0 id of the station
 * @retval 0 not found
 * @retval -1 fail
 */
int findStation(char *s_name);

/**
 * @param s_name name of station to find
 * @param c_name name of city of the station
 * @retval >0 id of the created station
 * @retval 0 already exist
 * @retval -1 fail
 */
int insertStation(char *s_name, char *c_name);

/**
 * @param c_name name of city to insert
 * @retval >0 id of the inserted city
 * @retval 0 if the city already exists
 * @retval -1 fail
 * @note This function will not check if the city already exists, it will always try to insert.
 *       If the city already exists, it will return 0.
 *       If the insertion fails, it will return -1.
 *       If the insertion is successful, it will return the ID of the inserted city.
 */
int insertCity(char *c_name);

/*
增加火车车次，先字符串查找车次（train_id）是否已经存在。如果存在且 `expire == 1`，则将该车次的 `expire` 赋为 0；如果 `expire == 0` 返回增加失败，信息为已经存在。
如果不存在，要求按格式输入：
第一行车站数 n，接下来一行 n 个字符串表示车站名，
必须都已经存在，接下来 n-1 行每行依次输入，
第 i 站到第 i+1 站这段的 出发时间(HH:MM)，到达时间(HH:MM)，票价（两位小数）。

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
下面这个函数是专为最后一种情况调用。
*/
/**
 * @brief Insert a train with the given parameters.
 * @param train_id The ID of the train, such as 1023, G102, ...
 * @param n The number of stations of the train
 * @param st_names An array of station names, each of which must already exist.
 * @param t_arr An array of arrival times at each station.
 * @param t_depart An array of departure times from each station.
 * @param price An array of prices for each segment between stations.
 * @retval >0 ID of the inserted train.
 * @retval 0 if the train already exists and is not expired.
 * @retval -1 if the insertion fails, such as if the train ID is invalid or
 *         if any of the station names do not exist.
 * @note The train ID must be formatted correctly, and the station names must
 *       already exist in the system. The function will check if the train ID
 *       is already in use and whether it is expired. If the train ID is valid
 *       and not already in use, it will insert the train and return its ID.
 *       If the train ID is already in use and not expired, it will return 0
 *       to indicate that the train already exists. If the train ID is expired,
 *       it will update the `expire` status to 0 and return the ID of the
 *       existing train.
 *       If the insertion fails for any reason, it will return -1.
 */
int insertTrain(char *train_id, int n, char **st_names, TimeType *t_arr, TimeType *t_depart, double *price);


/*
删除火车车次，先字符串查找车次（train_id）是否已经存在。如果存在且 `expire == 0`，则将该车次的 `expire` 赋为 1；否则失败，已经删除。
*/
/**
 * @brief Delete a station by name.
 * @param s_name The name of the station to delete.
 * @retval 1 if the station is successfully deleted.
 * @retval 0 if the station is inserted and expired.
 * @retval -1 if the station has never been inserted.
 */
int deleteStation(char *s_name);

/**
 * @brief Delete a train by its ID string.
 * @param train_id The train ID as a string.
 * @retval 1 if the train is successfully deleted.
 * @retval 0 if the train is inserted and expired.
 * @retval -1 if the train has never been inserted.
 */
int deleteTrain(char *train_id);

#endif