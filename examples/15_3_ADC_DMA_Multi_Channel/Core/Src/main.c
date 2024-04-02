/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       ADC_DMA_Multi_Channel code
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


/* USER CODE BEGIN Header */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../BSP/LED/led.h"
#include "../../BSP/KEY/key.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

#define ADC_DMA_BUF_SIZE (7 * 50)                           /* ADC DMA buffer size */
static uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];            /* ADC DMA buffer */
extern uint8_t g_adc_dma_sta;                               /* DMA transfer complete flag */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void stm32f103c8t6_show_mesg(void)
{
	/* The serial port outputs the experimental information */
	printf("\n");
	printf("********************************\r\n");
	printf("STM32F103\r\n");
	printf("ADC(MultiChannel DMA Read) Test\r\n");
	printf("ATOM@ALIENTEK\r\n");
	printf("********************************\r\n");
	printf("\r\n");
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

    uint16_t i;
    uint8_t j;
    uint32_t sum;
    uint16_t adcdata;
    uint16_t voltage;

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
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  stm32f103c8t6_show_mesg();
  HAL_DMA_Start_IT(&hdma_adc1, (uint32_t)&ADC1->DR, (uint32_t)&g_adc_dma_buf, 0);	/* Start DMA and enable interrupt */
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&g_adc_dma_buf, 0);           				/* Turn on the ADC and transmit the result via DMA */

  adc_dma_enable(ADC_DMA_BUF_SIZE);   												/* Start ADC DMA acquisition */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      if (g_adc_dma_sta == 1)                             	/* Wait for the DMA transfer to end */
      {
          for (j = 0; j < 7; j++)                         	/* Traversal processing of 7 channels of data */
          {
              sum = 0;

              for (i = 0; i < (ADC_DMA_BUF_SIZE / 7); i++)	/* The multiple sampling values of the ADC are averaged */
              {
                  sum += g_adc_dma_buf[(7 * i) + j];
              }

              adcdata = sum / (ADC_DMA_BUF_SIZE / 7);
              printf("\r\n original value[%d]：%d\r\n", j + 1, adcdata);

              voltage = (adcdata * 3300) / 4095;          	/* Calculate the actual voltage value (1000x enlargement) */
              printf("\r\nvoltage[%d]:%d.%d\r\n", j + 1, voltage / 1000, voltage % 1000);

              g_adc_dma_sta = 0;                          	/* Clear the DMA transfer end flag */
              adc_dma_enable(ADC_DMA_BUF_SIZE);           	/* Enable the next DMA transmission of ADC data */
          }
      }

	  LED_TOGGLE();       								  	/* LED state is flipped */
	  HAL_Delay(100);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
