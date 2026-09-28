/*汇总报表模块实现 刘孟灏2026-7-17
以自然月为统计单位，生成两张独立报表
表1：各科室月度总挂号人次表 按科室升序
表2：科室医生月度挂号明细表 按科室→编号→月份升序，无数据不显示*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "report.h"
#include "utils.h"

/* 报表统计结果临时节点 */
typedef struct ReportNode {
    char department[DEPT_LEN]; /* 科室 */
    char doctorId[ID_LEN];     /* 医生编号 */
    int count;                  /* 挂号人次 */
    struct ReportNode *next;
} ReportNode;

/*汇总报表主函数
输入年份和月份，依次生成两张报表*/
void generateMonthlyReport(SystemManager *mgr)
{
    int year, month;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("              汇总报表\n");
    printf("————————————————————————————————————————————\n\n");

    /* 输入统计年份 */
    readInt("  请输入统计年份: ", &year);
    if (year < 1900) {
        printf("  [错误] 年份范围无效！\n");
        return;
    }
    if (year > 9999) {
        printf("  [错误] 年份范围无效！\n");
        return;
    }

    /* 输入统计月份 */
    readInt("  请输入统计月份(1-12): ", &month);
    if (month < 1) {
        printf("  [错误] 月份范围无效！\n");
        return;
    }
    if (month > 12) {
        printf("  [错误] 月份范围无效！\n");
        return;
    }

    printf("\n  正在生成%d年%d月汇总报表...\n", year, month);
    /* 显示进度条 扩展 */
    showProgressBar(1, 2);

    /* 表1 各科室月度总挂号人次 */
    reportDeptMonthlyTotal(mgr, year, month);
    showProgressBar(2, 2);

    /* 表2 科室医生月度挂号明细 */
    reportDoctorMonthlyDetail(mgr, year, month);
}

/* 各科室月度总挂号人次表
表头:| 科室 | 起始日期 | 终止日期 | 挂号人数 |
排序: 科室升序*/
void reportDeptMonthlyTotal(SystemManager *mgr, int year, int month)
{
    char startDate[DATE_LEN], endDate[DATE_LEN];
    Registration *current;
    ReportNode *reportHead = NULL, *node;
    int widths[] = {14, 14, 14, 12};
    char *headers[] = {"科室", "起始日期", "终止日期", "挂号人数"};

    /* 格式化月度起止日期 */
    formatDateRange(year, month, startDate, endDate);

    printf("\n");
    printf("————————————————————————————————————————————\n");
    printf("    各科室月度总挂号人次表\n");
    printf("    统计期间: %s ~ %s\n", startDate, endDate);
    printf("————————————————————————————————————————————\n\n");

    printTableHeader(4, widths, headers);

    /* 遍历挂号记录，按科室聚合统计 */
    current = mgr->registrationHead;
    while (current != NULL) {
        int rYear, rMonth, rDay;
        sscanf(current->regDate, "%d-%d-%d", &rYear, &rMonth, &rDay);
        /* 仅统计指定年月的记录 */
        if (rYear == year) {
            if (rMonth == month) {
                /* 查找已有科室或创建新分组 */
                node = reportHead;
                while (node != NULL) {
                    if (strcmp(node->department, current->department) == 0) {
                        break;
                    }
                    node = node->next;
                }
                if (node == NULL) {
                    node = (ReportNode *)malloc(sizeof(ReportNode));
                    if (node != NULL) {
                        strcpy(node->department, current->department);
                        node->doctorId[0] = '\0';
                        node->count = 0;
                        node->next = reportHead;
                        reportHead = node;
                    }
                }
                if (node != NULL) {
                    node->count++;
                }
            }
        }
        current = current->next;
    }

    /* 冒泡排序 按科室名升序 */
    {
        ReportNode *i, *j;
        char tempDept[DEPT_LEN];
        int tempCount;
        for (i = reportHead; i != NULL; i = i->next) {
            for (j = i->next; j != NULL; j = j->next) {
                if (strcmp(i->department, j->department) > 0) {
                    /* 交换科室名和计数 */
                    strcpy(tempDept, i->department);
                    strcpy(i->department, j->department);
                    strcpy(j->department, tempDept);
                    tempCount = i->count;
                    i->count = j->count;
                    j->count = tempCount;
                }
            }
        }
    }

    /* 输出排序后的结果 */
    node = reportHead;
    while (node != NULL) {
        printf("| %-*s", widths[0]-1, node->department);
        printf("| %-*s", widths[1]-1, startDate);
        printf("| %-*s", widths[2]-1, endDate);
        printf("| %-*d", widths[3]-1, node->count);
        printf("|\n");
        node = node->next;
    }

    printTableLine(4, widths);

    if (reportHead == NULL) {
        printf("  本月暂无挂号数据\n");
    }

    /* 释放统计链表 */
    while (reportHead != NULL) {
        ReportNode *tmp = reportHead;
        reportHead = reportHead->next;
        free(tmp);
    }
}

