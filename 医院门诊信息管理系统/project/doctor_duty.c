/*医生出诊信息模块 刘孟灏2026-7-17
实现医生出诊排班的生成、显示、文件读写
出诊信息从医生基本信息中复制，已挂号人数初始为0
采用动态链表存储
存储格式:编号|日期|姓名|科室|价格|职称|专家|可挂号|已挂号*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doctor_duty.h"
#include "doctor_base.h"
#include "utils.h"

/*字符串转整数
str-待转换的字符串
返回转换后的整数值*/
static int manualAtoi(const char *str)
{
    int result = 0;
    int sign = 1;
    int i = 0;

    /* 跳过前导空格 */
    while (str[i] == ' ') {
        i++;
    }

    /* 处理符号 */
    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        i++;
    }

    /* 逐位转换数字 */
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }

    return result * sign;
}

/*字符串转浮点数
str-待转换的字符串
返回转换后的浮点数值*/
static double manualAtof(const char *str)
{
    double result = 0.0;
    double fraction = 0.0;
    double divisor = 1.0;
    int sign = 1;
    int i = 0;

    /* 跳过前导空格 */
    while (str[i] == ' ') {
        i++;
    }

    /* 处理符号 */
    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        i++;
    }

    /* 整数部分 */
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10.0 + (str[i] - '0');
        i++;
    }

    /* 小数部分 */
    if (str[i] == '.') {
        i++;
        while (str[i] >= '0' && str[i] <= '9') {
            fraction = fraction * 10.0 + (str[i] - '0');
            divisor = divisor * 10.0;
            i++;
        }
        result = result + fraction / divisor;
    }

    return result * sign;
}

/*解析管道符分隔的数据
line-待解析的字符串 tokens-存储各字段的数组 maxTokens-最大字段数
返回实际解析到的字段数量*/
static int parseLine(char *line, char tokens[][REMARK_LEN], int maxTokens)
{
    int count = 0;
    char *start = line;
    char *p = line;

    while (*p != '\0' && count < maxTokens) {
        if (*p == '|') {
            *p = '\0';
            strncpy(tokens[count], start, REMARK_LEN - 1);
            tokens[count][REMARK_LEN - 1] = '\0';
            count++;
            start = p + 1;
        }
        p++;
    }
    /* 处理最后一个字段 */
    if (count < maxTokens) {
        strncpy(tokens[count], start, REMARK_LEN - 1);
        tokens[count][REMARK_LEN - 1] = '\0';
        count++;
    }

    return count;
}

/* 根据编号+日期查找出诊记录
head-链表头指针 id-医生编号 date-值班日期
返回匹配的节点指针 未找到返回NULL*/
DoctorDuty* findDutyByDoctorAndDate(DoctorDuty *head, const char *id,
                                     const char *date)
{
    DoctorDuty *current = head;
    while (current != NULL) {
        /* 同时匹配医生编号和值班日期 */
        if (strcmp(current->doctorId, id) == 0) {
            if (strcmp(current->dutyDate, date) == 0) {
                return current;
            }
        }
        current = current->next;
    }
    return NULL;
}

