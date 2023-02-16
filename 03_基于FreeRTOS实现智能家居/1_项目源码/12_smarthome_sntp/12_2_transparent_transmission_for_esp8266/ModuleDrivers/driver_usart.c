/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_usart.c
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2021.8.21      v01         百问科技      创建文件
 *--------------------------------------------------
*/

#include "driver_usart.h"
#include <stdio.h>
#include <string.h>

static UART_HandleTypeDef debug_uart;
static UART_HandleTypeDef wifi_uart;

static DMA_HandleTypeDef debug_dma_tx;
static DMA_HandleTypeDef wifi_dma_tx;

static ring_buffer debug_rx_buf = {0};
static ring_buffer wifi_rx_buf = {0};

static void DebugUart_Init(void);
static void WiFiUart_Init(void);

/*
 *  函数名：UART_Init
 *  功能描述：初始化用到的UART外设
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
void UART_Init(void)
{
    #if DEBUG
    DebugUart_Init();
    #endif
    WiFiUart_Init();
}

static void DebugUart_Init(void)
{
    ring_buffer_init(&debug_rx_buf);
    
    debug_uart.Instance         = DEBUG_UART;           // 调试用的串口外设
    debug_uart.Init.BaudRate    = DEBUG_BAUDRATE;       // 波特率：
    debug_uart.Init.WordLength  = UART_WORDLENGTH_8B;   // 数据位：8bit
    debug_uart.Init.StopBits    = UART_STOPBITS_1;      // 停止位：1bit
    debug_uart.Init.Parity      = UART_PARITY_NONE;     // 校验位：None
    debug_uart.Init.HwFlowCtl   = UART_HWCONTROL_NONE;  // 流控制：None
    debug_uart.Init.Mode        = UART_MODE_TX_RX;      // 方向：全双工
    debug_uart.Init.OverSampling= UART_OVERSAMPLING_16; // 过采样
    
    if(HAL_UART_Init(&debug_uart) != HAL_OK)
    {
        ErrorLogHandler("Debug Uart Init Error.");
    }
}

static void WiFiUart_Init(void)
{
    ring_buffer_init(&wifi_rx_buf);
    
    wifi_uart.Instance         = WIFI_UART;           // 调试用的串口外设
    wifi_uart.Init.BaudRate    = WIFI_BAUDRATE;       // 波特率：
    wifi_uart.Init.WordLength  = UART_WORDLENGTH_8B;   // 数据位：8bit
    wifi_uart.Init.StopBits    = UART_STOPBITS_1;      // 停止位：1bit
    wifi_uart.Init.Parity      = UART_PARITY_NONE;     // 校验位：None
    wifi_uart.Init.HwFlowCtl   = UART_HWCONTROL_NONE;  // 流控制：None
    wifi_uart.Init.Mode        = UART_MODE_TX_RX;      // 方向：全双工
    wifi_uart.Init.OverSampling= UART_OVERSAMPLING_16; // 过采样
    
    if(HAL_UART_Init(&wifi_uart) != HAL_OK)
    {
        ErrorLogHandler("WiFi Uart Init Error.");
    }
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
    GPIO_InitTypeDef uart_gpio = {0};
    
    if(uartHandle->Instance == DEBUG_UART)
    {
        DEBUG_UART_CLK_ENABLE();
        DEBUG_GPIO_CLK_ENABLE();
        
        uart_gpio.Pin   = DEBUG_TX_PIN;
        uart_gpio.Mode  = GPIO_MODE_AF_PP;
        uart_gpio.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(DEBUG_PORT, &uart_gpio);
        
        uart_gpio.Pin   = DEBUG_RX_PIN;
        uart_gpio.Mode  = GPIO_MODE_INPUT;
        uart_gpio.Pull  = GPIO_PULLUP;
        HAL_GPIO_Init(DEBUG_PORT, &uart_gpio);
        
        #if DEBUG_UART_IRQ
        __HAL_RCC_DMA1_CLK_ENABLE();
        debug_dma_tx.Instance                   = DEBUG_DMA_TX_CH;
        debug_dma_tx.Init.Direction             = DMA_MEMORY_TO_PERIPH;
        debug_dma_tx.Init.PeriphInc             = DMA_PINC_DISABLE;
        debug_dma_tx.Init.MemInc                = DMA_MINC_ENABLE;
        debug_dma_tx.Init.PeriphDataAlignment   = DMA_PDATAALIGN_BYTE;
        debug_dma_tx.Init.MemDataAlignment      = DMA_MDATAALIGN_BYTE;
        debug_dma_tx.Init.Mode                  = DMA_NORMAL;
        debug_dma_tx.Init.Priority              = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&debug_dma_tx) != HAL_OK)
        {
            ErrorLogHandler("Debug Uart TX DMA Init Error.");
        }

        __HAL_LINKDMA(uartHandle,hdmatx,debug_dma_tx);

        HAL_NVIC_SetPriority(DEBUG_UART_IRQn, 2, 0);
        HAL_NVIC_EnableIRQ(DEBUG_UART_IRQn);
        __HAL_UART_ENABLE_IT(uartHandle, UART_IT_TC | UART_IT_RXNE);
        
        HAL_NVIC_SetPriority(DEBUG_DMA_CH_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(DEBUG_DMA_CH_IRQn);
        #endif
    }
    else if(uartHandle->Instance == WIFI_UART)
    {
        WIFI_UART_CLK_ENABLE();
        WIFI_GPIO_CLK_ENABLE();
        WIFI_GPIO_REMAP_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();
        
        uart_gpio.Pin   = WIFI_TX_PIN;
        uart_gpio.Mode  = GPIO_MODE_AF_PP;
        uart_gpio.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(WIFI_PORT, &uart_gpio);
        
        uart_gpio.Pin   = WIFI_RX_PIN;
        uart_gpio.Mode  = GPIO_MODE_INPUT;
        uart_gpio.Pull  = GPIO_PULLUP;
        HAL_GPIO_Init(WIFI_PORT, &uart_gpio);
        
        #if WIFI_UART_IRQ
        
        wifi_dma_tx.Instance                   = WIFI_DMA_TX_CH;
        wifi_dma_tx.Init.Direction             = DMA_MEMORY_TO_PERIPH;
        wifi_dma_tx.Init.PeriphInc             = DMA_PINC_DISABLE;
        wifi_dma_tx.Init.MemInc                = DMA_MINC_ENABLE;
        wifi_dma_tx.Init.PeriphDataAlignment   = DMA_PDATAALIGN_BYTE;
        wifi_dma_tx.Init.MemDataAlignment      = DMA_MDATAALIGN_BYTE;
        wifi_dma_tx.Init.Mode                  = DMA_NORMAL;
        wifi_dma_tx.Init.Priority              = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&wifi_dma_tx) != HAL_OK)
        {
            ErrorLogHandler("WiFi Uart TX DMA Init Error.");
        }

        __HAL_LINKDMA(uartHandle,hdmatx,wifi_dma_tx);
        HAL_NVIC_SetPriority(WIFI_UART_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(WIFI_UART_IRQn);
        __HAL_UART_ENABLE_IT(uartHandle, UART_IT_TC | UART_IT_RXNE);
        
        HAL_NVIC_SetPriority(WIFI_DMA_CH_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(WIFI_DMA_CH_IRQn);
        #endif
    }
}

void ErrorLogHandler(void *arg)
{
    char *str = (char*)arg;
    #if DEBUG
    printf("%s\r\n", str);
    #endif
}


void DEBUG_UART_IRQHandler(void)
{
    #if DEBUG_UART_IRQ
    char c;
    if(__HAL_UART_GET_FLAG(&debug_uart, UART_FLAG_RXNE) == SET)
    {
        c = DEBUG_UART->DR;
        ring_buffer_write((uint8_t)c, &debug_rx_buf);
    }
    HAL_UART_IRQHandler(&debug_uart);
    #endif
}

void DEBUG_DMA_CH_IRQHandler(void)
{
    #if DEBUG_UART_IRQ
    HAL_DMA_IRQHandler(&debug_dma_tx);
    #endif
}

/*
 *  函数名：fputc
 *  功能描述：printf/putchar 标准输出函数的底层输出函数
 *  输入参数：ch --> 要输出的数据
 *  输出参数：无
 *  返回值：无
*/
int fputc(int ch, FILE *f)
{
    #if DEBUG_UART_IRQ
    while(HAL_UART_Transmit(&debug_uart, (uint8_t*)&ch, 1, 1)!=HAL_OK);
//    HAL_UART_Transmit_DMA(&debug_uart, (uint8_t*)&ch, 1);
    #else
    HAL_UART_Transmit(&debug_uart, (uint8_t*)&ch, 1, 1);
    #endif
}

