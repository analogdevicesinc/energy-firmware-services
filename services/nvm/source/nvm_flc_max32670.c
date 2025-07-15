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
#include "nvm_device.h"
#include <stdint.h>
#include <string.h>

/** Total size of the non-volatile memory */
#define NVM_FLC_PAGE_SIZE 8192

/** Number of bytes for the header to be included (pgeNum + offset) to send to Flash */
#define FLASH_HEADER_NUM_BYTES 3

/**
 * @brief Erases the non volatile memory device
 * @param[in] pInfo 		- pointer to NVM data
 * @param[in] addr     	- address to where data to be erased
 *
 * @return  #ADI_NVM_STATUS_SUCCESS\n
 * #ADI_NVM_STATUS_INVALID_NUM_REGISTERS \n
 */
static ADI_NVM_STATUS NvmErase(void *pInfo, uint32_t addr);

/**
 * @brief Format the non volatile memory device
 * @param[in] pFormat - pointer to the device format structure
 * @param[in] pDst - pointer to the data to be sent
 *
 */
static uint32_t NvmFormat(void *pFormat, uint8_t *pDst);

ADI_NVM_STATUS NvmDeviceInit(ADI_NVM_INFO *pInfo)
{
    ADI_NVM_STATUS status = ADI_NVM_STATUS_SUCCESS;
    pInfo->pfEraseFn = NvmErase;
    pInfo->pfFormat = NvmFormat;
    pInfo->maxNumBytes = NVM_FLC_PAGE_SIZE - FLASH_HEADER_NUM_BYTES - NUM_CRC_BYTES;
    pInfo->rxOffset = 0;
    return status;
}

ADI_NVM_STATUS NvmErase(void *pNvmInfo, uint32_t addr)
{
    ADI_NVM_STATUS nvmStatus = ADI_NVM_STATUS_SUCCESS;
    ADI_NVM_INFO *pInfo = (ADI_NVM_INFO *)pNvmInfo;
    pInfo->config.pfErase(pInfo, addr);

    return nvmStatus;
}

uint32_t NvmFormat(void *pFormat, uint8_t *pDst)
{
    // The command field is not required for FLC; it can be ignored. Here, 'addr' represents the
    // page number and 'offset' is the byte offset within the page.
    NvmDeviceCmdFormat *pFlashFormat = (NvmDeviceCmdFormat *)pFormat;
    pDst[0] = (uint8_t)(pFlashFormat->addr);
    pDst[1] = (uint8_t)(pFlashFormat->offset >> 8);
    pDst[2] = (uint8_t)pFlashFormat->offset;
    return FLASH_HEADER_NUM_BYTES;
}

/**
 * @}
 */
