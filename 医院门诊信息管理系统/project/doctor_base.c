/*医生基本信息模块实现 刘孟灏2026-7-17
实现医生基本信息的录入修改删除查询显示及doctor_base.dat文件的读写操作
采用动态链表存储，通过SystemManager传递头指针
存储格式:医生编号|姓名|科室|挂号价格|职称|是否专家|可挂号人数|备注*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doctor_base.h"
#include "doctor_duty.h"
#include "registration.h"
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
    int hasDecimal = 0;

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
        hasDecimal = 1;
        i++;
        while (str[i] >= '0' && str[i] <= '9') {
            fraction = fraction * 10.0 + (str[i] - '0');
            divisor = divisor * 10.0;
            i++;
        }
        if (hasDecimal) {
            result = result + fraction / divisor;
        }
    }

    return result * sign;
}

/*解析一行管道符分隔的数据
line-待解析的字符串, tokens-存储各字段的数组, maxTokens-最大字段数
返回实际解析到的字段数量
与strtok不同，本函数正确处理连续管道符*/
static int parseLine(char *line, char tokens[][REMARK_LEN], int maxTokens)
{
    int count = 0;
    char *start = line;
    char *p = line;

    while (*p != '\0' && count < maxTokens) {
        if (*p == '|') {
            *p = '\0';
            /* 将字段值复制到tokens数组 */
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

/*根据编号查找医生
head-链表头指针 id-目标医生编号
返回匹配的节点指针，未找到返回NULL*/
DoctorBase* findDoctorById(DoctorBase *head, const char *id)
{
    DoctorBase *current = head;
    while (current != NULL) {
        if (strcmp(current->doctorId, id) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/*校验医生输入字段
doc-待校验的医生信息指针
0通过 1失败
校验项: 编号/姓名/科室/职称非空、价格>0、人数>0、专家标志为0或1*/
int validateDoctorInput(DoctorBase *doc)
{
    /* 字段非空校验 */
    if (strlen(doc->doctorId) == 0) {
        printf("  [错误] 医生编号不能为空！\n");
        return 1;
    }
    if (strlen(doc->name) == 0) {
        printf("  [错误] 医生姓名不能为空！\n");
        return 1;
    }
    if (strlen(doc->department) == 0) {
        printf("  [错误] 科室不能为空！\n");
        return 1;
    }
    if (strlen(doc->title) == 0) {
        printf("  [错误] 职称不能为空！\n");
        return 1;
    }
    /* 挂号价格必须大于0 */
    if (doc->registerPrice <= 0) {
        printf("  [错误] 挂号价格必须大于0！\n");
        return 1;
    }
    /* 可挂号人数必须大于0 */
    if (doc->maxPatients <= 0) {
        printf("  [错误] 可挂号人数必须大于0！\n");
        return 1;
    }
    /* 是否专家只能是0或1 */
    if (doc->isExpert != 0) {
        if (doc->isExpert != 1) {
            printf("  [错误] 是否专家只能输入 0(否) 或 1(是)！\n");
            return 1;
        }
    }
    return 0;
}

/*录入医生基本信息
mgr系统管理器指针
1成功 0取消或失败
流程:输入各字段 校验 编号唯一性检查 创建节点 插入链表 保存文件*/
int addDoctorBase(SystemManager *mgr)
{
    DoctorBase *newNode;
    DoctorBase temp;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("          录入医生基本信息\n");
    printf("————————————————————————————————————————————\n\n");

    /* 逐个输入医生信息字段 */
    readString("  医生编号: ", temp.doctorId, ID_LEN);
    /* 输入0取消操作 */
    if (strcmp(temp.doctorId, "0") == 0) {
        printf("  [取消] 已取消录入操作\n");
        return 0;
    }

    readString("  医生姓名: ", temp.name, NAME_LEN);
    readString("  科室: ", temp.department, DEPT_LEN);
    readDouble("  挂号价格: ", &temp.registerPrice);
    readString("  职称: ", temp.title, TITLE_LEN);
    readInt("  是否专家(1是/0否): ", &temp.isExpert);
    readInt("  可挂号人数: ", &temp.maxPatients);
    readString("  备注(可输入'无'): ", temp.remark, REMARK_LEN);

    /* 字段校验：非空、价格>0、人数>0 */
    if (validateDoctorInput(&temp) != 0) {
        return 0;
    }

    /* 编号唯一性校验 */
    if (findDoctorById(mgr->doctorBaseHead, temp.doctorId) != NULL) {
        printf("  [错误] 医生编号已存在，保存失败！\n");
        return 0;
    }

    /* 动态创建链表节点 */
    newNode = (DoctorBase *)malloc(sizeof(DoctorBase));
    if (newNode == NULL) {
        printf("  [错误] 内存分配失败！\n");
        return 0;
    }

    /* 复制数据到新节点 */
    strcpy(newNode->doctorId, temp.doctorId);
    strcpy(newNode->name, temp.name);
    strcpy(newNode->department, temp.department);
    newNode->registerPrice = temp.registerPrice;
    strcpy(newNode->title, temp.title);
    newNode->isExpert = temp.isExpert;
    newNode->maxPatients = temp.maxPatients;
    strcpy(newNode->remark, temp.remark);

    /* 头插法插入链表 */
    newNode->next = mgr->doctorBaseHead;
    mgr->doctorBaseHead = newNode;

    /* 保存到文件并记录日志 */
    saveDoctorBase(mgr);
    logOperation("录入医生基本信息");
    printf("  [成功] 医生 %s(%s) 信息录入成功！\n",
           newNode->name, newNode->doctorId);

    return 1;
}

/*修改医生基本信息
mgr-系统管理器指针
1成功 0取消或失败
医生编号不可改*/
int modifyDoctorBase(SystemManager *mgr)
{
    char id[ID_LEN];
    DoctorBase *doc;
    double price;
    int value;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("          修改医生基本信息\n");
    printf("————————————————————————————————————————————\n\n");

    readString("  请输入要修改的医生编号: ", id, ID_LEN);
    if (strcmp(id, "0") == 0) {
        printf("  [取消] 已取消修改操作\n");
        return 0;
    }

    /* 根据编号定位医生 */
    doc = findDoctorById(mgr->doctorBaseHead, id);
    if (doc == NULL) {
        printf("  [错误] 未找到编号为 %s 的医生！\n", id);
        return 0;
    }

    /* 显示当前信息，逐个字段修改 */
    printf("\n  当前信息 → 姓名:%s 科室:%s 价格:%.1f 职称:%s\n",
           doc->name, doc->department, doc->registerPrice, doc->title);
    if (doc->isExpert) {
        printf("  专家:是 人数:%d 备注:%s\n\n", doc->maxPatients, doc->remark);
    } else {
        printf("  专家:否 人数:%d 备注:%s\n\n", doc->maxPatients, doc->remark);
    }
    printf("  (医生编号不可修改)\n\n");

    /* 修改姓名 */
    readString("  新姓名: ", doc->name, NAME_LEN);
    /* 修改科室 */
    readString("  新科室: ", doc->department, DEPT_LEN);
    /* 修改挂号价格 */
    readDouble("  新挂号价格: ", &price);
    if (price > 0) {
        doc->registerPrice = price;
    } else {
        printf("  [提示] 价格无效，保留原值\n");
    }
    /* 修改职称 */
    readString("  新职称: ", doc->title, TITLE_LEN);
    /* 修改专家标志 */
    readInt("  是否专家(1是/0否): ", &value);
    if (value == 0 || value == 1) {
        doc->isExpert = value;
    }
    /* 修改可挂号人数 */
    readInt("  新可挂号人数: ", &value);
    if (value > 0) {
        doc->maxPatients = value;
    } else {
        printf("  [提示] 人数无效，保留原值\n");
    }
    /* 修改备注 */
    readString("  新备注: ", doc->remark, REMARK_LEN);

    /* 保存修改到文件 */
    saveDoctorBase(mgr);
    logOperation("修改医生基本信息");
    printf("\n  [成功] 医生 %s 信息修改成功！\n", doc->doctorId);

    return 1;
}

/*删除医生基本信息
mgr-系统管理器指针
1成功 0取消或失败
检查挂号记录 有则拒绝 删除base节点 清理duty记录 保存*/

int deleteDoctorBase(SystemManager *mgr)
{
    char id[ID_LEN];
    DoctorBase *doc, *prev, *current;
    Registration *reg;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("          删除医生基本信息\n");
    printf("————————————————————————————————————————————\n\n");

    readString("  请输入要删除的医生编号: ", id, ID_LEN);//输入0退出
    if (strcmp(id, "0") == 0) {
        printf("  [取消] 已取消删除操作\n");
        return 0;
    }

    /* 查找目标医生 */
    doc = findDoctorById(mgr->doctorBaseHead, id);
    if (doc == NULL) {
        printf("  [错误] 未找到编号为 %s 的医生！\n", id);
        return 0;
    }

    /* 检查是否有挂号记录 */
    reg = mgr->registrationHead;
    while (reg != NULL) {
        if (strcmp(reg->doctorId, id) == 0) {
            printf("  [错误] 该医生已有挂号记录，无法删除！\n");//防止产生孤儿数据
            return 0;
        }
        reg = reg->next;
    }

    /* 从链表中删除医生节点 */
    current = mgr->doctorBaseHead;
    prev = NULL;
    while (current != NULL) {
        if (strcmp(current->doctorId, id) == 0) {
            /* 从链表中摘除节点 */
            if (prev == NULL) {
                mgr->doctorBaseHead = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }

    /* 保存医生基本信息 */
    saveDoctorBase(mgr);
    /* 同时清理该医生的出诊信息 */
    deleteDutyByDoctorId(mgr, id);
    logOperation("删除医生基本信息");
    printf("  [成功] 医生 %s 信息已删除！\n", id);

    return 1;
}

/*表格形式输出全部医生基本信息
mgr-系统管理器指针
输出对齐的表格，包含所有医生记录*/
void displayAllDoctorBase(SystemManager *mgr)
{
    DoctorBase *current;
    /* 表格列定义 */
    int widths[] = {10, 12, 12, 12, 14, 10, 12, 16};
    char *headers[] = {"编号", "姓名", "科室", "价格",
                       "职称", "专家", "可挂号", "备注"};
    int colCount = 8;

    clearScreen();
    printf("————————————————————————————————————————————\n");
    printf("        全部医生基本信息\n");
    printf("————————————————————————————————————————————\n\n");

    current = mgr->doctorBaseHead;
    if (current == NULL) {
        printf("  暂无医生基本信息记录！\n");
        return;
    }

    /* 打印表头 */
    printTableHeader(colCount, widths, headers);

    /* 遍历链表，逐行输出 */
    while (current != NULL) {
        printf("| %-*s", widths[0] - 1, current->doctorId);
        printf("| %-*s", widths[1] - 1, current->name);
        printf("| %-*s", widths[2] - 1, current->department);
        printf("| %-*.1f", widths[3] - 1, current->registerPrice);
        printf("| %-*s", widths[4] - 1, current->title);
        if (current->isExpert) {
            printf("| %-*s", widths[5] - 1, "是");
        } else {
            printf("| %-*s", widths[5] - 1, "否");
        }
        printf("| %-*d", widths[6] - 1, current->maxPatients);
        printf("| %-*s", widths[7] - 1, current->remark);
        printf("|\n");
        current = current->next;
    }

    /* 打印底部横线 */
    printTableLine(colCount, widths);
}

/*从doctor_base.dat读取数据构建链表
mgr-系统管理器指针
1成功 0文件不存在
逐行读取，解析管道符分隔的字段，头插法构建链表*/
int loadDoctorBase(SystemManager *mgr)
{
    FILE *fp;
    char line[LINE_LEN];
    /* tokens 数组用于存放解析出的各字段 */
    char tokens[8][REMARK_LEN];
    int count;
    DoctorBase *node;

    fp = fopen(FILE_DOCTOR_BASE, "r");
    if (fp == NULL) {
        return 0; /* 文件不存在，首次运行 */
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        /* 跳过空行 */
        if (strlen(line) == 0) {
            continue;
        }

        /* 解析一行数据 */
        count = parseLine(line, tokens, 8);
        if (count < 7) {
            continue; /* 字段数不足，跳过无效行 */
        }

        /* 分配新节点 */
        node = (DoctorBase *)malloc(sizeof(DoctorBase));
        if (node == NULL) {
            continue;
        }

        /* 填充节点各字段 */
        strcpy(node->doctorId, tokens[0]);
        strcpy(node->name, tokens[1]);
        strcpy(node->department, tokens[2]);
        node->registerPrice = manualAtof(tokens[3]);
        strcpy(node->title, tokens[4]);
        node->isExpert = manualAtoi(tokens[5]);
        node->maxPatients = manualAtoi(tokens[6]);
        /* 备注字段可能为空 */
        if (count >= 8) {
            strcpy(node->remark, tokens[7]);
        } else {
            node->remark[0] = '\0';
        }

        /* 头插法插入链表 */
        node->next = mgr->doctorBaseHead;
        mgr->doctorBaseHead = node;
    }

    fclose(fp);
    return 1;
}

/*将链表数据写入doctor_base.dat
mgr-系统管理器指针
1成功 0失败
格式:医生编号|姓名|科室|挂号价格|职称|是否专家|可挂号人数|备注*/
int saveDoctorBase(SystemManager *mgr)
{
    FILE *fp;
    DoctorBase *current;

    fp = fopen(FILE_DOCTOR_BASE, "w");
    if (fp == NULL) {
        printf("  [错误] 无法打开文件 %s！\n", FILE_DOCTOR_BASE);
        return 0;
    }

    current = mgr->doctorBaseHead;
    while (current != NULL) {
        /* 逐行写入，字段间用管道符分隔 */
        fprintf(fp, "%s|%s|%s|%.1f|%s|%d|%d|%s\n",
                current->doctorId,
                current->name,
                current->department,
                current->registerPrice,
                current->title,
                current->isExpert,
                current->maxPatients,
                current->remark);
        current = current->next;
    }

    fclose(fp);
    return 1;
}
