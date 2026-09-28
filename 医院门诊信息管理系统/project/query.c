/*查询管理模块 刘孟灏2026-7-17
实现多条件查询医生基本信息、出诊信息、病人挂号信息
查询结果以表格列表形式显示全部符合条件的记录*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "query.h"
#include "doctor_base.h"
#include "doctor_duty.h"
#include "utils.h"

/*子串查找即模糊查找 扩展
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

/*查询子菜单
mgr-系统管理器指针
显示查询选项，用户选择后跳转到对应查询功能*/
void queryMenu(SystemManager *mgr)
{
    int choice;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("              查询管理\n");
    printf("————————————————————————————————————————————\n\n");
    printf("  1. 查询医生基本信息\n");
    printf("  2. 查询医生出诊信息\n");
    printf("  3. 查询病人挂号信息\n");
    printf("  0. 返回主菜单\n");
    printf("\n请选择: ");

    scanf("%d", &choice);
    /* 清空输入缓冲区 */
    while (getchar() != '\n');

    /* 根据选择调用对应查询功能 */
    if (choice == 1) {
        queryDoctorBase(mgr);
    } else if (choice == 2) {
        queryDoctorDuty(mgr);
    } else if (choice == 3) {
        queryRegistration(mgr);
    } else if (choice == 0) {
        /* 返回主菜单 */
    } else {
        printf("  [错误] 无效选择！\n");
    }
}

/*按条件查询医生基本信息
mgr-系统管理器指针
查询条件:编号/姓名/科室/价格/职称/是否专家/可挂号人数
遍历链表，匹配条件后以表格形式显示全部结果*/

void queryDoctorBase(SystemManager *mgr)
{
    int field, found = 0;
    char searchStr[NAME_LEN];
    int searchInt;
    double searchDouble;
    DoctorBase *current;
    /* 表格列定义 */
    int widths[] = {10, 12, 12, 12, 14, 10, 12, 16};
    char *headers[] = {"编号", "姓名", "科室", "价格", "职称", "专家", "可挂号", "备注"};
    int colCount = 8;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("          查询医生基本信息\n");
    printf("————————————————————————————————————————————\n\n");
    printf("  查询条件:\n");
    printf("  1.医生编号 2.姓名 3.科室 4.价格\n");
    printf("  5.职称 6.是否专家 7.可挂号人数\n\n");
    readInt("  请选择查询条件: ", &field);

    /* 根据选择的字段获取查询值 */
    if (field == 4) {
        readDouble("  请输入查询价格: ", &searchDouble);
    } else if (field == 6 || field == 7) {
        readInt("  请输入查询值: ", &searchInt);
    } else {
        readString("  请输入查询值: ", searchStr, NAME_LEN);
    }

    /* 打印表头 */
    printf("\n");
    printTableHeader(colCount, widths, headers);

    /* 遍历链表，逐条匹配 */
    current = mgr->doctorBaseHead;
    while (current != NULL) {
        int match = 0;
        if (field == 1) {
            match = (strcmp(current->doctorId, searchStr) == 0);
        } else if (field == 2) {
            match = (manualStrstr(current->name, searchStr) != NULL);
        } else if (field == 3) {
            match = (manualStrstr(current->department, searchStr) != NULL);
        } else if (field == 4) {
            match = (current->registerPrice == searchDouble);
        } else if (field == 5) {
            match = (manualStrstr(current->title, searchStr) != NULL);
        } else if (field == 6) {
            match = (current->isExpert == searchInt);
        } else if (field == 7) {
            match = (current->maxPatients == searchInt);
        }
        /* 匹配成功则输出该行 */
        if (match) {
            printf("| %-*s", widths[0]-1, current->doctorId);
            printf("| %-*s", widths[1]-1, current->name);
            printf("| %-*s", widths[2]-1, current->department);
            printf("| %-*.1f", widths[3]-1, current->registerPrice);
            printf("| %-*s", widths[4]-1, current->title);
            if (current->isExpert) {
                printf("| %-*s", widths[5]-1, "是");
            } else {
                printf("| %-*s", widths[5]-1, "否");
            }
            printf("| %-*d", widths[6]-1, current->maxPatients);
            printf("| %-*s", widths[7]-1, current->remark);
            printf("|\n");
            found++;
        }
        current = current->next;
    }

    printTableLine(colCount, widths);
    if (found == 0) {
        printf("  未找到符合条件的记录！\n");
    } else {
        printf("  共找到 %d 条记录\n", found);
    }
}

