/*病人挂号管理模块实现 刘孟灏2026-7-17
实现病人门诊挂号、显示、文件读写
挂号单号格式YYYYMMDD+4位序号
挂号后同步更新doctor_duty
采用动态链表存储
存储格式:单号|日期|价格|医生编号|医生姓名|科室|病人编号|病人姓名*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registration.h"
#include "doctor_duty.h"
#include "utils.h"

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

/*解析一行管道符分隔的数据
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
    if (count < maxTokens) {
        strncpy(tokens[count], start, REMARK_LEN - 1);
        tokens[count][REMARK_LEN - 1] = '\0';
        count++;
    }

    return count;
}

/*统计指定日期已有挂号的最大序号
head-链表头指针，dateCompact-YYYYMMDD
返回当前最大序号，无记录时返回0
通过查找最大值确保序号唯一性*/
int getMaxSeqForDate(Registration *head, const char *dateCompact)
{
    Registration *current = head;
    int maxSeq = 0;
    int seq;

    while (current != NULL) {
        /*比较挂号单号前8位是否匹配指定日期*/
        if (strncmp(current->regId, dateCompact, 8) == 0) {
            /*提取后4位序号*/
            if (sscanf(current->regId + 8, "%d", &seq) == 1) {
                if (seq > maxSeq) {
                    maxSeq = seq;
                }
            }
        }
        current = current->next;
    }

    return maxSeq;
}

/*生成挂号单号
date-日期字符串 seq-序号 regId-输出缓冲区
输出YYYYMMDD+4位零填充序号 日期部分严格8位零填充*/
void generateRegId(const char *date, int seq, char *regId)
{
    int year, month, day;
    /* 解析 YYYY-M-D 格式的日期 */
    sscanf(date, "%d-%d-%d", &year, &month, &day);
    /* 生成 YYYYMMDD + %04d 格式的单号 */
    sprintf(regId, "%04d%02d%02d%04d", year, month, day, seq);
}

/*检查是否重复挂号
head-链表头，doctorId-医生编号，patientId-病人编号，date-日期
返回匹配的节点指针（存在重复）或NULL（无重复*/
Registration* findDuplicateReg(Registration *head, const char *doctorId,
                                const char *patientId, const char *date)
{
    Registration *current = head;
    while (current != NULL) {
        if (strcmp(current->doctorId, doctorId) == 0) {
            if (strcmp(current->patientId, patientId) == 0) {
                if (strcmp(current->regDate, date) == 0) {
                    return current;
                }
            }
        }
        current = current->next;
    }
    return NULL;
}

/*病人门诊挂号
mgr-系统管理器指针
返回1成功，0取消或失败
流程:1输入挂号日期、医生编号、病人编号、病人姓名
2校验医生当日出诊信息存在
3校验剩余号源>0
4检查是否重复挂号
5生成挂号单号 日期8位序号4位
6创建挂号记录插入链表
7更新duty已挂号人数+1
8保存文件，号源预警*/
int registerPatient(SystemManager *mgr)
{
    char regDate[DATE_LEN];
    char doctorId[ID_LEN];
    char patientId[ID_LEN];
    char patientName[NAME_LEN];
    int year, month, day;
    char dateCompact[9]; /*YYYYMMDD+'\0'*/
    int maxSeq, newSeq;
    DoctorDuty *duty;
    Registration *newReg;
    int remaining;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("          病人门诊挂号\n");
    printf("————————————————————————————————————————————\n\n");

    /*输入挂号日期*/
    readString("  挂号日期(YYYY-M-D): ", regDate, DATE_LEN);
    if (strcmp(regDate, "0") == 0) {
        printf("  [取消] 已取消挂号操作\n");
        return 0;
    }
    /*校验日期合法性*/
    if (sscanf(regDate, "%d-%d-%d", &year, &month, &day) != 3) {
        printf("  [错误] 日期格式不合法，请输入YYYY-M-D格式！\n");
        return 0;
    }
    if (!isValidDate(year, month, day)) {
        printf("  [错误] 日期格式不合法，请输入YYYY-M-D格式！\n");
        return 0;
    }

    /*输入医生编号和病人信息*/
    readString("  医生编号: ", doctorId, ID_LEN);
    readString("  病人编号: ", patientId, ID_LEN);
    readString("  病人姓名: ", patientName, NAME_LEN);

    /*查询该医生当日出诊信息*/
    duty = findDutyByDoctorAndDate(mgr->doctorDutyHead, doctorId, regDate);
    if (duty == NULL) {
        printf("  [错误] 无该医生当日出诊信息！\n");
        return 0;
    }

    /*计算剩余号源*/
    remaining = duty->maxPatients - duty->registeredCount;
    if (remaining <= 0) {
        printf("  [错误] 该医生当日号源已满，无法挂号！\n");
        return 0;
    }

    /* 检查是否重复挂号 同病人、同医生、同日期 */
    if (findDuplicateReg(mgr->registrationHead, doctorId,
                         patientId, regDate) != NULL) {
        printf("  [提示] 该病人当日已在该医生处挂号，不可重复挂号！\n");
        return 0;
    }

    /* 生成挂号单号: YYYYMMDD+4位序号 */
    sprintf(dateCompact, "%04d%02d%02d", year, month, day);
    maxSeq = getMaxSeqForDate(mgr->registrationHead, dateCompact);
    newSeq = maxSeq + 1;

    /* 动态创建挂号记录节点 */
    newReg = (Registration *)malloc(sizeof(Registration));
    if (newReg == NULL) {
        printf("  [错误] 内存分配失败！\n");
        return 0;
    }

    /* 填充挂号记录各字段 */
    generateRegId(regDate, newSeq, newReg->regId);
    strcpy(newReg->regDate, regDate);
    newReg->registerPrice = duty->registerPrice;
    strcpy(newReg->doctorId, duty->doctorId);
    strcpy(newReg->doctorName, duty->name);
    strcpy(newReg->department, duty->department);
    strcpy(newReg->patientId, patientId);
    strcpy(newReg->patientName, patientName);

    /* 头插法插入链表 */
    newReg->next = mgr->registrationHead;
    mgr->registrationHead = newReg;

    /* 更新出诊信息: 已挂号人数+1 */
    duty->registeredCount++;

    /* 保存两个文件 */
    saveRegistration(mgr);
    saveDoctorDuty(mgr);
    logOperation("病人门诊挂号");

    /* 显示挂号结果 */
    printf("\n  [成功] 挂号成功！\n");
    printf("  挂号单号: %s\n", newReg->regId);

    /* 号源预警（扩展功能） */
    remaining = duty->maxPatients - duty->registeredCount;
    checkQuotaWarning(remaining);

    return 1;
}

