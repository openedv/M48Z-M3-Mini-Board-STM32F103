/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       IAP BootLoader V1.0
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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../BSP/LED/led.h"
#include "../../BSP/KEY/key.h"
#include "../../BSP/STMFLASH/stmflash.h"
#include "../../ATK_Middlewares/IAP/iap.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


void stm32f103c8t6_show_mesg(void)
{
	/* The serial port outputs the experimental information */
	printf("\n");
	printf("********************************\r\n");
	printf("STM32F103\r\n");
	printf("Flash EEPROM Test\r\n");
	printf("ATOM@ALIENTEK\r\n");
    printf("KEY0:Starting firmware update\r\n");
    printf("KEY_UP:Start executing FLASH user code\r\n");
	printf("********************************\r\n");
	printf("\r\n");
}

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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
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

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

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
