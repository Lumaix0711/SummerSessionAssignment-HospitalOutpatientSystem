/*主程序入口 刘孟灏2026-7-17
系统主入口，负责初始化，登录验证，主菜单循环
所有链表头指针封装在 SystemManager 结构体中
不使用全局变量，通过参数传递管理器*/

/* 扩展：
1.XOR密码加密 encryptPassword
2.日志记录(有时间戳) logOperation
3.进度条 showProgressBar
4.日期校验 isValidDate
5.防崩溃安全输入 readString/readInt/readDouble
6.号源不足自动预警 checkQuotaWarning
7.模糊搜索 manualStrstr
8.显示时间 showCurrentDateTime
9.一键清空测试数据 clearAllTestData
10.数据柱状图 printBarChart*/

#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "types.h"
#include "utils.h"
#include "password.h"
#include "doctor_base.h"
#include "doctor_duty.h"
#include "registration.h"
#include "query.h"
#include "statistics.h"
#include "report.h"

//显示系统主菜单
void showMainMenu(void)
{
    showCurrentDateTime();
    printf("\n");
    printf("————————————————————————————————————————————\n");
    printf("        医院门诊信息管理系统\n");
    printf("————————————————————————————————————————————\n");
    printf("  1. 设置及修改密码\n");
    printf("  2. 录入医生基本信息\n");
    printf("  3. 医生出诊信息的生成\n");
    printf("  4. 病人门诊挂号管理\n");
    printf("  5. 修改医生基本信息\n");
    printf("  6. 输出全部信息\n");
    printf("  7. 查询管理\n");
    printf("  8. 统计管理\n");
    printf("  9. 汇总报表\n");
    printf("  0. 退出系统\n");
    printf("————————————————————————————————————————————\n");
    printf("请选择功能(0-9): ");
}

//初始化管理器 mgr系统管理器 指针所有链表头指针置NULL密码清空
void initManager(SystemManager *mgr)
{
    mgr->doctorBaseHead = NULL;
    mgr->doctorDutyHead = NULL;
    mgr->registrationHead = NULL;
    mgr->password[0] = '\0';
    mgr->isPasswordSet = 0;
}

/*加载所有数据文件到内存链表
mgr-系统管理器指针
依次加载密码、医生信息、出诊信息、挂号信息*/
void loadAllData(SystemManager *mgr)
{
    /* 加载密码文件 */
    if (loadPassword(mgr)) {
        printf("  [加载] 密码数据加载成功\n");
    } else {
        printf("  [提示] 密码文件不存在，首次使用需设置密码\n");
    }
    /* 加载医生基本信息 */
    loadDoctorBase(mgr);
    /* 加载医生出诊排班 */
    loadDoctorDuty(mgr);
    /* 加载病人挂号信息 */
    loadRegistration(mgr);
}

/*保存所有链表数据到文件
mgr-系统管理器指针*/
void saveAllData(SystemManager *mgr)
{
    savePassword(mgr);
    saveDoctorBase(mgr);
    saveDoctorDuty(mgr);
    saveRegistration(mgr);
}

/*释放所有链表内存
mgr-系统管理器指针
遍历三条链表，逐个free每个节点*/
void freeAllLists(SystemManager *mgr)
{
    DoctorBase *docCur, *docNext;
    DoctorDuty *dutyCur, *dutyNext;
    Registration *regCur, *regNext;

    /* 释放医生基本信息链表 */
    docCur = mgr->doctorBaseHead;
    while (docCur != NULL) {
        docNext = docCur->next;
        free(docCur);
        docCur = docNext;
    }
    mgr->doctorBaseHead = NULL;

    /* 释放医生出诊信息链表 */
    dutyCur = mgr->doctorDutyHead;
    while (dutyCur != NULL) {
        dutyNext = dutyCur->next;
        free(dutyCur);
        dutyCur = dutyNext;
    }
    mgr->doctorDutyHead = NULL;

    /* 释放病人挂号信息链表 */
    regCur = mgr->registrationHead;
    while (regCur != NULL) {
        regNext = regCur->next;
        free(regCur);
        regCur = regNext;
    }
    mgr->registrationHead = NULL;
}

/*输出全部信息子菜单
mgr-系统管理器指针
选择输出医生信息、出诊信息或挂号信息*/
void displayOutputMenu(SystemManager *mgr)
{
    int subChoice;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("              输出全部信息\n");
    printf("————————————————————————————————————————————\n\n");
    printf("  1. 全部医生基本信息\n");
    printf("  2. 全部医生出诊信息\n");
    printf("  3. 全部病人挂号信息\n");
    printf("  4. 输出所有信息\n");
    printf("  0. 返回\n\n");
    printf("请选择: ");

    scanf("%d", &subChoice);
    while (getchar() != '\n');

    if (subChoice == 1) {
        displayAllDoctorBase(mgr);
    } else if (subChoice == 2) {
        displayAllDoctorDuty(mgr);
    } else if (subChoice == 3) {
        displayAllRegistration(mgr);
    } else if (subChoice == 4) {
        /* 依次输出三类信息 */
        displayAllDoctorBase(mgr);
        pressAnyKeyToContinue();
        displayAllDoctorDuty(mgr);
        pressAnyKeyToContinue();
        displayAllRegistration(mgr);
    } else if (subChoice == 0) {
        /* 返回，不做任何操作 */
    } else {
        printf("  [错误] 无效选择！\n");
    }
}

