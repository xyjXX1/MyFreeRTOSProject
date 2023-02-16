#include "driver_dht11.h"
#include "main.h"

static TIM_HandleTypeDef        htim;

static void DHT11_TIM_Init(void);

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
}

static void DHT11_TIM_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    
    __HAL_RCC_TIM3_CLK_ENABLE();
    
    htim.Instance              = TIM3;
    htim.Init.Period           = 0;
    htim.Init.Prescaler        = 72-1;
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


int DHT11_Read(unsigned int *temperature, unsigned int *humidity)
{
	uint8_t val[5] = {0};
	uint32_t temp = 0;

	DHT11_Start();	// 开始信号
	DHT11_H;
	DHT11_usDelay(40);
	if(!DHT11_IN)
	{
		while(!DHT11_IN);
		while(DHT11_IN);

		val[0] = DHT11_RecvByte();
		val[1] = DHT11_RecvByte();
		val[2] = DHT11_RecvByte();
		val[3] = DHT11_RecvByte();
		val[4] = DHT11_RecvByte();

		temp = val[0] + val[1] + val[2]+ val[3];
		if(temp == val[4])
		{
			*humidity = val[0];
			*temperature = val[2];
            
            return 0;
		}
	}
	return -1;
}


