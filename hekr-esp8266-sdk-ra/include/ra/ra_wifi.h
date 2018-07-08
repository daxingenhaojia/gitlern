/**
* @file         ra_wifi.h
* @brief        配网及连接路由接口
* @par Copyright (c):
*       Hangzhou District Nine Technology Co., Ltd.
*/

#ifndef __RA_WIFI_H__
#define __RA_WIFI_H__
#include "ra_types.h"

/**
* 配网的方式
*/
typedef enum
{
	RA_WIFI_CONFIG_TYPE_HEKR = 0,
	RA_WIFI_CONFIG_TYPE_AIRKISS
} ra_wifi_config_type_t;


/**
* ra_wifi_config_callback_t 函数原型定义
*/
typedef void(*ra_wifi_config_callback_t)(char *ssid, char *password);


/**
* 启动wifi config
* @param[in]	config_type
* @retval		无
*/
void ra_start_wifi_config(ra_wifi_config_type_t config_type);


/**
* 注册wifi config配网回调函数
* @param[in]	callback
* @retval		无
* @note		在回调函数中，配网失败则ssid返回NULL
*/
void ra_register_wifi_config_callback(ra_wifi_config_callback_t callback);

/**
* 连接WIFI接口
* @param[in]  ssid  WIFI热点名称
* @param[in]  password  WIFI热点密码
* @param[in]  bssid BSSID(可为NULL)
* @param[in]  timeout 连接超时时间（ms）
* @retval	0        开始连接成功
* @retval	-1       内部错误
* @retval	-3       参数错误
*/
int ra_connect_wifi(char *ssid, char *password, char *bssid, ra_uint32_t timeout);

/**
* 断开wifi连接
* @retval	0        开始连接成功
* @retval	-1       内部错误
*/
int ra_disconnect_wifi(void);

#endif