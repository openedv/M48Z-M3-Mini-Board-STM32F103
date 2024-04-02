## USB_VSP example<a name="catalogue"></a>


### 1 Brief
The function of the code is to control the LED by creating a task, while the serial port prints the corresponding string every second.
### 2 Hardware Hookup
The hardware resources used in this experiment are:
+ LED - PA8
+ USART1 - PA9\PA10

The FreeRTOS used in this chapter is a software library, so there is no corresponding connection schematic.

### 3 STM32CubeIDE Configuration


We copy the **12_RTC** project and name both the project and the.ioc file **21_FreeRTOS_Demo**.Next we start the DAC configuration by double-clicking the **21_FreeRTOS_Demo.ioc** file.However, it should be noted that because the RAM of C8T6 is too small, the stack overflow will be caused by too much code, so the LCD driver file needs to be deleted and the LCD driver file is not needed in this experiment

Configure the FreeRTOS as shown in the following figure:

First click **Middleware and Software Packs->FREERTOS**,and choose **CMSIS_V2** in **Interface**， as shown below.

![ ](./1_docs/3_figures/21_FreeRTOS_Demo/f1.png)

This option has three options: Disable, CMSIS_V1, and CMSIS_V2. Since we will be using FreeRTOS,
Therefore, you need to choose between two options: CMSIS_V1 and CMSIS_V2. For the difference between the two, detail.For more information, see the definition of the microcontroller software interface standard CMSIS on the ARM website.

For the rest of the FreeRTOS configuration, keep the default Settings.

![ ](./1_docs/3_figures/21_FreeRTOS_Demo/f2.png)

In this experiment, we need to create two tasks, namely task01 and task02, and then configure these two tasks,as shown below.

![ ](./1_docs/3_figures/21_FreeRTOS_Demo/f3.png)

Follow the steps above. You can complete the task name, task function, and task stack Settings.

Then Click **System Core > SYS**,tick count source for configuring the HAL library .

![ ](./1_docs/3_figures/21_FreeRTOS_Demo/f4.png)

Because freertos scheduling clock is mandatory to use systick and systick's interrupt priority is set to the lowest,this will cause the delay function provided by the HAL library to produce an inaccurate count.So we chose TIM1 to provide a time benchmark for the HAL library.

##### code
###### main.c
```c#
int main(void)
{
  /* USER CODE BEGIN 1 */

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

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}
```

### 4 Running
#### 4.1 Compile & Download
After the compilation is complete, connect the DAP and the Mini Board, and then connect to the computer together to download the program to the development board.
#### 4.2 Phenomenon
Press the reset button to restart the development board, the LED is on and off every second,and connect a Mini HSDAP module outside the DuPont wire, open the "XCOM" on the PC, and you can see that "STM32F103C8T6Runing..... \r\n " character string is printed every second.This indicates that the development board is running the program and output debugging information.

[jump to tittle](#catalogue)
