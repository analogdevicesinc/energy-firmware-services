/******************************************************************************
 Copyright (c) 2024 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file        adi_nvm_private.h
 * @brief       Structures to hold NVM Service Data and Configurations
 * @{
 */

#ifndef __ADI_NVM_PRIVATE_H__
#define __ADI_NVM_PRIVATE_H__

/*=============  I N C L U D E S   =============*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "adi_nvm.h"
#include "adi_nvm_status.h"

/** Maximum size of buffer to store */
#define ADI_NVM_MAX_SIZE 512

/** Function pointer type for formatting data to be sent to the NVM device (internal use). */
typedef uint32_t (*NvmFormatFunc)(void *, uint8_t *);
/** Function pointer type for the internal erase function called by the erase API. */
typedef ADI_NVM_STATUS (*NvmEraseFunc)(void *, uint32_t);

/**
 * NVM info
 */
typedef struct
{
    /** NVM configuration*/
    ADI_NVM_CONFIG config;
    /** Function pointer to the device-specific internal function to be called. */
    NvmFormatFunc pfFormat;
    /** Function pointer to the device-specific internal function to be called. */
    NvmEraseFunc pfEraseFn;
    /** Maximum number of bytes the function can write to NVM device. */
    uint32_t maxNumBytes;
    /** Tx Data to be sent */
    uint8_t txData[ADI_NVM_MAX_SIZE];
    /** Rx data received */
    uint8_t rxData[ADI_NVM_MAX_SIZE];
    /** temporary buffer. */
    uint8_t tempBuffer[ADI_NVM_MAX_SIZE];
    /** buffer to be filled with 0xFF to erase contents in FRAM */
    uint8_t eraseData[NUM_CRC_BYTES];
    /** product id of device */
    uint32_t productId;
    /** command to indicate the erase */
    uint8_t isErase;
    /** Offset in the rxData buffer where the data starts */
    uint16_t rxOffset;
} ADI_NVM_INFO;

#ifdef __cplusplus
}
#endif

#endif /* __ADI_NVM_PRIVATE_H__ */

/**
 * @}
 */
