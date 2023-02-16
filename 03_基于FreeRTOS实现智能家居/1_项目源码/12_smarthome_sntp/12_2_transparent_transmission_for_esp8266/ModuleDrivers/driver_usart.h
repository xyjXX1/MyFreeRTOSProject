/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_usart.h
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2020.6.6      v01        百问科技      创建文件
 *--------------------------------------------------
*/

#ifndef __DRIVER_USART_H
#define __DRIVER_USART_H

#include "stm32f1xx_hal.h"
#include "ring_buffer.h"

/* 调试串口的接口硬件定义 */
#define DEBUG   (1)

#define DEBUG_RX_PIN                GPIO_PIN_10
#define DEBUG_TX_PIN                GPIO_PIN_9
#define DEBUG_PORT                  GPIOA
#define DEBUG_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()

#define DEBUG_UART                  USART1
#define DEBUG_BAUDRATE              (115200)
#define DEBUG_UART_CLK_ENABLE()     __HAL_RCC_USART1_CLK_ENABLE()

#define DEBUG_UART_IRQ   (1)
#define DEBUG_UART_IRQn             USART1_IRQn
#define DEBUG_UART_IRQHandler       USART1_IRQHandler
#define DEBUG_DMA_TX_CH             DMA1_Channel4
#define DEBUG_DMA_CH_IRQn           DMA1_Channel4_IRQn
#define DEBUG_DMA_CH_IRQHandler     DMA1_Channel4_IRQHandler

/* ESP8266模块的接口硬件定义 */
#define WIFI_RX_PIN                 GPIO_PIN_9
#define WIFI_TX_PIN                 GPIO_PIN_8
#define WIFI_PORT                   GPIOD
#define WIFI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define WIFI_GPIO_REMAP_ENABLE()    __HAL_AFIO_REMAP_USART3_ENABLE()

#define WIFI_UART                   USART3
#define WIFI_BAUDRATE               (115200)
#define WIFI_UART_CLK_ENABLE()     __HAL_RCC_USART3_CLK_ENABLE()

#define WIFI_UART_IRQ   (1)
#define WIFI_UART_IRQn              USART3_IRQn
#define WIFI_UART_IRQHandler        USART3_IRQHandler
#define WIFI_DMA_TX_CH              DMA1_Channel2
#define WIFI_DMA_CH_IRQn            DMA1_Channel2_IRQn
#define WIFI_DMA_CH_IRQHandler      DMA1_Channel2_IRQHandler

/* 接口声明 */

/*
 *  函数名：UART_Init
 *  功能描述：初始化用到的UART外设
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
void UART_Init(void);

void ErrorLogHandler(void *arg);
ring_buffer* Debug_Uart_GetBuffer(void);

void WiFi_Uart_SendString(const char *s, uint16_t len);
ring_buffer* WiFi_Uart_GetBuffer(void);

#endif /* __DRIVER_USART_H */
