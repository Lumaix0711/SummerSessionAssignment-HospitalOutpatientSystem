/*统计管理模块接口声明 按条件统计出诊信息、挂号信息 刘孟灏2026-7-17*/
#ifndef STATISTICS_H
#define STATISTICS_H

#include "types.h"

/* 统计子菜单 选择统计类别 */
void statisticsMenu(SystemManager *mgr);

/* 按条件统计出诊信息 */
void statisticsDoctorDuty(SystemManager *mgr);

/* 按条件统计挂号信息 */
void statisticsRegistration(SystemManager *mgr);

#endif /* STATISTICS_H */
