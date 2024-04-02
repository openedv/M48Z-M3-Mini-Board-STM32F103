/**
 ****************************************************************************************************
 * @file        iap.h
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

#ifndef __IAP_H
#define __IAP_H

#include "stm32f1xx.h"
#include "main.h"



typedef void (*iapfun)(void);                   							/* Defines the parameters of a function type */

#define FLASH_APP1_ADDR         0x08008000      							/* First application start address (stored in internal FLASH)
                                                							 * Reserve the space 0X08000000 to 0X08008FFF(36KB) for the Bootloader
                                                							 */


void iap_load_app(uint32_t appxaddr);   									/* Jump to APP program execution */
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);   /* At the specified address, write to bin */

#endif
