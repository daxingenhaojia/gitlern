/**
* @file         ra_utils.h
* @brief        通用接口
* @par Copyright (c):
*       Hangzhou District Nine Technology Co., Ltd.
*/

#ifndef __RA_UTILS_H__
#define __RA_UTILS_H__
#include "ra_types.h"

/**
* 通用类型回调原型
*/
typedef void(*ra_callback_t)(void *arg);

/**
* SDK初始化
*/
void ra_init(void *arg);

/**
* 查询系统剩余可用 heap 区空间大小
* @retval	ra_uint32_t	可用 heap 空间大小
*/
ra_uint32_t ra_get_free_heap_size(void);

/**
* 查询系统时间
* @retval	ra_uint32_t	系统上电时间，即上电至查询时刻的时间（微秒）
*/
ra_uint32_t ra_get_system_uptime(void);

/**查询devTid
* @retval	非NULL devTid
* @note 用完需要free
*/
char *ra_get_devtid(void);

/** 恢复至出厂默认设置
* @note 清除WIFI设置，并进入配置模式
*/
void ra_restore_to_factory_default_settings(void);

/** 系统重启
*/
void ra_system_reboot(void);

/** WIFI休眠
@retval	0	休眠成功
@retval	-1	休眠失败
*/
int ra_wifi_sleep(void);

/** WIFI唤醒
*/
void ra_wifi_wakeup(void);

/**
* 注册系统初始化完成的回调函数
* @param[in]	cb	回调函数
* @param[in]	arg	回调函数的参数
* @retval	无
* @note		ra_disconnect_wifi()须在系统初始化完成后调用
*/
void ra_register_system_init_done_callback(ra_callback_t cb, void *arg);

/**
* 设置固件当前版本号
* @param[in]	version	版本号，格式为"xx.xx.xx.xx"，如："4.1.1.1"
* @retval	0 设置成功
* @retval	-1 设置失败
* @note		version版本号格式必须是"xx.xx.xx.xx"，要不然影响OTA升级
*/
int ra_set_current_firmware_version(char *version);

/**
* 查询固件当前版本号
* @retval	'A'	当前固件运行在A区
* @retval	'B'	当前固件运行在B区
*/
char ra_get_current_firmware_type(void);
#endif