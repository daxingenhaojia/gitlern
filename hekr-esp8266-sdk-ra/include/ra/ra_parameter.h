/**
* @file         ra_parameter.h
* @brief        参数存取操作接口
* @par Copyright (c):
*       Hangzhou District Nine Technology Co., Ltd.
*/

#ifndef __RA_PARAMETER_H__
#define __RA_PARAMETER_H__
#include "ra_types.h"

/** 设置整数类型参数
* @param[in]  key   参数名
* @param[in]  value 参数值
* @retval	0       设置成功
* @retval	-2      内存溢出
* @retval	-3      参数错误
*/
int ra_set_parameter_integer(const char *key, ra_int32_t value);

/** 获取整数类型参数
* @param[in]  key   参数名
* @param[out]  value   参数值
* @retval	0       获取成功
* @retval	-1      内部错误
*/
int ra_get_parameter_integer(const char *key, ra_int32_t *value);

/** 设置字符串类型参数
* @param[in]  key   参数名
* @param[in]  value 参数值
* @retval	0       设置成功
* @retval	-2      内存溢出
* @retval	-3      参数错误
*/
int ra_set_parameter_string(const char *key, const char *value);

/** 获取字符串类型参数
* @param[in]  key   参数名
* @param[out]  buf  缓冲区
* @param[in]  buf_len  缓冲区长度(包括结束符)
* @retval	非0     value实际长度
* @retval	-1      内部错误
* @retval	-3      参数错误
*/
int ra_get_parameter_string(const char *key, char *buf, ra_size_t buf_len);

/** 删除参数
* @param[in]  key   参数名
* @retval	0       获取成功
* @retval	-2      内存溢出
* @retval	-3       参数错误
*/
int ra_delete_parameter(const char *key);

#endif