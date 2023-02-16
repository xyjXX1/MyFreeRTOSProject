/** @file		driver_led.c
  * @author 	百问网团队
  * @version	v1.0
  * @date		2021-8-10
  * *******************************************
  * @brief		LED驱动源文件
  * 			+
  *
  * @include	包含的头文件
  * 			+ main.h
  * 			+ driver_led.h
  * -------------------------------------------
  * *******************************************
  * -------------------------------------------
  * @attention
  *
  * Copyright (c) 深圳百问网科技有限公司
  * All rights reserved.
  *
  */

#include "driver_led.h"
#include "main.h"

/**
  * @brief  Initializes the LED GPIO in LED_GPIO_Init().
  * @param  None
  * @param  None
  * @retval None
  */
void LED_GPIO_Init(void)
{
    GPIO_InitTypeDef led_gpio;
    
    WHILE_LED_GPIO_CLK_ENABLE();
    BLUE_LED_GPIO_CLK_ENABLE();
    GREEN_LED_GPIO_CLK_ENABLE();
    
    led_gpio.Mode = GPIO_MODE_OUTPUT_PP;        // 将LED的GPIO模式设置为推挽输出
    led_gpio.Pull = GPIO_PULLUP;                // 使能GPIO的内部上拉
    led_gpio.Speed = GPIO_SPEED_FREQ_HIGH;      // 将LED的GPIO响应速度设置为最快
    
    led_gpio.Pin = WHITE_LED_PIN;               // 选择不同颜色LED对应的GPIO引脚
    HAL_GPIO_Init(WHITE_LED_PORT, &led_gpio);   // 调用初始化函数初始化指定的GPIO
    
    led_gpio.Pin = BLUE_LED_PIN;                // 选择不同颜色LED对应的GPIO引脚
    HAL_GPIO_Init(BLUE_LED_PORT, &led_gpio);    // 调用初始化函数初始化指定的GPIO
    
    led_gpio.Pin = GREEN_LED_PIN;               // 选择不同颜色LED对应的GPIO引脚
    HAL_GPIO_Init(GREEN_LED_PORT, &led_gpio);   // 调用初始化函数初始化指定的GPIO
}

void ESP8266_GPIO_Init(void)
{
    GPIO_InitTypeDef esp_rst_gpio;

    __HAL_RCC_GPIOG_CLK_ENABLE();
    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);

    esp_rst_gpio.Mode = GPIO_MODE_OUTPUT_PP;  
    esp_rst_gpio.Pull = GPIO_PULLUP;          
    esp_rst_gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    esp_rst_gpio.Pin = GPIO_PIN_12;                // 选择不同颜色LED对应的GPIO引脚
    HAL_GPIO_Init(GPIOG, &esp_rst_gpio);    // 调用初始化函数初始化指定的GPIO
}
  