## ADC_Temperature example<a name="catalogue"></a>


### 1 Brief
The function of this program is that ADC collects the voltage on channel 16, converts the voltage value into the temperature value through the algorithm, and displays the converted temperature value on the host computer of the serial port.
### 2 Hardware Hookup
The hardware resources used in this experiment are:
+ LED - PA8
+ USART1 - PA9\PA10
+ ADC1 - Channel16

The ADC used in this experiment is an on-chip resource of STM32F103, so there is no corresponding connection schematic.

### 3 STM32CubeIDE Configuration


We copy the ** 10_TFTLCD ** project and name both the project and the.ioc file **16_ADC_Temperature**.Next we start the ADC configuration by double-clicking the **16_ADC_Temperature.ioc** file.

The ADC channel 16 used in this experiment is set as shown below.

![ ](./1_docs/3_figures/16_ADC_Temperature/a11.png)

![ ](./1_docs/3_figures/16_ADC_Temperature/a12.png)

Click **File > Save**, and you will be asked to generate code.Click **Yes**.

##### code
We add some code to adc.c, starting with ADC1's initialization function ``MX_ADC1_Init``, as follows.
###### adc.c
```c#
  /* USER CODE BEGIN ADC1_Init 2 */
  HAL_ADCEx_Calibration_Start(&hadc1);   /* Calibration ADC */
  /* USER CODE END ADC1_Init 2 */
```
This line of code is added to the initialization function to calibrate the ADC.
We also added three separate functions as follows.
```c#
uint32_t adc_get_result(uint32_t ch)
{
    adc_channel_set(&hadc1 , ch, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_239CYCLES_5);    /* Set the channel, sequence, and sampling time */

    HAL_ADC_Start(&hadc1);                            /* Start ADC */
    HAL_ADC_PollForConversion(&hadc1, 10);            /* Polling conversion */
    return (uint16_t)HAL_ADC_GetValue(&hadc1);        /* Returns the result of the last conversion of the ADC1 rule group */
}

/**
* @brief 	Obtains the conversion value of channel ch, takes times, and averages
* @param 	ch		: Channel number, 0 to 17
* @param 	times	: Indicates the number of times obtained
* @retval 	Indicates the average value of The times conversion result of channel ch
*/
uint32_t adc_get_result_average(uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)     /* takes times */
    {
        temp_val += adc_get_result(ch);
        HAL_Delay(5);
    }

    return temp_val / times;        /* Return average */
}

/**
* @brief 	Obtains the temperature of the internal temperature sensor
* @param 	None
* @retval 	Temperature value (expanded by 100 times, unit: ° C)
*/
short adc_get_temperature(void)
{
    uint32_t adcx;
    short result;
    double temperature;

    adcx = adc_get_result_average(ADC_CHANNEL_TEMPSENSOR, 20);  /* Read the internal temperature sensor channel and average it 10 times */
    temperature = (float)adcx * (3.3 / 4096);               	/* Convert to a voltage value */
    temperature = (1.43 - temperature) / 0.0043 + 25;       	/* Calculating temperature */
    result = temperature *= 100;    							/* 100 times larger */
    return result;
}
```
After obtaining the output voltage of the temperature sensor, the above code converts the voltage value to the actual temperature value through the temperature conversion formula.

###### main.c
Here's the main function.
```c#
int main(void)
{
  /* USER CODE BEGIN 1 */
	short temp;

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
  MX_SPI1_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  stm32f103c8t6_show_mesg();
  lcd_init();
  lcd_show_string(0, 5, 240, 32, 32, "STM32F103C8T6", RED);
  lcd_show_string(0, 34, 240, 24, 24, "Temperature TEST", RED);
  lcd_show_string(0, 60, 240, 16, 16, "ATOM@ALIENTEK", RED);
  lcd_show_string(0, 80, 240, 16, 16, "TEMPERATE: 00.00C", BLUE);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  temp = adc_get_temperature();   							/* Get the temperature value */

	  if (temp < 0)
	  {
		  temp = -temp;
		  lcd_show_string(10 * 8, 80, 16, 16, 16, "-", BLUE);   /* Display negative sign */
	  }
	  else
	  {
		  lcd_show_string(10 * 8, 80, 16, 16, 16, " ", BLUE);   /* Unsigned */
	  }

	  printf("Temperature:%d.%dC°\r\n",temp / 100, temp % 100);
	  lcd_show_xnum(11 * 8, 80, temp / 100, 2, 16, 0, BLUE);    /* Display integer part */
	  lcd_show_xnum(14 * 8, 80, temp % 100, 2, 16, 0X80, BLUE); /* Display integer part */

	  LED_TOGGLE();       										/* LED state is flipped */
	  HAL_Delay(100);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
```
This code is relatively simple, constantly calling the ``adc_get_temperature`` function to obtain the temperature value collected by the internal temperature sensor, and the temperature value is displayed in real time through the serial port host computer.


### 4 Running
#### 4.1 Compile & Download
After the compilation is complete, connect the DAP and the minimal system board, and then connect to the computer together to download the program to the development board.
#### 4.2 Phenomenon
Press the reset button to restart the development board, observe the LED flashing on the development board, open the serial port and the host computer **ATK-XCOM** can see the prompt information of the experiment, indicating that the code download is successful. The temperature value of the internal temperature sensor on the minimum system board will be displayed on the host computer of the serial port, as shown in the following figure.

![ ](./1_docs/3_figures/16_ADC_Temperature/a13.png)

[jump to tittle](#catalogue)