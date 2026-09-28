/* 汇总报表模块接口声明 刘孟灏2026-7-17
以自然月为统计单位，生成科室总挂号人次表和医生挂号明细表 */
#ifndef REPORT_H
#define REPORT_H

#include "types.h"

/* 汇总报表主函数：输入年份+月份，生成两张报表 */
void generateMonthlyReport(SystemManager *mgr);

/* 各科室月度总挂号人次表 按科室升序 */
void reportDeptMonthlyTotal(SystemManager *mgr, int year, int month);

/* 科室医生月度挂号明细表 按科室、编号、月份升序，无数据不显示 */
void reportDoctorMonthlyDetail(SystemManager *mgr, int year, int month);

/* 获取某月最后一天 */
int getMonthEndDay(int year, int month);

/* 格式化月度起止日期字符串 */
void formatDateRange(int year, int month, char *startDate, char *endDate);

#endif /* REPORT_H */
