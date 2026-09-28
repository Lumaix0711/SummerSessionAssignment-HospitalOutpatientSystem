/* 密码管理模块接口声明 刘孟灏2026-7-17
密码设置、验证、修改、文件读写 */
#ifndef PASSWORD_H
#define PASSWORD_H

#include "types.h"

/* 登录验证或首次设置密码，返回1成功 0失败 */
int loginOrSetPassword(SystemManager *mgr);

/* 修改密码：验证旧密码 输入新密码 两次确认，返回1成功 */
int changePassword(SystemManager *mgr);

/* 从pwd.dat读取密码，返回1成功 0文件不存在 */
int loadPassword(SystemManager *mgr);

/* 将密码保存到pwd.dat，返回1成功 0失败 */
int savePassword(SystemManager *mgr);

#endif /* PASSWORD_H */
