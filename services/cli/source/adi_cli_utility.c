/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file     adi_cli_utility.c
 * @brief    This file contains utilities for command line interface
 * @{
 */
/*=============  I N C L U D E S   =============*/
#include "adi_cli_utility.h"
#include "app_cfg.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*=============  C O D E  =============*/

static char matchParams0[APP_CFG_CLI_MAX_CMD_LENGTH];
static char matchParams1[APP_CFG_CLI_MAX_CMD_LENGTH];

void ConvertToLower(const char *pSrc, int32_t maxSize, char *pDst)
{
    int32_t i;
    for (i = 0; (pSrc[i] && (i < maxSize)); i++)
    {
        pDst[i] = (char)tolower(pSrc[i]);
    }

    pDst[i] = '\0';
}

void ConvertToUpper(const char *pSrc, int32_t maxSize, char *pDst)
{
    int32_t i;
    for (i = 0; (pSrc[i] && (i < maxSize)); i++)
    {
        pDst[i] = (char)toupper(pSrc[i]);
    }

    pDst[i] = '\0';
}

int32_t GetChoice(char *pChoices[], char *pArg, int32_t numChoices, char *pParam)
{
    int32_t i;
    int32_t choice = -1;
    bool status = false;

    if ((pArg != NULL) && (pParam != NULL))
    {
        /* Got at leasT one parameter, copy first parameter to a variable and
         * change to lower case
         */
        ConvertToLower(pArg, APP_CFG_CLI_MAX_PARAM_LENGTH, pParam);
        for (i = 0; i < numChoices; i++)
        {
            /* Check this parameter against token */
            status = CheckParams(pParam, APP_CFG_CLI_MAX_PARAM_LENGTH, pChoices[i]);
            if (status == true)
            {
                choice = i;
                break;
            }
        }
    }

    return choice;
}

/** FIXME: Avoiding strncpy to workaround flash issue */
void StrCopy(const char *pSrc, int32_t maxLength, char *pDst)
{
    int32_t i;
    for (i = 0; i < maxLength; i++)
    {
        pDst[i] = pSrc[i];
        if (pDst[i] == 0)
        {
            break;
        }
    }
}

bool CheckParams(char *pParam, int32_t maxLength, char *pString)
{
    bool status = false;
    ConvertToLower(pParam, APP_CFG_CLI_MAX_PARAM_LENGTH, matchParams0);
    ConvertToLower(pString, APP_CFG_CLI_MAX_PARAM_LENGTH, matchParams1);

    if (strncmp(matchParams1, matchParams0, (size_t)maxLength) == 0)
    {
        status = true;
    }

    return status;
}

size_t TrimWhiteSpaces(const char *pCommand, char *pTrimCommand)
{
    size_t commandLength;
    size_t trimLength = 0;
    const char *pEnd;

    /* Return zero if pCommand was all space/empty string */
    if (pCommand == NULL)
    {
        pTrimCommand[trimLength] = '\0';
    }
    else
    {
        /* Trim leading whitespace */
        while (*pCommand && isspace((unsigned char)*pCommand))
        {
            pCommand++;
        }

        /* Trim trailing whitespace */
        commandLength = StrnLen(pCommand, APP_CFG_CLI_MAX_CMD_LENGTH);
        pEnd = pCommand + commandLength - 1;
        while ((pCommand < pEnd) && isspace((unsigned char)*pEnd))
        {
            pEnd--;
        }
        pEnd++;

        /* Copy what's left of the string and return the length */
        trimLength = (size_t)(pEnd - pCommand);
        StrCopy(pCommand, (int32_t)trimLength, pTrimCommand);
        pTrimCommand[trimLength] = '\0';
    }
    return trimLength;
}

size_t StrnLen(const char *pStr, size_t maxLen)
{
    size_t len = 0;
    while ((len <= maxLen) && (*pStr))
    {
        pStr++;
        len++;
    }
    return len;
}

/**
 * @}
 */
