/******************************************************************************
 Copyright (c) 2024 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file
 * @brief    Structures to hold CLI Data and Configurations
 * @{
 */

#ifndef __ADI_CLI_PRIVATE_H__
#define __ADI_CLI_PRIVATE_H__

/*=============  I N C L U D E S   =============*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "adi_cli.h"
#include "cli_dispatch.h"
#include "cli_history.h"
#include "cli_private.h"

/** @brief Maximum size (in bytes) for CLI buffer */
#define ADI_CLI_MAX_SIZE (1024 * 10)

/** @brief Maximum size (in bytes) for a message block */
#define ADI_CLI_MAX_MSG_SIZE 512

/**
 * CLI information structure
 */
typedef struct
{
    /** command line interface instance */
    CLI_PRIVATE cliIfData;
    /** ping buffer for the data transmission */
    uint8_t cliBuffer0[ADI_CLI_MAX_SIZE];
    /** pong buffer for the data transmission */
    uint8_t cliBuffer1[ADI_CLI_MAX_SIZE];
    /** pointer to the message string */
    char *pMsgString;
    /** pointer to the message string to copy */
    char *pMsgStringToCopy;
    /** CLI configuration */
    ADI_CLI_CONFIG config;
    /** Tx completion flag */
    int32_t isTxComplete;
    /** Pointer to temporary memory buffer */
    uint8_t *pTempMemory;
    /** Size of memory in (bytes) given to store temporary data */
    uint32_t tempMemSize;
} ADI_CLI_INFO;

#ifdef __cplusplus
}
#endif

#endif /* __ADI_CLI_PRIVATE_H__ */

/**
 * @}
 */