/* 生成医生出诊信息
mgr-系统管理器指针
1成功 0取消或失败
流程:
输入医生编号和值班日期
校验医生编号在doctor_base中存在
校验该医生当日未排班
从doctor_base复制信息到新节点
已挂号人数初始为0
插入链表并保存文件*/
int createDutyRecord(SystemManager *mgr)
{
    char doctorId[ID_LEN];
    char dutyDate[DATE_LEN];
    int year, month, day;
    DoctorBase *doc;
    DoctorDuty *newNode;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("          生成医生出诊信息\n");
    printf("————————————————————————————————————————————\n\n");

    /* 输入医生编号 */
    readString("  医生编号: ", doctorId, ID_LEN);
    if (strcmp(doctorId, "0") == 0) {
        printf("  [取消] 已取消出诊生成操作\n");
        return 0;
    }

    /* 输入值班日期 */
    readString("  值班日期(YYYY-M-D): ", dutyDate, DATE_LEN);

    /* 日期格式合法性校验 */
    if (sscanf(dutyDate, "%d-%d-%d", &year, &month, &day) != 3) {
        printf("  [错误] 日期格式不合法，请输入 YYYY-M-D 格式！\n");
        return 0;
    }
    if (!isValidDate(year, month, day)) {
        printf("  [错误] 日期格式不合法，请输入 YYYY-M-D 格式！\n");
        return 0;
    }

    /* 校验医生编号在doctor_base.dat中是否存在 */
    doc = findDoctorById(mgr->doctorBaseHead, doctorId);
    if (doc == NULL) {
        printf("  [错误] 无此医生信息，生成失败！\n");
        return 0;
    }

    /* 校验该医生当日是否已排班 */
    if (findDutyByDoctorAndDate(mgr->doctorDutyHead, doctorId, dutyDate)
        != NULL) {
        printf("  [提示] 该医生当日排班已生成，无需重复创建\n");
        return 0;
    }

    /* 动态创建新的出诊记录节点 */
    newNode = (DoctorDuty *)malloc(sizeof(DoctorDuty));
    if (newNode == NULL) {
        printf("  [错误] 内存分配失败！\n");
        return 0;
    }

    /* 从医生基本信息中复制字段 */
    strcpy(newNode->doctorId, doc->doctorId);
    strcpy(newNode->dutyDate, dutyDate);
    strcpy(newNode->name, doc->name);
    strcpy(newNode->department, doc->department);
    newNode->registerPrice = doc->registerPrice;
    strcpy(newNode->title, doc->title);
    newNode->isExpert = doc->isExpert;
    newNode->maxPatients = doc->maxPatients;
    /* 已挂号人数固定初始为0 */
    newNode->registeredCount = 0;

    /* 头插法插入链表 */
    newNode->next = mgr->doctorDutyHead;
    mgr->doctorDutyHead = newNode;

    /* 保存到文件并记录日志 */
    saveDoctorDuty(mgr);
    logOperation("生成医生出诊信息");
    printf("  [成功] 医生 %s(%s) %s 出诊信息生成成功！\n",
           newNode->name, newNode->doctorId, dutyDate);

    return 1;
}

/* 表格形式输出全部出诊信息
mgr-系统管理器指针
输出对齐的表格，包含所有出诊记录，显示剩余号源*/
void displayAllDoctorDuty(SystemManager *mgr)
{
    DoctorDuty *current;
    /* 表格列定义 */
    int widths[] = {10, 14, 12, 12, 12, 14, 10, 10, 10, 10};
    char *headers[] = {"编号", "日期", "姓名", "科室", "价格", "职称", "专家", "可挂号", "已挂号", "剩余"};
    int colCount = 10;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("          全部医生出诊信息\n");
    printf("————————————————————————————————————————————\n\n");

    current = mgr->doctorDutyHead;
    if (current == NULL) {
        printf("  暂无医生出诊信息记录！\n");
        return;
    }

    /* 打印表头 */
    printTableHeader(colCount, widths, headers);

    /* 遍历链表，逐行输出 */
    while (current != NULL) {
        int remaining = current->maxPatients - current->registeredCount;
        printf("| %-*s", widths[0] - 1, current->doctorId);
        printf("| %-*s", widths[1] - 1, current->dutyDate);
        printf("| %-*s", widths[2] - 1, current->name);
        printf("| %-*s", widths[3] - 1, current->department);
        printf("| %-*.1f", widths[4] - 1, current->registerPrice);
        printf("| %-*s", widths[5] - 1, current->title);
        if (current->isExpert) {
            printf("| %-*s", widths[6] - 1, "是");
        } else {
            printf("| %-*s", widths[6] - 1, "否");
        }
        printf("| %-*d", widths[7] - 1, current->maxPatients);
        printf("| %-*d", widths[8] - 1, current->registeredCount);
        printf("| %-*d", widths[9] - 1, remaining);
        printf("|\n");
        current = current->next;
    }

    printTableLine(colCount, widths);
}

