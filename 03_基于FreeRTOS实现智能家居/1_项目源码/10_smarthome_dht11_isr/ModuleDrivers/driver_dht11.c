#include "driver_dht11.h"
#include "main.h"

static TIM_HandleTypeDef        htim;

static void DHT11_TIM_Init(void);

/* 二进制信号量句柄 */
static SemaphoreHandle_t xBinarySemaphore;

void DHT11_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
    DHT11_TIM_Init();
    
    HAL_GPIO_DeInit(DHT11_PORT, DHT11_PIN);
	DHT11_CLK_EN();
    
    GPIO_InitStruct.Pin = DHT11_PIN;           
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;         
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;   
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);

	DHT11_GPIO_Init();

	/* 创建二进制信号量 */
	xBinarySemaphore = xSemaphoreCreateBinary( );
	if( xBinarySemaphore == NULL )
	{
		printf("can not create Semaphore\r\n");
	}
}

static void DHT11_TIM_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    
    __HAL_RCC_TIM3_CLK_ENABLE();
    
    htim.Instance              = TIM3;
    htim.Init.Period           = 0;
    htim.Init.Prescaler        = 72-1;  /* 72M/72 = 1Mhz, T=1us  */
    htim.Init.ClockDivision    = 0;
    htim.Init.CounterMode      = TIM_COUNTERMODE_UP;
    htim.Init.RepetitionCounter = 0;
    htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if(HAL_TIM_Base_Init(&htim) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

static void DHT11_usDelay(uint32_t us)
{
    uint16_t counter = 0;
	__HAL_TIM_SET_AUTORELOAD(&htim, us);		
	__HAL_TIM_SET_COUNTER(&htim, counter);
	HAL_TIM_Base_Start(&htim);
	while(counter != us)
	{
		counter = __HAL_TIM_GET_COUNTER(&htim);
	}
	HAL_TIM_Base_Stop(&htim);
}

static void DHT11_StartTimer(void)
{
    uint16_t counter = 0;
	uint32_t us = 10000; /* 10ms */
	__HAL_TIM_SET_AUTORELOAD(&htim, us);		
	__HAL_TIM_SET_COUNTER(&htim, counter);
	HAL_TIM_Base_Start(&htim);
}

static int DHT11_ReadTimer_us(void)
{
	return __HAL_TIM_GET_COUNTER(&htim);
}

static void DHT11_StopTimer(void)
{
	HAL_TIM_Base_Stop(&htim);
}


static void DHT11_Start(void)
{
	DHT11_L;
	DHT11_usDelay(20000);
}

static int DHT11_WaitForVal(uint8_t val, uint16_t timeout)
{
	while(timeout--)
	{
		if(DHT11_IN==val)
			return 0;
		DHT11_usDelay(1);
	}
	
	return -1;
}

static int DHT11_RecvByte(void)
{
	uint8_t i = 0;
	uint8_t data = 0;

	for(i=0; i<8; i++)
	{
		if(DHT11_WaitForVal(1, 1000))
			return -1;
		DHT11_usDelay(40);
		data <<= 1;
		if(DHT11_IN)
		{
			data |= 1;
		}
		if(DHT11_WaitForVal(0, 1000))
			return -1;
	}

	return data;
}


static int DHT11_ParseData(unsigned int *temperature, unsigned int *humidity)
{
	int *p = Get_DHT11_Edge_Times();
	int i, j, m = 0;
	unsigned char data[5];

	for (i = 0; i < 5; i++)
	{
		data[i] = 0;
		for (j = 0; j < 8; j++)
		{
			data[i] <<= 1;

			if ((p[m+1] - p[m]) > 40)
				data[i] |= 1;

			m += 2;
		}		
	}

	/* 检查检验码 */
	if (data[0] + data[1] + data[2] + data[3] != data[4])
		return -1;

	*humidity = data[0];
	*temperature = data[2];
	return 0;
}

int DHT11_Read(unsigned int *temperature, unsigned int *humidity)
{
	int timeout;

	/* 1. 发出Start */
	DHT11_Start();	// 开始信号
	DHT11_H;


	/* 2. 等待ACK */
	DHT11_usDelay(60);
	/* 读取ACK */
	if (DHT11_IN)
	{
		/* 没有回应 */
		return -1;
	}

	/* 收到了ACK */
	
	
	/* 3. 启动定时器/使能中断 */
	timeout = 100;
	while (!DHT11_IN && --timeout)
	{
		DHT11_usDelay(1);		
	}
	if (!timeout)
		return -1;

	timeout = 100;
	while (DHT11_IN && --timeout)
	{
		DHT11_usDelay(1);		
	}
	if (!timeout)
		return -1;

	/* 现在进入了数据阶段 */
	/* 启动定时器/使能中断 */
	DHT11_StartTimer();
	DHT11_IRQ_Enable();
	
	/* 4. 阻塞 */
	if (xSemaphoreTake(xBinarySemaphore, pdMS_TO_TICKS(10)) != pdTRUE)
	{
		DHT11_StopTimer();
		DHT11_IRQ_Disable();
		return -1;
	}
	
	DHT11_StopTimer();
	DHT11_IRQ_Disable();	

	/* 5. 被唤醒后解析数据 */
	return DHT11_ParseData(temperature, humidity);
}


