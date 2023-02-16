#include <stdio.h>
#include <string.h>
#include "net_device.h"
#include "at_command\at_command.h"


#define ESP8266_DEFAULT_TIMEROUT 10  /* ms */

/**********************************************************************
 * 函数名称： ESP8266Init
 * 功能描述： 初始化ESP8266
 * 输入参数： ptDev-ESP8266设备
 * 输出参数： 无
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int ESP8266Init(struct NetDevice *ptDev)
{
	/* 设置复位引脚 */
	
	/* 初始化硬件接口 */
	ATInterfaceInit();

	ATCommandSend("AT+CWMODE=3", ESP8266_DEFAULT_TIMEROUT);

	ATCommandSend("AT+CIPMUX=0", ESP8266_DEFAULT_TIMEROUT);

	return 0;
}

/**********************************************************************
 * 函数名称： ESP8266Connect
 * 功能描述： 连接WIFI热点
 * 输入参数： ptDev-ESP8266设备
 * 输入参数： SSID-WIFI热点名称
 * 输入参数： password-WIFI热点密码
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int ESP8266Connect(struct NetDevice *ptDev, char *SSID, char *password)
{
	char cmd[100];
	
	sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", SSID, password);
	return ATCommandSend(cmd, ESP8266_DEFAULT_TIMEROUT*1000);
}

/**********************************************************************
 * 函数名称： ESP8266CloseTransfer
 * 功能描述： 关闭网络传输
 * 输入参数： ptDev-ESP8266设备
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void ESP8266CloseTransfer(struct NetDevice *ptDev)
{
	ATCommandSend("AT+CIPCLOSE", ESP8266_DEFAULT_TIMEROUT);
}

/**********************************************************************
 * 函数名称： ESP8266Send
 * 功能描述： 发送网络数据
 * 输入参数： ptDev-ESP8266设备
 * 输入参数： Type-网络连接类型:"tcl","udp"
 * 输入参数： pDestIP-目标IP
 * 输入参数： iDestPort-目标端口
 * 输入参数： Data-数据buffer
 * 输入参数： Data-数据长度
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int ESP8266Send(struct NetDevice *ptDev, char *Type, char *pDestIP, int iDestPort, unsigned char *Data, int iLen)
{
	/* 未实现 */
	return 0;
}

/**********************************************************************
 * 函数名称： ESP8266Recv
 * 功能描述： 接收网络数据
 * 输入参数： ptDev-ESP8266设备
 * 输入参数： Data-数据buffer
 * 输入参数： iTimeoutMS-超时时间
 * 输出参数： piLen-用来数据长度
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int ESP8266Recv(struct NetDevice *ptDev, unsigned char *Data, int *piLen, int iTimeoutMS)
{
	return ATDataRecv(Data, piLen, iTimeoutMS);
}

/**********************************************************************
 * 函数名称： ESP8266GetInfo
 * 功能描述： 获得网卡设备的信息(目前主要是IP)
 * 输入参数： ptDev-ESP8266设备
 * 输出参数： ip_buf-用来存放IP
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int ESP8266GetInfo(struct NetDevice *ptDev, char *ip_buf)
{
	char buf[200];
	int ret;
	char *ip;
	int i;
	
	/* 发出AT命令:AT+CIFSR */
	/* 从得到的数据里解析出IP
	 * 
	 +CIFSR:APIP,"192.168.4.1"
	 +CIFSR:APMAC,"1a:fe:34:a5:8d:c6"
	 +CIFSR:STAIP,"192.168.3.133"
	 +CIFSR:STAMAC,"18:fe:34:a5:8d:c6"
	 OK

	 */
	ret = ATCommandSendAndRecv("AT+CIFSR", buf, ESP8266_DEFAULT_TIMEROUT*1000);
	if (!ret)
	{
		/* 解析出IP */
		ip = strstr(buf, "+CIFSR:STAIP,\"");
		if (ip)
		{
			ip += strlen("+CIFSR:STAIP,\"");
			for (i = 0; ip[i] != '\"'; i++)
			{
				ptDev->ip[i] = ip[i];
			}
			ptDev->ip[i] = '\0';
			strcpy(ip_buf, ptDev->ip);
			return 0;
		}
	}

	return -1;

}


