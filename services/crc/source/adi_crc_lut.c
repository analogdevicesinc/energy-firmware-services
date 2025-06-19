/******************************************************************************
 Copyright (c) 2022 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file     adi_crc_lut.c
 * @brief    This file contains the routines for initializing and calculating
 *           software based CRC using LUT based implementation.
 * @{
 */

/*=============  I N C L U D E S   =============*/
#include "adi_crc_sw.h"
#include <stdint.h>
#include <string.h>

/*============= F U N C T I O N S =============*/
/**
 * @brief Assign CRC configuration parameters.
 */
static ADI_CRC_RESULT CrcSetConfig(ADI_CRC_DATA *pData);

/**
 * @brief Function to initialize CRC16 table
 * @param poly - CRC polynomial
 *
 */
static void Crc16InitTable8Bit(ADI_CRC_DATA *pData);

/**
 * @brief Function to initialize CRC8 table
 */
static void Crc8InitTable8Bit(ADI_CRC_DATA *pData);

/**
 * @brief Function to calculate CRC-16
 * @param hCrc          - Handle to the library instance.
 * @param pBuff         - Pointer to buffer
 * @param numBytes      - Number of bytes in the buffer
 * @param offset        - offset in the buffer to data to calculate CRC
 * @return - Returns calculated CRC
 */
static uint32_t Crc16CalcLUT8Bit(ADI_CRC_HANDLE hCrc, uint8_t *pBuff, uint32_t numBytes,
                                 uint16_t offset);

/**
 * @brief Function to calculate CRC-8.
 * @param hCrc          - Handle to the library instance.
 * @param pBuff         - Pointer to buffer
 * @param numBytes      - Number of bytes in the buffer
 * @param offset        - offset in the buffer to data to calculate CRC
 * @return - Returns calculated CRC
 */
static uint32_t Crc8CalcLUT8Bit(ADI_CRC_HANDLE hCrc, uint8_t *pBuff, uint32_t numBytes,
                                uint16_t offset);
/*=============  C O D E  =============*/
/**
 * \ref adi_crc_OpenSw
 */
ADI_CRC_RESULT adi_crc_OpenSw(ADI_CRC_HANDLE *phCrc, void *pStateMemory, uint32_t stateMemorySize)
{
    ADI_CRC_RESULT status = ADI_CRC_RESULT_SUCCESS;
    ADI_CRC_DATA *pCrcData = NULL;
    uint32_t reqSize = sizeof(ADI_CRC_DATA) + LOOK_UP_TABLE_SIZE * 2;

    /* Check the given pointers before we set their contents */
    if ((phCrc == (void *)NULL) || (pStateMemory == (void *)NULL))
    {
        status = ADI_CRC_RESULT_NULL_PTR;
    }

    if (status == ADI_CRC_RESULT_SUCCESS)
    {
        *phCrc = (ADI_CRC_HANDLE)NULL;

        if (stateMemorySize < reqSize)
        {
            status = ADI_CRC_RESULT_INSUFFICIENT_MEMORY;
        }
        else
        {
            pCrcData = (ADI_CRC_DATA *)pStateMemory;
            memset(pCrcData, 0, sizeof(ADI_CRC_DATA));
            pCrcData->pfReset = NULL;
            pCrcData->pfCalc = (ADI_CRC_CALC_API_FUNC)adi_crc_CalculateSw;
            pCrcData->pfConfig = adi_crc_SetConfigSw;
            pCrcData->pfGetCrc = adi_crc_GetCrcSw;

            pCrcData->pLookUpTable = (uint16_t *)pStateMemory;
            // Moving look up table to end of CrcData struct
            pCrcData->pLookUpTable = &pCrcData->pLookUpTable[sizeof(ADI_CRC_DATA) / 2];

            *phCrc = (ADI_CRC_HANDLE *)pCrcData;
        }
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
        status = CrcSetConfig(pCrcData);
    }

    return status;
}

/**
 * \ref adi_crc_CalculateSw
 */
ADI_CRC_RESULT adi_crc_CalculateSw(ADI_CRC_HANDLE hCrc, uint8_t *pData, uint32_t numBytes,
                                   uint16_t offset)
{
    ADI_CRC_RESULT status = ADI_CRC_RESULT_SUCCESS;
    ADI_CRC_DATA *pCrcData = (ADI_CRC_DATA *)hCrc;

    if (hCrc == NULL)
    {
        status = ADI_CRC_RESULT_NULL_PTR;
    }
    else
    {
        pCrcData->crcValue = pCrcData->pFunc(hCrc, pData, numBytes, offset);
        if (pCrcData->crcCfg.pfCallback != NULL)
        {
            pCrcData->crcCfg.pfCallback(pCrcData->crcCfg.pCBData);
        }
    }

    return status;
}

