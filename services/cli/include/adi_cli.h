/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file adi_cli.h
 * @defgroup SERVICES Firmware Services
 * @brief APIs for creating, initializing, and interacting with firmware services. These APIs
 * provide a consistent interface for creating, configuring, and using different firmware services.
 * Each service uses a handle to manage its state and a configuration structure to set up
 * hardware-specific or application-specific options.
 * @{
 */

/**
 * @defgroup ADI_CLI CLI Service
 * @brief Command Line Interface routines for user interaction.
 *
 * @details
 * ## Overview
 *
 * ## Typical API Usage
 * 1. **Create the CLI Instance**: Call `adi_cli_Create()` to allocate and initialize internal
 * resources.
 * 2. **Configure the CLI**: Populate an `ADI_CLI_CONFIG` structure with required function pointers.
 * 3. **Initialize the CLI**: Call `adi_cli_Init()` with the configuration structure.
 * 4. **Run the CLI Interface**: Periodically call `adi_cli_Interface()` to process input and
 * dispatch commands.
 *
 * @{
 */
#ifndef __ADI_CLI_H__
#define __ADI_CLI_H__

/*============= I N C L U D E S =============*/
#include "adi_circ_buf.h"
#include "adi_cli_status.h"
#include "cli_dispatch.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============= F U N C T I O N  P R O T O T Y P E S =============*/

/** @defgroup CLIINIT Handle and Configurations
 *
 * The CLI service uses a handle (`ADI_CLI_HANDLE`) to keep track of its state. Pass this handle to
 * CLI API calls and the configuration (`ADI_CLI_CONFIG`) lets you set up how the CLI communicates
 * and which commands it supports.
 * @note Only a single CLI instance is supported. All memory provided to the CLI service must be
 * 32-bit aligned.
 *
 * @brief CLI Service configuration structures and types.
 * @{
 */

/** Device handle for CLI API functions, obtained from Create API. */
typedef void *ADI_CLI_HANDLE;
/** Function pointer type for asynchronous transmit. */
typedef int32_t (*ADI_CLI_TRANSMIT_ASYNC_FUNC)(void *, uint8_t *, uint32_t);
/** Function pointer type for asynchronous receive. */
typedef int32_t (*ADI_CLI_RECEIVE_ASYNC_FUNC)(void *, char *, uint32_t);

/**
 * CLI configuration structure.
 */
typedef struct
{
    /** Function pointer for asynchronous transmission. */
    ADI_CLI_TRANSMIT_ASYNC_FUNC pfTransmitAsync;
    /** Function pointer for asynchronous reception. */
    ADI_CLI_RECEIVE_ASYNC_FUNC pfReceiveAsync;
    /** Function pointer to the command dispatch table. */
    const Command *pDispatchTable;
    /** Number of commands in the dispatch table. */
    int32_t numRecords;
    /** User-defined handle for callback context. */
    void *hUser;

} ADI_CLI_CONFIG;

/** @} */

/** @defgroup CLIAPI Service API
 * @brief API functions for creating, initializing, and interacting with the CLI Service.
 * All APIs return codes defined in #ADI_CLI_STATUS.
 * @{
 */

/**
 * @brief Initializes the CLI with the provided configuration.
 * @param[in] pConfig Pointer to the CLI configuration structure.
 * @return  #ADI_CLI_STATUS_SUCCESS on success,
 *          #ADI_CLI_STATUS_COMM_ERROR on communication error.
 */
ADI_CLI_STATUS adi_cli_Init(ADI_CLI_CONFIG *pConfig);

/**
 * @brief Creates and initializes the CLI service instance.
 * @details Allocates memory and sets up internal structures for the CLI service.
 *          Only a single instance is supported.
 *
 * @param[out] phCli           Pointer to the CLI handle location.
 * @param[in]  pStateMemory    Pointer to persistent state memory (must be 32-bit aligned).
 * @param[in]  stateMemorySize Size of the state memory in bytes (minimum:
 * #ADI_CLI_STATE_MEM_NUM_BYTES).
 * @param[in]  pTempMemory     Pointer to temporary memory (must be 32-bit aligned).
 * @param[in]  tempMemorySize  Size of the temporary memory in bytes (minimum:
 * #ADI_CLI_TEMP_MEM_NUM_BYTES).
 *
 * @return  #ADI_CLI_STATUS_SUCCESS on success,
 *          #ADI_CLI_STATUS_NULL_PTR if a pointer is NULL,
 *          #ADI_CLI_STATUS_INSUFFICIENT_STATE_MEMORY if state memory is insufficient,
 *          #ADI_CLI_STATUS_INSUFFICIENT_TEMP_MEMORY if temp memory is insufficient.
 */
ADI_CLI_STATUS adi_cli_Create(ADI_CLI_HANDLE *phCli, void *pStateMemory, uint32_t stateMemorySize,
                              void *pTempMemory, uint32_t tempMemorySize);

/**
 * @brief Processes input, parses commands, and dispatches the appropriate handler.
 * @return Status code indicating the result of command execution.
 */
int32_t adi_cli_Interface(void);

/**
 * @brief Displays the CLI prompt, overwriting the current line.
 */
void adi_cli_DisplayPrompt(void);

/**
 * @brief Moves the cursor to the start of the line and prints a newline.
 */
void adi_cli_NewLine(void);

/**
 * @brief Retrieves a character from the communication interface.
 * @return  #ADI_CLI_STATUS_SUCCESS on success,
 *          #ADI_CLI_STATUS_COMM_ERROR on communication error.
 */
int32_t adi_cli_GetChar(void);

/**
 * @brief Sends a character through the communication interface.
 * @param[in] inputchar Character to transmit.
 * @return  #ADI_CLI_STATUS_SUCCESS on success,
 *          #ADI_CLI_STATUS_COMM_ERROR on communication error.
 */
ADI_CLI_STATUS adi_cli_PutChar(char inputchar);

/**
 * @brief Sends a string through the communication interface (non-blocking).
 * @param[in] pStr Pointer to the string.
 * @return  #ADI_CLI_STATUS_SUCCESS on success,
 *          #ADI_CLI_STATUS_BUFFER_FULL if the buffer is full,
 *          #ADI_CLI_STATUS_COMM_ERROR on communication error.
 */
ADI_CLI_STATUS adi_cli_PutStringNb(const char *pStr);

/**
 * @brief Sends a string through the communication interface (blocking).
 * @param[in] pStr Pointer to the string.
 * @return  #ADI_CLI_STATUS_SUCCESS on success,
 *          #ADI_CLI_STATUS_BUFFER_FULL if the buffer is full,
 *          #ADI_CLI_STATUS_COMM_ERROR on communication error.
 */
ADI_CLI_STATUS adi_cli_PutString(const char *pStr);

/**
 * @brief Sends a buffer through the communication interface.
 * @param[in] pData    Pointer to the buffer.
 * @param[in] numBytes Number of bytes to send.
 * @return  #ADI_CLI_STATUS_SUCCESS on success,
 *          #ADI_CLI_STATUS_COMM_ERROR on communication error.
 */
ADI_CLI_STATUS adi_cli_PutBuffer(uint8_t *pData, uint32_t numBytes);

/**
 * @brief Sends a string through the communication interface (non-blocking).
 * @param[in] pStr Pointer to the string.
 * @return  #ADI_CLI_STATUS_SUCCESS on success,
 *          #ADI_CLI_STATUS_COMM_ERROR on communication error.
 * @note This function is functionally similar to adi_cli_PutStringNb.
 */
ADI_CLI_STATUS adi_cli_PutStringNonBlocking(char *pStr);

/**
 * @brief CLI receive callback handler.
 * @details Should be called by the user when a receive event occurs.
 */
void adi_cli_RxCallback(void);

/**
 * @brief CLI transmit callback handler.
 * @details Should be called by the user when a transmit event occurs.
 */
void adi_cli_TxCallback(void);

/**
 * @brief Returns the number of bytes waiting to be processed from the communication interface.
 * @return Number of bytes pending in the receive buffer.
 */
int32_t adi_cli_GetNumCharsWaiting(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __ADI_CLI_H__ */
/**
 * @}
 */

/**
 * @}
 */
