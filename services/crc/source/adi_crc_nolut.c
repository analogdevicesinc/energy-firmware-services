/******************************************************************************
 Copyright (c) 2022 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file     adi_crc_nolut.c
 * @brief    This file contains the routines for initializing and calculating
 *           software based CRC using non LUT based implementation.
 * @{
 */

/*=============  I N C L U D E S   =============*/
#include "adi_crc.h"
#include <string.h>

/*============= D E F I N E S =============*/

/** Isolate 16th bit*/
#define MSB_SELECT (1u << 15) /* MSB_SELECT & uint16_t isolates the MSB of a 16 bit integer*/

/*============= F U N C T I O N S =============*/
/**
 * @brief Assign CRC configuration parameters.
 */
static ADI_CRC_RESULT CrcSetConfig(ADI_CRC_DATA *pData);

/**
 * @brief Function to calculate CRC without LUT
 * @param hCrc          - Handle to the library instance.
 * @param pData         - Pointer to buffer
 * @param numBytes      - Number of bytes in the buffer
 * @param offset        - offset in the buffer to data to calculate CRC
 * @return - Returns calculated CRC
 */
static uint32_t CRC16Calculate(ADI_CRC_HANDLE hCrc, uint8_t *pBuff, uint32_t numBytes,
                               uint16_t offset);

/*=============  C O D E  =============*/
/**
 * \ref adi_crc_OpenSw
 */
ADI_CRC_RESULT adi_crc_OpenSw(ADI_CRC_HANDLE *phCrc, void *pStateMemory, uint32_t stateMemorySize,
                              void *pTempMemory, uint32_t tempMemorySize)
{
    ADI_CRC_RESULT status = ADI_CRC_RESULT_SUCCESS;
    ADI_CRC_DATA *pCrcData = NULL;
    uint32_t reqSize = sizeof(ADI_CRC_DATA);

    /* Check the given pointers before we set their contents */
    if (phCrc == ((void *)NULL) || (pStateMemory == (void *)NULL))
    {
        status = ADI_CRC_RESULT_NULL_PTR;
    }

    if (status == ADI_CRC_RESULT_SUCCESS)
    {
        /* Set handle to NULL in case of error */
        *phCrc = (ADI_CRC_HANDLE)NULL;

        if (stateMemorySize < reqSize)
        {
            status = ADI_CRC_RESULT_INSUFFICIENT_MEMORY;
        }
        else
        {
            pCrcData = (ADI_CRC_DATA *)pStateMemory;
            memset(pCrcData, 0, sizeof(ADI_CRC_DATA));
        }
    }
    if (status == ADI_CRC_RESULT_SUCCESS)
    {
        *phCrc = (ADI_CRC_HANDLE *)pCrcData;
        CrcSetConfig(pCrcData);
    }

    return status;
}

/**
 * \ref adi_crc_SetConfigSw
 */
ADI_CRC_RESULT adi_crc_SetConfigSw(ADI_CRC_HANDLE hCrc, ADI_CRC_CONFIG *pConfig)
{
    ADI_CRC_RESULT status = ADI_CRC_RESULT_SUCCESS;
    ADI_CRC_DATA *pCrcData = (ADI_CRC_DATA *)hCrc;

    if ((hCrc == NULL) || (pConfig == NULL))
    {
        status = ADI_CRC_RESULT_NULL_PTR;
    }
    else
    {
        memcpy(&pCrcData->crcCfg, pConfig, sizeof(pCrcData->crcCfg));
        CrcSetConfig(pCrcData);
    }

    return status;
}

/**
 * \ref adi_crc_CalculateSw
 */
ADI_CRC_RESULT adi_crc_CalculateSw(ADI_CRC_HANDLE hCrc, uint8_t *pData, uint16_t numBytes,
                                   uint16_t offset)
{
    ADI_CRC_RESULT status = ADI_CRC_RESULT_SUCCESS;
    ADI_CRC_DATA *pCrcData = NULL;

    if (hCrc == NULL)
    {
        status = ADI_CRC_RESULT_NULL_PTR;
    }
    else
    {
        pCrcData = (ADI_CRC_DATA *)hCrc;
        pCrcData->crcValue = pCrcData->pFunc(hCrc, pData, numBytes, offset);
    }

    return status;
}

/**
 * \ref adi_crc_GetCrcSw
 */
ADI_CRC_RESULT adi_crc_GetCrcSw(ADI_CRC_HANDLE hCrc, volatile uint32_t *pData)
{
    ADI_CRC_RESULT status = ADI_CRC_RESULT_SUCCESS;
    ADI_CRC_DATA *pCrcData = NULL;

    if (hCrc == NULL)
    {
        status = ADI_CRC_RESULT_NULL_PTR;
    }
    else
    {
        pCrcData = (ADI_CRC_DATA *)hCrc;
        // Get CRC result
        *pData = pCrcData->crcValue;
    }

    return status;
}

/**
 * \ref CrcSetConfig
 */
static ADI_CRC_RESULT CrcSetConfig(ADI_CRC_DATA *pData)
{
    ADI_CRC_RESULT status = ADI_CRC_RESULT_SUCCESS;

    switch (pData->crcCfg.crcType)
    {
    case ADI_CRC_TYPE_CRC16:
        pData->pFunc = CRC16Calculate;
        break;
    default:
        status = ADI_CRC_RESULT_FAILURE;
    }

    return status;
}

/**
 * \ref CRC16Calculate
 */
static uint32_t CRC16Calculate(ADI_CRC_HANDLE hCrc, uint8_t *pBuff, uint32_t numBytes,
                               uint16_t offset)
{
    ADI_CRC_DATA *pData = (ADI_CRC_DATA *)hCrc;
    uint16_t checksum = (uint16_t)pData->crcCfg.seed;
    uint16_t finalXorValue = (uint16_t)pData->crcCfg.xorOut;
    uint16_t poly = (uint16_t)pData->crcCfg.poly;
    uint32_t i;
    uint16_t j;
    for (i = offset; i < (uint32_t)(offset + numBytes); i++)
    {
        checksum ^= (uint16_t)(pBuff[i] << 8);
        for (j = 8; j > 0; j--)
        {
            checksum = (uint16_t)(checksum << 1) ^ ((checksum & MSB_SELECT) >> 15) * poly;
        }
    }
    return (uint32_t)(checksum ^ finalXorValue);
}

/**
 * @}
 */
