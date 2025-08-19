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
 * The CLI Service provides a flexible interface for interacting with firmware via a command line.
 * It supports asynchronous transmit/receive, command parsing, and dispatching to user-defined
 * handlers.
 *
 * ## Typical API Usage
 * 1. **Create the CLI Instance**: Use `adi_cli_Create()` to allocate and initialize the CLI
 * service.
 * 2. **Configure the CLI**: Fill an `ADI_CLI_CONFIG` structure with transmit/receive function
 * pointers and context.
 * 3. **Initialize the CLI**: Call `adi_cli_Init()` with the configuration to start the CLI.
 * 4. **Process Commands**: Use `adi_cli_GetCmd()` to retrieve user input, then `adi_cli_Dispatch()`
 * to execute commands.
 * 5. **Handle Callbacks**: Use `adi_cli_RxCallback()` and `adi_cli_TxCallback()` in your
 * communication event handlers.
 *
 * @{
 */
#ifndef __ADI_CLI_H__
#define __ADI_CLI_H__

/*============= I N C L U D E S =============*/
#include "adi_circ_buf.h"
#include "adi_cli_status.h"
#include "cli_dispatch.h"
#include <stdarg.h>
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
    /** User-defined handle for callback context. */
    void *hUser;

} ADI_CLI_CONFIG;

/** Prints Info  Message as such without new line */
#define INFO_MSG_RAW(...) adi_cli_PrintMessage("RAW", __VA_ARGS__);
/** Prints Info  Message */
#define INFO_MSG(...) adi_cli_PrintMessage("", __VA_ARGS__);
/** Prints warn message */
#define WARN_MSG(...) adi_cli_PrintMessage("Warn : ", __VA_ARGS__);
/** Prints error message */
#define ERROR_MSG(...) adi_cli_PrintMessage("Error : ", __VA_ARGS__);
/** Prints debug message */
#ifdef ENABLE_DEBUG
#define DEBUG_MSG(...) adi_cli_PrintMessage("Debug : ", __VA_ARGS__);
#else
#define DEBUG_MSG(...)
#endif
/** Prints debug message as such without new line */
#ifdef ENABLE_DEBUG
#define DEBUG_MSG_RAW(...) adi_cli_PrintMessage("DBGRAW", __VA_ARGS__);
#else
#define DEBUG_MSG_RAW(...) NULL
#endif

/** @} */

/** @defgroup CLIAPI Service API
 * @brief API functions for creating, initializing, and interacting with the CLI Service.
 * All APIs return codes defined in #ADI_CLI_STATUS.
 * @{
 */

/**
 * @brief Creates and initializes the CLI service instance.
 * @details Allocates memory and sets up internal structures for the CLI service.
 *
 * @param[out] phCli           - Pointer to the CLI handle location.
 * @param[in]  pStateMemory    - Pointer to persistent state memory (must be 32-bit aligned).
 * @param[in]  stateMemorySize - Size of the state memory in bytes (minimum:
 *                                #ADI_CLI_STATE_MEM_NUM_BYTES).
 * @param[in]  pTempMemory     - Pointer to temporary memory (must be 32-bit aligned).
 * @param[in]  tempMemorySize  - Size of the temporary memory in bytes (minimum:
 *                                #ADI_CLI_TEMP_MEM_NUM_BYTES).
 *
 * @return  #ADI_CLI_STATUS_SUCCESS on success,
 *          #ADI_CLI_STATUS_NULL_PTR if a pointer is NULL,
 *          #ADI_CLI_STATUS_INSUFFICIENT_STATE_MEMORY if state memory is insufficient,
 *          #ADI_CLI_STATUS_INSUFFICIENT_TEMP_MEMORY if temp memory is insufficient.
 */
ADI_CLI_STATUS adi_cli_Create(ADI_CLI_HANDLE *phCli, void *pStateMemory, uint32_t stateMemorySize,
                              void *pTempMemory, uint32_t tempMemorySize);

/**
 * @brief Initializes the CLI Service and starts receiving data from the terminal.
 * @details This function must be called after creating the CLI instance with `adi_cli_Create()`
 * @param[in]  hCli    - Handle to the CLI instance.
 * @param[in]  pConfig - Pointer to the CLI configuration structure.
 * @return     #ADI_CLI_STATUS_SUCCESS on success,
 *             #ADI_CLI_STATUS_COMM_ERROR on communication error.
 */
