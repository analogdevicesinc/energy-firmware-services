/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file        cli_interface.h
 * @addtogroup ADI_CLI
 * @brief       This file contains routines declared  for command line interface
 * @{
 */
#ifndef __CLI_INTERFACE_H__
#define __CLI_INTERFACE_H__
/*============= I N C L U D E S =============*/

#include "app_cfg.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============= D E F I N E S =============*/
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

/**
 * Holds parameter value
 */
typedef union
{
    char *pS; /**< holds string value */
    char c;   /**< holds char type value */
    double f; /**< holds double type value */
    long d;   /**< holds long type value */
} PARAM;

/**
 *  Holds a full set of parsed parameter values for passing to cli_commands
 */
typedef struct
{
    int c;                                /**< arguments count value */
    PARAM v[APP_CFG_CLI_MAX_PARAM_COUNT]; /**< buffer to hold parameter values */
} Args;

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
    /** Holds a full set of parsed parameter values for passing to commands */
    Args sArgs;
    /** Holds current line buffers and pointers */
    EditLine editLine;
} CLI_INTERFACE;

/*============= F U N C T I O N  P R O T O T Y P E S =============*/

/**
 *  @brief Command Line Interface.
 *  This function will retrieve one line from the TTY I/O stream, parse that
 *  line for a command and subsequent parameters, and dispatch the respective
 *  command function (with parameters).
 *  @return status code
 *
 */
int32_t CliInterface(void);

/**
 *  @brief Defer printing of prompt until next keypress.
 *  @param[in] enable - Enables/disables prompt
 */
void CliDeferPrompt(bool enable);

/**
 *  @brief Command Line Interface terminal initialization.
 *  @return status
 */
int32_t CliInit(void);

/**
 *  @brief  Read a command line input from CLI
 *  Prompt for command/password and read a input from user.
 *  @param[in] pBuffer - input command buffer.
 *  @return LCR status code.
 */
int32_t CliReadCommandLineInput(char *pBuffer);

/**
 * @brief Function for CLI "help" command.
 * @param[in] pArgs       - pointer to command arguments storage
 * @return status 0 on Success
 */
int32_t CliHelp(Args *pArgs);

/**
 * @brief Function for CLI "expertHelp" command to display all hidden commands.
 * @param[in] pArgs       - pointer to command arguments storage
 * @return status 0 on Success
 */
int32_t CliExpertHelp(Args *pArgs);

/**
 * @brief Function for CLI manual mode command. If off disables
 *  echo and some control charecters
 * @param[in] pArgs      - pointer to command arguments storage
 * @return status 0 on Success
 */
int32_t CliCmdManual(Args *pArgs);

#ifdef ENABLE_X86_BUILD
int32_t CliExit(Args *pArgs);
#endif
/**
 * @brief Check if a user is typing a command
 * @return false if the last key pressed was a carriage return (or no key has been pressed yet)
 *         true if any other key was last pressed
 */
bool CliUserIsTyping(void);

/**
 * @brief Overwrites the edit line and displays the prompt
 */
void CliOverwriteLineWithPrompt(void);

/**
 * @brief Clears the line, brings the cursor to the start and prints a newline character
 */
void CliNewLine(void);

/**
 * @brief Clears the line and brings the cursor to the start
 */
void CliClearLine(void);

#ifdef __cplusplus
}
#endif

#endif /* __CLI_INTERFACE_H__ */
/**
 * @}
 */
