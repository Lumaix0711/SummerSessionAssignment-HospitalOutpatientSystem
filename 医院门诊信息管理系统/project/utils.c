/*通用工具函数实现 刘孟灏2026-7-17
实现清屏、安全输入、日期校验、表格打印、操作日志、进度条显示、号源预警等通用工具函数*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include "types.h"

/*清屏
用system("cls")或ANSI转义序列 跨平台兼容*/
void clearScreen(void)
{
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
#endif
}

/*暂停等待用户按键
清空输入缓冲区后等待用户按回车键*/
void pressAnyKeyToContinue(void)
{
    printf("\n按回车键继续...");
    /* 清空输入缓冲区中的残留字符 */
    while(getchar() != '\n');
    getchar();
}

/*判断闰年
1闰年 0平年*/
int isLeapYear(int year)
{
    if (year % 4 == 0) {
        if (year % 100 != 0) {
            return 1;
        } else {
            if (year % 400 == 0) {
                return 1;
            }
            return 0;
        }
    }
    return 0;
}

/*日期合法性校验 扩展
1合法0非法*/
int isValidDate(int year, int month, int day)
{
    /* 每月天数表 */
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    /* 校验月份范围 */
    if (month < 1) {
        return 0;
    }
    if (month > 12) {
        return 0;
    }

    /* 闰年 */
    if (month == 2) {
        if (isLeapYear(year)) {
            daysInMonth[2] = 29;
        }
    }

    /* 校验日期范围 */
    if (day < 1) {
        return 0;
    }
    if (day > daysInMonth[month]) {
        return 0;
    }

    return 1;
}

/*安全字符串输入 扩展
prompt-提示信息, buff-储缓冲区, maxLen-最大长度
1成功
显示提示符，读取一行输入，去除末尾换行符，空输入时重试*/
int readString(const char *prompt, char *buf, int maxLen)
{
    while (1) {
        printf("%s", prompt);
        fflush(stdout);
        if (fgets(buf, maxLen, stdin) == NULL) {
            buf[0] = '\0';
            continue;
        }
        /* 去除末尾换行符 */
        buf[strcspn(buf, "\n")] = '\0';
        /* 空输入时提示重试 */
        if (strlen(buf) == 0) {
            printf("  [错误] 输入不能为空，请重新输入！\n");
            continue;
        }
        return 1;
    }
}

/*安全整数输入 扩展
prompt-提示信息, value-存储整数的指针
1成功
读取整数输入，非法字符时提示重试*/
int readInt(const char *prompt, int *value)
{
    char buf[LINE_LEN];
    while (1) {
        printf("%s", prompt);
        fflush(stdout);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            continue;
        }
        buf[strcspn(buf, "\n")] = '\0';
        /* 尝试解析整数，失败则重试 */
        if (sscanf(buf, "%d", value) != 1) {
            printf("  [错误] 请输入有效的整数！\n");
            continue;
        }
        return 1;
    }
}

/*安全浮点数输入 扩展
prompt-提示信息, value-存储浮点数的指针
1成功
读取浮点数输入，非法字符时提示重试*/
int readDouble(const char *prompt, double *value)
{
    char buf[LINE_LEN];
    while (1) {
        printf("%s", prompt);
        fflush(stdout);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            continue;
        }
        buf[strcspn(buf, "\n")] = '\0';
        /* 尝试解析浮点数，失败则重试 */
        if (sscanf(buf, "%lf", value) != 1) {
            printf("  [错误] 请输入有效的数字！\n");
            continue;
        }
        return 1;
    }
}

/*打印表格水平分隔线
colCount-列数, widths-各列宽度数组
输出分隔线*/
void printTableLine(int colCount, int *widths)
{
    int i, j;
    for (i = 0; i < colCount; i++) {
        printf("+");
        for (j = 0; j < widths[i]; j++) {
            printf("-");
        }
    }
    printf("+\n");
}

/*打印表格表头行
colCount-列数, widths-各列宽度, headers-表头文字
输出:| 标题1 | 标题2 |*/
void printTableHeader(int colCount, int *widths, char **headers)
{
    int i;
    /* 先打印分隔线 */
    printTableLine(colCount, widths);
    /* 打印表头文字，左对齐 */
    for (i = 0; i < colCount; i++) {
        printf("| %-*s", widths[i] - 1, headers[i]);
    }
    printf("|\n");
    /* 打印下分隔线 */
    printTableLine(colCount, widths);
}

/*操作日志写入log.txt 扩展
operation-操作描述文字
将时间戳和操作描述追加写入日志文件*/
void logOperation(const char *operation)
{
    FILE *fp;
    time_t now;
    struct tm *t;

    fp = fopen(FILE_LOG, "a");
    if (fp == NULL) {
        return; /* 日志文件打开失败时静默跳过 */
    }

    now = time(NULL);
    if (now == (time_t)-1) {
        fclose(fp);
        return; /* time()失败时关闭文件并跳过 */
    }

    t = localtime(&now);
    if (t == NULL) {
        fclose(fp);
        return; /* localtime()失败时关闭文件并跳过 */
    }

    /* 写入格式: [YYYY-MM-DD HH:MM:SS] 操作描述 */
    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec,
            operation);

    fclose(fp);
}

/*字符进度条显示 扩展
current-当前进度值 total-总进度值
输出进度条*/
void showProgressBar(int current, int total)
{
    int percent, filled, i;
    int barWidth = 30; /* 进度条总宽度 */

    if (total <= 0) {
        return;
    }

    percent = (current * 100) / total;
    filled = (current * barWidth) / total;//计算已填充的字符数

    printf("\r  [");//原地刷新
    for (i = 0; i < barWidth; i++) {
        if (i < filled) {
            printf("—");
        } else {
            if (i == filled) {
                printf(">");
            } else {
                printf(" ");
            }
        }
    }
    printf("] %d%%", percent);

    if (current >= total) {
        printf("\n");
    }
    fflush(stdout);//强制刷新输出 避免被缓冲
}

/*号源预警 扩展
remaining-剩余号源数
剩余号源少于QUOTA_WARNING(5)时输出警告提示*/
void checkQuotaWarning(int remaining)
{
    if (remaining < QUOTA_WARNING) {
        if (remaining > 0) {
            printf("\n  [预警] 该医生剩余号源不足%d个，请及时关注！\n", QUOTA_WARNING);
        } else {
            if (remaining == 0) {
                printf("\n  [预警] 该医生当日号源已全部用完！\n");
            }
        }
    }
}

/*实时系统时间显示 扩展
获取并显示当前系统日期和时间，用于主菜单头部展示
对time()和localtime()返回值进行NULL检查，防止崩溃*/
void showCurrentDateTime(void)
{
    time_t now;
    struct tm *t;

    now = time(NULL);
    if (now == (time_t)-1) {
        printf("——————————————————————————————————————————\n");
        printf("       医院门诊信息管理系统 V2.0\n");
        printf("  当前系统时间: 获取失败\n");
        printf("——————————————————————————————————————————\n");
        return;
    }

    t = localtime(&now);
    if (t == NULL) {
        printf("——————————————————————————————————————————\n");
        printf("       医院门诊信息管理系统 V2.0\n");
        printf("  当前系统时间: 获取失败\n");
        printf("——————————————————————————————————————————\n");
        return;
    }

    printf("——————————————————————————————————————————\n");
    printf("       医院门诊信息管理系统 V2.0\n");
    printf("  当前系统时间: %04d-%02d-%02d %02d:%02d:%02d\n",
           t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
           t->tm_hour, t->tm_min, t->tm_sec);
    printf("——————————————————————————————————————————\n");
}