/**********************************************************************
 * 函数名称： ESP8266CreateTransfer
 * 功能描述： 创建网络传输
 * 输入参数： ptDev-ESP8266设备
 * 输入参数： Type-传输类型("tcl","udp")
 * 输入参数： iLocalPort-本地端口
 * 输出参数： 无
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int ESP8266CreateTransfer(struct NetDevice *ptDev, char *Type, int iLocalPort)
{
	char cmd[100];
	/* 只支持udp */
	/* AT+CIPSTART="UDP","192.168.101.110",8080,1112,2 */

	sprintf(cmd, "AT+CIPSTART=\"UDP\",\"192.168.1.1\",8080,%d,2", iLocalPort);
	
	return ATCommandSend(cmd, ESP8266_DEFAULT_TIMEROUT);

}

static int ESP8266_GetNetTime(struct NetDevice *ptDev, ptNetTime time)
{
    int ret = -1;
    static int mode = 0;
    char buf[128] = {0};
    
    char *base_addr = NULL;
    
    /* 发出AT命令:AT+CIPSNTPCFG=1,8,"ntp.ntsc.ac.cn" 设置时区和NTP服务器*/
    /* 发出AT命令:AT+CIPSNTPTIME?获取SNTP时间 */
	/* 从得到的数据里解析出时间
	 * 
	 +CIPSNTPTIME:Thu Dec 30 10:01:21 2021
     OK

	 */
    
    while(1)
    {
        if(mode == 0)
        {
            ret = ATCommandSend("AT+CIPSNTPCFG=1,8,\"120.25.115.20\"", ESP8266_DEFAULT_TIMEROUT*1000);
            if(!ret)
                mode = 1;
        }
        
        if(mode == 1)
        {
            ret = ATCommandSendAndRecv("AT+CIPSNTPTIME?", buf, ESP8266_DEFAULT_TIMEROUT*1000);
            if(!ret)
            {
                base_addr = strstr(buf, "+CIPSNTPTIME:");
                if(base_addr)
                {
                    base_addr = base_addr + strlen("+CIPSNTPTIME:") + 4;    // 月份首地址
                    memcpy(time->month, base_addr, 3);
                    time->month[3] = '\0';
                    
                    base_addr = base_addr + 4;  // 日期首地址
                    memcpy(time->day, base_addr, 2);
                    time->day[2] = '\0';
                    
                    base_addr = base_addr + 3; // 时首地址
                    memcpy(time->hour, base_addr, 2);
                    time->hour[2] = '\0';
                    
                    base_addr = base_addr + 3; // 分首地址
                    memcpy(time->min, base_addr, 2);
                    time->min[2] = '\0';
                    
                    base_addr = base_addr + 3; // 秒首地址
                    memcpy(time->sec, base_addr, 2);
                    time->sec[2] = '\0';
                    
                    base_addr = base_addr + 3; // 年份首地址
                    memcpy(time->year, base_addr, 4);
                    time->year[4] = '\0';
                    
                    if(strstr(time->year, "1970"))
                    {
                        mode = 0;
                        continue;
                    }
                    mode = 1;
                    return 0;
                }
            }
        }
    }
    
    return -1;
}


static NetDevice g_tESP8266NetDevice = {
	"esp8266",
	"0.0.0.0",
	{0,0,0,0,0,0},
    0,
	ESP8266Init,
	ESP8266Connect,
	ESP8266GetInfo,
	ESP8266CreateTransfer,
	ESP8266CloseTransfer,
	ESP8266Send,
	ESP8266Recv,
    ESP8266_GetNetTime
};

/**********************************************************************
 * 函数名称： AddNetDeviceESP8266
 * 功能描述： 注册ESP8266网卡设备
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/14	     V1.0	  韦东山	      创建
 ***********************************************************************/
void AddNetDeviceESP8266(void)
{
	NetDeviceRegister(&g_tESP8266NetDevice);
}

