/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file    nvm_flc_max32670.c
 * @brief   Source file for the flash of mx32670.
 * @{
 */

#include "adi_nvm.h"
#include "adi_nvm_private.h"
#include "adi_nvm_status.h"
#include "flc.h"
#include "icc.h"
#include "nvm_device.h"
#include <stdint.h>
#include <string.h>

/** Last Adress of flash */
#define LAST_MEM_ADRR_FLASH MXC_FLASH_MEM_BASE + MXC_FLASH_MEM_SIZE

ADI_NVM_STATUS NvmInit(ADI_NVM_INFO *pInfo)
{
    ADI_NVM_STATUS nvmStatus = ADI_NVM_STATUS_SUCCESS;
    int status;
    if (pInfo == NULL)
    {
        nvmStatus = ADI_NVM_STATUS_NULL_PTR;
    }
    else
    {
        status = MXC_FLC_Init();
        if (status != 0)
        {
            nvmStatus = ADI_NVM_STATUS_INIT_FAILED;
        }
    }
    return nvmStatus;
}

// this api is not used for flash
void NVMFormat(ADI_NVM_INFO *pInfo)
{
    if (pInfo != NULL)
    {
        // Added to resolve warnings
    }
}

ADI_NVM_STATUS NvmWrite(ADI_NVM_INFO *pInfo, uint8_t *pData, uint32_t addr, uint32_t numBytes)
{
    ADI_NVM_STATUS nvmStatus = ADI_NVM_STATUS_SUCCESS;
    int status = 0;
    uint32_t memAddr = LAST_MEM_ADRR_FLASH - ((addr + 1) * MXC_FLASH_PAGE_SIZE);
    if (pInfo == NULL)
    {
        nvmStatus = ADI_NVM_STATUS_NULL_PTR;
    }
    else
    {
        if (memAddr < LAST_MEM_ADRR_FLASH)
        {
            // make sure to disable ICC with ICC_Disable(); before writing
            MXC_ICC_Disable();
            status = MXC_FLC_PageErase(memAddr);
            if (status == 0)
            {
                status = MXC_FLC_Write(memAddr, numBytes, (void *)pData);
                if (status != 0)
                {
                    nvmStatus = ADI_NVM_STATUS_COMM_ERROR;
                }
            }
            else
            {
                nvmStatus = ADI_NVM_STATUS_PAGE_ERASE_FAILED;
            }
            MXC_ICC_Enable();
        }
        else
        {
            nvmStatus = ADI_NVM_STATUS_INVALID_ADDRESS;
        }
    }

    return nvmStatus;
}

ADI_NVM_STATUS NvmRead(ADI_NVM_INFO *pInfo, uint32_t addr, uint32_t numBytes, uint8_t *pData)
{
    ADI_NVM_STATUS nvmStatus = ADI_NVM_STATUS_SUCCESS;
    uint32_t memAddr = LAST_MEM_ADRR_FLASH - ((addr + 1) * MXC_FLASH_PAGE_SIZE);
    if (pInfo == NULL)
    {
        nvmStatus = ADI_NVM_STATUS_NULL_PTR;
    }
    else
    {
        if (memAddr < LAST_MEM_ADRR_FLASH)
        {
            MXC_FLC_Read((int)memAddr, pData, (int)numBytes);
        }
        else
        {
            nvmStatus = ADI_NVM_STATUS_INVALID_ADDRESS;
        }
    }

    return nvmStatus;
}

/**
 * @}
 */
