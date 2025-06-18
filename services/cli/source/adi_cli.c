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
#include "app_cfg.h"
#include "cli_dispatch.h"
#include "cli_history.h"
#include "cli_interface.h"
#include "cli_utils.h"
#include "string.h"
#include <stdint.h>
#include <stdio.h>

/** Maximum msg size*/
#define CLI_MAX_MSG_BLOCKSIZE 0xFFFF

static void InitCircBuff(void);

static ADI_CLI_HANDLE hCli;

/** Maximum buffer size to store the CLI data */
#define MAX_CLI_MSG_STORAGE_SIZE 1024
/** Buffer to write and store the message */
static int32_t CliFlushMessages(void);
static void SetTempMem(ADI_CLI_INFO *pInfo);
static void IntialiseStateData(ADI_CLI_INFO *pInfo);

int32_t adi_cli_GetNumCharsWaiting(void)
{
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    ADI_CLI_RX_DATA *pCliData = &pInfo->cliData;
    return ADICircBufGetNumBytesAvailable(pCliData->pRxBuff);
}

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
            hCli = pInfo;
            memset(pInfo, 0, sizeof(ADI_CLI_INFO));
            InitCircBuff();
            IntialiseStateData(pInfo);

            if (tempMemorySize < reqTempSize)
            {
                return ADI_CLI_STATUS_INSUFFICIENT_TEMP_MEMORY;
            }
            else
            {
                pInfo->pTempMemory = pTempMemory;
                pInfo->tempMemSize = tempMemorySize;
                // Assign temporary memory to state pointers
                SetTempMem(pInfo);
            }
        }
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
    pInfo->cliIfData.sArgs.c = 0;
    pInfo->isTxComplete = 1;
    pInfo->cliBytesStored = 0;
    pInfo->pBufferToWrite = &pInfo->cliBuffer0[0];
}

static void SetTempMem(ADI_CLI_INFO *pInfo)
{
    pInfo->cliUtilsData.pMatchParam0 = (char *)&pInfo->pTempMemory[0];
    pInfo->cliUtilsData.pMatchParam1 = (char *)&pInfo->pTempMemory[APP_CFG_CLI_MAX_CMD_LENGTH];
    pInfo->cliDispatchData.pCmdLower = (char *)&pInfo->pTempMemory[2 * APP_CFG_CLI_MAX_CMD_LENGTH];
    pInfo->cliDispatchData.pDispatchCmd =
        (char *)&pInfo->pTempMemory[3 * APP_CFG_CLI_MAX_CMD_LENGTH];
    pInfo->cliIfData.pCliTrimString = (char *)&pInfo->pTempMemory[4 * APP_CFG_CLI_MAX_CMD_LENGTH];
    pInfo->cliIfData.pCliPrintString = (char *)&pInfo->pTempMemory[5 * APP_CFG_CLI_MAX_CMD_LENGTH];
}

ADI_CLI_INFO *GetCliInfo(void)
{
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    return pInfo;
}

ADI_CLI_STATUS adi_cli_Init(ADI_CLI_CONFIG *pConfig)
{
    ADI_CLI_STATUS cliStatus = 0;
    int32_t status = 0;
    ADI_CLI_INFO *pInfo;
    if (pConfig == NULL)
    {
        status = ADI_CLI_STATUS_NULL_PTR;
    }
    else
    {
        pInfo = (ADI_CLI_INFO *)hCli;
        pConfig->hUser = pInfo;
        pInfo->config = *pConfig;
        DispatchInit(pInfo->config.pDispatchTable, pInfo->config.numRecords);
        status =
            pInfo->config.pfReceiveAsync(pInfo->config.hUser, (char *)&pInfo->cliData.rxByte, 1);
        if (status != 0)
        {
            cliStatus = ADI_CLI_STATUS_COMM_ERROR;
        }
        CliInit();
        CliFlushMessages();
        adi_cli_DisplayPrompt();
    }

    return cliStatus;
}

int32_t adi_cli_Interface(void)
{
    int32_t status = 0;
    status = CliInterface();
    CliFlushMessages();
    return status;
}

void adi_cli_DisplayPrompt(void)
{
    CliOverwriteLineWithPrompt();
}

void adi_cli_NewLine(void)
{
    CliNewLine();
}

ADI_CLI_STATUS adi_cli_PutChar(char inputchar)
{
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    memcpy(&pInfo->pBufferToWrite[pInfo->cliBytesStored], &inputchar, 1);
    pInfo->cliBytesStored += 1;
    return 0;
}

