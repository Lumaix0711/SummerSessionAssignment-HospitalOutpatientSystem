/*统计管理模块实现 刘孟灏2026-7-17
按条件统计医生出诊信息和病人挂号信息
出诊统计: 按科室/姓名/日期/职称/专家  可挂号/已挂号/剩余
挂号统计: 按日期/科室/医生/病人  挂号人数*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "statistics.h"
#include "utils.h"

/*子串查找
haystack-主字符串，needle-待查找的子串
返回第一次出现的位置指针，未找到返回NULL*/
static char* manualStrstr(const char *haystack, const char *needle)
{
    int i, j;
    int hayLen = (int)strlen(haystack);
    int neeLen = (int)strlen(needle);

    if (neeLen == 0) {
        return (char *)haystack;
    }

    for (i = 0; i <= hayLen - neeLen; i++) {
        int found = 1;
        for (j = 0; j < neeLen; j++) {
            if (haystack[i + j] != needle[j]) {
                found = 0;
                break;
            }
        }
        if (found) {
            return (char *)(haystack + i);
        }
    }

    return NULL;
}

/* 统计结果临时节点 */
typedef struct StatNode {
    char key[NAME_LEN];      /* 分组关键字 */
    int totalMax;            /* 可挂号总人数 */
    int totalRegistered;     /* 已挂号总人数 */
    int totalCount;          /* 挂号总人次 */
    struct StatNode *next;
} StatNode;

/*柱状图打印 扩展
titles-各分组名称数组, values-各分组对应数值数组, count-分组数量
按最大值等比缩放，最大宽度40个#字符*/
static void printBarChart(const char* titles[], int values[], int count)
{
    int maxVal = 0;
    int i, j;
    int barLen;

    if (count <= 0) {
        return;
    }

    /* 找出最大值 */
    for (i = 0; i < count; i++) {
        if (values[i] > maxVal) {
            maxVal = values[i];
        }
    }

    /* 全部为0时不绘制柱状图 */
    if (maxVal == 0) {
        printf("\n  --- 挂号人数统计柱状图 ---\n");
        printf("  (所有分组数据均为0，无法绘制柱状图)\n");
        return;
    }

    printf("\n  --- 挂号人数统计柱状图 ---\n");
    for (i = 0; i < count; i++) {
        /* 按比例计算柱状长度，最大40个字符 */
        barLen = (values[i] * 40) / maxVal;
        /* 值大于0时至少显示1个# */
        if (values[i] > 0 && barLen == 0) {
            barLen = 1;
        }
        printf("  %-10s | ", titles[i]);
        for (j = 0; j < barLen; j++) {
            printf("#");
        }
        printf(" (%d人)\n", values[i]);
    }
    printf("\n");
}

/*统计子菜单
选择统计类别*/
void statisticsMenu(SystemManager *mgr)
{
    int choice;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("              统计管理\n");
    printf("————————————————————————————————————————————\n\n");
    printf("  1. 统计医生出诊信息\n");
    printf("  2. 统计病人挂号信息\n");
    printf("  0. 返回主菜单\n");
    printf("\n请选择: ");

    scanf("%d", &choice);
    while (getchar() != '\n');

    if (choice == 1) {
        statisticsDoctorDuty(mgr);
    } else if (choice == 2) {
        statisticsRegistration(mgr);
    } else if (choice == 0) {
        /* 返回主菜单 */
    } else {
        printf("  [错误] 无效选择！\n");
    }
}

/*按条件统计出诊信息
统计条件:科室/姓名/日期/职称/是否专家
输出每组可挂号人数、已挂号人数、剩余号源数*/
void statisticsDoctorDuty(SystemManager *mgr)
{
    int field, found = 0;
    char searchStr[NAME_LEN];
    int searchInt;
    DoctorDuty *current;
    StatNode *statHead = NULL, *statCurrent;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("          统计医生出诊信息\n");
    printf("————————————————————————————————————————————\n\n");
    printf("  统计条件:\n");
    printf("  1.科室 2.姓名 3.日期 4.职称 5.是否专家\n\n");
    readInt("  请选择统计条件: ", &field);

    if (field == 5) {
        readInt("  请输入查询值(1是/0否): ", &searchInt);
    } else {
        readString("  请输入查询值: ", searchStr, NAME_LEN);
    }

    /* 遍历出诊链表，按条件分组聚合 */
    current = mgr->doctorDutyHead;
    while (current != NULL) {
        int match = 0;
        char keyValue[NAME_LEN] = "";

        /* 根据统计字段判断是否匹配 */
        if (field == 1) {
            match = (manualStrstr(current->department, searchStr) != NULL);
            strcpy(keyValue, current->department);
        } else if (field == 2) {
            match = (manualStrstr(current->name, searchStr) != NULL);
            strcpy(keyValue, current->name);
        } else if (field == 3) {
            match = (strcmp(current->dutyDate, searchStr) == 0);
            strcpy(keyValue, current->dutyDate);
        } else if (field == 4) {
            match = (manualStrstr(current->title, searchStr) != NULL);
            strcpy(keyValue, current->title);
        } else if (field == 5) {
            if (current->isExpert == searchInt) {
                match = 1;
            }
            if (searchInt) {
                strcpy(keyValue, "专家");
            } else {
                strcpy(keyValue, "非专家");
            }
        }

        if (match) {
            /* 在统计链表中查找或创建分组 */
            StatNode *stat = statHead;
            while (stat != NULL) {
                if (strcmp(stat->key, keyValue) == 0) {
                    break;
                }
                stat = stat->next;
            }
            /* 未找到则创建新的统计分组 */
            if (stat == NULL) {
                stat = (StatNode *)malloc(sizeof(StatNode));
                if (stat != NULL) {
                    strcpy(stat->key, keyValue);
                    stat->totalMax = 0;
                    stat->totalRegistered = 0;
                    stat->totalCount = 0;
                    stat->next = statHead;
                    statHead = stat;
                }
            }
            /* 累加统计数据 */
            if (stat != NULL) {
                stat->totalMax += current->maxPatients;
                stat->totalRegistered += current->registeredCount;
                found++;
            }
        }
        current = current->next;
    }

    /* 输出统计结果表格 */
    printf("\n");
    {
        int widths[] = {20, 14, 14, 14};
        char *headers[] = {"分组", "可挂号总数", "已挂号总数", "剩余号源"};
        printTableHeader(4, widths, headers);

        statCurrent = statHead;
        while (statCurrent != NULL) {
            int remaining = statCurrent->totalMax - statCurrent->totalRegistered;
            printf("| %-*s", widths[0]-1, statCurrent->key);
            printf("| %-*d", widths[1]-1, statCurrent->totalMax);
            printf("| %-*d", widths[2]-1, statCurrent->totalRegistered);
            printf("| %-*d", widths[3]-1, remaining);
            printf("|\n");
            statCurrent = statCurrent->next;
        }
        printTableLine(4, widths);
    }

    if (found == 0) {
        printf("  未找到符合条件的记录！\n");
    } else {
        printf("  共统计 %d 条记录\n", found);
    }

    /* 释放统计链表 */
    while (statHead != NULL) {
        StatNode *tmp = statHead;
        statHead = statHead->next;
        free(tmp);
    }
}

