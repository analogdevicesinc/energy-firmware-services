/******************************************************************************
 Copyright (c) 2024 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file        cli_history.h
 * @addtogroup ADI_CLI
 * @brief       This file contains routines declared for CLI command history
 * @{
 */

#ifndef __HISTORY_H__
#define __HISTORY_H__

/*============= I N C L U D E S =============*/

#include "app_cfg.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============= D E F I N E S =============*/
/** @brief module success status */
#define HISTORY_SUCCESS (0)
/** @brief module failure status */
#define HISTORY_FAILURE (1)

/** @brief maximum number of commands in history */
#define HISTORY_DEPTH (16)
/** @brief maximum size of an entry = (size(command) + size(parameters)) */
#define HISTORY_ENTRY_LENGTH (APP_CFG_CLI_MAX_CMD_LENGTH + APP_CFG_CLI_MAX_PARAM_LENGTH)

/** @brief details of a command and its parameters */
typedef char HISTORY_DATA[HISTORY_ENTRY_LENGTH];

/**
 * @brief  typedef to hold the history and pointers for managing
 */
typedef struct CLI_HISTORY
{
    /** Array of commands stored in history */
    HISTORY_DATA list[HISTORY_DEPTH];
    /** Index to next available (empty) command */
    uint32_t headIndex;
    /** Index to oldest existing (occupied) command */
    uint32_t tailIndex;
    /* If ( H == T ) then history file is empty */
    /* If ( H++ == T ) then history file is full */
    /** Current position (history line now at prompt)*/
    uint32_t curIndex;
} CLI_HISTORY;

/**
 * @brief  typedef to hold the history data and pointer for holding the temporary data
 */
typedef struct
{
    /** pointer to the history of commands issued. */
    CLI_HISTORY history;
    /** pointer to the trimmed command */
    char trimCommand[APP_CFG_CLI_MAX_CMD_LENGTH];
} CLI_HISTORY_DATA;

/*========================== P R O T O T Y P E S ==========================*/

/**
 * @brief Add a command line to the head of history list.
 * @param[in] pCommand	- pointer to a command
 * @return status 0 on Success
 */
int32_t HistoryAppend(const char *pCommand);

/**
 * @brief Initialize the history list.
 */
void HistoryInit(void);

/**
 * @brief Get a command line from the history file, moving up.
 * @return a command pointer
 */
char *HistoryScrollUp(void);

/**
 * @brief Get a command line from the history file, moving down.
 * @return a command pointer
 **/
char *HistroyScrollDown(void);

/**
 * @brief flush out history
 */
void HistoryFlush(void);

#ifdef __cplusplus
}
#endif

#endif /** __HISTORY_H__ */

/**
 * @}
 */
