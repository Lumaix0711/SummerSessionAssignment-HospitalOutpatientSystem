/*医生基本信息模块接口声明 刘孟灏2026-7-17*/
#ifndef DOCTOR_BASE_H
#define DOCTOR_BASE_H

#include "types.h"

/* 录入医生基本信息 */
int addDoctorBase(SystemManager *mgr);

/* 修改医生基本信息 */
int modifyDoctorBase(SystemManager *mgr);

/* 删除医生 */
int deleteDoctorBase(SystemManager *mgr);

/* 表格形式输出全部医生基本信息 */
void displayAllDoctorBase(SystemManager *mgr);

/* 从doctor_base.dat加载数据到链表 */
int loadDoctorBase(SystemManager *mgr);

/* 将链表数据保存到doctor_base.dat */
int saveDoctorBase(SystemManager *mgr);

/* 根据编号查找医生，返回节点指针或NULL */
DoctorBase* findDoctorById(DoctorBase *head, const char *id);

/* 校验医生输入字段：非空、价格>0、人数>0 */
int validateDoctorInput(DoctorBase *doc);

#endif /* DOCTOR_BASE_H */