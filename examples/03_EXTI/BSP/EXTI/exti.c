/**
 ****************************************************************************************************
 * @file        exti.c
 * @author      ALIENTEK
 * @brief       This file provides the driver for the exit
 * @license     Copyright (C) 2020-2032, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform  	: ALIENTEK STM32F103 development board
 * website		: https://www.alientek.com
 * forum		: http://www.openedv.com/forum.php
 *
 * change logs	:
 * version		data		notes
 * V1.0			20240222	the first version
 *
 ****************************************************************************************************
 */


#include "../EXTI/exti.h"
#include "../KEY/key.h"
#include "../LED/led.h"


/**
  * @brief  EXTI line rising detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    HAL_Delay(20);       	  /* delay 20ms to eliminate chattering */
    switch (GPIO_Pin)
    {
        case KEY0_Pin:
        	if (KEY0 == 1)
        	{
        		LED_TOGGLE(); /* LED0 state is flipped */
        	}
        	break;
        case WK_UP_Pin:
        	if (WK_UP == 1)
        	{
        		LED_TOGGLE(); /* LED0 state is flipped */
        	}
        	break;
    }
}
