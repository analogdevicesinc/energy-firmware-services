/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file     adi_cli.c
 * @brief    This file contains API definitions for command line interface
 * @{
 */
/*==========================  I N C L U D E S   ==========================*/
#include "adi_cli.h"
#include "adi_cli_memory.h"
#include "adi_cli_private.h"
#include "adi_cli_status.h"
#include "adi_cli_utility.h"
#include "app_cfg.h"
#include "cli_dispatch.h"
#include "cli_history.h"
#include "cli_private.h"
#include "string.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief  Intialises the circular buffer used for receiving data.
 * @param[in] pInfo - pointer to CLI interface structure.
 */
static void InitCircBuff(CLI_PRIVATE *pInfo);

/**
 * @brief  Allocates the temporary memory for CLI.
 * @param[in] pInfo - pointer to CLI interface structure.
 */
static void AllocateTempMem(ADI_CLI_INFO *pInfo);

/**
 * @brief  Initializes the state data for CLI.
 * @param[in] pInfo - pointer to CLI interface structure.
 */
static void IntialiseStateData(ADI_CLI_INFO *pInfo);

/**
 * @brief  Copies the message to the CLI buffer.
 * @param[in] pMessage - pointer to the message string.
 * @return 0 on success, 1 on failure.
 */
static int32_t CopyToBuffer(char *pMessage);

/**
 * @brief  Handle for the terminal interface.
 * This is used for storing messages in the CLI buffer.
 * Before storing any messages, the user must call adi_cli_SetHandleTerminal to set this handle.
 */
static ADI_CLI_HANDLE hTerminalHandle = NULL;

ADI_CLI_STATUS adi_cli_Create(ADI_CLI_HANDLE *phCli, void *pStateMemory, uint32_t stateMemorySize,
                              void *pTempMemory, uint32_t tempMemorySize)
{

    ADI_CLI_STATUS status = ADI_CLI_STATUS_SUCCESS;
    ADI_CLI_INFO *pInfo = NULL;
    uint32_t reqSize = sizeof(ADI_CLI_INFO);
    uint32_t reqTempSize = ADI_CLI_TEMP_MEM_NUM_BYTES;

    /* Check the given pointers before we set their contents */
    if ((phCli == (void *)NULL) || (pStateMemory == (void *)NULL))
    {
        return ADI_CLI_STATUS_NULL_PTR;
    }
    if (status == ADI_CLI_STATUS_SUCCESS)
    {
        /* Set handle to NULL in case of error */
        *phCli = (ADI_CLI_HANDLE)NULL;
        if (stateMemorySize < reqSize)
        {
            return ADI_CLI_STATUS_INSUFFICIENT_STATE_MEMORY;
        }
        else
        {
            pInfo = (ADI_CLI_INFO *)pStateMemory;
            *phCli = (ADI_CLI_HANDLE *)pInfo;
            memset(pInfo, 0, sizeof(ADI_CLI_INFO));
            InitCircBuff(&pInfo->cliIfData);
            IntialiseStateData(pInfo);
            if (tempMemorySize < reqTempSize)
            {
                return ADI_CLI_STATUS_INSUFFICIENT_TEMP_MEMORY;
            }
            else
            {
                pInfo->pTempMemory = pTempMemory;
                pInfo->tempMemSize = tempMemorySize;
                AllocateTempMem(pInfo);
            }
        }
    }
    return status;
}

ADI_CLI_STATUS adi_cli_Init(ADI_CLI_HANDLE hCli, ADI_CLI_CONFIG *pConfig)
{
    ADI_CLI_STATUS cliStatus = 0;
    int32_t status = 0;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    if (pConfig == NULL || hCli == NULL)
    {
        return ADI_CLI_STATUS_NULL_PTR;
    }
    else
    {
        pConfig->hUser = pInfo;
        pInfo->config = *pConfig;
        status = pInfo->config.pfReceiveAsync(pInfo->config.hUser,
                                              (char *)&pInfo->cliIfData.cliData.rxByte, 1);
        if (status != 0)
        {
            cliStatus = ADI_CLI_STATUS_COMM_ERROR;
        }
        CliInit(&pInfo->cliIfData);
    }

    return cliStatus;
}

ADI_CLI_STATUS adi_cli_GetCmd(ADI_CLI_HANDLE hCli, char *pCommand)
{
    int32_t status = ADI_CLI_STATUS_SUCCESS;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    char *pTempCommand;
    if (hCli == NULL || pCommand == NULL)
    {
        return ADI_CLI_STATUS_NULL_PTR;
    }
    else
    {
        pTempCommand = &pInfo->cliIfData.cliString[0];
        status = CliGetCmd(&pInfo->cliIfData, pTempCommand);
        if (status != 0)
        {
            status = ADI_CLI_STATUS_INVALID_COMMAND;
        }
        else
        {
            strcpy(pCommand, pTempCommand);
        }
    }
    return status;
}

ADI_CLI_STATUS adi_cli_Dispatch(ADI_CLI_HANDLE hCli, char *pCommand, const Command *pDispatchTable,
                                int32_t numRecords)
{
    int32_t status = 0;
    ADI_CLI_STATUS cliStatus = ADI_CLI_STATUS_SUCCESS;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    char *pTrimCommand;
    CLI_PRIVATE *pInterfaceInfo;
    if (hCli == NULL)
    {
        cliStatus = ADI_CLI_STATUS_NULL_PTR;
    }
    else
    {
        pTrimCommand = pInfo->cliIfData.pCliTrimString;
        pInterfaceInfo = &pInfo->cliIfData;
        TrimWhiteSpaces(pCommand, pTrimCommand);
        status = CliParse(pInterfaceInfo, pTrimCommand, pDispatchTable, numRecords);
        if (status != 0)
        {
            cliStatus = ADI_CLI_STATUS_INVALID_COMMAND;
        }
    }
    return cliStatus;
}

ADI_CLI_STATUS adi_cli_RxCallback(ADI_CLI_HANDLE hCli)
{
    int32_t status = 0;
    ADI_CLI_STATUS cliStatus = ADI_CLI_STATUS_SUCCESS;
    ADI_CLI_RX_DATA *pCliData;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    if (hCli == NULL)
    {
        cliStatus = ADI_CLI_STATUS_NULL_PTR;
    }
    else
    {
        pCliData = &pInfo->cliIfData.cliData;
        ADICircBufWrite(pCliData->pRxBuff, &pCliData->rxByte, 1);
        status = pInfo->config.pfReceiveAsync(pInfo->config.hUser, (char *)&pCliData->rxByte, 1);
        if (status != 0)
        {
            cliStatus = ADI_CLI_STATUS_COMM_ERROR;
        }
    }
    return cliStatus;
}

ADI_CLI_STATUS adi_cli_TxCallback(ADI_CLI_HANDLE hCli)
{
    ADI_CLI_STATUS status = ADI_CLI_STATUS_SUCCESS;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    if (hCli == NULL)
    {
        status = ADI_CLI_STATUS_NULL_PTR;
    }
    else
    {
        pInfo->isTxComplete = 1;
    }
    return status;
}

ADI_CLI_STATUS adi_cli_FlushMessages(ADI_CLI_HANDLE hCli)
{
    ADI_CLI_STATUS status = ADI_CLI_STATUS_SUCCESS;
    static int32_t bufId = 0;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    if (hCli == NULL)
    {
        return ADI_CLI_STATUS_NULL_PTR;
    }
    else
    {
        /* Do not flush until previous transfer is complete*/
        if ((pInfo->cliIfData.bufferInfo.bytesStored > 0) && (pInfo->isTxComplete == true))
        {
            pInfo->isTxComplete = false;
            status = pInfo->config.pfTransmitAsync(pInfo->config.hUser,
                                                   pInfo->cliIfData.bufferInfo.pBufferToWrite,
                                                   pInfo->cliIfData.bufferInfo.bytesStored);

            if (bufId == 0)
            {
                pInfo->cliIfData.bufferInfo.pBufferToWrite = pInfo->cliBuffer1;
            }
            else
            {
                pInfo->cliIfData.bufferInfo.pBufferToWrite = pInfo->cliBuffer0;
            }
            // Toggle the bufID
            bufId ^= 0x1;
            pInfo->cliIfData.bufferInfo.bytesStored = 0;
        }
        if ((pInfo->isTxComplete == false) || (pInfo->cliIfData.bufferInfo.bytesStored > 0))
        {
            status = ADI_CLI_STATUS_TRANSMISSION_IN_PROGRESS;
        }
    }
    return status;
}

ADI_CLI_STATUS adi_cli_DisplayPrompt(ADI_CLI_HANDLE hCli)
{
    ADI_CLI_STATUS status = ADI_CLI_STATUS_SUCCESS;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    if (hCli == NULL)
    {
        status = ADI_CLI_STATUS_NULL_PTR;
    }
    else
    {
        CliOverwriteLineWithPrompt(&pInfo->cliIfData);
    }
    return status;
}

ADI_CLI_STATUS adi_cli_NewLine(ADI_CLI_HANDLE hCli)
{
    ADI_CLI_STATUS status = ADI_CLI_STATUS_SUCCESS;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    if (hCli == NULL)
    {
        status = ADI_CLI_STATUS_NULL_PTR;
    }
    else
    {
        CliNewLine(&pInfo->cliIfData);
    }
    return status;
}

/**
 * @details Prints message
 */
// added this to avoid warning format sring is not a literal.
__attribute__((__format__(__printf__, 2, 0))) int32_t adi_cli_PrintMessage(char *pMsgType,
                                                                           char *pFormat, ...)
{
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hTerminalHandle;
    int32_t status = 0;
    int32_t ret = 0;
    va_list pArgs;
    va_start(pArgs, pFormat);
    vsnprintf(pInfo->pMsgString, ADI_CLI_MAX_MSG_SIZE, pFormat, pArgs);

    if ((strcmp(pMsgType, "RAW") == 0) || (strcmp(pMsgType, "DBGRAW") == 0))
    {
        va_end(pArgs);
        /* Just print the message as such*/
        status = CopyToBuffer(pInfo->pMsgString);
    }
    else
    {
        ret = snprintf(pInfo->pMsgStringToCopy, ADI_CLI_MAX_MSG_SIZE, "%s\n\r", pInfo->pMsgString);
        va_end(pArgs);
        if (ret >= 0)
        {

            CopyToBuffer(pMsgType);
            status = CopyToBuffer(pInfo->pMsgStringToCopy);
        }
        else
        {
            status = 1;
        }
    }

    return status;
}

ADI_CLI_STATUS adi_cli_GetNumCharsWaiting(ADI_CLI_HANDLE hCli, int32_t *pNumChars)
{
    ADI_CLI_STATUS status = ADI_CLI_STATUS_SUCCESS;
    ADI_CLI_RX_DATA *pCliData;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    if (hCli == NULL || pNumChars == NULL)
    {
        status = ADI_CLI_STATUS_NULL_PTR;
    }
    else
    {
        pCliData = &pInfo->cliIfData.cliData;
        *pNumChars = ADICircBufGetNumBytesAvailable(pCliData->pRxBuff);
        if (*pNumChars < 0)
        {
            status = ADI_CLI_STATUS_BUFFER_FULL;
        }
    }

    return status;
}

ADI_CLI_STATUS adi_cli_SetHandleTerminal(ADI_CLI_HANDLE hCli)
{
    ADI_CLI_STATUS status = ADI_CLI_STATUS_SUCCESS;
    if (hCli == NULL)
    {
        status = ADI_CLI_STATUS_NULL_PTR;
    }
    else
    {
        hTerminalHandle = hCli;
    }
    return status;
}

ADI_CLI_STATUS adi_cli_GetChar(ADI_CLI_HANDLE hCli, int32_t *pChar)
{
    ADI_CLI_STATUS status = ADI_CLI_STATUS_SUCCESS;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    int32_t charRead = 0;
    if (hCli == NULL)
    {
        status = ADI_CLI_STATUS_NULL_PTR;
    }
    else
    {
        charRead = CliGetChar(&pInfo->cliIfData);
        *pChar = charRead;
        if (charRead == EOF)
        {
            status = ADI_CLI_STATUS_BUFFER_FULL;
        }
    }
    return status;
}

ADI_CLI_STATUS adi_cli_PutChar(ADI_CLI_HANDLE hCli, char inputchar)
{
    ADI_CLI_STATUS status = ADI_CLI_STATUS_SUCCESS;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    if (hCli == NULL)
    {
        status = ADI_CLI_STATUS_NULL_PTR;
    }
    else
    {
        status = CliPutChar(&pInfo->cliIfData, inputchar);
        if (status != ADI_CLI_STATUS_SUCCESS)
        {
            status = ADI_CLI_STATUS_BUFFER_FULL;
        }
    }
    return status;
}

ADI_CLI_STATUS adi_cli_PutString(ADI_CLI_HANDLE hCli, const char *pStr)
{
    int32_t status = 0;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    ADI_CLI_STATUS cliStatus = 0;
    if (hCli == NULL || pStr == NULL)
    {
        cliStatus = ADI_CLI_STATUS_NULL_PTR;
    }
    else
    {
        status = CliPutString(&pInfo->cliIfData, pStr);
        if (status != ADI_CLI_STATUS_SUCCESS)
        {
            cliStatus = ADI_CLI_STATUS_BUFFER_FULL;
        }
    }

    return cliStatus;
}

ADI_CLI_STATUS adi_cli_PutBuffer(ADI_CLI_HANDLE hCli, uint8_t *pData, uint32_t numBytes)
{
    ADI_CLI_STATUS cliStatus = 0;
    int32_t status = 0;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    if (hCli == NULL || pData == NULL)
    {
        status = ADI_CLI_STATUS_NULL_PTR;
    }
    else
    {
        status = CliPutBuffer(&pInfo->cliIfData, (const char *)pData, numBytes);
        if (status != 0)
        {
            cliStatus = ADI_CLI_STATUS_BUFFER_FULL;
        }
    }
    return cliStatus;
}

ADI_CLI_STATUS adi_cli_GetFreeMessageSpace(ADI_CLI_HANDLE hCli, uint32_t *pFreeSpace)
{
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    ADI_CLI_STATUS status = 0;
    if (hCli == NULL || pFreeSpace == NULL)
    {
        status = ADI_CLI_STATUS_NULL_PTR;
    }
    else
    {
        *pFreeSpace = ADI_CLI_MAX_SIZE - pInfo->cliIfData.bufferInfo.bytesStored;
    }
    return status;
}

void *GetHandleForDispatchCommands(ADI_CLI_HANDLE hCli)
{
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    if (hCli == NULL)
    {
        return NULL;
    }
    return &pInfo->cliIfData;
}

static void InitCircBuff(CLI_PRIVATE *pInfo)
{
    ADI_CLI_RX_DATA *pCliData = &pInfo->cliData;

    // Initialize Receive Buffer
    pCliData->pRxBuff = &pCliData->rxCircBuff;
    pCliData->pRxBuff->pBase = (uint8_t *)&pCliData->rxBuff[0];
    pCliData->pRxBuff->nSize = RX_BUFFER_SIZE;
    pCliData->pRxBuff->nReadIndex = 0;
    pCliData->pRxBuff->nWriteIndex = 0;
}

int32_t CopyToBuffer(char *pMessage)
{
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hTerminalHandle;
    uint8_t *pBufferToWrite = pInfo->cliIfData.bufferInfo.pBufferToWrite;
    int32_t space = (int32_t)(ADI_CLI_MAX_SIZE - pInfo->cliIfData.bufferInfo.bytesStored - 1);
    int32_t bytesToWrite = (int32_t)strlen(pMessage);
    int32_t status = 0;

    if (space >= bytesToWrite)
    {
        memcpy(&pBufferToWrite[pInfo->cliIfData.bufferInfo.bytesStored], pMessage,
               (size_t)bytesToWrite);
        pInfo->cliIfData.bufferInfo.bytesStored += (uint32_t)bytesToWrite;
    }
    else
    {
        status = 1;
    }

    return status;
}

static void IntialiseStateData(ADI_CLI_INFO *pInfo)
{
    pInfo->cliIfData.echo = true;
    pInfo->cliIfData.deferPrompt = false;
    pInfo->cliIfData.displayPrompt = false;
    pInfo->cliIfData.userIsTyping = false;
    pInfo->cliIfData.displayCtrlChars = true;
    pInfo->isTxComplete = 1;
    pInfo->cliIfData.bufferInfo.bytesStored = 0;
    pInfo->cliIfData.bufferInfo.pBufferToWrite = &pInfo->cliBuffer0[0];
}

static void AllocateTempMem(ADI_CLI_INFO *pInfo)
{
    pInfo->cliIfData.cliDispatchData.pCmdLower = (char *)&pInfo->pTempMemory[0];
    pInfo->cliIfData.cliDispatchData.pDispatchCmd =
        (char *)&pInfo->pTempMemory[1 * APP_CFG_CLI_MAX_CMD_LENGTH];
    pInfo->cliIfData.pCliTrimString = (char *)&pInfo->pTempMemory[2 * APP_CFG_CLI_MAX_CMD_LENGTH];
    pInfo->cliIfData.pCliPrintString = (char *)&pInfo->pTempMemory[3 * APP_CFG_CLI_MAX_CMD_LENGTH];
    pInfo->pMsgString = (char *)&pInfo->pTempMemory[4 * APP_CFG_CLI_MAX_CMD_LENGTH];
    pInfo->pMsgStringToCopy =
        (char *)&pInfo->pTempMemory[(5 * APP_CFG_CLI_MAX_CMD_LENGTH) + ADI_CLI_MAX_MSG_SIZE];
}

/**
 * @}
 */
