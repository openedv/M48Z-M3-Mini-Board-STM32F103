/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       USB virtual serial port(Slave) code
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
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../BSP/LED/led.h"
#include "../../ATK_Middlewares/MALLOC/malloc.h"
#include "usbd_cdc_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

extern volatile uint8_t g_device_state;     /* USB connection status */

void stm32f103c8t6_show_mesg(void)
{
	/* The serial port outputs the experimental information */
	printf("\n");
	printf("********************************\r\n");
	printf("STM32F103\r\n");
	printf("USB Virtual USART Test\r\n");
	printf("ATOM@ALIENTEK\r\n");
	printf("USB Connecting...\r\n");
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
    uint16_t len;
    uint16_t times = 0;
    uint8_t usbstatus = 0;

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
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  stm32f103c8t6_show_mesg();
  my_mem_init(SRAMIN);                		/* Initializes the internal SRAM memory pool */

  usbd_port_config(0);    					/* USB disconnect first */
  HAL_Delay(500);
  usbd_port_config(1);    					/* USB connect again */
  HAL_Delay(500);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (usbstatus != g_device_state)   	/* The USB connection status changes. */
	  {
		  usbstatus = g_device_state; 		/* Record new status */

		  if (usbstatus == 1)
		  {
			  printf("USB Connected    "); 	/* A message is displayed indicating that the USB connection is successful */
		  }
		  else
		  {
			  printf("USB disConnected "); 	/* Prompt USB disconnect */
		  }
	  }

	  if (g_usb_usart_rx_sta & 0x8000)
	  {
		  len = g_usb_usart_rx_sta & 0x3FFF;  	/* Get the length of the received data */
		  usb_printf("\r\nThe length of the message you are sending is:%d\r\n\r\n", len);
		  cdc_vcp_data_tx(g_usb_usart_rx_buffer, len);;
		  usb_printf("\r\n\r\n");				/* Insert line feed */
		  g_usb_usart_rx_sta = 0;
	  }
	  else
	  {
		  times++;

		  if (times % 5000 == 0)
		  {
			  usb_printf("\r\nSTM32F103 development board USB Virtual USART Test\r\n");
			  usb_printf("ALIENTEK\r\n\r\n");
		  }

		  if (times % 200 == 0)usb_printf("Please enter the data and end with the Enter key\r\n");

		  if (times % 30 == 0)
		  {
			  LED_TOGGLE();  /* Blinking LED indicates that the system is running */
		  }

		  HAL_Delay(10);
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
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
