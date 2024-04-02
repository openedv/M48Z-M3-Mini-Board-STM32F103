/**
 ****************************************************************************************************
 * @file        malloc.c
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


#include "malloc.h"

/* Memory pool (64-byte alignment) */
static __ALIGNED(64) uint8_t mem1base[MEM1_MAX_SIZE];           /* Internal SRAM memory pool */

/* Memory management table */
static MT_TYPE mem1mapbase[MEM1_ALLOC_TABLE_SIZE];              /* Internal SRAM memory pool MAP */


/* Memory management parameter */
const uint32_t memtblsize[SRAMBANK] = {MEM1_ALLOC_TABLE_SIZE};  /* Memory table size */
const uint32_t memblksize[SRAMBANK] = {MEM1_BLOCK_SIZE};        /* Memory block size */
const uint32_t memsize[SRAMBANK] = {MEM1_MAX_SIZE,};            /* Total memory size */

/* Memory management controller */
struct _m_mallco_dev mallco_dev =
{
    my_mem_init,                    /* Memory initialization */
    my_mem_perused,     			/* Memory usage */
    mem1base,           			/* Memory pool */
    mem1mapbase,        			/* Memory management status table */
    0,                  			/* Memory management not ready */
};

/**
* @brief 	copy memory
* @param 	*des : Iindicates the destination address
* @param 	*src : Indicates the source address
* @param 	n	 : Length of memory to be copied (bytes)
* @retval 	None
*/
void my_mem_copy(void *des, void *src, uint32_t n)
{
    uint8_t *xdes = des;
    uint8_t *xsrc = src;

    while (n--)*xdes++ = *xsrc++;
}

/**
* @brief 	Sets the memory value
* @param 	*s	  : Indicates the first address of the memory
* @param 	c	  : The value to set
* @param 	count : Memory size to be set (in bytes)
* @retval 	None
*/
void my_mem_set(void *s, uint8_t c, uint32_t count)
{
    uint8_t *xs = s;

    while (count--)*xs++ = c;
}

/**
* @brief 	Memory management initialization
* @param 	Memx: owning memory block
* @retval 	None
*/
void my_mem_init(uint8_t memx)
{
    uint8_t mttsize = sizeof(MT_TYPE);  								/* Gets the type length of the memmap array (uint16_t /uint32_t)*/
    my_mem_set(mallco_dev.memmap[memx], 0, memtblsize[memx]*mttsize); 	/* Clear memory status table data */
    mallco_dev.memrdy[memx] = 1;     									/* Memory management initialization OK */
}

/**
* @brief 	Displays the memory usage
* @param 	Memx: owning memory block
* @retval 	Usage rate (increased by 10 times,0~1000, representing 0.0%~100.0%)
*/
uint16_t my_mem_perused(uint8_t memx)
{
    uint32_t used = 0;
    uint32_t i;

    for (i = 0; i < memtblsize[memx]; i++)
    {
        if (mallco_dev.memmap[memx][i])used++;
    }

    return (used * 1000) / (memtblsize[memx]);
}

/**
* @brief 	Memory allocation (internal call)
* @param 	memx : owning memory block
* @param 	size : Size of memory to allocate (in bytes)
* @retval 	Indicates the offset memory address
* @arg 		0 to 0XFFFFFFFE : Indicates a valid memory offset address
* @arg 		0XFFFFFFFF		: Invalid memory offset address
*/
static uint32_t my_mem_malloc(uint8_t memx, uint32_t size)
{
    signed long offset = 0;
    uint32_t nmemb;     /* The number of memory blocks required */
    uint32_t cmemb = 0; /* Number of consecutive empty memory blocks */
    uint32_t i;

    if (!mallco_dev.memrdy[memx])
    {
        mallco_dev.init(memx);          /* If no initialization is performed, initialize the system first */
    }
    
    if (size == 0) return 0XFFFFFFFF;   /* No need for distribution */

    nmemb = size / memblksize[memx];    /* Gets the number of contiguous memory blocks to allocate */

    if (size % memblksize[memx]) nmemb++;

    for (offset = memtblsize[memx] - 1; offset >= 0; offset--)  /* Search the entire memory control area */
    {
        if (!mallco_dev.memmap[memx][offset])
        {
            cmemb++;            /* The number of consecutive empty memory blocks increases */
        }
        else 
        {
            cmemb = 0;          /* Continuous memory block clear */
        }
        
        if (cmemb == nmemb)     /* Consecutive nmemb empty memory blocks found */
        {
            for (i = 0; i < nmemb; i++) /* The memory block is not empty */
            {
                mallco_dev.memmap[memx][offset + i] = nmemb;
            }

            return (offset * memblksize[memx]); /* Return offset address */
        }
    }

    return 0XFFFFFFFF;  /* No memory block matching the allocation criteria was found */
}

