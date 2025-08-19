/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file        cli_private.h
 * @addtogroup  ADI_CLI
 * @brief       This file contains routines declared for the command line interface.
 * @{
 */

#ifndef __CLI_PRIVATE_H__
#define __CLI_PRIVATE_H__
/*============= I N C L U D E S =============*/

#include "adi_circ_buf.h"
#include "app_cfg.h"
#include "cli_dispatch.h"
#include "cli_history.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============= D E F I N E S =============*/
/** @brief rx buffer size */
#define RX_BUFFER_SIZE 256
/** @brief controlArrays index for Alert */
#define CLI_CTRL_ALERT (0)
/** @brief controlArrays index for bold */
#define CLI_CTRL_BOLD (1)
/** @brief controlArrays index for cr */
#define CLI_CTRL_CR (2)
/** @brief controlArrays index for cls */
#define CLI_CTRL_CLS (3)
/** @brief controlArrays index for kill*/
#define CLI_CTRL_KILL (4)
/** @brief controlArrays index for prev*/
#define CLI_CTRL_PREV (5)
/** @brief controlArrays index for next*/
#define CLI_CTRL_NEXT (6)
/** @brief controlArrays index for newline*/
#define CLI_CTRL_NEWLINE (7)
/** @brief controlArrays index for normal*/
#define CLI_CTRL_NORMAL (8)
/** @brief controlArrays index for red */
#define CLI_CTRL_RED (9)
/** @brief controlArrays index for restore */
#define CLI_CTRL_RESTORE (10)
/** @brief controlArrays index for save*/
#define CLI_CTRL_SAVE (11)
/** Number of calibration choices */
#define CAL_NUM_CHOICES 11
/** Number of gain channels */
#define NUM_CHANNELS 2
/** Number of error checks */
#define NUM_ERROR_CHECKS 2
/** Number of compensation choices */
#define COMP_NUM_CHOICES 8
/** Number of echo modes*/
#define NUM_ECHO_MODES 2
/** Success */
#define CMD_SUCCESS 0
/** Faiure */
#define CMD_FAILURE 1

/*============= D A T A  T Y P E S =============*/

/**
 * @brief typedef to hold the current line buffer and pointers for editing
 */

typedef struct editlineData
{
    /** Pointer to prompt string */
    const char *pPrompt;
    /** Pointer to command string buffer */
    char *pBuffer;
    /** Index of cursor */
    uint32_t indexCur;
    /** Index of end character */
    uint32_t indexEnd;
    /** Number of characters buffered for printing */
    uint32_t numCharsToPrint;
} EditLine;

/**
 * @brief BufferInfo structure to hold the buffer information
 * This structure is used to hold the buffer information for CLI interface.
 */
typedef struct
{
    /** Pointer to the current data buffer filling */
    uint8_t *pBufferToWrite;
    /** Number of bytes stored in the buffer */
    uint32_t bytesStored;
} BufferInfo;

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
 * Holds parameter value
 */
typedef struct
{
    /** pointer to store the incoming input characters*/
    char cliString[APP_CFG_CLI_MAX_CMD_LENGTH];
    /** pointer to store the command after trimming the white spaces*/
    char *pCliTrimString;
    /** pointer to send the echoed characters*/
    char *pCliPrintString;
    /** Echo back the charecters received */
    volatile bool echo;
    /** Flag to indicate whether prompt should not be printed until the next keypress */
    bool deferPrompt;
    /** Flag to indicate whether prompt should be printed immediately */
    bool displayPrompt;
    /** Flag to indicate whether the user is currently entering a command */
    bool userIsTyping;
    /** Flag to indicate whether the control characters to be printed or not */
    bool displayCtrlChars;
    /** Holds current line buffers and pointers */
    EditLine editLine;
    /** Buffer info structure to hold current data buffer filling */
    BufferInfo bufferInfo;
    /** CLI data */
    ADI_CLI_RX_DATA cliData;
    /** history data */
    CLI_HISTORY_DATA cliHistData;
    /** pointer to the dispatch data */
    CLI_DISPATCH_DATA cliDispatchData;
} CLI_PRIVATE;

/*============= F U N C T I O N  P R O T O T Y P E S =============*/

/**
 *  @brief Command Line Interface.
 *  This function will retrieve one line from the TTY I/O stream.
 *  @param[in] pInfo - pointer to CLI interface structure.
 *  @param[in] pString - pointer to the string buffer to store the command.
 *  @returns 0 on Success, 1 on command not found.
 *
 */
int32_t CliGetCmd(CLI_PRIVATE *pInfo, char *pString);

/**
 * @brief Defer printing of prompt until next keypress.
 * @param[in] pInfo - pointer to CLI interface structure.
 * @param[in] enable - Enables/disables prompt
 */
void CliDeferPrompt(CLI_PRIVATE *pInfo, bool enable);

/**
 * @brief Command Line Interface terminal initialization.
 * @param[in] pInfo - pointer to CLI interface structure.
 * @return status
 */
int32_t CliInit(CLI_PRIVATE *pInfo);

/**
 *  @brief  Read a command line input from CLI
 *  Prompt for command/password and read a input from user.
 *  @param[in] pInfo - pointer to CLI interface structure.
 *  @param[in] pBuffer - input command buffer.
 *  @return LCR status code.
 */
int32_t CliReadCommandLineInput(CLI_PRIVATE *pInfo, char *pBuffer);

/**
 * @brief Puts the character into the internal buffer.
 * @param[in] pInfo        - pointer to CLI interface structure.
 * @param[in] inputChar    - input character.
 */
int32_t CliPutChar(CLI_PRIVATE *pInfo, const char inputChar);

/**
 * @brief Puts the string into the internal buffer.
 * @param[in] pInfo        - pointer to CLI interface structure.
 * @param[in] pString      - input string.
 */
int32_t CliPutString(CLI_PRIVATE *pInfo, const char *pString);

/**
 * @brief Puts the buffer into the internal buffer.
 * @param[in] pInfo        - pointer to CLI interface structure.
 * @param[in] pBuffer      - input buffer.
 * @param[in] length       - length of the input buffer.
 * @return 0 on success, 1 on failure.
 */
int32_t CliPutBuffer(CLI_PRIVATE *pInfo, const char *pBuffer, int32_t length);

/**
 * @brief Get character from the circular buffer.
 * @param[in] pInfo - pointer to CLI interface structure.
 * @return character read from the circular buffer.
 */
int32_t CliGetChar(CLI_PRIVATE *pInfo);

/**
 * @brief Parses the input command string, searches the dispatch table for a match, and calls the
 * corresponding handler function if found.
 * @param[in] pInfo        - pointer to CLI interface structure.
 * @param[in] pCommand     - input command string.
 * @param[in] pDispatchTable - pointer to the dispatch table containing command definitions.
 * @param[in] numRecords   - number of records in the dispatch table.
 * @return  0 - Success, 1 - Failed.
 */
int32_t CliParse(CLI_PRIVATE *pInfo, char *pCommand, const Command *pDispatchTable,
                 int32_t numRecords);

/**
 * @brief Function for CLI "help" command.
 * @param[in] pInfo       - pointer to CLI interface structure. This is a void* to keep the
 * CLI_PRIVATE handle private. Users should not access this structure directly. To invoke internal
 * commands, use the GetHandleForDispatchCommands function to obtain a valid handle.
 * @param[in] pDispatchRecord - pointer to the command record in the dispatch table.
 * @param[in] pArgs       - pointer to command arguments storage.
 * @param[in] numRecords  - number of records in the dispatch table.
 * @return status 0 on Success
 */
int32_t CliHelp(void *pInfo, const Command *pDispatchRecord, Args *pArgs, int32_t numRecords);

/**
 * @brief Function for CLI "expertHelp" command to display all hidden commands.
 * @param[in] pInfo       - pointer to CLI interface structure. This is a void* to keep the
 * CLI_PRIVATE handle private. Users should not access this structure directly. To invoke internal
 * commands, use the GetHandleForDispatchCommands function to obtain a valid handle.
 * @param[in] pDispatchRecord - pointer to the command record in the dispatch table.
 * @param[in] pArgs       - pointer to command arguments storage.
 * @param[in] numRecords  - number of records in the dispatch table.
 * @return status 0 on Success
 */
int32_t CliExpertHelp(void *pInfo, const Command *pDispatchRecord, Args *pArgs, int32_t numRecords);

/**
 * @brief Function for CLI manual mode command. If off disables
 *  echo and some control characters.
 * @param[in] pInfo       - pointer to CLI interface structure. This is a void* to keep the
 * CLI_PRIVATE handle private. Users should not access this structure directly. To invoke internal
 * commands, use the GetHandleForDispatchCommands function to obtain a valid handle.
 * @param[in] pDispatchRecord - pointer to the command record in the dispatch table.
 * @param[in] pArgs       - pointer to command arguments storage.
 * @param[in] numRecords  - number of records in the dispatch table.
 * @return status 0 on Success
 */
int32_t CliCmdEcho(void *pInfo, const Command *pDispatchRecord, Args *pArgs, int32_t numRecords);

#ifdef ENABLE_X86_BUILD
/**
 * @brief Function for CLI "exit" command to exit the CLI.
 * @param[in] pInfo       - pointer to CLI interface structure. This is a void* to keep the
 * CLI_PRIVATE handle private. Users should not access this structure directly. To invoke internal
 * commands, use the GetHandleForDispatchCommands function to obtain a valid handle.
 * @param[in] pDispatchRecord - pointer to the command record in the dispatch table.
 * @param[in] pArgs       - pointer to command arguments storage.
 * @param[in] numRecords  - number of records in the dispatch table.
 * @return status 0 on Success
 */
int32_t CliExit(void *pInfo, const Command *pDispatchRecord, Args *pArgs, int32_t numRecords);
#endif
/**
 * @brief Check if a user is typing a command
 * @param[in] pInfo - pointer to CLI interface structure.
 * @return false if the last key pressed was a carriage return (or no key has been pressed yet)
 *         true if any other key was last pressed
 */
bool CliUserIsTyping(CLI_PRIVATE *pInfo);

/**
 * @brief Overwrites the edit line and displays the prompt
 * @param[in] pInfo - pointer to CLI interface structure.
 */
void CliOverwriteLineWithPrompt(CLI_PRIVATE *pInfo);

/**
 * @brief Clears the line, brings the cursor to the start and prints a newline character.
 * @param[in] pInfo - pointer to CLI interface structure.
 */
void CliNewLine(CLI_PRIVATE *pInfo);

/**
 * @brief Clears the line and brings the cursor to the start
 * @param[in] pInfo - pointer to CLI interface structure.
 */
void CliClearLine(CLI_PRIVATE *pInfo);

/**
 * @brief Gets the number of characters available in the CLI buffer.
 * @param[in] pInfo - pointer to CLI interface structure.
 */
int32_t CliGetNumCharAvailable(CLI_PRIVATE *pInfo);

#ifdef __cplusplus
}
#endif

#endif /* __CLI_PRIVATE_H__ */
/**
 * @}
 */
