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

#include "adi_circ_buf.h"
#include "adi_cli.h"
#include "cli_dispatch.h"
#include "cli_history.h"
#include "cli_utils.h"

/** @brief rx buffer size */
#define RX_BUFFER_SIZE 256

/** @brief Maximum size (in bytes) for CLI buffer */
#define ADI_CLI_MAX_SIZE (1024 * 10)

/** @brief Maximum size (in bytes) for a message block */
#define ADI_CLI_MAX_MSG_SIZE 512

/**
 * CLI data structure for storing UART Receive output data
 */
typedef struct
{
    /** Pointer to rx circular buffer instance */
    volatile ADI_CIRC_BUF *pRxBuff;
    /** byte index */
    uint8_t rxByte;
    /** Circular buffer for storing received data */
    volatile ADI_CIRC_BUF rxCircBuff;
    /** rx buffer */
    uint8_t rxBuff[RX_BUFFER_SIZE];
} ADI_CLI_RX_DATA;

/**
 * CLI information structure
 */
typedef struct
{
    /** command line interface instance */
    CLI_INTERFACE cliIfData;
    /** history data */
    CLI_HISTORY_DATA cliHistData;
    /** ping buffer for the data transmission */
    uint8_t cliBuffer0[ADI_CLI_MAX_SIZE];
    /** pong buffer for the data transmission */
    uint8_t cliBuffer1[ADI_CLI_MAX_SIZE];
    /** pointer to the message string */
    char *pMsgString;
    /** pointer to the message string to copy */
    char *pMsgStringToCopy;
    /** pointer to the current data buffer filling */
    uint8_t *pBufferToWrite;
    /** CLI data */
    ADI_CLI_RX_DATA cliData;
    /** CLI configuration */
    ADI_CLI_CONFIG config;
    /** Tx completion flag */
    int32_t isTxComplete;
    /** Pointer to temporary memory buffer */
    uint8_t *pTempMemory;
    /** Size of memory in (bytes) given to store temporary data */
    uint32_t tempMemSize;
    /** pointer to the utils data */
    CLI_UTILS_DATA cliUtilsData;
    /** pointer to the dispatch data */
    CLI_DISPATCH_DATA cliDispatchData;
    /** message bytes stored */
    uint32_t cliBytesStored;

} ADI_CLI_INFO;

/**
 * @brief Gets the CLI information.
 * @return info of the CLI.
 */
ADI_CLI_INFO *GetCliInfo(void);

#ifdef __cplusplus
}
#endif

#endif /* __ADI_CLI_PRIVATE_H__ */

/**
 * @}
 */