/* 科室医生月度挂号明细表
| 科室 | 医生编号 | 起始日期 | 终止日期 | 挂号人数 |
科室升序 医生编号升序 月份升序
无挂号数据的医生不展示*/
void reportDoctorMonthlyDetail(SystemManager *mgr, int year, int month)
{
    char startDate[DATE_LEN], endDate[DATE_LEN];
    Registration *current;
    ReportNode *reportHead = NULL, *node;
    int widths[] = {14, 14, 14, 14, 12};
    char *headers[] = {"科室", "医生编号", "起始日期", "终止日期", "挂号人数"};

    formatDateRange(year, month, startDate, endDate);

    printf("\n");
    printf("————————————————————————————————————————————\n");
    printf("    科室、医生月度挂号明细表\n");
    printf("    统计期间: %s ~ %s\n", startDate, endDate);
    printf("————————————————————————————————————————————\n\n");

    printTableHeader(5, widths, headers);

    /* 遍历挂号记录，按科室+医生编号聚合统计 */
    current = mgr->registrationHead;
    while (current != NULL) {
        int rYear, rMonth, rDay;
        sscanf(current->regDate, "%d-%d-%d", &rYear, &rMonth, &rDay);
        if (rYear == year) {
            if (rMonth == month) {
                /* 查找已有科室+医生分组 */
                node = reportHead;
                while (node != NULL) {
                    if (strcmp(node->department, current->department) == 0) {
                        if (strcmp(node->doctorId, current->doctorId) == 0) {
                            break;
                        }
                    }
                    node = node->next;
                }
                if (node == NULL) {
                    node = (ReportNode *)malloc(sizeof(ReportNode));
                    if (node != NULL) {
                        strcpy(node->department, current->department);
                        strcpy(node->doctorId, current->doctorId);
                        node->count = 0;
                        node->next = reportHead;
                        reportHead = node;
                    }
                }
                if (node != NULL) {
                    node->count++;
                }
            }
        }
        current = current->next;
    }

    /* 冒泡排序 科室升序 医生编号升序 */
    {
        ReportNode *i, *j;
        char tempDept[DEPT_LEN], tempId[ID_LEN];
        int tempCount;
        for (i = reportHead; i != NULL; i = i->next) {
            for (j = i->next; j != NULL; j = j->next) {
                int swap = 0;
                /* 先比较科室 */
                if (strcmp(i->department, j->department) > 0) {
                    swap = 1;
                } else if (strcmp(i->department, j->department) == 0) {
                    /* 科室相同，再比较医生编号 */
                    if (strcmp(i->doctorId, j->doctorId) > 0) {
                        swap = 1;
                    }
                }
                if (swap) {
                    /* 交换所有字段 */
                    strcpy(tempDept, i->department);
                    strcpy(i->department, j->department);
                    strcpy(j->department, tempDept);
                    strcpy(tempId, i->doctorId);
                    strcpy(i->doctorId, j->doctorId);
                    strcpy(j->doctorId, tempId);
                    tempCount = i->count;
                    i->count = j->count;
                    j->count = tempCount;
                }
            }
        }
    }

    /* 输出排序后的结果 */
    node = reportHead;
    while (node != NULL) {
        printf("| %-*s", widths[0]-1, node->department);
        printf("| %-*s", widths[1]-1, node->doctorId);
        printf("| %-*s", widths[2]-1, startDate);
        printf("| %-*s", widths[3]-1, endDate);
        printf("| %-*d", widths[4]-1, node->count);
        printf("|\n");
        node = node->next;
    }

    printTableLine(5, widths);

    if (reportHead == NULL) {
        printf("  本月暂无挂号数据\n");
    }

    /* 释放统计链表 */
    while (reportHead != NULL) {
        ReportNode *tmp = reportHead;
        reportHead = reportHead->next;
        free(tmp);
    }
}

//获取某月最后一天
int getMonthEndDay(int year, int month)
{
    /* 每月天数表 */
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    /* 二月特殊处理：闰年29天，平年28天 */
    if (month == 2) {
        if (isLeapYear(year)) {
            return 29;
        }
        return 28;
    }

    return daysInMonth[month];
}

/* 格式化月度起止日期字符串
startDate-输出起始日期
endDate-输出终止日期
YYYY-M-D*/
void formatDateRange(int year, int month, char *startDate, char *endDate)
{
    int endDay = getMonthEndDay(year, month);
    /* 起始日期固定为当月1日 */
    sprintf(startDate, "%d-%d-1", year, month);
    /* 终止日期为当月最后一天 */
    sprintf(endDate, "%d-%d-%d", year, month, endDay);
}