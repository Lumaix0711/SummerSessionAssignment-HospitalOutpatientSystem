/* 公共类型定义 刘孟灏2026-7-17
定义系统所有结构体类型、常量宏
SystemManager封装所有链表头指针 不使用全局变量 */
#ifndef TYPES_H
#define TYPES_H

/* 文件名常量 */
#define FILE_PWD          "pwd.dat"           /* 密码文件 */
#define FILE_DOCTOR_BASE  "doctor_base.dat"   /* 医生基本信息文件 */
#define FILE_DOCTOR_DUTY  "doctor_duty.dat"   /* 医生出诊排班文件 */
#define FILE_REGISTRATION "register-sell.dat" /* 病人挂号信息文件 */
#define FILE_LOG          "log.txt"           /* 操作日志文件 扩展 */

/* 字段长度常量 */
#define ID_LEN      20   /* 编号最大长度 */
#define NAME_LEN    30   /* 姓名最大长度 */
#define DEPT_LEN    30   /* 科室最大长度 */
#define TITLE_LEN   20   /* 职称最大长度 */
#define REMARK_LEN  100  /* 备注最大长度 */
#define DATE_LEN    11   /* 日期最大长度(YYYY-MM-DD + '\0') */
#define PWD_LEN     17   /* 密码最大长度(16 + '\0') */
#define REG_ID_LEN  20   /* 挂号单号最大长度 */
#define LINE_LEN    512  /* 文件读取行缓冲区大小 */

/* 密码长度限制 */
#define PWD_MIN_LEN 6    /* 密码最小长度 */
#define PWD_MAX_LEN 16   /* 密码最大长度 */

/* 号源预警阈值 扩展 */
#define QUOTA_WARNING 5  /* 剩余号源少于此值时触发预警 */

/*医生基本信息链表节点
存储文件doctor_base.dat
doctorId医生编号
name医生姓名
department所属科室
registerPrice挂号价格
title职称
isExpert是否专家（1是 0否）
maxPatients可挂号人数上限
remark备注信息
next链表下一节点指针*/
typedef struct DoctorBase {
    char doctorId[ID_LEN];
    char name[NAME_LEN];
    char department[DEPT_LEN];
    double registerPrice;
    char title[TITLE_LEN];
    int isExpert;
    int maxPatients;
    char remark[REMARK_LEN];
    struct DoctorBase *next;
} DoctorBase;

/*医生出诊信息链表节点
存储doctor_duty.dat
生成时从DoctorBase复制姓名、科室、价格、职称、专家标志、可挂号人数
registeredCount初始为0，每次挂号后+1*/
typedef struct DoctorDuty {
    char doctorId[ID_LEN];
    char dutyDate[DATE_LEN];       /* 值班日期 格式YYYY-M-D */
    char name[NAME_LEN];
    char department[DEPT_LEN];
    double registerPrice;
    char title[TITLE_LEN];
    int isExpert;
    int maxPatients;               /* 可挂号人数上限 */
    int registeredCount;           /* 已挂号人数 */
    struct DoctorDuty *next;
} DoctorDuty;

/*病人挂号信息链表节点
存储register-sell.dat
挂号单号格式YYYYMMDD+4位序号
日期部分严格8位零填充，序号部分4位零填充*/
typedef struct Registration {
    char regId[REG_ID_LEN];        /* 挂号单号 */
    char regDate[DATE_LEN];        /* 挂号日期 格式YYYY-M-D */
    double registerPrice;
    char doctorId[ID_LEN];
    char doctorName[NAME_LEN];
    char department[DEPT_LEN];
    char patientId[ID_LEN];        /* 病人编号 */
    char patientName[NAME_LEN];    /* 病人姓名 */
    struct Registration *next;
} Registration;

/*系统数据管理器
封装所有链表头指针和密码信息
通过函数参数传递，严格遵守"不使用全局变量"的代码规范*/
typedef struct SystemManager {
    DoctorBase   *doctorBaseHead;  /* 医生基本信息链表头 */
    DoctorDuty   *doctorDutyHead;  /* 医生出诊信息链表头 */
    Registration *registrationHead;/* 病人挂号信息链表头 */
    char password[PWD_LEN];        /* 系统密码 */
    int isPasswordSet;             /* 密码是否已设置 1已设 0未设 */
} SystemManager;

#endif /* TYPES_H */