ADI_CLI_STATUS adi_cli_Init(ADI_CLI_HANDLE hCli, ADI_CLI_CONFIG *pConfig);

/**
 * @brief Retrieves a command from the CLI input buffer.
 * @param[in] hCli Handle to the CLI instance.
 * @param[out] pCommand Pointer to the buffer where the command will be stored.
 * @return  #ADI_CLI_STATUS_SUCCESS on success,
 *          #ADI_CLI_STATUS_NULL_PTR if hCli or pCommand is NULL,
 *          #ADI_CLI_STATUS_INVALID_COMMAND on invalid command.
 */
ADI_CLI_STATUS adi_cli_GetCmd(ADI_CLI_HANDLE hCli, char *pCommand);

/**
 * @brief Dispatches a command to the appropriate function.
 * @param[in] hCli           - Handle to the CLI instance.
 * @param[in] pCommand       - Pointer to the command string.
 * @param[in] pDispatchTable - Pointer to the command dispatch table.
 * @param[in] numRecords     - Number of commands in the dispatch table.
 * @return  #ADI_CLI_STATUS_SUCCESS on success,
 *          #ADI_CLI_STATUS_NULL_PTR if hCli, pCommand or pDispatchTable is NULL,
 *          #ADI_CLI_STATUS_INVALID_COMMAND on invalid command.
 */
ADI_CLI_STATUS adi_cli_Dispatch(ADI_CLI_HANDLE hCli, char *pCommand, const Command *pDispatchTable,
                                int32_t numRecords);

/**
 * @brief CLI receive callback handler.
 * @details Should be called by the user when a receive event occurs.
 * @param[in]  hCli - Handle to the CLI instance.
 * @return     #ADI_CLI_STATUS_SUCCESS on success,
 *             #ADI_CLI_STATUS_NULL_PTR if hCli is NULL,
 *             #ADI_CLI_STATUS_COMM_ERROR on communication error.
 */
ADI_CLI_STATUS adi_cli_RxCallback(ADI_CLI_HANDLE hCli);

/**
 * @brief CLI transmit callback handler.
 * @details Should be called by the user when a transmit event occurs.
 * @param[in]  hCli - Handle to the CLI instance.
 * @return     #ADI_CLI_STATUS_SUCCESS on success,
 *             #ADI_CLI_STATUS_NULL_PTR if hCli is NULL.
 *
 */
ADI_CLI_STATUS adi_cli_TxCallback(ADI_CLI_HANDLE hCli);

/**
 * @brief Flushes the CLI message buffer, ensuring all pending messages are transmitted.
 * @param[in] hCli - Handle to the CLI instance.
 * @return    #ADI_CLI_STATUS_SUCCESS on success,
 *            #ADI_CLI_STATUS_NULL_PTR if hCli is NULL,
 *            #ADI_CLI_STATUS_TRANSMISSION_IN_PROGRESS if a transmission is in progress.
 *
 */
ADI_CLI_STATUS adi_cli_FlushMessages(ADI_CLI_HANDLE hCli);

/**
 * @brief Displays the prompt.
 * @param[in] hCli - Handle to the CLI instance.
 * @return    #ADI_CLI_STATUS_SUCCESS on success,
 *            #ADI_CLI_STATUS_NULL_PTR if hCli is NULL.
 */
ADI_CLI_STATUS adi_cli_DisplayPrompt(ADI_CLI_HANDLE hCli);

/**
 * @brief Moves the cursor to the start of the line and prints a newline.
 * @param[in] hCli - Handle to the CLI instance.
 * @return    #ADI_CLI_STATUS_SUCCESS on success,
 *            #ADI_CLI_STATUS_NULL_PTR if hCli is NULL.
 */
ADI_CLI_STATUS adi_cli_NewLine(ADI_CLI_HANDLE hCli);

/**
 * @details Stores the messages in the CLI buffer.
 * @param[in] pMsgType - Type of message to be printed
 * @param[in] pFormat  - Format specifier for the message
 * @return status      - SUCCESS - 0
 *                     - FAILURE - 1
 */
int32_t adi_cli_PrintMessage(char *pMsgType, char *pFormat, ...);