/*按条件查询出诊信息
mgr-系统管理器指针
查询条件: 编号/日期/姓名/科室/价格/职称/专家/可挂号数/已挂号数*/
void queryDoctorDuty(SystemManager *mgr)
{
    int field, found = 0;
    char searchStr[NAME_LEN];
    int searchInt;
    double searchDouble;
    DoctorDuty *current;
    int widths[] = {10, 14, 12, 12, 12, 14, 10, 10, 10, 10};
    char *headers[] = {"编号", "日期", "姓名", "科室", "价格",
                       "职称", "专家", "可挂号", "已挂号", "剩余"};
    int colCount = 10;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("          查询医生出诊信息\n");
    printf("————————————————————————————————————————————\n\n");
    printf("  查询条件:\n");
    printf("  1.编号 2.日期 3.姓名 4.科室 5.价格\n");
    printf("  6.职称 7.专家 8.可挂号数 9.已挂号数\n\n");
    readInt("  请选择查询条件: ", &field);

    /* 根据字段类型获取查询值 */
    if (field == 5) {
        readDouble("  请输入查询价格: ", &searchDouble);
    } else if (field >= 7) {
        if (field <= 9) {
            readInt("  请输入查询值: ", &searchInt);
        } else {
            readString("  请输入查询值: ", searchStr, NAME_LEN);
        }
    } else {
        readString("  请输入查询值: ", searchStr, NAME_LEN);
    }

    printf("\n");
    printTableHeader(colCount, widths, headers);

    /* 遍历出诊链表，逐条匹配 */
    current = mgr->doctorDutyHead;
    while (current != NULL) {
        int match = 0;
        int remaining = current->maxPatients - current->registeredCount;
        if (field == 1) {
            match = (strcmp(current->doctorId, searchStr) == 0);
        } else if (field == 2) {
            match = (strcmp(current->dutyDate, searchStr) == 0);
        } else if (field == 3) {
            match = (manualStrstr(current->name, searchStr) != NULL);
        } else if (field == 4) {
            match = (manualStrstr(current->department, searchStr) != NULL);
        } else if (field == 5) {
            match = (current->registerPrice == searchDouble);
        } else if (field == 6) {
            match = (manualStrstr(current->title, searchStr) != NULL);
        } else if (field == 7) {
            match = (current->isExpert == searchInt);
        } else if (field == 8) {
            match = (current->maxPatients == searchInt);
        } else if (field == 9) {
            match = (current->registeredCount == searchInt);
        }
        if (match) {
            printf("| %-*s", widths[0]-1, current->doctorId);
            printf("| %-*s", widths[1]-1, current->dutyDate);
            printf("| %-*s", widths[2]-1, current->name);
            printf("| %-*s", widths[3]-1, current->department);
            printf("| %-*.1f", widths[4]-1, current->registerPrice);
            printf("| %-*s", widths[5]-1, current->title);
            if (current->isExpert) {
                printf("| %-*s", widths[6]-1, "是");
            } else {
                printf("| %-*s", widths[6]-1, "否");
            }
            printf("| %-*d", widths[7]-1, current->maxPatients);
            printf("| %-*d", widths[8]-1, current->registeredCount);
            printf("| %-*d", widths[9]-1, remaining);
            printf("|\n");
            found++;
        }
        current = current->next;
    }

    printTableLine(colCount, widths);
    if (found == 0) {
        printf("  未找到符合条件的记录！\n");
    } else {
        printf("  共找到%d条记录\n", found);
    }
}

/*按条件查询挂号信息
mgr-系统管理器指针
查询条件:单号/日期/价格/医生编号/医生姓名/科室/病人编号/病人姓名*/
void queryRegistration(SystemManager *mgr)
{
    int field, found = 0;
    char searchStr[NAME_LEN];
    double searchDouble;
    Registration *current;
    int widths[] = {16, 14, 10, 10, 12, 12, 12, 14};
    char *headers[] = {"单号", "日期", "价格", "医生编号", "医生姓名", "科室", "病人编号", "病人姓名"};
    int colCount = 8;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("          查询病人挂号信息\n");
    printf("————————————————————————————————————————————\n\n");
    printf("  查询条件:\n");
    printf("  1.单号 2.日期 3.价格 4.医生编号\n");
    printf("  5.医生姓名 6.科室 7.病人编号 8.病人姓名\n\n");
    readInt("  请选择查询条件: ", &field);

    if (field == 3) {
        readDouble("  请输入查询价格: ", &searchDouble);
    } else {
        readString("  请输入查询值: ", searchStr, NAME_LEN);
    }

    printf("\n");
    printTableHeader(colCount, widths, headers);

    /* 遍历挂号链表，逐条匹配 */
    current = mgr->registrationHead;
    while (current != NULL) {
        int match = 0;
        if (field == 1) {
            match = (strcmp(current->regId, searchStr) == 0);
        } else if (field == 2) {
            match = (strcmp(current->regDate, searchStr) == 0);
        } else if (field == 3) {
            match = (current->registerPrice == searchDouble);
        } else if (field == 4) {
            match = (strcmp(current->doctorId, searchStr) == 0);
        } else if (field == 5) {
            match = (manualStrstr(current->doctorName, searchStr) != NULL);
        } else if (field == 6) {
            match = (manualStrstr(current->department, searchStr) != NULL);
        } else if (field == 7) {
            match = (strcmp(current->patientId, searchStr) == 0);
        } else if (field == 8) {
            match = (manualStrstr(current->patientName, searchStr) != NULL);
        }
        if (match) {
            printf("| %-*s", widths[0]-1, current->regId);
            printf("| %-*s", widths[1]-1, current->regDate);
            printf("| %-*.1f", widths[2]-1, current->registerPrice);
            printf("| %-*s", widths[3]-1, current->doctorId);
            printf("| %-*s", widths[4]-1, current->doctorName);
            printf("| %-*s", widths[5]-1, current->department);
            printf("| %-*s", widths[6]-1, current->patientId);
            printf("| %-*s", widths[7]-1, current->patientName);
            printf("|\n");
            found++;
        }
        current = current->next;
    }

    printTableLine(colCount, widths);
    if (found == 0) {
        printf("  未找到符合条件的记录！\n");
    } else {
        printf("  共找到%d条记录\n", found);
    }
}