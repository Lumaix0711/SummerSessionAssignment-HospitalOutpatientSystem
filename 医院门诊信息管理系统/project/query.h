/* 查询管理模块接口声明 刘孟灏2026-7-17
多条件查询医生基本信息、出诊信息、病人挂号信息*/
#ifndef QUERY_H
#define QUERY_H

#include "types.h"

/* 查询子菜单 选择查询类别 */
void queryMenu(SystemManager *mgr);

/* 按条件查询医生基本信息 */
void queryDoctorBase(SystemManager *mgr);

/* 按条件查询出诊信息 */
void queryDoctorDuty(SystemManager *mgr);

/* 按条件查询挂号信息 */
void queryRegistration(SystemManager *mgr);

#endif /* QUERY_H */
