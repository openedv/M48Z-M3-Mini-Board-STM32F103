## IAP example<a name="catalogue"></a>


### 1 Brief
The function of this code is that after the program receives the APP's bin data, press the KEY0 and WKUP keys successively, and then load the APP's bin data into FLASH and jump to the APP's position and execute the APP's operation.
### 2 Hardware Hookup
The hardware resources used in this experiment are:
+ LED - PA8
+ USART1 - PA9\PA10
+ KEY - WKUP(PA0)
+ KEY - KEY0(PC13) 

The IAP used in this experiment is a software algorithm, so there is no corresponding connection schematic.

### 3 STM32CubeIDE Configuration


We copy the **11_USMART** project and name both the project and the.ioc file **19_IAP**.
The iap used in the experiments of this chapter is a software algorithm, so no other configuration on STM32CubeIDE is needed, and the corresponding driver code ``iap.c\iap.h`` is added directly.

##### code
###### iap.h
```c#
#define FLASH_APP1_ADDR         0x08008000      							/* First application start address (stored in internal FLASH)
                                                							 * Reserve the space 0X08000000 to 0X08008FFF(36KB) for the Bootloader
                                                							 */
```
Here is our macro definition **FLASH_APP1_ADDR**, which points to the address where the APP is stored in FLASH. The following is also based on the address to jump to the corresponding APP.

###### iap.c
```c#
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
```
With a full APP saved in place, let's check the top of the stack. Then we jump, before jumping, we have to make sure that the program is set up correctly, and then we can jump the program to the correct position to perform the corresponding operation.

###### usart.c
```c#
/* Receive buffer, maximum USART REC LEN bytes */
uint8_t g_usart_rx_buf[USART_REC_LEN] __attribute__((section(".app_addr")));
```
This code means to put the array **g_usart_rx_buf** in a section called ``.app_addr``. The address of the segment points to the file ``STM32F103C8TX_FLASH.ld``.

###### STM32F103C8TX_FLASH.ld
Open this file and add the red boxed code shown in the following screenshot:

![ ](./1_docs/3_figures/21_IAP_Bootloader/i1.png)

The preceding code sets the address of **g_usart_rx_buf** to 0x20002000. When we receive the APP bin file using the serial port, the file will be stored at 0x20002000.

###### main.c
```c#
int main(void)
{
  /* USER CODE BEGIN 1 */

    uint32_t lastcount = 0;
    uint32_t applenth = 0;
    uint8_t clearflag = 0;
    uint8_t t = 0;
    uint8_t key;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  stm32f103c8t6_show_mesg();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      if (g_usart_rx_cnt)
      {
          if (lastcount == g_usart_rx_cnt)   /* If no data is received within the new period, the data reception is considered complete */
          {
              applenth = g_usart_rx_cnt;
              lastcount = 0;
              g_usart_rx_cnt = 0;
              printf("User program reception completed !\r\n");
              printf("Code length :%dBytes\r\n", applenth);
          }
          else
          {
              lastcount = g_usart_rx_cnt;
          }
      }

      t++;
      key = key_scan(0);

      switch (key)
      {
          case KEY0_PRES:                                     /* The bin data of the received APP is written to the Flash */
          {
              if (applenth != 0)
              {
                  printf("Start updating firmware...\r\n");
                  printf("Copying APP2FLASH...\r\n");
                  if (((*(volatile uint32_t *)(0x20002000 + 4)) & 0xFF000000) == 0x08000000)
                  {
                      iap_write_appbin(FLASH_APP1_ADDR, g_usart_rx_buf, applenth);
                      printf("Copy APP Successed!!\r\n");
                      printf("Firmware update complete !\r\n");
                  }
                  else
                  {
                      printf("Illegal FLASH APP!  \r\n");
                      printf("Non-flash applications !\r\n");
                  }
              }
              else
              {
                  printf("There is no firmware to update !\r\n");
                  printf("No APP!\r\n");
              }
              clearflag = 7;
              break;
          }
          case WKUP_PRES:                                         /* Run the APP in Flash */
          {
              printf("flash addr :%x \r\n",(*(volatile uint32_t *)(FLASH_APP1_ADDR + 4)) & 0xFF000000);

              if (((*(volatile uint32_t *)(FLASH_APP1_ADDR + 4)) & 0xFF000000) == 0x08000000)
              {
                  printf("The FLASH user code starts to execute!\r\n\r\n");
                  HAL_Delay(10);
                  iap_load_app(FLASH_APP1_ADDR);
              }
              else
              {
                  printf("There is no firmware to run !\r\n");
                  printf("No APP!\r\n");
              }
              clearflag = 7;
              break;
          }
          default:
          {
              break;
          }
      }

      if (t == 2)
      {
          LED_TOGGLE();
          if (clearflag != 0)
          {
              clearflag--;
              if (clearflag == 0)
              {
              }
          }
          t = 0;
      }

      HAL_Delay(100);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
```
The above code uses the serial port to receive binary data from the APP. We will not introduce the APP code, you can open the source code of the routine to view, pay attention to reset the offset of the interrupt vector table at the main function, otherwise the APP will not run normally.


### 4 Running
#### 4.1 Compile & Download
After the compilation is complete, connect the DAP and the Mini Board, and then connect to the computer together to download the program to the development board.
#### 4.2 Phenomenon
Press the reset button to restart the development board, observe the LED flashing on the development board, open the serial port and the host computer ATK-XCOM can see the prompt information of the experiment, indicating that the code download is successful. We send the bin file of the APP on the host computer of the serial port, and the phenomenon is shown as follows:

![ ](../../1_docs/3_figures/21_IAP_Bootloader/i2.png)

After receiving, press KEY0 and WKUP as shown below:

![ ](../../1_docs/3_figures/21_IAP_Bootloader/i3.png)

![ ](../../1_docs/3_figures/21_IAP_Bootloader/i4.png)

[jump to tittle](#catalogue)