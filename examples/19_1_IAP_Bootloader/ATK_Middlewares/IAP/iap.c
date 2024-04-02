/**
 ****************************************************************************************************
 * @file        iap.c
 * @author      ALIENTEK
 * @brief       IAP code
 *
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

#include "iap.h"
#include "usart.h"
#include "../../BSP/STMFLASH/stmflash.h"


iapfun jump2app;
uint16_t g_iapbuf[1024];       /* 2K byte cache */

/**
* @brief 	IAP is written to the APP BIN
* @param 	appxaddr: Start address of the application
* @param 	appbuf: Application CODE
* @param 	appsize: Application size (bytes)
* @retval 	None
*/
void iap_write_appbin(uint32_t appxaddr, uint8_t *appbuf, uint32_t appsize)
{
    uint16_t t;
    uint16_t i = 0;
    uint16_t temp;
    uint32_t fwaddr = appxaddr; /* The address of the current write */
    uint8_t *dfu = appbuf;

    for (t = 0; t < appsize; t += 2)
    {
        temp = (uint16_t)dfu[1] << 8;
        temp |= (uint16_t)dfu[0];
        dfu += 2;               /* Offset by 2 bytes */
        g_iapbuf[i++] = temp;

        if (i == 1024)
        {
            i = 0;
            stmflash_write(fwaddr, g_iapbuf, 1024);
            fwaddr += 2048;     /* Offset 2048 16 = 2 * 8,so multiply by 2 */
        }
    }

    if (i)
    {
        stmflash_write(fwaddr, g_iapbuf, i);  /* Write in the last few content bytes */
    }
}

/**
* @brief 	Jump to application segment (execute APP)
* @param 	appxaddr: Start address of the application
* @retval 	None
*/
void iap_load_app(uint32_t appxaddr)
{
    if (((*(volatile  uint32_t *)appxaddr) & 0x2FFE0000) == 0x20000000)     /* Check whether the top address of the stack is valid. Can be placed in internal SRAM total 64KB(0x20000000) */
    {
        /* The second word in the user code area is the program start address (reset address) */
        jump2app = (iapfun) * (volatile uint32_t *)(appxaddr + 4);
        
        /* Initializes the APP stack pointer (the first word in the user code area is used to store the stack top address) */
        __set_MSP(*(volatile uint32_t *)appxaddr);
        
        /* Jump to APP */
        jump2app();
    }
}
