/******************************************************************************
 Copyright (c) 2024 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file         adi_cli_status.h
 * @brief        Status Codes of CLI Service
 * @addtogroup ADI_CLI
 * @{
 */

#ifndef __ADI_CLI_STATUS_H__
#define __ADI_CLI_STATUS_H__

/*=============  I N C L U D E S   =============*/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup   CLISTATUS Return Codes

    @brief CLI Service returns #ADI_CLI_STATUS for every API
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
 * Status Codes of CLI service
 */
typedef enum
{
    /** No error, API succeeded. */
    ADI_CLI_STATUS_SUCCESS = 0u,
    /** handle is a Null pointer */
    ADI_CLI_STATUS_NULL_PTR,
    /** State memory provided to #adi_cli_Create is less than required memory
     * for the configuration.
     */
    ADI_CLI_STATUS_INSUFFICIENT_STATE_MEMORY,
    /** Temporary memory provided to #adi_cli_Create is less than required memory
     * for the configuration.
     */
    ADI_CLI_STATUS_INSUFFICIENT_TEMP_MEMORY,
    /** Communication error to send or receive data through UART */
    ADI_CLI_STATUS_COMM_ERROR,
    /** Buffer is full */
    ADI_CLI_STATUS_BUFFER_FULL,
    /** Invalid command */
    ADI_CLI_STATUS_INVALID_COMMAND,
    /** Transmission in progress */
    ADI_CLI_STATUS_TRANSMISSION_IN_PROGRESS,
} ADI_CLI_STATUS;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __ADI_CLI_STATUS_H__ */

/**
 * @}
 */
