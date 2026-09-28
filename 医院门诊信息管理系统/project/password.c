/*密码管理模块 实现系统密码的设置、验证、修改功能 刘孟灏2026-7-17
密码存储于pwd.dat，采用简易XOR加密
密码长度限制:6~16 位*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "password.h"
#include "utils.h"
#include "types.h"

/* XOR加密密钥 */
#define XOR_KEY 0x5A

/*密码加密解密 扩展
pwd-待加密解密的密码字符串
加密和解密使用同一函数*/
static void encryptPassword(char *pwd)
{
    int i;
    int len = (int)strlen(pwd);
    for (i = 0; i < len; i++) {
        pwd[i] ^= XOR_KEY;
    }
}

/*登录验证或首次设置密码
mgr-系统管理器指针
1成功 0失败
密码未设置 引导用户设置新密码
已设置 要求输入密码验证，最多3次*/
int loginOrSetPassword(SystemManager *mgr)
{
    char input[PWD_LEN];
    int attempts;
    int inputLen;

    /* 首次运行，密码未设置 */
    if (!mgr->isPasswordSet) {
        printf("————————————————————————————————————————————\n");
        printf("    首次使用系统，请设置登录密码\n");
        printf("————————————————————————————————————————————\n\n");

        while (1) {
            /* 第一次输入新密码 */
            readString("  请设置密码(6-16位): ", input, PWD_LEN);
            inputLen = (int)strlen(input);
            if (inputLen < PWD_MIN_LEN) {
                printf("  [错误] 密码长度必须在 %d~%d 位之间！\n",
                       PWD_MIN_LEN, PWD_MAX_LEN);
                continue;
            }
            if (inputLen > PWD_MAX_LEN) {
                printf("  [错误] 密码长度必须在 %d~%d 位之间！\n",
                       PWD_MIN_LEN, PWD_MAX_LEN);
                continue;
            }
            /* 确认密码 */
            {
                char confirm[PWD_LEN];
                readString("  请再次确认密码: ", confirm, PWD_LEN);
                if (strcmp(input, confirm) != 0) {
                    printf("  [错误] 两次输入不一致，请重新设置！\n");
                    continue;
                }
            }
            /* 密码校验通过，保存 */
            strcpy(mgr->password, input);
            mgr->isPasswordSet = 1;
            savePassword(mgr);
            printf("  [成功] 密码设置成功！\n");
            logOperation("系统首次设置密码");
            return 1;
        }
    }

    /* 已有密码，进行登录验证 */
    printf("————————————————————————————————————————————\n");
    printf("        医院门诊信息管理系统 登录\n");
    printf("————————————————————————————————————————————\n\n");

    for (attempts = 0; attempts < 3; attempts++) {
        readString("  请输入密码: ", input, PWD_LEN);
        if (strcmp(input, mgr->password) == 0) {
            printf("  [成功] 登录成功！\n");
            return 1;
        }
        /* 密码错误，提示剩余次数 */
        printf("  [错误] 密码错误！还剩%d次机会\n", 2 - attempts);
    }

    printf("  [失败] 密码错误次数过多，系统退出！\n");
    return 0;
}

/*修改密码
mgr-系统管理器指针
1成功0失败
验证旧密码 3次 输入新密码 长度校验 确认新密码 保存*/
int changePassword(SystemManager *mgr)
{
    char oldPwd[PWD_LEN];
    char newPwd[PWD_LEN];
    char confirmPwd[PWD_LEN];
    int attempts;
    int newLen;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("              修改系统密码\n");
    printf("————————————————————————————————————————————\n\n");

    /* 验证旧密码 最多3次 */
    for (attempts = 0; attempts < 3; attempts++) {
        readString("  请输入旧密码: ", oldPwd, PWD_LEN);
        if (strcmp(oldPwd, mgr->password) == 0) {
            break; /* 旧密码验证通过 */
        }
        printf("  [错误] 旧密码不正确！还剩 %d 次机会\n", 2 - attempts);
    }
    /* 旧密码验证失败 */
    if (attempts >= 3) {
        printf("  [失败] 旧密码错误次数过多！\n");
        return 0;
    }

    /* 输入新密码 校验长度 */
    while (1) {
        readString("  请输入新密码(6-16位): ", newPwd, PWD_LEN);
        newLen = (int)strlen(newPwd);
        if (newLen < PWD_MIN_LEN) {
            printf("  [错误] 密码长度必须在 %d~%d 位之间！\n",
                   PWD_MIN_LEN, PWD_MAX_LEN);
            continue;
        }
        if (newLen > PWD_MAX_LEN) {
            printf("  [错误] 密码长度必须在 %d~%d 位之间！\n",
                   PWD_MIN_LEN, PWD_MAX_LEN);
            continue;
        }
        break;
    }

    /* 确认新密码，两次必须一致 */
    readString("  请再次确认新密码: ", confirmPwd, PWD_LEN);
    if (strcmp(newPwd, confirmPwd) != 0) {
        printf("  [错误] 两次输入不一致，修改取消！\n");
        return 0;
    }

    /* 保存新密码到管理器并写入文件 */
    strcpy(mgr->password, newPwd);
    savePassword(mgr);
    printf("  [成功] 密码修改成功！\n");
    logOperation("修改系统密码");

    return 1;
}

/*从pwd.dat读取密码
mgr-系统管理器指针
1成功, 0文件不存在或读取失败
读取后自动解密 文件不存在时标记密码未设置*/
int loadPassword(SystemManager *mgr)
{
    FILE *fp = fopen(FILE_PWD, "r");
    if (fp == NULL) {
        /* 密码文件不存在，首次运行 */
        mgr->isPasswordSet = 0;
        mgr->password[0] = '\0';
        return 0;
    }

    /* 读取加密的密码 */
    if (fgets(mgr->password, PWD_LEN, fp) != NULL) {
        mgr->password[strcspn(mgr->password, "\n")] = '\0';
        /* 解密 */
        encryptPassword(mgr->password);
        mgr->isPasswordSet = 1;
    } else {
        mgr->isPasswordSet = 0;
        mgr->password[0] = '\0';
    }

    fclose(fp);
    return 1;
}

/*将密码保存到pwd.dat简易加密
mgr-系统管理器指针
1成功0失败
保存时加密，写入后恢复内存中的明文密码*/
int savePassword(SystemManager *mgr)
{
    FILE *fp;
    char encrypted[PWD_LEN];

    fp = fopen(FILE_PWD, "w");
    if (fp == NULL) {
        printf("  [错误] 无法创建密码文件！\n");
        return 0;
    }

    /* 复制一份用于加密，不修改内存中的明文密码 */
    strcpy(encrypted, mgr->password);
    encryptPassword(encrypted);
    fprintf(fp, "%s\n", encrypted);
    fclose(fp);

    return 1;
}