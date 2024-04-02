/**
 ****************************************************************************************************
 * @file        pwr.c
 * @author      ALIENTEK Team
 * @version     V1.0
 * @date        Feb 22, 2024
 * @brief       Low power mode driver code
 * @license     Copyright (C) 2020-2032, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * Experimental platform  : ALIENTEK STM32F103 development board
 * Online video           : https://www.yuanzige.com
 * Technical Forum        : http://www.openedv.com/forum.php
 * Company website        : https://www.alientek.com
 * Buy from               : https://www.aliexpress.com/store/1102909571
 *
 ****************************************************************************************************
 */

#include "pwr.h"
#include "../LED/led.h"
#include "../LCD/lcd.h"
#include "usart.h"

/**
 * @brief       Enter CPU standby mode
 * @param       None
 * @retval      None
 */
void pwr_enter_standby(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();               /* The power clock was enabled */

    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1); /* The wake UP function of the WK_UP pin was enabled */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);        /* This flag needs to be cleared, otherwise it will remain awake */
    HAL_PWR_EnterSTANDBYMode();               /* Enter standby mode */
}
