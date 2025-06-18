/******************************************************************************
 Copyright (c) 2024 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file     adi_crc.c
 * @brief    This file contains the implementation of setting CRC configuration, calculating CRC,
 *           getting CRC value, and resetting the CRC module.
 * @{
 */

/*=============  I N C L U D E S   =============*/
#include "adi_crc.h"
#include <stdint.h>
#include <stdlib.h>

/*=============  C O D E  =============*/

ADI_CRC_RESULT adi_crc_SetConfig(ADI_CRC_HANDLE hCrc, ADI_CRC_CONFIG *pConfig)
{
    ADI_CRC_RESULT status = ADI_CRC_RESULT_SUCCESS;
    ADI_CRC_DATA *pCrcData = (ADI_CRC_DATA *)hCrc;

    if (hCrc == NULL)
    {
        status = ADI_CRC_RESULT_NULL_PTR;
    }
    else
    {
        if (pCrcData->pfConfig != NULL)
        {
            status = pCrcData->pfConfig(hCrc, pConfig);
        }
        else
        {
            status = ADI_CRC_RESULT_NULL_PTR;
        }
    }
    return status;
}

ADI_CRC_RESULT adi_crc_Calculate(ADI_CRC_HANDLE hCrc, uint8_t *pData, uint16_t numBytes,
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
        if (pCrcData->pfCalc != NULL)
        {
            status = pCrcData->pfCalc(hCrc, pData, numBytes, offset);
        }
        else
        {
            status = ADI_CRC_RESULT_NULL_PTR;
        }
    }
    return status;
}

ADI_CRC_RESULT adi_crc_GetCrc(ADI_CRC_HANDLE hCrc, uint32_t *pData)
{
    ADI_CRC_RESULT status = ADI_CRC_RESULT_SUCCESS;
    ADI_CRC_DATA *pCrcData = (ADI_CRC_DATA *)hCrc;
    if (hCrc == NULL)
    {
        status = ADI_CRC_RESULT_NULL_PTR;
    }
    else
    {
        if (pCrcData->pfGetCrc != NULL)
        {
            status = pCrcData->pfGetCrc(hCrc, pData);
        }
        else
        {
            status = ADI_CRC_RESULT_NULL_PTR;
        }
    }
    return status;
}

void adi_crc_Reset(ADI_CRC_HANDLE hCrc)
{
    ADI_CRC_DATA *pCrcData = (ADI_CRC_DATA *)hCrc;

    if (hCrc != NULL)
    {
        if (pCrcData->pfReset != NULL)
        {
            pCrcData->pfReset(hCrc);
        }
    }
}

/**
 * @}
 */
