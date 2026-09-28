/* 病人挂号管理模块接口声明 刘孟灏2026-7-17
病人挂号、显示、文件读写、链表操作、挂号单号生成 */
#ifndef REGISTRATION_H
#define REGISTRATION_H

#include "types.h"

/* 病人挂号 校验出诊存在、号源大于0、生成单号、更新duty计数 */
int registerPatient(SystemManager *mgr);

/* 表格形式输出全部挂号信息 */
void displayAllRegistration(SystemManager *mgr);

/* 从register-sell.dat加载数据到链表 */
int loadRegistration(SystemManager *mgr);

/* 将链表数据保存到register-sell.dat */
int saveRegistration(SystemManager *mgr);

/* 生成挂号单号 YYYYMMDD+4位序号 */
void generateRegId(const char *date, int seq, char *regId);

/* 统计指定日期已有挂号的最大序号 */
int getMaxSeqForDate(Registration *head, const char *dateCompact);

/* 检查是否重复挂号 同病人、同医生、同日期 */
Registration* findDuplicateReg(Registration *head, const char *doctorId, const char *patientId, const char *date);

#endif /* REGISTRATION_H */
