/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_led.h
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2020.6.6      v01         百问科技      创建文件
 *--------------------------------------------------
*/

#ifndef __DRIVER_LED_H
#define __DRIVER_LED_H

/*********************
 * 函数宏定义
**********************/
#define ON  (1)
#define OFF (0)

#define WHITE_LED_PORT      GPIOF
#define WHITE_LED_PIN       GPIO_PIN_7
#define WHILE_LED_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOF_CLK_ENABLE()


#define BLUE_LED_PORT       GPIOF
#define BLUE_LED_PIN        GPIO_PIN_8
#define BLUE_LED_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOF_CLK_ENABLE()

#define GREEN_LED_PORT      GPIOF
#define GREEN_LED_PIN       GPIO_PIN_9
#define GREEN_LED_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOF_CLK_ENABLE()

#define WHITE(VALUE)        HAL_GPIO_WritePin(WHITE_LED_PORT, WHITE_LED_PIN, (VALUE==ON)?GPIO_PIN_RESET:GPIO_PIN_SET)
#define BLUE(VALUE)         HAL_GPIO_WritePin(BLUE_LED_PORT, BLUE_LED_PIN, (VALUE==ON)?GPIO_PIN_RESET:GPIO_PIN_SET)
#define GREEN(VALUE)        HAL_GPIO_WritePin(GREEN_LED_PORT, GREEN_LED_PIN, (VALUE==ON)?GPIO_PIN_RESET:GPIO_PIN_SET)

extern void LED_GPIO_Init(void);

#endif /* __DRIVER_LED_H */