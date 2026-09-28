/* 医生出诊信息模块接口声明 刘孟灏2026-7-17
出诊排班生成、显示、文件读写、链表操作*/
#ifndef DOCTOR_DUTY_H
#define DOCTOR_DUTY_H

#include "types.h"

/* 生成医生出诊信息 */
int createDutyRecord(SystemManager *mgr);

/* 表格形式输出全部出诊信息 */
void displayAllDoctorDuty(SystemManager *mgr);

/* 从doctor_duty.dat加载数据到链表 */
int loadDoctorDuty(SystemManager *mgr);

/* 将链表数据保存到doctor_duty.dat */
int saveDoctorDuty(SystemManager *mgr);

/* 根据医生编号+日期查找出诊记录 */
DoctorDuty* findDutyByDoctorAndDate(DoctorDuty *head, const char *id, const char *date);

/* 删除指定医生的所有出诊记录 */
int deleteDutyByDoctorId(SystemManager *mgr, const char *doctorId);

#endif /* DOCTOR_DUTY_H */