ADI_CLI_STATUS adi_cli_PutStringNb(const char *pStr)
{
    ADI_CLI_STATUS status = 0;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    uint16_t stringLen = (uint16_t)strlen(pStr);
    if (stringLen < APP_CFG_MAX_STRING_LENGTH)
    {
        memcpy(&pInfo->pBufferToWrite[pInfo->cliBytesStored], pStr, stringLen);
        pInfo->cliBytesStored += stringLen;
    }
    else
    {
        status = 1;
    }
    return status;
}

ADI_CLI_STATUS adi_cli_PutBuffer(uint8_t *pData, uint32_t numBytes)
{
    ADI_CLI_STATUS status = 0;
    uint16_t readIndex = 0;
    uint32_t numBytesToSend = 0;
    uint16_t index = 0;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    while (index < numBytes)
    {
        numBytesToSend = numBytes - readIndex;
        if (numBytesToSend > CLI_MAX_MSG_BLOCKSIZE)
        {
            index += CLI_MAX_MSG_BLOCKSIZE;
            numBytesToSend = CLI_MAX_MSG_BLOCKSIZE;
        }
        else
        {
            index += numBytesToSend;
        }
        memcpy(&pInfo->pBufferToWrite[pInfo->cliBytesStored], (uint8_t *)&pData[readIndex],
               numBytesToSend);
        pInfo->cliBytesStored += numBytesToSend;
        readIndex += index;
    }
    return status;
}

/**
 * @brief Get a character from UART
 */
int32_t adi_cli_GetChar(void)
{
    int32_t ret = 0;
    int32_t status = 0;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    ADI_CLI_RX_DATA *pCliData = &pInfo->cliData;
    status = ADICircBufRead(pCliData->pRxBuff, (volatile uint8_t *)&ret, 1);
    if (status == 1)
    {
        ret = EOF;
    }
    return ret;
}

/**
 * @brief Sends a string through UART Nonblocking mode
 */
ADI_CLI_STATUS adi_cli_PutString(const char *pStr)
{
    ADI_CLI_STATUS status = 0;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    uint16_t stringLen = (uint16_t)strlen(pStr);
    if (stringLen < APP_CFG_MAX_STRING_LENGTH)
    {
        memcpy(&pInfo->pBufferToWrite[pInfo->cliBytesStored], pStr, stringLen);
        pInfo->cliBytesStored += stringLen;
    }
    else
    {
        status = ADI_CLI_STATUS_BUFFER_FULL;
    }
    return status;
}

void adi_cli_RxCallback(void)
{
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    ADI_CLI_RX_DATA *pCliData = &pInfo->cliData;
    ADICircBufWrite(pCliData->pRxBuff, &pCliData->rxByte, 1);
    pInfo->config.pfReceiveAsync(pInfo->config.hUser, (char *)&pCliData->rxByte, 1);
}

void adi_cli_TxCallback(void)
{
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    pInfo->isTxComplete = 1;
}

/**
 * @brief Initialization of tx and rx circular buffers
 */
static void InitCircBuff(void)
{
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    ADI_CLI_RX_DATA *pCliData = &pInfo->cliData;

    // Initialize Receive Buffer
    pCliData->pRxBuff = &pCliData->rxCircBuff;
    pCliData->pRxBuff->pBase = (uint8_t *)&pCliData->rxBuff[0];
    pCliData->pRxBuff->nSize = RX_BUFFER_SIZE;
    pCliData->pRxBuff->nReadIndex = 0;
    pCliData->pRxBuff->nWriteIndex = 0;
}

/**
 * @brief Flush the messages stored in the CLI Buffer
 * @return 0 if the buffer is empty, 1 if the buffer is not empty.
 */
int32_t CliFlushMessages(void)
{
    int32_t status = 0;
    static int32_t bufId = 0;
    ADI_CLI_INFO *pInfo = (ADI_CLI_INFO *)hCli;
    /* Do not flush until previous transfer is complete*/
    if ((pInfo->cliBytesStored > 0) && (pInfo->isTxComplete == true))
    {
        pInfo->isTxComplete = false;
        status = pInfo->config.pfTransmitAsync(pInfo->config.hUser, pInfo->pBufferToWrite,
                                               pInfo->cliBytesStored);
        if (bufId == 0)
        {
            pInfo->pBufferToWrite = (uint8_t *)&pInfo->cliBuffer1;
        }
        else
        {
            pInfo->pBufferToWrite = (uint8_t *)&pInfo->cliBuffer0;
        }
        // Toggle the bufID
        bufId ^= 0x1;
        pInfo->cliBytesStored = 0;
    }
    if ((pInfo->isTxComplete == false) || (pInfo->cliBytesStored > 0))
    {
        status = 1;
    }
    return status;
}

/**
 * @}
 */
