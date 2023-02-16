#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>

#include "driver_usart.h"

extern osThreadId_t defaultTaskHandle;

const char senc_start_str[] = "AT";
const char send_end_str[] = "\r\n";

void Next(char*T,int *next);
int KMP(char * S,char * T);

static void DebugToWifi(void)
{
    int start = 0, end = 0;
    uint8_t c = 0;
    uint16_t i = 0, str_len = 0;
    ring_buffer *buf = Debug_Uart_GetBuffer();
    if(buf->valid_len != 0)
    {
        start = KMP((char*)&buf->buffer[buf->pR], (char*)senc_start_str);
        end = KMP((char*)&buf->buffer[buf->pR], (char*)send_end_str);
        if(start != -1 && end != -1)
        {
            start = buf->pR + start-1;
            end = buf->pR + end-1;
            buf->pR = start;
            str_len = end + strlen(send_end_str)-start;
            while(i<str_len)
            {
                if(ring_buffer_read(&c, buf) != -1)
                {
                    WiFi_Uart_SendString((const char*)&c, 1);
                    i++;
                }
            }
        }
    }
}

static void WiFiToDebug(void)
{
    ring_buffer *buf = WiFi_Uart_GetBuffer();
    uint8_t c = 0xFF;

    if(ring_buffer_read(&c, buf) != -1)
        printf("%c", c);

}

void ConsolerFun(void *argument)
{
    uint32_t tick = 0;
    UART_Init();
    printf("Consoler Function\r\n");
    while(1)
    {
        tick = osKernelLock();
        DebugToWifi();
        WiFiToDebug();
        osKernelUnlock();
        osDelay(1);
    }
}

void Next(char*T,int *next)
{
    int i=1;
    next[1]=0;
    int j=0;
    while (i<strlen(T)) 
    {
        if (j==0||T[i-1]==T[j-1]) 
        {
            i++;
            j++;
            next[i]=j;
        }
        else
        {
            j=next[j];
        }
    }
}

int KMP(char * S,char * T)
{
    int next[10];
    Next(T,next);//根据模式串T,初始化next数组
    int i=1;
    int j=1;
    while (i<=strlen(S)&&j<=strlen(T)) 
    {
        //j==0:代表模式串的第一个字符就和当前测试的字符不相等；S[i-1]==T[j-1],如果对应位置字符相等，两种情况下，指向当前测试的两个指针下标i和j都向后移
        if (j==0 || S[i-1]==T[j-1]) 
        {
            i++;
            j++;
        }
        else
        {
            j=next[j];//如果测试的两个字符不相等，i不动，j变为当前测试字符串的next值
        }
    }
    if (j>strlen(T)) 
    {//如果条件为真，说明匹配成功
        return i-(int)strlen(T);
    }
    return -1;
}