/**
 * @brief Gets the number of characters waiting in the CLI receive buffer.
 * @param[in]  hCli      - Handle to the CLI instance.
 * @param[out] pNumChars - Pointer to store the number of characters waiting.
 * @return  #ADI_CLI_STATUS_SUCCESS on success,
 *          #ADI_CLI_STATUS_NULL_PTR if hCli or pNumChars is NULL.
 *          #ADI_CLI_STATUS_BUFFER_FULL if the buffer is full.
 */
ADI_CLI_STATUS adi_cli_GetNumCharsWaiting(ADI_CLI_HANDLE hCli, int32_t *pNumChars);

/**
 * @brief Sets the handle for the terminal interface.
 * @details This handle is required for storing messages in the CLI buffer. Before using message
 * macros such as INFO_MSG, WARN_MSG, or ERROR_MSG, the user must call this function to set the CLI
 * handle for terminal output.
 * @param[in]  hCli - Handle to the CLI instance.
 * @return     #ADI_CLI_STATUS_SUCCESS on success,
 *             #ADI_CLI_STATUS_NULL_PTR if hCli is NULL.
 */
ADI_CLI_STATUS adi_cli_SetHandleTerminal(ADI_CLI_HANDLE hCli);

/**
 * @brief Retrieves a character from the circular buffer.
 * @param[in]  hCli - Handle to the CLI instance.
 * @param[out] pChar - Pointer to store the retrieved character.
 * @return     #ADI_CLI_STATUS_SUCCESS on success,
 *             #ADI_CLI_STATUS_BUFFER_FULL on buffer full.
 */
ADI_CLI_STATUS adi_cli_GetChar(ADI_CLI_HANDLE hCli, int32_t *pChar);

/**
 * @brief Puts the character into the CLI buffer.
 * @param[in]  hCli      - Handle to the CLI instance.
 * @param[in]  inputchar - Character to transmit.
 * @return     #ADI_CLI_STATUS_SUCCESS on success,
 *             #ADI_CLI_STATUS_NULL_PTR if hCli is NULL,
 */
ADI_CLI_STATUS adi_cli_PutChar(ADI_CLI_HANDLE hCli, char inputchar);

/**
 * @brief Puts the string into the CLI buffer.
 * @param[in]  hCli  - Handle to the CLI instance.
 * @param[in]  pStr  - Pointer to the string.
 * @return     #ADI_CLI_STATUS_SUCCESS on success,
 *             #ADI_CLI_STATUS_NULL_PTR if hCli is NULL,
 */
ADI_CLI_STATUS adi_cli_PutString(ADI_CLI_HANDLE hCli, const char *pStr);

/**
 * @brief Puts a buffer into the CLI buffer.
 * @param[in]  hCli     - Handle to the CLI instance.
 * @param[in]  pData    - Pointer to the buffer.
 * @param[in]  numBytes - Number of bytes to send.
 * @return     #ADI_CLI_STATUS_SUCCESS on success,
 *             #ADI_CLI_STATUS_NULL_PTR if hCli is NULL,
 */
ADI_CLI_STATUS adi_cli_PutBuffer(ADI_CLI_HANDLE hCli, uint8_t *pData, uint32_t numBytes);

/**
 * @brief Gets the number of bytes available in the CLI message buffer.
 * @param[in]   hCli       - Handle to the CLI instance.
 * @param[out]  pFreeSpace - Pointer to store the number of free bytes available.
 * @return     #ADI_CLI_STATUS_SUCCESS on success,
 *            #ADI_CLI_STATUS_NULL_PTR if hCli or pFreeSpace is NULL.
 */
ADI_CLI_STATUS adi_cli_GetFreeMessageSpace(ADI_CLI_HANDLE hCli, uint32_t *pFreeSpace);

/**
 * @brief Gets the handle for dispatching internal commands.
 * @details This function returns a pointer to the CLI interface data structure used for dispatching
 * internal (non-user) commands. This handle should not be used for user commands.
 * @param[in]  hCli - Handle to the CLI instance.
 * @return     Pointer to the CLI interface data structure, or NULL if hCli is NULL.
 */
void *GetHandleForDispatchCommands(ADI_CLI_HANDLE hCli);

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