/* 从doctor_duty.dat读取数据构建链表
1成功 0文件不存在
格式:编号|日期|姓名|科室|价格|职称|专家|可挂号|已挂号*/
int loadDoctorDuty(SystemManager *mgr)
{
    FILE *fp;
    char line[LINE_LEN];
    char tokens[9][REMARK_LEN];
    int count;
    DoctorDuty *node;

    fp = fopen(FILE_DOCTOR_DUTY, "r");
    if (fp == NULL) {
        return 0; /* 文件不存在 */
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) {
            continue; /* 跳过空行 */
        }

        /* 解析一行数据 */
        count = parseLine(line, tokens, 9);
        if (count < 9) {
            continue; /* 字段数不足，跳过 */
        }

        /* 分配新节点 */
        node = (DoctorDuty *)malloc(sizeof(DoctorDuty));
        if (node == NULL) {
            continue;
        }

        /* 填充节点字段 */
        strcpy(node->doctorId, tokens[0]);
        strcpy(node->dutyDate, tokens[1]);
        strcpy(node->name, tokens[2]);
        strcpy(node->department, tokens[3]);
        node->registerPrice = manualAtof(tokens[4]);
        strcpy(node->title, tokens[5]);
        node->isExpert = manualAtoi(tokens[6]);
        node->maxPatients = manualAtoi(tokens[7]);
        node->registeredCount = manualAtoi(tokens[8]);

        /* 头插法插入链表 */
        node->next = mgr->doctorDutyHead;
        mgr->doctorDutyHead = node;
    }

    fclose(fp);
    return 1;
}

/* 将链表数据写入doctor_duty.dat
1成功 0失败*/
int saveDoctorDuty(SystemManager *mgr)
{
    FILE *fp;
    DoctorDuty *current;

    fp = fopen(FILE_DOCTOR_DUTY, "w");
    if (fp == NULL) {
        printf("  [错误] 无法打开文件 %s！\n", FILE_DOCTOR_DUTY);
        return 0;
    }

    current = mgr->doctorDutyHead;
    while (current != NULL) {
        /* 逐行写入9个字段 */
        fprintf(fp, "%s|%s|%s|%s|%.1f|%s|%d|%d|%d\n",
                current->doctorId,
                current->dutyDate,
                current->name,
                current->department,
                current->registerPrice,
                current->title,
                current->isExpert,
                current->maxPatients,
                current->registeredCount);
        current = current->next;
    }

    fclose(fp);
    return 1;
}

/*删除指定医生的所有出诊记录
doctorId-目标医生编号
返回删除的记录数量
删除医生基本信息时调用，同步清理出诊数据*/
int deleteDutyByDoctorId(SystemManager *mgr, const char *doctorId)
{
    DoctorDuty *current, *prev, *toDelete;
    int count = 0;

    /* 先处理链表头部连续匹配的情况 */
    while (mgr->doctorDutyHead != NULL) {
        if (strcmp(mgr->doctorDutyHead->doctorId, doctorId) != 0) {
            break;
        }
        toDelete = mgr->doctorDutyHead;
        mgr->doctorDutyHead = mgr->doctorDutyHead->next;
        free(toDelete);
        count++;
    }

    /* 遍历链表中间和尾部，删除匹配节点 */
    current = mgr->doctorDutyHead;
    prev = NULL;
    while (current != NULL) {
        if (strcmp(current->doctorId, doctorId) == 0) {
            /* 摘除当前节点 */
            prev->next = current->next;
            toDelete = current;
            current = current->next;
            free(toDelete);
            count++;
        } else {
            prev = current;
            current = current->next;
        }
    }

    /* 如果有删除记录，保存文件并记录日志 */
    if (count > 0) {
        saveDoctorDuty(mgr);
        logOperation("清理医生出诊信息");
    }

    return count;
}