/**
* @brief 	Free memory (internal call)
* @param 	memx   : owning memory block
* @param 	offset : indicates the memory address offset
* @retval 	Releases the result
* @arg 		0, release success;
* @arg 		1, release failed;
* @arg 		2, out of range (failed);
*/
static uint8_t my_mem_free(uint8_t memx, uint32_t offset)
{
    int i;

    if (!mallco_dev.memrdy[memx])   /* If no initialization is performed, initialize  first */
    {
        mallco_dev.init(memx);
        return 1;                   /* Uninitialized */
    }

    if (offset < memsize[memx])     /* The offset is in the memory pool. */
    {
        int index = offset / memblksize[memx];      /* Number of the memory block where the offset resides */
        int nmemb = mallco_dev.memmap[memx][index]; /* Memory block number */

        for (i = 0; i < nmemb; i++)                 /* Memory block cleared */
        {
            mallco_dev.memmap[memx][index + i] = 0;
        }

        return 0;
    }
    else
    {
        return 2;   /* Offset out of range. */
    }
}

/**
* @brief 	Free memory (external call)
* @param 	memx : Owning memory block
* @param 	ptr  : Indicates the first address of the memory
* @retval 	None
*/
void myfree(uint8_t memx, void *ptr)
{
    uint32_t offset;

    if (ptr == NULL)return;     /* Address is 0. */

    offset = (uint32_t)ptr - (uint32_t)mallco_dev.membase[memx];
    my_mem_free(memx, offset);  /* Free the memory */
}

/**
* @brief 	Allocates memory (external call)
* @param 	memx : owning memory block
* @param 	size : Size of memory to allocate (in bytes)
* @retval 	Indicates the first address of the allocated memory.
*/
void *mymalloc(uint8_t memx, uint32_t size)
{
    uint32_t offset;
    offset = my_mem_malloc(memx, size);

    if (offset == 0XFFFFFFFF)   /* Application error */
    {
        return NULL;            /* Return null(0) */
    }
    else    /* Application is fine. Return to first address */
    {
        return (void *)((uint32_t)mallco_dev.membase[memx] + offset);
    }
}

/**
* @brief 	Reallocation of memory (external call)
* @param 	memx : owning memory block
* @param 	*ptr : indicates the first address of the old memory
* @param 	size : Size of memory to allocate (in bytes)
* @retval 	Indicates the first address of the newly allocated memory.
*/
void *myrealloc(uint8_t memx, void *ptr, uint32_t size)
{
    uint32_t offset;
    offset = my_mem_malloc(memx, size);

    if (offset == 0XFFFFFFFF)   /* Application error */
    {
        return NULL;            /* Return null(0) */
    }
    else    					/* Application is fine. Return to first address */
    {
        my_mem_copy((void *)((uint32_t)mallco_dev.membase[memx] + offset), ptr, size); 	/* Copy old memory contents to new memory */
        myfree(memx, ptr);  															/* Free old memory */
        return (void *)((uint32_t)mallco_dev.membase[memx] + offset);   				/* Return the first address of the new memory */
    }
}
