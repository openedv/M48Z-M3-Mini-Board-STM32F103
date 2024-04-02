## DMA example<a name="catalogue"></a>


### 1 Brief
The function of this code is to press the KEY0 button, serial port USART1 to send data in DMA mode.
### 2 Hardware Hookup
The hardware resources used in this experiment are:
+ LED - PA8
+ USART1 - PA9\PA10
+ KEY - KEY0(PC13) 
+ DMA1 - Channel : USART1

The DMA used in this experiment is an on-chip resource of STM32F103, so there is no corresponding connection schematic.

### 3 STM32CubeIDE Configuration


We copy the **10_TFTLCD_MCU** project and name both the project and the.ioc file **14_DMA**.Next we start the DMA configuration by double-clicking the **14_DMA.ioc** file.

Open **System Core > DMA** as shown below.

![ ](./1_docs/3_figures/14_DMA/d1.png)

![ ](./1_docs/3_figures/14_DMA/d2.png)

Click **File > Save**, and you will be asked to generate code.Click **Yes**.

##### code
###### main.c
In the main function, we have added some additional code, which is as follows.
```c#
/* USER CODE BEGIN PFP */

const uint8_t text_to_send[] = {"ALIENTEK STM32F103 development board DMA Test\r\n"};               /* The character to loop */
#define SEND_BUF_SIZE (sizeof(text_to_send) + 2) * 200                                              /* Length of data to be sent */
uint8_t g_sendbuf[SEND_BUF_SIZE];
extern DMA_HandleTypeDef hdma_usart1_tx;

/* USER CODE END PFP */

int main(void)
{
  /* USER CODE BEGIN 1 */
	  uint8_t  key = 0;
	  uint16_t i, k;
	  uint16_t len;
	  uint8_t  mask = 0;
	  float pro = 0;          /* progress */

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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  stm32f103c8t6_show_mesg();
  lcd_init();
  lcd_show_string(0, 5, 240, 32, 32, "STM32F103C8T6", RED);
  lcd_show_string(0, 34, 240, 24, 24, "DMA TEST", RED);
  lcd_show_string(0, 60, 240, 16, 16, "ATOM@ALIENTEK", RED);
  lcd_show_string(0, 80, 200, 16, 16, "KEY0:Start", RED);

  len = sizeof(text_to_send);                             /* Fill the send data buffer */
  k = 0;

  for (i = 0; i < SEND_BUF_SIZE; i++) /* Fill ASCII character set data */
  {
	  if (k >= len)   /* Enter a newline character */
	  {
		  if (mask)
		  {
			  g_sendbuf[i] = 0x0a;
			  k = 0;
		  }
		  else
		  {
			  g_sendbuf[i] = 0x0d;
			  mask++;
		  }
	  }
	  else     /* Copy the TEXT TO SEND statement */
	  {
		  mask = 0;
		  g_sendbuf[i] = text_to_send[k];
		  k++;
	  }
  }

  i = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      key = key_scan(0);

      if (key == KEY0_PRES)       /* KEY0 is pressed */
      {
          printf("Start Transimit....\r\n");
          printf( "\r\n");

		  lcd_show_string(0, 100, 200, 16, 16, "Start Transimit....", BLUE);
		  lcd_show_string(0, 120, 200, 16, 16, "   %", BLUE);    /* Display percent */

          HAL_UART_Transmit_DMA(&huart1, g_sendbuf, SEND_BUF_SIZE);

          /* Wait for the DMA transfer to complete, at which point we can do something else, such as lighting the lamp
           * In practical applications, additional tasks can be performed during data transmission
           */

          while (1)
          {
              /* Wait for the DMA1 Channel4 transfer to complete */
              if ( __HAL_DMA_GET_FLAG(&hdma_usart1_tx, DMA_FLAG_TC4))   /* Wait for the DMA1 Channel4 transfer to complete */
              {
                  __HAL_DMA_CLEAR_FLAG(&hdma_usart1_tx, DMA_FLAG_TC4);  /* Clear the DMA1 Channel4 transfer Completed flag */
                  HAL_UART_DMAStop(&huart1);                  			/* Disable the serial port DMA after the transmission is complete */
                  break;
              }

              pro = __HAL_DMA_GET_COUNTER(&hdma_usart1_tx);             /* Get the number of data remaining */
              len = SEND_BUF_SIZE;                                    	/* total length */
              pro = 1 - (pro / len);                                  	/* Get a percentage */
              pro *= 100;                                             	/* 100 times larger */
              printf("%d\r\n",(unsigned int)pro);
              lcd_show_num(0, 120, pro, 3, 16, BLUE);
          }

          printf("Transimit Finished!\r\n");
		  lcd_show_num(0, 120, 100, 3, 16, BLUE);    							/* Display 100% */
		  lcd_show_string(0, 100, 200, 16, 16, "Transimit Finished!", BLUE); 	/* Prompt transfer complete */


      }

      i++;
      HAL_Delay(10);

      if (i == 20)
      {
    	  LED_TOGGLE();                                              			/* LED state is flipped */
          i = 0;
      }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
```
The flow of the main function is as follows: initialize the value of **g_sendbuf**, then use KEY0 to enable DMA transmission over the serial port, and finally clear the appropriate flag to indicate that the transfer has been completed.


### 4 Running
#### 4.1 Compile & Download
After the compilation is complete, connect the DAP and the Mini Board, and then connect to the computer together to download the program to the Mini Board.
#### 4.2 Phenomenon
Press the reset button to restart the Mini Board, observe the LED flashing on the Mini Board, open the serial port and the host computer **ATK-XCOM** can see the prompt information of the experiment, indicating that the code download is successful. After pressing the KEY0 button, you can see the data sent by the development board in the serial port debugging assistant, and the transmission will be prompted after the completion of transmission, as shown in the following figure:

![ ](./1_docs/3_figures/14_DMA/d3.png)

[jump to tittle](#catalogue)