/**
 * \ref adi_crc_GetCrcSw
 */
ADI_CRC_RESULT adi_crc_GetCrcSw(ADI_CRC_HANDLE hCrc, uint32_t *pData)
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
        Crc16InitTable8Bit(pData);
        pData->pFunc = Crc16CalcLUT8Bit;
        break;
    case ADI_CRC_TYPE_CRC8:
        Crc8InitTable8Bit(pData);
        pData->pFunc = Crc8CalcLUT8Bit;
        break;
    case ADI_CRC_TYPE_CRC32:
        status = ADI_CRC_RESULT_FAILURE;
        break;
    }

    return status;
}

/**
 * \ref Crc16InitTable8Bit
 */
static void Crc16InitTable8Bit(ADI_CRC_DATA *pData)
{
    uint16_t checkSum;
    uint32_t i, j;
    uint16_t currPoly;
    uint16_t poly = (uint16_t)pData->crcCfg.poly;
    for (i = 0; i < 256; i++)
    {
        /* Append zeros. */
        checkSum = (uint16_t)(i << 8);
        for (j = 0; j < 8; j++)
        {
            if ((checkSum & (1u << 15)) == (1u << 15))
            {
                currPoly = poly;
            }
            else
            {
                currPoly = 0;
            }
            /* Shift out last bit. */
            checkSum = (uint16_t)(checkSum << 1);
            /* XOR */
            checkSum ^= currPoly;
        }
        /* Add entry to LUT */
        pData->pLookUpTable[i] = checkSum;
    }
}

/**
 * \ref Crc8InitTable8Bit
 */
static void Crc8InitTable8Bit(ADI_CRC_DATA *pData)
{
    uint8_t checkSum;
    uint32_t i, j;
    uint8_t currPoly;
    uint8_t poly = (uint8_t)pData->crcCfg.poly;
    for (i = 0; i < 256; i++)
    {
        checkSum = (uint8_t)i;
        for (j = 0; j < 8; j++)
        {
            if ((checkSum & (1u << 7)) == (1u << 7))
            {
                currPoly = poly;
            }
            else
            {
                currPoly = 0;
            }
            /* Shift out last bit. */
            checkSum = (uint8_t)(checkSum << 1);
            /* XOR */
            checkSum ^= currPoly;
        }
        /* Add entry to LUT */
        pData->pLookUpTable[i] = checkSum;
    }
}

/**
 * @brief CRC configuration using a 512 bytes LUT with 8 bit inputs.
 *        The function calculates the 512 bytes (8 bit word) LUT based CRC
 * value.
 */
static uint32_t Crc16CalcLUT8Bit(ADI_CRC_HANDLE hCrc, uint8_t *pBuff, uint32_t numBytes,
                                 uint16_t offset)
{
    ADI_CRC_DATA *pData = (ADI_CRC_DATA *)hCrc;
    uint16_t checksum = (uint16_t)pData->crcCfg.seed;
    uint16_t finalXorValue = (uint16_t)pData->crcCfg.xorOut;
    uint16_t byte;
    uint32_t i;

    for (i = offset; i < (uint32_t)(offset + numBytes); i++)
    {
        byte = (uint16_t)(pBuff[i] ^ (checksum >> 8));
        checksum = (uint16_t)(pData->pLookUpTable[byte] ^ ((uint16_t)(checksum << 8)));
    }

    return (uint32_t)(checksum ^ finalXorValue);
}

/**
 * \ref Crc8CalcLUT8Bit
 */
static uint32_t Crc8CalcLUT8Bit(ADI_CRC_HANDLE hCrc, uint8_t *pBuff, uint32_t numBytes,
                                uint16_t offset)
{
    ADI_CRC_DATA *pData = (ADI_CRC_DATA *)hCrc;
    uint8_t checksum = (uint8_t)pData->crcCfg.seed;
    uint8_t finalXorValue = (uint8_t)pData->crcCfg.xorOut;
    uint8_t byte;
    uint32_t i;

    for (i = offset; i < (uint32_t)(offset + numBytes); i++)
    {
        byte = (uint8_t)(pBuff[i] ^ (checksum));
        checksum = (uint8_t)(pData->pLookUpTable[byte] ^ ((uint16_t)(checksum << 8)));
    }
    return (uint32_t)(checksum ^ finalXorValue);
}

/**
 * @}
 */
