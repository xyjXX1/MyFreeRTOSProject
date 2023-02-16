#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>

extern osThreadId_t WiFiTaskHandle;

void WiFiFun(void *argument)
{
    printf("WiFi Function\r\n");
    
    while(1)
    {
        osDelay(1);
    }
}