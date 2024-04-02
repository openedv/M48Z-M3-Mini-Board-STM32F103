/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
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
#include "rtc.h"

/* USER CODE BEGIN 0 */

_calendar_obj calendar;         /* Time structure */

/* static function */
static uint8_t rtc_is_leap_year(uint16_t year);     /* Determine whether the current year is a leap year */
static long rtc_date2sec(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);   /* Convert year, month, day, hour, and second into seconds */

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

	/* Check whether the clock is configured for the first time */
    uint16_t bkpflag = 0;

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  bkpflag = rtc_read_bkr(0);  	/* Read the value of BKP0 */

  if (bkpflag != 0X5050)    	/* Previously uninitialized, reconfigure */
  {
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x9;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_MARCH;
  DateToUpdate.Date = 0x11;
  DateToUpdate.Year = 0x24;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
	  rtc_set_time(2024, 3, 11, 9, 0, 0);       	/* Set time */
	  rtc_write_bkr(0, 0X5050);						/* Write 0x5050 so that the RTC initialization time is not repeated */
  }
  __HAL_RTC_ALARM_ENABLE_IT(&hrtc, RTC_IT_SEC);   	/* Allow second interrupt */
  rtc_get_time(); /* Update time */
  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();

    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/**
* @brief 	RTC writes to back-up area SRAM
* @param 	bkrx: backup register number. The value ranges from 0 to 41
			It corresponds to RTC_BKP_DR1 to RTC_BKP_DR42
* @param 	data: Data to be written,16 bits
* @retval   None
*/
void rtc_write_bkr(uint32_t bkrx, uint16_t data)
{
    HAL_PWR_EnableBkUpAccess(); /* Disable write protection for the backup area */
    HAL_RTCEx_BKUPWrite(&hrtc, bkrx + 1, data);
}

/**
* @brief 	RTC reads the backup area SRAM
* @param 	bkrx: backup register number. The value ranges from 0 to 41
			It corresponds to RTC_BKP_DR1 to RTC_BKP_DR42
* @retval 	The value read
*/
uint16_t rtc_read_bkr(uint32_t bkrx)
{
    uint32_t temp = 0;
    temp = HAL_RTCEx_BKUPRead(&hrtc, bkrx + 1);
    return (uint16_t)temp; /* Returns the read value */
}

/**
* @brief 	Determines if the year is a leap year
* @note 	Monthly Days table:
			Month 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
* 			Leap Year 31 29 31 30 31 30 31 31 31 30 30 31
* 			Non-leap year 31 28 31 30 31 30 31 31 31 30 31 30 30 31
* @param 	year: year
* @retval 	0, non-leap year; 1, is leap year;
*/
static uint8_t rtc_is_leap_year(uint16_t year)
{
    /* Leap year rule: four years leap, one hundred years no leap, four hundred years and leap */
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
* @brief 	Set time, including year, month, day, hour, minute and second
* @note 	Is based on January 1, 1970, adding up time thereafter
* 			Legal year: 1970-2105
			HAL defaults to year 2000
* @param 	syear: year
* @param 	smon: Month
* @param 	sday: Date
* @param 	hour: hour
* @param 	min: minutes
* @param 	sec: seconds
* @retval 	0, successful; 1) Failure;
*/
uint8_t rtc_set_time(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t seccount = 0;

    seccount = rtc_date2sec(syear, smon, sday, hour, min, sec); /* Convert year, month, day, hour, and second into total seconds */

    __HAL_RCC_PWR_CLK_ENABLE(); /* Enable PWR clock */
    __HAL_RCC_BKP_CLK_ENABLE(); /* Enable BKP clock */
    HAL_PWR_EnableBkUpAccess(); /* Disable write protection for BKP */
    /* The above three steps are a must! */

    RTC->CRL |= 1 << 4;         /* Enable configuration */

    RTC->CNTL = seccount & 0xffff;
    RTC->CNTH = seccount >> 16;

    RTC->CRL &= ~(1 << 4);      /* Configuration update */

    while (!__HAL_RTC_ALARM_GET_FLAG(&hrtc, RTC_FLAG_RTOFF));       /* Wait for the RTC register operation to complete, that is, wait for RTOFF == 1 */

    return 0;
}

/**
* @brief 	Set the alarm clock, specific to the year, month, day, minute and second
* @note 	Is based on January 1, 1970, adding up time thereafter
* 			Legal year: 1970-2105
* @param 	syear	: Year
* @param 	smon	: Month
* @param 	sday	: Date
* @param 	hour	: hour
* @param 	min		: minutes
* @param 	sec		: seconds
* @retval 	0, successful; 1) Failure;
*/
uint8_t rtc_set_alarm(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t seccount = 0;

    seccount = rtc_date2sec(syear, smon, sday, hour, min, sec); /* Convert year, month, day, hour, and second into total seconds */

    __HAL_RCC_PWR_CLK_ENABLE(); /* Enable PWR clock */
    __HAL_RCC_BKP_CLK_ENABLE(); /* Enable BKP clock */
    HAL_PWR_EnableBkUpAccess(); /* Disable write protection for BKP */
    /* The above three steps are a must! */

    RTC->CRL |= 1 << 4;         /* Enable configuration */

    RTC->ALRL = seccount & 0xffff;
    RTC->ALRH = seccount >> 16;

    RTC->CRL &= ~(1 << 4);      /* Configuration update */

    while (!__HAL_RTC_ALARM_GET_FLAG(&hrtc, RTC_FLAG_RTOFF));       /* Wait for the RTC register operation to complete, that is, wait for RTOFF == 1 */

    return 0;
}

