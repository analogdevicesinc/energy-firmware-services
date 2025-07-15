/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file	    adi_nvm.c
 * @brief       Definitions needed for NVM.
 * @{
 */

#include "adi_nvm.h"
#include "adi_nvm_private.h"
#include "nvm_device.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

ADI_NVM_STATUS adi_nvm_Create(ADI_NVM_HANDLE *phNvm, void *pStateMemory, uint32_t stateMemorySize)
{
    ADI_NVM_STATUS status = ADI_NVM_STATUS_SUCCESS;
    ADI_NVM_INFO *pInfo = NULL;
    uint32_t reqSize = sizeof(ADI_NVM_INFO);
    /* Check the given pointers before we set their contents */
    if ((phNvm == (void *)NULL) || (pStateMemory == (void *)NULL))
    {
        return ADI_NVM_STATUS_NULL_PTR;
    }

    if (status == ADI_NVM_STATUS_SUCCESS)
    {
        /* Set handle to NULL in case of error */
        *phNvm = (ADI_NVM_HANDLE)NULL;
        if (stateMemorySize < reqSize)
        {
            return ADI_NVM_STATUS_INSUFFICIENT_STATE_MEMORY;
        }
        else
        {
            pInfo = (ADI_NVM_INFO *)pStateMemory;
            *phNvm = (ADI_NVM_HANDLE *)pInfo;
            memset(pInfo, 0, sizeof(ADI_NVM_INFO));
        }
    }
    return status;
}

ADI_NVM_STATUS adi_nvm_Init(ADI_NVM_HANDLE hNvm, ADI_NVM_CONFIG *pConfig)
{
    ADI_NVM_STATUS status = ADI_NVM_STATUS_SUCCESS;
    ADI_NVM_INFO *pInfo = (ADI_NVM_INFO *)hNvm;

    if ((hNvm == NULL) || (pConfig == NULL))
    {
        status = ADI_NVM_STATUS_NULL_PTR;
    }
    else
    {
        pInfo->config = *pConfig;
        NvmInit(pInfo);
    }
    return status;
}

ADI_NVM_STATUS adi_nvm_SetConfig(ADI_NVM_HANDLE hNvm, ADI_NVM_CONFIG *pConfig)
{
    ADI_NVM_STATUS status = ADI_NVM_STATUS_SUCCESS;
    ADI_NVM_INFO *pInfo = (ADI_NVM_INFO *)hNvm;
    if ((hNvm == NULL) || (pConfig == NULL))
    {
        status = ADI_NVM_STATUS_NULL_PTR;
    }
    else
    {
        pInfo->config = *pConfig;
    }
    return status;
}

ADI_NVM_STATUS adi_nvm_Write(ADI_NVM_HANDLE hNvm, uint8_t *pData, uint32_t addr, uint32_t numBytes)
{
    ADI_NVM_STATUS status = ADI_NVM_STATUS_SUCCESS;
    ADI_NVM_INFO *pInfo = (ADI_NVM_INFO *)hNvm;
    if (hNvm == NULL)
    {
        status = ADI_NVM_STATUS_NULL_PTR;
    }
    else
    {
        status = NvmWrite(pInfo, pData, addr, numBytes);
    }

    return status;
}

ADI_NVM_STATUS adi_nvm_WriteBlock(ADI_NVM_HANDLE hNvm, ADI_NVM_BLOCK_DATA *pBlockData,
                                  uint32_t addr)
{
    ADI_NVM_STATUS status = ADI_NVM_STATUS_SUCCESS;
    ADI_NVM_INFO *pInfo = (ADI_NVM_INFO *)hNvm;
    int32_t i;
    uint8_t *pWriteData;
    if (hNvm == NULL)
    {
        status = ADI_NVM_STATUS_NULL_PTR;
    }
    else
    {
        // Write data from pBlockData to NVM starting from the given address in a contiguous memory
        // region, including CRC. For each block, the data is written from pBlockData at the
        // corresponding offset determined by incrAddress. The address in NVM is incremented by the
        // size of each block (numBytes) plus the size of the CRC (ADI_NVM_NUM_BYTES_CRC).
        for (i = 0; i < pBlockData->numBlocks; i++)
        {
            pWriteData = pBlockData->pData + pBlockData->incrAddress * i;
            status = NvmWrite(pInfo, pWriteData, addr, pBlockData->numBytes);
            if (status != ADI_NVM_STATUS_SUCCESS)
            {
                break;
            }
            addr += pBlockData->numBytes + ADI_NVM_NUM_BYTES_CRC;
        }
    }
    return status;
}

ADI_NVM_STATUS adi_nvm_Read(ADI_NVM_HANDLE hNvm, uint32_t addr, uint32_t numBytes, uint8_t *pData)
{
    ADI_NVM_STATUS status = ADI_NVM_STATUS_SUCCESS;
    ADI_NVM_INFO *pInfo = (ADI_NVM_INFO *)hNvm;
    if (hNvm == NULL)
    {
        status = ADI_NVM_STATUS_NULL_PTR;
    }
    else
    {
        status = NvmRead(pInfo, addr, numBytes, pData);
    }
    return status;
}

ADI_NVM_STATUS adi_nvm_ReadBlock(ADI_NVM_HANDLE hNvm, uint32_t addr, ADI_NVM_BLOCK_DATA *pBlockData)
{
    ADI_NVM_STATUS status = ADI_NVM_STATUS_SUCCESS;
    ADI_NVM_INFO *pInfo = (ADI_NVM_INFO *)hNvm;
    int32_t i;
    uint8_t *pReadData;
    if (hNvm == NULL)
    {
        status = ADI_NVM_STATUS_NULL_PTR;
    }
    else
    {
        // Read data from NVM starting from the given address in a contiguous memory region,
        // including CRC. For each block, the data is stored in pBlockData at the corresponding
        // offset determined by incrAddress. The address in NVM is incremented by the size of each
        // block (numBytes) plus the size of the CRC (ADI_NVM_NUM_BYTES_CRC).
        for (i = 0; i < pBlockData->numBlocks; i++)
        {
            pReadData = pBlockData->pData + pBlockData->incrAddress * i;
            status = NvmRead(pInfo, addr, pBlockData->numBytes, pReadData);
            if (status != ADI_NVM_STATUS_SUCCESS)
            {
                break;
            }
            addr += pBlockData->numBytes + ADI_NVM_NUM_BYTES_CRC;
        }
    }
    return status;
}

ADI_NVM_STATUS adi_nvm_Erase(ADI_NVM_HANDLE hNvm, uint32_t addr)
{
    ADI_NVM_STATUS status = ADI_NVM_STATUS_SUCCESS;
    ADI_NVM_INFO *pInfo = (ADI_NVM_INFO *)hNvm;
    if (hNvm == NULL)
    {
        status = ADI_NVM_STATUS_NULL_PTR;
    }
    else
    {
        status = pInfo->pfEraseFn(pInfo, addr);
    }
    return status;
}

ADI_NVM_STATUS adi_nvm_EraseBlock(ADI_NVM_HANDLE hNvm, uint32_t addr,
                                  ADI_NVM_BLOCK_DATA *pBlockData)
{
    ADI_NVM_STATUS status = ADI_NVM_STATUS_SUCCESS;
    int32_t j;
    ADI_NVM_INFO *pInfo = (ADI_NVM_INFO *)hNvm;
    if (hNvm == NULL)
    {
        status = ADI_NVM_STATUS_NULL_PTR;
    }
    else
    {
        pInfo->isErase = 1;
        // Corrupt the CRC in NVM starting from the given address in a contiguous memory region.
        // For each block, the address determined by the number of  bytes present and the size of
        // the CRC (ADI_NVM_NUM_BYTES_CRC).
        memset(&pInfo->eraseData[0], 0xff, ADI_NVM_NUM_BYTES_CRC);
        for (j = 0; j < pBlockData->numBlocks; j++)
        {
            addr += pBlockData->numBytes;
            status = pInfo->pfEraseFn(pInfo, addr);
            if (status != ADI_NVM_STATUS_SUCCESS)
            {
                break;
            }
            addr += ADI_NVM_NUM_BYTES_CRC;
        }
        pInfo->isErase = 0;
    }
    return status;
}

/**
 * @}
 */
