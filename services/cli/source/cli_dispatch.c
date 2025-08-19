/******************************************************************************
 Copyright (c) 2024 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file     cli_dispatch.c
 * @brief    This file contains API definitions for dispatch table
 * @{
 */

/*==========================  I N C L U D E S ==========================*/
#include "cli_dispatch.h"
#include "adi_cli_utility.h"
#include "app_cfg.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*========================== D A T A T Y P E S ==========================*/

/*========================== C O D E ==========================*/

const Command *DispatchGetCommandDetails(CLI_DISPATCH_DATA *pInfo, char *pCommandToken,
                                         const Command *pDispatchTable, int32_t numRecords)
{
    int32_t i;
    const Command *pDispatchRecord = NULL;

    ConvertToLower(pCommandToken, APP_CFG_CLI_MAX_CMD_LENGTH, pInfo->pCmdLower);

    /* For each record in the dispatch table... */
    for (i = 0; i < numRecords; i++)
    {
        if (strlen(pInfo->pCmdLower) != strlen(pDispatchTable[i].pName))
        {
            /* This record cannot be a match; skip */
            continue;
        }
        ConvertToLower(pDispatchTable[i].pName, APP_CFG_CLI_MAX_CMD_LENGTH, pInfo->pDispatchCmd);
        if (!strncmp(pInfo->pCmdLower, pInfo->pDispatchCmd, strlen(pInfo->pCmdLower)))
        {
            pDispatchRecord = &pDispatchTable[i];
            break; // Exit loop once a match is found
        }
    }

    return pDispatchRecord;
}

/**
 * @}
 */
