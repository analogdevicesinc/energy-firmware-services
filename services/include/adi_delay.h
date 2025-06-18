/******************************************************************************
 Copyright (c) 2022 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file        adi_delay.h
 * @brief       wrapper for cmsis or os delay
 * @{
 */

#ifndef ADI_DELAY_H_
#define ADI_DELAY_H_

#ifdef CMSIS_OS2
#include "cmsis_os2.h"
#else
#include "stm32f4xx_hal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

/*============= D E F I N E S =============*/
#ifdef CMSIS_OS2
/** cmsis os delay*/
#define ADI_DELAY(delay) osDelay(delay)
#else
/** hal delay for stm */
#define ADI_DELAY(delay) HAL_Delay(delay)
#endif

#ifdef __cplusplus
}
#endif

#endif /* ADI_DELAY_H_ */
/**
 * @}
 */
