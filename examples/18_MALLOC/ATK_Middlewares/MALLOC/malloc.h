/**
 ****************************************************************************************************
 * @file        malloc.h
 * @author      ALIENTEK
 * @brief       Memory management driver code
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

#ifndef __MALLOC_H
#define __MALLOC_H

//#include "stdint.h"
#include "stm32f1xx.h"
#include "main.h"


/* Define a memory pool */
#define SRAMIN      0       /* Internal SRAM, 16KB in total */

#define SRAMBANK    1       /* Defines the number of supported SRAM blocks. */


/* Defines the memory management table type, when extension SDRAM，the value must be uint32_t type,otherwise it can be defined as uint16_t to save memory usage */
#define MT_TYPE     uint16_t


/* For a single memory, the total space occupied by memory management can be calculated using the following formula:
 * size = MEM1_MAX_SIZE + (MEM1_MAX_SIZE / MEM1_BLOCK_SIZE) * sizeof(MT_TYPE)

 * Given the total memory capacity (size), the maximum memory pool is calculated as follows:
 * MEM1_MAX_SIZE = (MEM1_BLOCK_SIZE * size) / (MEM1_BLOCK_SIZE + sizeof(MT_TYPE))
 * In SRAMIN, for example,MEM1_MAX_SIZE = (32 * 20)/(32 + 2) = 20.24KB ≈ 20KB
 * However, in order to reserve memory space for other global variables/arrays, etc., we set the maximum management to 10KB here
 * /
 
/* mem1 memory parameter settings,mem1 is the SRAM inside the F103. */
#define MEM1_BLOCK_SIZE         32                              /* The memory block size is 32 bytes */
#define MEM1_MAX_SIZE           15 * 1024                       /* Maximum management memory 15K, F103 internal SRAM total 20KB */
#define MEM1_ALLOC_TABLE_SIZE   MEM1_MAX_SIZE/MEM1_BLOCK_SIZE   /* Memory table size */

/* If NULL is not defined, so define null */
#ifndef NULL
#define NULL 0
#endif

/* Memory management controller */
struct _m_mallco_dev
{
    void (*init)(uint8_t);                                      /* Initialize */
    uint16_t (*perused)(uint8_t);                               /* Memory usage */
    uint8_t *membase[SRAMBANK];                                 /* Memory pool Manages the memory for each area of SRAMBANK */
    MT_TYPE *memmap[SRAMBANK];                                  /* Memory management status table */
    uint8_t  memrdy[SRAMBANK];                                  /* Memory management ready */
};

extern struct _m_mallco_dev mallco_dev;                         /* Defines in mallco.c */

/* 用户调用函数 */
void my_mem_init(uint8_t memx);                                 /* Memory management initialization function (external/internal call) */
uint16_t my_mem_perused(uint8_t memx) ;                         /* Gets memory usage (external/internal calls) */
void my_mem_set(void *s, uint8_t c, uint32_t count);            /* Memory setup function */
void my_mem_copy(void *des, void *src, uint32_t n);             /* Memory copy function */

void myfree(uint8_t memx, void *ptr);                           /* Memory release (external call) */
void *mymalloc(uint8_t memx, uint32_t size);                    /* Memory allocation (external call) */
void *myrealloc(uint8_t memx, void *ptr, uint32_t size);        /* Reallocate memory (external call) */

#endif
