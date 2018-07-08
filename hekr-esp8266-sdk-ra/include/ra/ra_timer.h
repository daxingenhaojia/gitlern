/**
* @file         ra_timer.h
* @brief        定时器操作接口
* @par Copyright (c):
*       Hangzhou District Nine Technology Co., Ltd.
*/

#ifndef __RA_TIMER_H__
#define __RA_TIMER_H__
#include "ra_types.h"


/**
* ra_timer_t 类型定义
*/
typedef ra_uint32_t ra_timer_t;

/**
* ra_timer_callback_t 函数原型定义
*/
typedef void(*ra_timer_callback_t)(ra_timer_t timer, void *arg);


/**
* 分配一个Timer
* @param[in]	timer	定时器指针
* @retval 0 成功
* @retval -1 内部错误
*/
int ra_timer_new(ra_timer_t *timer);

/**
* 释放一个Timer
* @param[in]	timer	定时器
* @retval 0 成功
* @retval -3 参数错误（可能原因：定时器不存在）
*/
int ra_timer_delete(ra_timer_t timer);

/**
* 设置定时器回调函数
* @param[in]	timer	定时器
* @param[in]	callback	定时器回调函数
* @param[in]	arg		回调函数的参数
* @retval 0 成功
* @retval -3 参数错误
*/
int ra_timer_set_callback(ra_timer_t timer, ra_timer_callback_t callback, void *arg);

/**
* 启动定时器
* @param[in]	timer		定时器
* @param[in]	interval		定时时间，单位ms
* @param[in]	repeated 定时器是否重复
* @retval 0 成功
* @retval -3 参数错误
*/
int ra_timer_start(ra_timer_t timer, ra_uint32_t interval, ra_bool repeated);

/**
* 停止定时器
* @param[in]  timer	定时器
* @retval 0 成功
* @retval -3 参数错误
*/
int ra_timer_stop(ra_timer_t timer);

/**
* 查询定时器剩余时间
* @param[in] timer 定时器
* @retval      		ra_uint32_t 剩余时间,单位ms
*/
ra_uint32_t ra_timer_remaining_time(ra_timer_t timer);

/**
* 查询定时器是否循环执行
* @param[in] timer 定时器
* @retval ra_bool 是否循环执行
*/
ra_bool ra_timer_is_repeated(ra_timer_t timer);
#endif