/**
* @brief 	Gets the current time
* @note 	This function does not return the time directly, the time data is stored in the calendar structure
* @param 	None
* @retval 	None
*/
rtc_get_time(void)
{
    static uint16_t daycnt = 0;
    uint32_t seccount = 0;
    uint32_t temp = 0;
    uint16_t temp1 = 0;
    const uint8_t month_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; /* A table of months and dates for ordinary years */

    seccount = RTC->CNTH;       /* Get the value in the counter (seconds) */
    seccount <<= 16;
    seccount += RTC->CNTL;

    temp = seccount / 86400;    /* Get the number of days (in seconds) */

    if (daycnt != temp)         /* More than a day */
    {
        daycnt = temp;
        temp1 = 1970;           /* Since 1970 */

        while (temp >= 365)
        {
            if (rtc_is_leap_year(temp1)) /* It is a leap year */
            {
                if (temp >= 366)
                {
                    temp -= 366; /* The number of seconds in leap years */
                }
                else
                {
                    break;
                }
            }
            else
            {
                temp -= 365;    /* non-leap year */
            }

            temp1++;
        }

        calendar.year = temp1;  /* Acquired year */
        temp1 = 0;

        while (temp >= 28)      /* More than a mounth */
        {
            if (rtc_is_leap_year(calendar.year) && temp1 == 1) /* It is not a leap year/February */
            {
                if (temp >= 29)
                {
                    temp -= 29; /* The number of seconds in leap years */
                }
                else
                {
                    break;
                }
            }
            else
            {
                if (temp >= month_table[temp1])
                {
                    temp -= month_table[temp1]; /* non-leap year */
                }
                else
                {
                    break;
                }
            }

            temp1++;
        }

        calendar.month = temp1 + 1; /* Acquired month */
        calendar.date = temp + 1;   /* Get date */
    }

    temp = seccount % 86400;                                                    /* Get seconds */
    calendar.hour = temp / 3600;                                                /* Hour */
    calendar.min = (temp % 3600) / 60;                                          /* Minute */
    calendar.sec = (temp % 3600) % 60;                                          /* Second */
    calendar.week = rtc_get_week(calendar.year, calendar.month, calendar.date); /* Acquisition week */
}

/**
* @brief 	Converts year, month, day, hour, minute and second into seconds
* @note 	Enter the Gregorian calendar date to get the week (start time is: March 1, AD 0, enter any date later, you can get the correct week)
* Using 	The Kim Larson calculation formula, the principle is explained in this post:
*              https://www.cnblogs.com/fengbohello/p/3264300.html
* @param 	syear	: year
* @param 	smon	: Month
* @param 	sday	: Date
* @retval 	0, Sunday; 1 to 6: Monday to Saturday
*/
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day)
{
    uint8_t week = 0;

    if (month < 3)
    {
        month += 12;
        --year;
    }

    week = (day + 1 + 2 * month + 3 * (month + 1) / 5 + year + (year >> 2) - year / 100 + year / 400) % 7;
    return week;
}

/**
* @brief 	Converts year, month, day, hour, minute and second into seconds
* @note 	Using January 1, 1970 as the base, January 1, 1970, 0:0:0, indicates the 0 second
* 			Max indicates the year 2105, because uint32_t Max indicates the number of seconds in 136 years (not including leap years)!
* 			This code reference only linux mktime function, principle description see this post:
*              http://www.openedv.com/thread-63389-1-1.html
* @param 	syear	: year
* @param 	smon	: Month
* @param 	sday	: Date
* @param 	hour	: hour
* @param 	min		: minutes
* @param 	sec		: seconds
* @retval 	Number of seconds after conversion
*/
static long rtc_date2sec(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t Y, M, D, X, T;
    signed char monx = smon;    /* Convert the month to a signed value for easy later operations */

    if (0 >= (monx -= 2))       /* 1..12 -> 11,12,1..10 */
    {
        monx += 12;             /* Puts Feb last since it has leap day */
        syear -= 1;
    }

    Y = (syear - 1) * 365 + syear / 4 - syear / 100 + syear / 400; /* The number of leap years from A.D. 1 to the present */
    M = 367 * monx / 12 - 30 + 59;
    D = sday - 1;
    X = Y + M + D - 719162;                      /* Subtract the number of days from A.D. 1 to 1970 */
    T = ((X * 24 + hour) * 60 + min) * 60 + sec; /* Total seconds */
    return T;
}

/**
 * @brief       Get the current time
 * @note      	This function does not return the time directly; the time data is stored in the calendar structure
 * @param       None
 * @retval      None
 */
void hal_rtc_get_tim(void)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
  calendar.hour = stimestructureget.Hours;                                   	/* Hour */
  calendar.min = stimestructureget.Minutes;                                 	/* Minute */
  calendar.sec = stimestructureget.Seconds;                                 	/* Second */
  calendar.year = 2000 + sdatestructureget.Year;								/* Year */
  calendar.month = sdatestructureget.Month;										/* Month */
  calendar.date = sdatestructureget.Date;										/* Date */
  calendar.week = rtc_get_week(calendar.year, calendar.month, calendar.date);	/* Week */
}

/* USER CODE END 1 */
