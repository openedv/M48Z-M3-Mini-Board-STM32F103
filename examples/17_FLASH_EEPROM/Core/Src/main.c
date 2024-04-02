/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       FLASH_EEPROM code
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
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../BSP/LED/led.h"
#include "../../BSP/KEY/key.h"
#include "../../BSP/LCD/lcd.h"
#include "../../ATK_Middlewares/USMART/usmart.h"
#include "../../BSP/STMFLASH/stmflash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

const uint8_t g_text_buf[] = {"STM32 FLASH TEST"};
#define TEXT_LENTH sizeof(g_text_buf) 	/* Array length */

/* SIZE represents half a word length (2 bytes), the size must be an integer multiple of 2, if not, force alignment to an integer multiple of 2 */
#define SIZE TEXT_LENTH / 2 + ((TEXT_LENTH % 2) ? 1 : 0)
#define FLASH_SAVE_ADDR 0X08008000 		/* Set FLASH save address (must be an even number, and its value must be greater than the size of the FLASH occupied by this code + 0X08000000) */

void stm32f103c8t6_show_mesg(void)
{
	/* The serial port outputs the experimental information */
	printf("\n");
	printf("********************************\r\n");
	printf("STM32F103\r\n");
	printf("Flash EEPROM Test\r\n");
	printf("ATOM@ALIENTEK\r\n");
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

    uint8_t key = 0;
    uint8_t i = 0;
    uint8_t datatemp[SIZE];

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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  stm32f103c8t6_show_mesg();
  lcd_init();
  lcd_show_string(0, 5, 240, 32, 32, "STM32F103C8T6", RED);
  lcd_show_string(0, 34, 240, 24, 24, "Flash EEPROM Test", RED);
  lcd_show_string(0, 60, 240, 16, 16, "ATOM@ALIENTEK", RED);
  lcd_show_string(0, 80, 200, 16, 16, "WK_UP:Write  KEY0:Read", RED);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  key = key_scan(0);

	  if (key == WKUP_PRES) /* KEY1 is pressed to write the STM32 FLASH */
	  {
		  lcd_fill(0, 150, 239, 319, WHITE); /* Clear half screen */
		  lcd_show_string(30, 150, 200, 16, 16, "Start Write FLASH....", RED);
		  stmflash_write(FLASH_SAVE_ADDR, (uint16_t *)g_text_buf, SIZE);
		  lcd_show_string(30, 150, 200, 16, 16, "FLASH Write Finished!", RED); /* Prompt transfer complete */
	  }

	  if (key == KEY0_PRES) /* KEY0 is pressed to read the string and display it */
	  {
		  lcd_show_string(30, 150, 200, 16, 16, "Start Read FLASH.... ", RED);
		  stmflash_read(FLASH_SAVE_ADDR, (uint16_t *)datatemp, SIZE);
		  lcd_show_string(30, 150, 200, 16, 16, "The Data Readed Is:  ", RED); /* Prompt transfer complete */
		  lcd_show_string(30, 170, 200, 16, 16, (char *)datatemp, BLUE);       /* Displays the read string */
	  }

	  i++;
	  HAL_Delay(10);

	  if (i == 20)
	  {
		  LED_TOGGLE(); /* A message is displayed indicating that the system is running */
		  i = 0;
	  }
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
