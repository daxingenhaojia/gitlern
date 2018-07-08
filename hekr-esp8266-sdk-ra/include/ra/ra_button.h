/**
* @file         ra_button.h
* @brief        按键接口
* @par Copyright (c):
*       Hangzhou District Nine Technology Co., Ltd.
*/

#ifndef __MODULE_KEY_H__
#define	__MODULE_KEY_H__
#include "ra_types.h"
#include <ra_gpio.h>
/**
* ra_button_pressed_callback_t 按键触发回调函数原型定义
*/
typedef void(*ra_button_pressed_callback_t)(void);

/**
* 注册按键中断处理函数
* @param[in]	pin					按键管脚序号
* @param[in]	irq_type			gpio中断类型
* @param[in]	long_pressed_threshold		长按阈值（毫秒）
* @param[in]	short_pressed_callback	短按处理函数指针
* @param[in]	long_pressed_callback	长按处理函数指针
* @retval		0	成功
* @retval		-1	失败
*/
int ra_register_button_irq_handler(
	ra_uint8_t pin,
	ra_gpio_irq_type_t irq_type,
	ra_size_t long_pressed_threshold,
	ra_button_pressed_callback_t short_pressed_callback,
	ra_button_pressed_callback_t long_pressed_callback);
#endif