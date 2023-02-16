#include "driver_dma.h"
#include "driver_usart.h"

static DMA_HandleTypeDef debug_to_wifi;
static DMA_HandleTypeDef wifi_to_debug;

static ring_buffer wifi_tx_buf;
static ring_buffer debug_tx_buf;

void MemToMem_DMA_Init(void)
{
    debug_to_wifi.Instance                  = DMA1_Channel1;
    debug_to_wifi.Init.Direction            = DMA_MEMORY_TO_MEMORY;
    debug_to_wifi.Init.PeriphInc            = DMA_PINC_ENABLE;
    debug_to_wifi.Init.MemInc               = DMA_MINC_ENABLE;
    debug_to_wifi.Init.PeriphDataAlignment  = DMA_PDATAALIGN_BYTE;
    debug_to_wifi.Init.MemDataAlignment     = DMA_MDATAALIGN_BYTE;
    debug_to_wifi.Init.Mode                 = DMA_NORMAL;
    debug_to_wifi.Init.Priority             = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&debug_to_wifi) != HAL_OK)
    {
        ErrorLogHandler("Debug to WiFi DMA Init Error.");
    }
    
    wifi_to_debug.Instance                  = DMA1_Channel3;
    wifi_to_debug.Init.Direction            = DMA_MEMORY_TO_MEMORY;
    wifi_to_debug.Init.PeriphInc            = DMA_PINC_ENABLE;
    wifi_to_debug.Init.MemInc               = DMA_MINC_ENABLE;
    wifi_to_debug.Init.PeriphDataAlignment  = DMA_PDATAALIGN_BYTE;
    wifi_to_debug.Init.MemDataAlignment     = DMA_MDATAALIGN_BYTE;
    wifi_to_debug.Init.Mode                 = DMA_NORMAL;
    wifi_to_debug.Init.Priority             = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&wifi_to_debug) != HAL_OK)
    {
        ErrorLogHandler("WiFi to Debug DMA Init Error.");
    }
}

void DebugToWifi_DMA_Transfer(void)
{
    ring_buffer buf = Debug_Uart_GetString();
    if(buf.valid_len != 0)
    {
        HAL_DMA_Start(&debug_to_wifi, (uint32_t)&buf.buffer, dst, length);
    }
    
}

void WiFiToDebug_DMA_Transfer(const uint32_t src, const uint32_t dst, uint16_t length)
{
    HAL_DMA_Start(&wifi_to_debug, src, dst, length);
}