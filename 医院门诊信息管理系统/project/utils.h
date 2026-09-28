/* 通用工具函数接口声明 刘孟灏2026-7-17
提供输入输出、日期校验、表格打印、日志、进度条等通用工具*/
#ifndef UTILS_H
#define UTILS_H

/* 清屏 */
void clearScreen(void);

/* 暂停等待用户按键 */
void pressAnyKeyToContinue(void);

/* 扩展 安全字符串输入 显示提示、去换行、防空输入 */
int readString(const char *prompt, char *buf, int maxLen);

/* 扩展 安全整数输入 含错误重试 */
int readInt(const char *prompt, int *value);

/* 扩展 安全浮点数输入 含错误重试 */
int readDouble(const char *prompt, double *value);

/* 判断闰年 */
int isLeapYear(int year);

/* 扩展 日期合法性校验 */
int isValidDate(int year, int month, int day);

/* 打印表格水平分隔线 */
void printTableLine(int colCount, int *widths);

/* 打印表格表头行 */
void printTableHeader(int colCount, int *widths, char **headers);

/* 扩展 操作日志写入log.txt */
void logOperation(const char *operation);

/*  扩展 字符进度条显示 */
void showProgressBar(int current, int total);

/*  扩展 剩余号源小于五时输出警告 */
void checkQuotaWarning(int remaining);

/* 扩展 实时系统时间显示 用于主菜单头部 */
void showCurrentDateTime(void);

#endif /* UTILS_H */
