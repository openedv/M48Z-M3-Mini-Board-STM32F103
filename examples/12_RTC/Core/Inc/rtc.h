/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.h
  * @brief   This file contains all the function prototypes for
  *          the rtc.c file
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
#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN Private defines */

/* Time structure, including information such as year, month, day, week, minute and second */
typedef struct
{
    uint8_t hour;       /* hour */
    uint8_t min;        /* minute */
    uint8_t sec;        /* second */
    /* Gregorian calendar year month Day week */
    uint16_t year;      /* year */
    uint8_t  month;     /* month */
    uint8_t  date;      /* date */
    uint8_t  week;      /* week */
} _calendar_obj;
extern _calendar_obj calendar;                      /* Time structure */

/* USER CODE END Private defines */

void MX_RTC_Init(void);

/* USER CODE BEGIN Prototypes */

/* 接口函数 */
void rtc_get_time(void);                            														/* Obtain RTC time information */
void hal_rtc_get_tim(void);																					/* The RTC time information is officially obtained */
uint16_t rtc_read_bkr(uint32_t bkrx);               														/* Read BKR register */
void rtc_write_bkr(uint32_t bkrx, uint16_t data);   														/* Write BKR register */
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day);    										/* Get the day of the week by year, month and day */
uint8_t rtc_set_time(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);   /* Set time */
uint8_t rtc_set_alarm(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);  /* Set the alarm time */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __RTC_H__ */