void WIFI_UART_IRQHandler(void)
{
    #if WIFI_UART_IRQ
    char c;
    if(__HAL_UART_GET_FLAG(&wifi_uart, UART_FLAG_RXNE) == SET)
    {
        c = WIFI_UART->DR;
        ring_buffer_write((uint8_t)c, &wifi_rx_buf);
    }
    HAL_UART_IRQHandler(&wifi_uart);
    #endif
}

void WIFI_DMA_CH_IRQHandler(void)
{
    #if WIFI_UART_IRQ
    HAL_DMA_IRQHandler(&wifi_dma_tx);
    #endif
}

void WiFi_Uart_SendString(const char *s, uint16_t len)
{
    #if WIFI_UART_IRQ
    while(HAL_UART_Transmit_DMA(&wifi_uart, (uint8_t*)s, len) != HAL_OK);
//    while(HAL_UART_Transmit(&wifi_uart, (uint8_t*)s, len, (len>>6)+1) != HAL_OK);
    #else
    HAL_UART_Transmit(&wifi_uart, (uint8_t*)s, len, (len>>6)+1);
    #endif
}

ring_buffer* Debug_Uart_GetBuffer(void)
{
    return &debug_rx_buf;
}

ring_buffer* WiFi_Uart_GetBuffer(void)
{
    return &wifi_rx_buf;
}

#if 0

/*
 *  函数名：HAL_UART_RxCpltCallback
 *  功能描述：HAL库中的UART接收完成回调函数
 *  输入参数：huart --> UART的设备句柄，用以指明UART设备是哪一个UART
 *  输出参数：无
 *  返回值：无
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == DEBUG_UART)   // 判断进来的是否是USART1这个UART设备
    {
    }
    else if(huart->Instance == WIFI_UART)
    {
    }
}

/*
 *  函数名：HAL_UART_TxCpltCallback
*  功能描述：HAL库中的UART发送完成回调函数
 *  输入参数：huart --> UART的设备句柄，用以指明UART设备是哪一个UART
 *  输出参数：无
 *  返回值：无
*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == DEBUG_UART)   // 判断进来的是否是USART1这个UART设备
    {
    }
    else if(huart->Instance == WIFI_UART)
    {
    }
}

#endif