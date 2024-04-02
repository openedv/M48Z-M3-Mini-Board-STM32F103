/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "adc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;

/* ADC1 init function */
void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  HAL_ADCEx_Calibration_Start(&hadc1);              /* Calibrating ADC */

  /* USER CODE END ADC1_Init 2 */

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA1     ------> ADC1_IN1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PA1     ------> ADC1_IN1
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/**
* @brief 	Sets the ADC channel sampling time
* @param 	adcx	: adc handle pointer,ADC_HandleTypeDef
* @param 	ch		: indicates the channel ID, ADC_CHANNEL_0 to ADC_CHANNEL_17
* @param 	stime	: Sampling time ranges from 0 to 7. The corresponding relationship is as follows:
* @arg 		ADC_SAMPLETIME_1CYCLE_5, 1.5 ADC clock periods ADC_SAMPLETIME_7CYCLES_5, 7.5 ADC clock periods
* @arg 		ADC_SAMPLETIME_13CYCLES_5, 13.5 ADC clock cycles ADC_SAMPLETIME_28CYCLES_5, 28.5 ADC clock cycles
* @arg 		ADC_SAMPLETIME_41CYCLES_5, 41.5 ADC clock periods ADC_SAMPLETIME_55CYCLES_5, 55.5 ADC clock periods
* @arg 		ADC_SAMPLETIME_71CYCLES_5, 71.5 ADC clock periods ADC_SAMPLETIME_239CYCLES_5, 239.5 ADC clock periods
* @param 	rank	: A collection number that needs to be set during multi-channel collection.
			If you define rank = 1 of channle1, and rank = 2 of channle2,
			Then the variable array AdcDMA[0] corresponding to your DMA cache space is the conversion result of channle1, and AdcDMA[1] is the conversion result of channel 2.
			The single-channel DMA is set to ADC_REGULAR_RANK_1
* @arg 		Numbers 1 to 16: ADC_REGULAR_RANK_1 to ADC_REGULAR_RANK_16
* @retval 	None
*/
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch, uint32_t rank, uint32_t stime)
{
    ADC_ChannelConfTypeDef adc_ch_conf;

    adc_ch_conf.Channel = ch;                            /* Channel */
    adc_ch_conf.Rank = rank;                             /* Rank */
    adc_ch_conf.SamplingTime = stime;                    /* SamplingTime */
    HAL_ADC_ConfigChannel(adc_handle, &adc_ch_conf);     /* Channel configuration */
}

/**
* @brief 	Gets the result of ADC conversion
* @param 	ch: The channel value ranges from 0 to 17. The value ranges from ADC_CHANNEL_0 to ADC_CHANNEL_17
* @retval 	None
*/
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
* @param 	times	: indicates the number of times obtained
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

/* USER CODE END 1 */
