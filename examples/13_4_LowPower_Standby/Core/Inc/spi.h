/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* SPI bus speed settings */
#define SPI_SPEED_2		0
#define SPI_SPEED_4 	1
#define SPI_SPEED_8 	2
#define SPI_SPEED_16 	3
#define SPI_SPEED_32 	4
#define SPI_SPEED_64 	5
#define SPI_SPEED_128 	6
#define SPI_SPEED_256 	7

/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_SPI1_Init(void);

/* USER CODE BEGIN Prototypes */

void spi1_set_speed(uint8_t speed);
uint8_t spi1_read_write_byte(uint8_t txdata);
void spi1_write_data(uint8_t *tx_data, uint16_t size);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

