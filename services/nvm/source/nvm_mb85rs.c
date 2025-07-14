/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file    nvm_mb85rs.c
 * @brief   File to communicate with FRAM device MB58RS via SPI
 * @{
 */

#include "adi_nvm_private.h"
#include "board_cfg.h"
#include "nvm_device.h"
#include <stdint.h>
#include <string.h>

/** Number of bytes for the header to be included (addr + cmd) to send data to FRAM */
#define MB85RS_HEADER_NUM_BYTES 4
/** Total size of the non-volatile memory */
#define NVM_MB85RS_SIZE 262144

/**
 * @brief Sends a command to the FRAM device and gets the response.
 * @param[in] pInfo 		- NVM service handle
 * @param[in] cmd - command to be sent to the FRAM device
 * @param[in] numBytes - number of bytes to read from the FRAM device
 * @param[in] pData - Pointer to data.
 * @return  #ADI_NVM_STATUS_SUCCESS\n
 * #ADI_NVM_STATUS_NULL_PTR \n
 * #ADI_NVM_STATUS_INVALID_PRODUCT_ID \n
 * #ADI_NVM_STATUS_COMM_ERROR
 */
static ADI_NVM_STATUS NvmSendCmdGetResponse(ADI_NVM_INFO *pInfo, uint8_t cmd, uint32_t numBytes,
                                            uint8_t *pData);
/**
 * @brief Erases the non volatile memory device
 * @param[in] pInfo 		- pointer to NVM data
 * @param[in] addr     	- address to where data to be erased
 * @param[in] numBytes  - number of bytes to erase
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

/**
 * @brief List of FRAM Op-Code
 */
typedef enum
{
    MB58RS_WREN = 0x6,  /*! @brief Set write enable latch. */
    MB58RS_WRDI = 0x4,  /*! @brief Reset write enable latch. */
    MB58RS_RDSR = 0x5,  /*! @brief Read status register. */
    MB58RS_WRSR = 0x1,  /*! @brief Write status register. */
    MB58RS_READ = 0x3,  /*! @brief Read memory code. */
    MB58RS_WRITE = 0x2, /*! @brief Write memory code. */
    MB58RS_RDID = 0x9f, /*! @brief Read device ID. */
    MB58RS_FSTRD = 0xb, /*! @brief Fast read memory code. */
    MB58RS_SLEEP = 0xb9 /*! @brief Sleep mode. */
} MB58RS_CMD;

ADI_NVM_STATUS NvmDeviceInit(ADI_NVM_INFO *pInfo)
{
    ADI_NVM_STATUS status = ADI_NVM_STATUS_COMM_ERROR;
    uint8_t dummyData = 0;

    status =
        NvmSendCmdGetResponse(pInfo, MB58RS_RDID, sizeof(uint32_t), (uint8_t *)&pInfo->productId);
    if (status == ADI_NVM_STATUS_SUCCESS)
    {
        if ((pInfo->productId & 0xFFFF0000) != BOARD_CFG_MB85RS_PRODUCT_ID)
        {
            status = ADI_NVM_STATUS_INVALID_PRODUCT_ID;
        }
        else
        {
            status = NvmSendCmdGetResponse(pInfo, MB58RS_WREN, 0, &dummyData);
        }
        pInfo->pfFormat = NvmFormat;
        pInfo->pfEraseFn = NvmErase;
        pInfo->maxNumBytes = NVM_MB85RS_SIZE - MB85RS_HEADER_NUM_BYTES - NUM_CRC_BYTES;
        pInfo->rxOffset = MB85RS_HEADER_NUM_BYTES;
    }
    return status;
}

ADI_NVM_STATUS NvmSendCmdGetResponse(ADI_NVM_INFO *pInfo, uint8_t cmd, uint32_t numBytes,
                                     uint8_t *pData)
{
    ADI_NVM_STATUS status = ADI_NVM_STATUS_COMM_ERROR;
    uint32_t numBytesToSend;
    pInfo->txData[0] = cmd;
    numBytesToSend = 1 + numBytes;
    status = pInfo->config.pfRead(pInfo->config.hUser, &pInfo->txData[0], numBytesToSend,
                                  &pInfo->rxData[0]);
    if (status == ADI_NVM_STATUS_SUCCESS)
    {
        memcpy(pData, &pInfo->rxData[1], numBytes);
    }
    else
    {
        status = ADI_NVM_STATUS_COMM_ERROR;
    }
    return status;
}

ADI_NVM_STATUS NvmErase(void *pNvmInfo, uint32_t addr)
{
    ADI_NVM_STATUS nvmStatus = ADI_NVM_STATUS_SUCCESS;
    ADI_NVM_INFO *pInfo = (ADI_NVM_INFO *)pNvmInfo;

    if (pInfo == NULL)
    {
        nvmStatus = ADI_NVM_STATUS_NULL_PTR;
    }
    else
    {
        memset(&pInfo->eraseData[0], 0xff, NUM_CRC_BYTES);
        NvmWrite(pInfo, &pInfo->eraseData[0], addr, NUM_CRC_BYTES);
    }

    return nvmStatus;
}

uint32_t NvmFormat(void *pFormat, uint8_t *pDst)
{
    uint8_t cmd = 0;
    NvmDeviceCmdFormat *pFramFormat = (NvmDeviceCmdFormat *)pFormat;
    switch (pFramFormat->cmd)
    {
    case ADI_NVM_WRITE:
        cmd = MB58RS_WRITE;
        break;
    case ADI_NVM_READ:
        cmd = MB58RS_READ;
        break;
    case ADI_NVM_ERASE:
        cmd = MB58RS_WRITE;
    default:
        // Handle other cases or error
        break;
    }
    // Refer to the MB85RS datasheet for command format.
    pDst[0] = cmd;
    pDst[1] = (uint8_t)((pFramFormat->addr + pFramFormat->offset) >> 16);
    pDst[2] = (uint8_t)((pFramFormat->addr + pFramFormat->offset) >> 8);
    pDst[3] = (uint8_t)((pFramFormat->addr + pFramFormat->offset));
    return MB85RS_HEADER_NUM_BYTES;
}

/**
 * @}
 */