/* 按条件统计挂号信息
统计条件:日期/科室/医生姓名/病人姓名
输出每组的挂号人数*/
void statisticsRegistration(SystemManager *mgr)
{
    int field, found = 0;
    char searchStr[NAME_LEN];
    Registration *current;
    StatNode *statHead = NULL, *statCurrent;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("          统计病人挂号信息\n");
    printf("————————————————————————————————————————————\n\n");
    printf("  统计条件:\n");
    printf("  1.挂号日期 2.科室 3.医生姓名 4.病人姓名\n\n");
    readInt("  请选择统计条件: ", &field);
    readString("  请输入查询值: ", searchStr, NAME_LEN);

    /* 遍历挂号链表，按条件分组聚合 */
    current = mgr->registrationHead;
    while (current != NULL) {
        int match = 0;
        char keyValue[NAME_LEN] = "";

        if (field == 1) {
            match = (strcmp(current->regDate, searchStr) == 0);
            strcpy(keyValue, current->regDate);
        } else if (field == 2) {
            match = (manualStrstr(current->department, searchStr) != NULL);
            strcpy(keyValue, current->department);
        } else if (field == 3) {
            match = (manualStrstr(current->doctorName, searchStr) != NULL);
            strcpy(keyValue, current->doctorName);
        } else if (field == 4) {
            match = (manualStrstr(current->patientName, searchStr) != NULL);
            strcpy(keyValue, current->patientName);
        }

        if (match) {
            /* 在统计链表中查找或创建分组 */
            StatNode *stat = statHead;
            while (stat != NULL) {
                if (strcmp(stat->key, keyValue) == 0) {
                    break;
                }
                stat = stat->next;
            }
            if (stat == NULL) {
                stat = (StatNode *)malloc(sizeof(StatNode));
                if (stat != NULL) {
                    strcpy(stat->key, keyValue);
                    stat->totalMax = 0;
                    stat->totalRegistered = 0;
                    stat->totalCount = 0;
                    stat->next = statHead;
                    statHead = stat;
                }
            }
            if (stat != NULL) {
                stat->totalCount++;
                found++;
            }
        }
        current = current->next;
    }

    /* 输出统计结果 */
    printf("\n");
    {
        int widths[] = {20, 14};
        char *headers[] = {"分组", "挂号人数"};
        printTableHeader(2, widths, headers);

        statCurrent = statHead;
        while (statCurrent != NULL) {
            printf("| %-*s", widths[0]-1, statCurrent->key);
            printf("| %-*d", widths[1]-1, statCurrent->totalCount);
            printf("|\n");
            statCurrent = statCurrent->next;
        }
        printTableLine(2, widths);
    }

    /* 柱状图可视化 扩展 */
    if (found > 0) {
        const char *chartTitles[50];
        int chartValues[50];
        int chartCount = 0;
        StatNode *chartNode = statHead;
        while (chartNode != NULL && chartCount < 50) {
            chartTitles[chartCount] = chartNode->key;
            chartValues[chartCount] = chartNode->totalCount;
            chartCount++;
            chartNode = chartNode->next;
        }
        if (chartCount > 0) {
            printBarChart(chartTitles, chartValues, chartCount);
        }
    }

    if (found == 0) {
        printf("  未找到符合条件的记录！\n");
    } else {
        printf("  共统计 %d 条记录\n", found);
    }

    /* 释放统计链表 */
    while (statHead != NULL) {
        StatNode *tmp = statHead;
        statHead = statHead->next;
        free(tmp);
    }
}
