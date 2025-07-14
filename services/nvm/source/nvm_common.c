/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file    nvm_common.c
 * @brief   Common source file for reading and writing to NVM devices. It adds CRC to the data
 * and verifies it during read operations.
 * @{
 */

#include "adi_nvm.h"
#include "adi_nvm_private.h"
#include "adi_nvm_status.h"
#include "nvm_device.h"
#include <stdint.h>
#include <string.h>

ADI_NVM_STATUS NvmInit(ADI_NVM_INFO *pInfo)
{
    ADI_NVM_STATUS status = ADI_NVM_STATUS_SUCCESS;
    status = NvmDeviceInit(pInfo);
    return status;
}

ADI_NVM_STATUS NvmWrite(ADI_NVM_INFO *pInfo, uint8_t *pData, uint32_t addr, uint32_t numBytes)
{
    ADI_NVM_STATUS nvmStatus = ADI_NVM_STATUS_SUCCESS;
    int32_t status = 0;
    uint16_t crc;
    uint32_t maxChunkData = ADI_NVM_MAX_SIZE - 6;
    uint16_t headerLen = 0;
    NvmDeviceCmdFormat format;
    if ((numBytes > pInfo->maxNumBytes) || (numBytes == 0))
    {
        return ADI_NVM_STATUS_INVALID_NUM_REGISTERS;
    }
    else
    {
        uint32_t bytesRemain = numBytes;
        uint32_t offset = 0;
        uint32_t chunkSize;
        uint32_t numBytesToSend;
        crc = pInfo->config.pfCalculateCrc(pInfo->config.hUser, (uint8_t *)pData, numBytes);

        // If the numBytes is greater than the maximum chunk size, split the write into chunks and
        // for the last chunk data add CRC bytes.
        while (bytesRemain > 0)
        {
            chunkSize = (bytesRemain > maxChunkData) ? maxChunkData : bytesRemain;
            // formats the data to be sent
            if (pInfo->pfFormat != NULL)
            {
                NvmDeviceCmdFormat *pFormat = &format;
                pFormat->addr = addr;
                pFormat->offset = offset;
                pFormat->cmd = ADI_NVM_WRITE;
                headerLen = pInfo->pfFormat(pFormat, pInfo->txData);
            }
            // Copy chunk data
            memcpy(&pInfo->txData[headerLen], &pData[offset], chunkSize);

            // Number of bytes to send: 4 address/offset + chunkSize
            numBytesToSend = headerLen + chunkSize;

            if (pInfo->isErase == 0)
            {
                // For the last chunk, add CRC bytes
                if (bytesRemain == chunkSize)
                {
                    pInfo->txData[chunkSize + headerLen] = (uint8_t)(crc & 0xFF);
                    pInfo->txData[chunkSize + headerLen + 1] = (uint8_t)((crc >> 8) & 0xFF);
                    numBytesToSend += NUM_CRC_BYTES; // Add CRC bytes
                }
            }

            status = pInfo->config.pfWrite(pInfo->config.hUser, &pInfo->txData[0], numBytesToSend);
            if (status != 0)
            {
                nvmStatus = ADI_NVM_STATUS_COMM_ERROR;
                break;
            }
            offset += chunkSize;
            bytesRemain -= chunkSize;
        }
    }

    return nvmStatus;
}

ADI_NVM_STATUS NvmRead(ADI_NVM_INFO *pInfo, uint32_t addr, uint32_t numBytes, uint8_t *pData)
{
    ADI_NVM_STATUS status = ADI_NVM_STATUS_COMM_ERROR;
    uint32_t numBytesToSend;
    uint32_t bytesRemain;
    uint32_t offset = 0;
    uint32_t maxChunkData = ADI_NVM_MAX_SIZE - 6;
    uint32_t chunkSize;
    uint16_t headerLen = 0;
    uint16_t crc;
    uint16_t expectedCrc;
    NvmDeviceCmdFormat format;
    if ((numBytes > pInfo->maxNumBytes) || (numBytes == 0))
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
            // formats the data to be sent
            if (pInfo->pfFormat != NULL)
            {
                NvmDeviceCmdFormat *pFormat = &format;
                pFormat->addr = addr;
                pFormat->offset = offset;
                pFormat->cmd = ADI_NVM_READ;
                headerLen = pInfo->pfFormat(pFormat, pInfo->txData);
            }
            // For the last chunk, verify CRC.
            numBytesToSend = chunkSize + headerLen;
            if (bytesRemain == chunkSize)
            {
                numBytesToSend += NUM_CRC_BYTES;
            }
            status = pInfo->config.pfRead(pInfo->config.hUser, &pInfo->txData[0], numBytesToSend,
                                          &pInfo->rxData[0]);
            if (status == ADI_NVM_STATUS_SUCCESS)
            {
                if (bytesRemain == chunkSize)
                {
                    memcpy(&pInfo->tempBuffer[0], &pData[offset], chunkSize);
                    memcpy(&pData[offset], &pInfo->rxData[pInfo->rxOffset], chunkSize);
                    crc = pInfo->config.pfCalculateCrc(pInfo->config.hUser, (uint8_t *)pData,
                                                       numBytes);
                    // Extract the received 16-bit CRC (little-endian: low byte first)
                    expectedCrc = ((uint16_t)pInfo->rxData[chunkSize + pInfo->rxOffset]) |
                                  ((uint16_t)pInfo->rxData[chunkSize + pInfo->rxOffset + 1] << 8);
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
                    memcpy(&pData[offset], &pInfo->rxData[pInfo->rxOffset], chunkSize);
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

/**
 * @}
 */