/*表格形式输出全部挂号信息
输出对齐的表格，包含所有挂号记录*/
void displayAllRegistration(SystemManager *mgr)
{
    Registration *current;
    int widths[] = {16, 14, 10, 10, 12, 12, 12, 14};
    char *headers[] = {"单号", "日期", "价格", "医生编号",
                       "医生姓名", "科室", "病人编号", "病人姓名"};
    int colCount = 8;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("          全部病人挂号信息\n");
    printf("————————————————————————————————————————————\n\n");

    current = mgr->registrationHead;
    if (current == NULL) {
        printf("  暂无挂号记录！\n");
        return;
    }

    printTableHeader(colCount, widths, headers);

    while (current != NULL) {
        printf("| %-*s", widths[0] - 1, current->regId);
        printf("| %-*s", widths[1] - 1, current->regDate);
        printf("| %-*.1f", widths[2] - 1, current->registerPrice);
        printf("| %-*s", widths[3] - 1, current->doctorId);
        printf("| %-*s", widths[4] - 1, current->doctorName);
        printf("| %-*s", widths[5] - 1, current->department);
        printf("| %-*s", widths[6] - 1, current->patientId);
        printf("| %-*s", widths[7] - 1, current->patientName);
        printf("|\n");
        current = current->next;
    }

    printTableLine(colCount, widths);
}

/*从register-sell.dat读取数据构建链表
1成功 0文件不存在
格式:单号|日期|价格|医生编号|医生姓名|科室|病人编号|病人姓名*/
int loadRegistration(SystemManager *mgr)
{
    FILE *fp;
    char line[LINE_LEN];
    char tokens[8][REMARK_LEN];
    int count;
    Registration *node;

    fp = fopen(FILE_REGISTRATION, "r");
    if (fp == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) {
            continue;
        }

        count = parseLine(line, tokens, 8);
        if (count < 8) {
            continue;
        }

        node = (Registration *)malloc(sizeof(Registration));
        if (node == NULL) {
            continue;
        }

        /* 填充挂号记录字段 */
        strcpy(node->regId, tokens[0]);
        strcpy(node->regDate, tokens[1]);
        node->registerPrice = manualAtof(tokens[2]);
        strcpy(node->doctorId, tokens[3]);
        strcpy(node->doctorName, tokens[4]);
        strcpy(node->department, tokens[5]);
        strcpy(node->patientId, tokens[6]);
        strcpy(node->patientName, tokens[7]);

        /* 头插法插入链表 */
        node->next = mgr->registrationHead;
        mgr->registrationHead = node;
    }

    fclose(fp);
    return 1;
}

/*将链表数据写入register-sell.dat
mgr-系统管理器指针
1成功，0失败*/
int saveRegistration(SystemManager *mgr)
{
    FILE *fp;
    Registration *current;

    fp = fopen(FILE_REGISTRATION, "w");
    if (fp == NULL) {
        printf("  [错误] 无法打开文件 %s！\n", FILE_REGISTRATION);
        return 0;
    }

    current = mgr->registrationHead;
    while (current != NULL) {
        fprintf(fp, "%s|%s|%.1f|%s|%s|%s|%s|%s\n",
                current->regId,
                current->regDate,
                current->registerPrice,
                current->doctorId,
                current->doctorName,
                current->department,
                current->patientId,
                current->patientName);
        current = current->next;
    }

    fclose(fp);
    return 1;
}
