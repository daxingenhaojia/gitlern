/**
* @file         ra_gpio.h
* @brief        gpio相关接口
* @par Copyright (c):
*       Hangzhou District Nine Technology Co., Ltd.
*/

#ifndef __RA_GPIO_H__
#define __RA_GPIO_H__
#include "ra_types.h"

/**
* IO口输入电平状态
*/
typedef enum
{
	/** 低电平 */
	RA_GPIO_LOW = 0,
	/** 高电平 */
	RA_GPIO_HIGH = 1
} ra_gpio_state_t;//高低电平 


/**
* ra_gpio_int_type_t 枚举类型定义
*/
typedef enum
{
	RA_GPIO_IRQ_TYPE_DISABLE = 0,
	RA_GPIO_IRQ_TYPE_POSEDGE,
	RA_GPIO_IRQ_TYPE_NEGEDGE,
	RA_GPIO_IRQ_TYPE_ANYEGDE,
	RA_GPIO_IRQ_TYPE_LOLEVEL,
	RA_GPIO_IRQ_TYPE_HILEVEL
} ra_gpio_irq_type_t;

typedef enum
{
	RA_GPIO_DIRECTION_INPUT,
	RA_GPIO_DIRECTION_OUTPUT,
} ra_gpio_direction_t;

/**
* ra_gpio_irq_callback_t 函数原型定义
*/
typedef void(*ra_gpio_irq_callback_t)(ra_uint8_t pin, void *arg);

/**
* 设置IO口输出电平
* 设置IO口输出高或者低电平
* @param[in]	pin		引脚
* @param[in]	value	电平
*/
void ra_gpio_output(ra_uint8_t pin, ra_gpio_state_t value);

/**
* 获取当前IO口输入电平
* @param[in]   pin 逻辑管脚号。
* @retval		电平
*/
ra_gpio_state_t ra_gpio_input(ra_uint8_t pin);

/**
* GPIO中断注册函数
* @param[in]	pin		引脚
* @param[in]	type
* @param[in]	callback
* @retval
*/
int ra_gpio_irq_register_callback(
	ra_uint8_t pin,
	ra_gpio_irq_type_t type,
	ra_gpio_irq_callback_t callback,
	void *arg);

/**
* GPIO模式设置
* @param[in]	pin		引脚
* @param[in]	mode
* @retval
*/
void ra_gpio_set_direction(ra_uint8_t pin, ra_gpio_direction_t direction);

/**
* GPIO设置上拉使能
* @param[in]	pin		引脚
* @param[in]	enabled  使能
* @retval
*/
void ra_gpio_set_pullup(ra_uint8_t pin, ra_bool enabled);


#endif