/*清空所有测试数据 扩展
二次确认后释放内存链表、清空数据文件和日志文件
对每个fopen进行NULL检查，打开失败时打印错误并继续
选项隐藏 输入88启动*/
void clearAllTestData(SystemManager *mgr)
{
    char confirm[REMARK_LEN];
    FILE *fp;

    printf("\n");
    printf("  ————————————————————————————————————————————\n");
    printf("  [警告] 此操作将彻底清空所有数据和日志文件！\n");
    printf("  包括doctor_base.dat, doctor_duty.dat,\n");
    printf("        register-sell.dat, log.txt\n");
    printf("  ————————————————————————————————————————————\n");
    printf("\n");
    readString("  请输入大写YES确认清空: ", confirm, REMARK_LEN);

    if (strcmp(confirm, "YES") != 0) {
        printf("  [提示] 操作已取消，数据未被清空。\n");
        return;
    }

    /* 释放所有链表内存 */
    freeAllLists(mgr);

    /* 显式再次置空三个头指针，确保安全 */
    mgr->doctorBaseHead = NULL;
    mgr->doctorDutyHead = NULL;
    mgr->registrationHead = NULL;

    /* 清空四个数据文件 */
    fp = fopen(FILE_DOCTOR_BASE, "w");
    if (fp != NULL) {
        fclose(fp);
    } else {
        printf("  [错误] 无法打开文件%s，跳过清空！\n", FILE_DOCTOR_BASE);
    }

    fp = fopen(FILE_DOCTOR_DUTY, "w");
    if (fp != NULL) {
        fclose(fp);
    } else {
        printf("  [错误] 无法打开文件%s，跳过清空！\n", FILE_DOCTOR_DUTY);
    }

    fp = fopen(FILE_REGISTRATION, "w");
    if (fp != NULL) {
        fclose(fp);
    } else {
        printf("  [错误] 无法打开文件%s，跳过清空！\n", FILE_REGISTRATION);
    }

    fp = fopen(FILE_LOG, "w");
    if (fp != NULL) {
        fclose(fp);
    } else {
        printf("  [错误] 无法打开文件%s，跳过清空！\n", FILE_LOG);
    }

    printf("  [成功] 所有测试数据及日志已彻底清空！\n");
}

/*修改医生信息子菜单
mgr-系统管理器指针
选择修改或删除医生基本信息*/
void modifyMenu(SystemManager *mgr)
{
    int subChoice;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("          修改医生基本信息\n");
    printf("————————————————————————————————————————————\n\n");
    printf("  1. 修改医生信息\n");
    printf("  2. 删除医生信息\n");
    printf("  0. 返回\n\n");
    printf("请选择: ");

    scanf("%d", &subChoice);
    while (getchar() != '\n');

    if (subChoice == 1) {
        modifyDoctorBase(mgr);
    } else if (subChoice == 2) {
        deleteDoctorBase(mgr);
    } else if (subChoice == 0) {
        /* 返回，不做任何操作 */
    } else {
        printf("  [错误] 无效选择！\n");
    }
}

/*程序主入口
流程:初始化管理器 加载数据 登录验证 主菜单循环 保存退出
返回0正常退出*/
int main(void)
{
    SystemManager mgr;
    int choice;

    /* 设置控制台编码为GBK 与编译器一样 */
#ifdef _WIN32
    SetConsoleOutputCP(936);  /* 控制台输出使用GBK */
    SetConsoleCP(936);        /* 控制台输入使用GBK */
#endif

    /* 初始化管理器 */
    initManager(&mgr);

    /* 从文件加载所有数据到内存链表 */
    printf("  系统启动中，正在加载数据...\n\n");
    loadAllData(&mgr);

    /* 登录验证或首次设置密码 */
    if (!loginOrSetPassword(&mgr)) {
        freeAllLists(&mgr);
        return 0;
    }

    /* 主菜单循环 */
    do {
        showMainMenu();
        scanf("%d", &choice);
        /* 清空输入缓冲区 */
        while (getchar() != '\n');

        if (choice == 1) {
            changePassword(&mgr);
        } else if (choice == 2) {
            addDoctorBase(&mgr);
        } else if (choice == 3) {
            createDutyRecord(&mgr);
        } else if (choice == 4) {
            registerPatient(&mgr);
        } else if (choice == 5) {
            modifyMenu(&mgr);
        } else if (choice == 6) {
            displayOutputMenu(&mgr);
        } else if (choice == 7) {
            queryMenu(&mgr);
        } else if (choice == 8) {
            statisticsMenu(&mgr);
        } else if (choice == 9) {
            generateMonthlyReport(&mgr);
        } else if (choice == 88) {
            clearAllTestData(&mgr);//隐藏选项 清除测试数据
        } else if (choice == 0) {
            /* 退出:保存数据 释放内存 */
            saveAllData(&mgr);
            freeAllLists(&mgr);
            printf("\n  所有数据已保存，系统正常退出。\n");
            logOperation("系统退出");
        } else {
            printf("  [错误] 无效选择，请重新输入！\n");
        }

        /* 每次操作后暂停，便于查看结果 */
        if (choice != 0) {
            pressAnyKeyToContinue();
        }

    } while (choice != 0);

    return 0;
}
