/******************************************************************************
 Copyright (c) 2024 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file     cli_history.c
 * @brief    This file contains API definitions related to command history
 * @{
 */

/*==========================  I N C L U D E S   ==========================*/
#include "cli_history.h"
#include "adi_cli_utility.h"
#include "board_cfg.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*========================== P R O T O T Y P E S ==========================*/

/**
 * @brief Check for duplicate command at head of history file.
 * @param[in] pCommand	- pointer to command
 * @return status - true or false
 */
static bool HistoryCheckForDuplicate(CLI_HISTORY_DATA *pHistory, const char *pCommand);

/*==========================  D E F I N I T I O N S ==========================*/

static bool HistoryCheckForDuplicate(CLI_HISTORY_DATA *pInfo, const char *pCommand)
{
    uint32_t recentIndex;
    bool status = false;
    CLI_HISTORY *pHistory = &pInfo->history;

    /* If history file is empty then not a duplicate */
    if (pHistory->headIndex != pHistory->tailIndex)
    {
        /* Get index of most recent command line history */
        recentIndex = pHistory->headIndex - 1;
        recentIndex = (recentIndex < HISTORY_DEPTH ? recentIndex : HISTORY_DEPTH - 1);

        /* Compare the pCommand with history command and return true if match */
        if (strncmp(pCommand, pHistory->list[recentIndex], APP_CFG_CLI_MAX_CMD_LENGTH) == 0)
        {
            status = true;
        }
    }
    /* pCommand is not same as immediate command history */
    return status;
}

int32_t HistoryAppend(CLI_HISTORY_DATA *pInfo, const char *pCommand)
{
    size_t commandSize = 0;
    /** Controls pointer adjustment */
    int32_t control = 0;
    /** API return status */
    int32_t status = HISTORY_SUCCESS;
    CLI_HISTORY *pHistory = &pInfo->history;

    /* Trim leading and trailing whitespace, return if empty string */
    commandSize = TrimWhiteSpaces(pCommand, &pInfo->trimCommand[0]);

    if (commandSize != 0)
    {
        /* Check if candidate is duplicate with most recent command history */
        if (HistoryCheckForDuplicate(pInfo, &pInfo->trimCommand[0]) == true)
        {
            /* Push pointer p to h because this candidate may have been
             * an up arrow (which lifted curIndex)
             */
            pHistory->curIndex = pHistory->headIndex;
            control = 1;
        }
        else
        {
            /* Copy the command to head and update counter for that line */
            StrCopy(&pInfo->trimCommand[0], HISTORY_ENTRY_LENGTH,
                    pHistory->list[pHistory->headIndex]);
        }
        if (!control)
        {
            /* Increment head pointer and see if tail should follow */
            pHistory->headIndex++;
            pHistory->headIndex = (pHistory->headIndex < HISTORY_DEPTH ? pHistory->headIndex : 0);
            pHistory->curIndex = pHistory->headIndex;
            if (pHistory->headIndex == pHistory->tailIndex)
            {
                pHistory->tailIndex++;
                pHistory->tailIndex =
                    (pHistory->tailIndex < HISTORY_DEPTH ? pHistory->tailIndex : 0);
            }
        }
    }
    return status;
}

void HistoryInit(CLI_HISTORY_DATA *pInfo)
{
    int32_t i = 0;
    int32_t j = 0;
    CLI_HISTORY *pHistory = &pInfo->history;
    pHistory->headIndex = 0;
    pHistory->tailIndex = 0;

    /* Iteratively clearing all the 16 command elements in the list */
    for (i = 0; i < HISTORY_DEPTH; i++)
    {
        for (j = 0; j < APP_CFG_CLI_MAX_PARAM_LENGTH; j++)
        {
            pHistory->list[i][j] = '\0';
        }
    }
}

char *HistoryScrollUp(CLI_HISTORY_DATA *pInfo)
{
    char *pCommand = NULL;
    CLI_HISTORY *pHistory = &pInfo->history;
    /* Check for an earlier command line in the history file,
     * if not, return null
     */
    if (pHistory->curIndex != pHistory->tailIndex)
    {
        /* Move pointer back one position, modulo */
        pHistory->curIndex--;
        pHistory->curIndex =
            (pHistory->curIndex >= HISTORY_DEPTH ? HISTORY_DEPTH - 1 : pHistory->curIndex);

        /* Return pointer to this command string */
        pCommand = (char *)&pHistory->list[pHistory->curIndex];
    }

    return pCommand;
}

char *HistoryScrollDown(CLI_HISTORY_DATA *pInfo)
{
    char *pCommand = NULL;
    CLI_HISTORY *pHistory = &pInfo->history;
    /* Commands available/non-empty history list */
    if (pHistory->curIndex != pHistory->headIndex)
    {

        /* Increment curIndex and if it points to head,
         * then no subsequent commands, return null,
         * otherwise return current index command
         */
        pHistory->curIndex++;
        pHistory->curIndex = (pHistory->curIndex < HISTORY_DEPTH ? pHistory->curIndex : 0);
        if (pHistory->curIndex != pHistory->headIndex)
        {
            /* pointer is now on valid history line, return point to it */
            pCommand = (char *)&pHistory->list[pHistory->curIndex];
        }
    }
    return pCommand;
}

void HistoryFlush(CLI_HISTORY_DATA *pInfo)
{
    HistoryInit(pInfo);
}
/**
 * @}
 */
