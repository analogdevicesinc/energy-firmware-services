/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file    nvm_mb85rs.c
 * @brief   File to communicate with FRAM device MB58RS via SPI
 * @{
 */

#include "adi_crc_ccitt16.h"
#include "adi_nvm_private.h"
#include "app_cfg.h"
#include "board_cfg.h"
#include "nvm_device.h"
#include <stdint.h>
#include <string.h>

/** Number of bytes for the header to be included (addr + cmd) to send data to FRAM */
#define MB85RS_HEADER_NUM_BYTES 4
/** Number of bytes for CRC addition*/
#define MB85RS_CRC_NUM_BYTES 2
/** Number of bytes to be included (addr + cmd) with header and CRC to send data to FRAM */
#define MB85RS_NUM_BYTES MB85RS_HEADER_NUM_BYTES + MB85RS_CRC_NUM_BYTES
/** Total size of the non-volatile memory */
#define NVM_MB85RS_SIZE 262144

static ADI_NVM_STATUS NvmSendCmdGetResponse(ADI_NVM_INFO *pInfo, uint8_t cmd, uint32_t numBytes,
                                            uint8_t *pData);
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

// this api is not used for fram
void NVMFormat(ADI_NVM_INFO *pInfo)
{
    (void)pInfo;
}

ADI_NVM_STATUS NvmInit(ADI_NVM_INFO *pInfo)
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
    }
    return status;
}

ADI_NVM_STATUS NvmWrite(ADI_NVM_INFO *pInfo, uint8_t *pData, uint32_t addr, uint32_t numBytes)
{
    ADI_NVM_STATUS status = ADI_NVM_STATUS_SUCCESS;
    uint32_t bytesRemain;
    uint32_t offset = 0;
    uint32_t chunkSize;
    uint32_t numBytesToSend;
    uint16_t crc;
    uint32_t maxChunkData = ADI_NVM_MAX_SIZE - 6;

    if (numBytes == 0)
    {
        return ADI_NVM_STATUS_INVALID_NUM_REGISTERS;
    }
    else if (numBytes > (NVM_MB85RS_SIZE - MB85RS_NUM_BYTES))
    {
        return ADI_NVM_STATUS_INVALID_NUM_REGISTERS;
    }
    bytesRemain = numBytes;
    // Calculate CRC for the entire data to be written
    crc = adi_crc_CalculateCCITT16(pData, (uint16_t)numBytes);
    // If the numBytes is greater than the maximum chunk size, split the write into chunks and for
    // the last chunk data add CRC bytes.
    while (bytesRemain > 0)
    {
        chunkSize = (bytesRemain > maxChunkData) ? maxChunkData : bytesRemain;
        pInfo->txData[0] = MB58RS_WRITE;
        pInfo->txData[1] = (uint8_t)((addr + offset) >> 16);
        pInfo->txData[2] = (uint8_t)((addr + offset) >> 8);
        pInfo->txData[3] = (uint8_t)(addr + offset);
        memcpy(&pInfo->txData[MB85RS_HEADER_NUM_BYTES], &pData[offset], chunkSize);
        numBytesToSend = chunkSize + MB85RS_HEADER_NUM_BYTES;
        // For the last chunk, add CRC bytes
        if (bytesRemain == chunkSize)
        {
            pInfo->txData[MB85RS_HEADER_NUM_BYTES + chunkSize] = (uint8_t)(crc >> 8);
            pInfo->txData[MB85RS_HEADER_NUM_BYTES + chunkSize + 1] = (uint8_t)(crc & 0xFF);
            numBytesToSend += MB85RS_CRC_NUM_BYTES;
        }
        status = pInfo->config.pfStartTx(pInfo->config.hUser, &pInfo->txData[0], numBytesToSend);
        if (status != 0)
        {
            status = ADI_NVM_STATUS_COMM_ERROR;
            break;
        }
        offset += chunkSize;
        bytesRemain -= chunkSize;
    }

    return status;
}

ADI_NVM_STATUS NvmRead(ADI_NVM_INFO *pInfo, uint32_t addr, uint32_t numBytes, uint8_t *pData)
{
    ADI_NVM_STATUS status = ADI_NVM_STATUS_COMM_ERROR;
    uint32_t numBytesToSend;
    uint32_t bytesRemain;
    uint32_t offset = 0;
    uint32_t maxChunkData = ADI_NVM_MAX_SIZE - 6;
    uint32_t chunkSize;
    uint16_t crc;
    uint16_t expectedCrc;
    if (numBytes == 0)
    {
        return ADI_NVM_STATUS_INVALID_NUM_REGISTERS;
    }
    else if (numBytes > (NVM_MB85RS_SIZE - MB85RS_NUM_BYTES))
    {
        return ADI_NVM_STATUS_INVALID_NUM_REGISTERS;
    }
    else
    {
        // If the numBytes is greater than the maximum chunk size, split the read into chunks and
        // for the last chunk data expect CRC bytes.
        bytesRemain = numBytes;
        while (bytesRemain > 0)
        {
            chunkSize = (bytesRemain > maxChunkData) ? maxChunkData : bytesRemain;
            pInfo->txData[0] = MB58RS_READ;
            pInfo->txData[1] = (uint8_t)((addr + offset) >> 16);
            pInfo->txData[2] = (uint8_t)((addr + offset) >> 8);
            pInfo->txData[3] = (uint8_t)(addr + offset);

            // For the last chunk, verify CRC.
            numBytesToSend = chunkSize + MB85RS_HEADER_NUM_BYTES;
            if (bytesRemain == chunkSize)
            {
                numBytesToSend += MB85RS_CRC_NUM_BYTES;
            }
            status =
                pInfo->config.pfStartTxRx(pInfo->config.hUser, &pInfo->txData[0], numBytesToSend,
                                          APP_CFG_TIMEOUT_COUNT, &pInfo->rxData[0]);
            if (status == ADI_NVM_STATUS_SUCCESS)
            {
                if (bytesRemain == chunkSize)
                {
                    memcpy(&pInfo->tempBuffer[0], &pData[offset], chunkSize);
                    memcpy(&pData[offset], &pInfo->rxData[4], chunkSize);
                    crc = adi_crc_CalculateCCITT16(pData, numBytes);
                    expectedCrc = pInfo->rxData[4 + chunkSize] << 8 | pInfo->rxData[5 + chunkSize];
                    if (crc != expectedCrc)
                    {
                        // retrieve the orginal content if the CRC is mismatched.
                        memcpy(&pData[offset], &pInfo->tempBuffer[0], chunkSize);
                        status = ADI_NVM_STATUS_CRC_MISMATCH;
                        break;
                    }
                }
                else
                {
                    // Not last chunk: just copy data
                    memcpy(&pData[offset], &pInfo->rxData[4], chunkSize);
                }
            }
            else
            {
                status = ADI_NVM_STATUS_COMM_ERROR;
                break;
            }

            offset += chunkSize;
            bytesRemain -= chunkSize;
        }
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
    status = pInfo->config.pfStartTxRx(pInfo->config.hUser, &pInfo->txData[0], numBytesToSend,
                                       APP_CFG_TIMEOUT_COUNT, &pInfo->rxData[0]);
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

/**
 * @}
 */
