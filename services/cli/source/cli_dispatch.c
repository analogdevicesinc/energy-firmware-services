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
#include "adi_cli_private.h"
#include "app_cfg.h"
#include "cli_interface.h"
#include "cli_utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*========================== D A T A T Y P E S ==========================*/

/*========================== C O D E ==========================*/

int32_t DispatchGetNumRecords()
{
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_DISPATCH_DATA *pInfo = &pCliInfo->cliDispatchData;
    return pInfo->numDispatchRecords;
}

const Command *DispatchGetCommandDetails(char *pCommandToken)
{
    int i;
    int numRecords = -1;

    const Command *pDispatchRecord = NULL;
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_DISPATCH_DATA *pInfo = &pCliInfo->cliDispatchData;

    ConvertToLower(pCommandToken, APP_CFG_CLI_MAX_CMD_LENGTH, pInfo->pCmdLower);
    /* Number of records in the dispatch table */
    numRecords = pInfo->numDispatchRecords;

    /* For each record in the dispatch table... */
    for (i = 0; i < numRecords; i++)
    {
        if (strlen(pInfo->pCmdLower) != strlen(pInfo->pDispatchTable[i].pName))
        {
            /* This record cannot be a match; skip */
            continue;
        }
        ConvertToLower(pInfo->pDispatchTable[i].pName, APP_CFG_CLI_MAX_CMD_LENGTH,
                       pInfo->pDispatchCmd);
        if (!strncmp(pInfo->pCmdLower, pInfo->pDispatchCmd, strlen(pInfo->pCmdLower)))
        {
            pDispatchRecord = &pInfo->pDispatchTable[i];
            break; // Exit loop once a match is found
        }
    }

    return pDispatchRecord;
}

const Command *DispatchGetRecords()
{
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_DISPATCH_DATA *pInfo = &pCliInfo->cliDispatchData;
    return pInfo->pDispatchTable;
}

const Command *DispatchMatchCommand(const char *pCommandToken)
{

    const Command *pCommandDetails = NULL;
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_DISPATCH_DATA *pInfo = &pCliInfo->cliDispatchData;
    /** convert command token to lower case (assuming everything in dispatch is
     also lower case*/
    ConvertToLower(pCommandToken, APP_CFG_CLI_MAX_PARAM_LENGTH, pInfo->pCmdLower);
    /* Find match in dispatch */
    pCommandDetails = DispatchGetCommandDetails((char *)(pInfo->pCmdLower));

    return pCommandDetails;
}

void DispatchInit(const Command *pRecords, int32_t numRecords)
{
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_DISPATCH_DATA *pInfo = &pCliInfo->cliDispatchData;
    pInfo->pDispatchTable = pRecords;
    pInfo->numDispatchRecords = numRecords;
}

/**
 * @}
 */
