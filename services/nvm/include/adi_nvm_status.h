/******************************************************************************
 Copyright (c) 2024 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file         adi_nvm_status.h
 * @brief        Status Codes of NVM Service
 * @addtogroup ADI_NVM
 * @{
 */

#ifndef __ADI_NVM_STATUS_H__
#define __ADI_NVM_STATUS_H__

/*=============  I N C L U D E S   =============*/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup   NVMSTATUS Return Codes

    @brief NVM Service returns #ADI_NVM_STATUS for every API
 call. The application can look at the return codes and can take appropriate
 action for recovery. If multiple errors are there, the API will return  the
    return code of first encountered error.

    If an API returns an error, its description in the doxygen documentation
    gives hints at the cause of the error.

    Errors detected at development time
    regardless of the type of error, all errors should be identified and only
    ignored if the cause is known and understood, including informational
    errors.
    - Identify the API that returns the error.
    - Look at the doxygen description of the API that returned the error.
    - Ensure that any recommendations in the API description are followed.

    @note It is recommended that at the development stage all errors are
    evaluated, even the ones that can be ignored once the product is deployed,
    since they may help debugging issues in the application. Look into each
    error to determine the possible causes in each API.
 * @{
 */

/**
 * Status Codes of NVM service
 */
typedef enum
{
    /** No error, API succeeded. */
    ADI_NVM_STATUS_SUCCESS = 0u,
    /** Init failure */
    ADI_NVM_STATUS_INIT_FAILED,
    /** Invalid Memory */
    ADI_NVM_STATUS_INVALID_MEMORY,
    /** handle is a Null pointer */
    ADI_NVM_STATUS_NULL_PTR,
    /** State memory provided to #adi_nvm_Create is less than required memory
     * for the configuration.
     */
    ADI_NVM_STATUS_INSUFFICIENT_STATE_MEMORY,
    /** Communication error to send or receive data through SPI */
    ADI_NVM_STATUS_COMM_ERROR,
    /** Invalid Product ID. */
    ADI_NVM_STATUS_INVALID_PRODUCT_ID,
    /** CRC Mismatch of data received from NVM */
    ADI_NVM_STATUS_CRC_MISMATCH,
    /** Invalid Number of registers */
    ADI_NVM_STATUS_INVALID_NUM_REGISTERS,
    /** Invalid address received */
    ADI_NVM_STATUS_INVALID_ADDRESS,
    /** Erase failed */
    ADI_NVM_STATUS_PAGE_ERASE_FAILED
} ADI_NVM_STATUS;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __ADI_NVM_STATUS_H__ */

/**
 * @}
 */
