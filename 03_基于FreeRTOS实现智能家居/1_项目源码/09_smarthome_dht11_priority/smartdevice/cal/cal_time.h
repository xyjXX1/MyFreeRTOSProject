
#ifndef __CAL_TIMER_H
#define __CAL_TIMER_H


#include <input_system.h>
 
/**********************************************************************
 * 函数名称： CAL_GetTime
 * 功能描述： 获得系统时间
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值：系统时间
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/24	     V1.0	  韦东山	      创建
 ***********************************************************************/ 
TIME_T CAL_GetTime(void);

/**********************************************************************
 * 函数名称： CAL_Delay
 * 功能描述： 延时
 * 输入参数： DelayMS-延时时间,单位: ms
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/27	     V1.0	  韦东山	      创建
 ***********************************************************************/
void CAL_Delay(unsigned int DelayMS);

/**********************************************************************
 * 函数名称： CAL_GetTick
 * 功能描述： 获得Tick值
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： Tick值
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/18	     V1.0	  韦东山	      创建
 ***********************************************************************/
unsigned int CAL_GetTick(void);


#endif /* __CAL_TIMER_H */

