/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file        adi_crc_sw.h
 * @addtogroup CRCAPI
 * @{
 */

#ifndef __ADI_CRC_SW_H__
#define __ADI_CRC_SW_H__

/*============= I N C L U D E S =============*/
#include "adi_crc.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============= P U B L I C   P R O T O T Y P E S =============*/
/**
 * @brief Function to initialize software based CRC.
 * Assign memory and sets up the internal structures and configuration
 * of the library.
 *
 * @param[out] phCrc            - Pointer to a location where the
 *                                handle to the library is written.
 *                                This handle is required in all other
 *                                library APIs.
 * @param[in]  pStateMemory     - The pointer to the memory for the library.
 *                                This pointer must be 32-bit aligned. This
 *                                memory must be persistent in the application
 *                                so it is recommended that it is not allocated
 *                                in the stack.
 * @param[in]  stateMemorySize	- Size of the memory pointed by pStateMemory
 *                                This must be at least
 *                                ADI_CRC_STATE_MEM_NUM_BYTES bytes.
 * @return  One of the codes documented in #ADI_CRC_RESULT. Refer to
 * #ADI_CRC_RESULT documentation for details.
 *
 */
ADI_CRC_RESULT adi_crc_OpenSw(ADI_CRC_HANDLE *phCrc, void *pStateMemory, uint32_t stateMemorySize);

/**
 * @brief Function to assign CRC configuration parameters.
 * This API can be called whenever the user wants to reconfigure
 * library. This API will not reset the internal states of the
 * library.
 *
 * @param[in]  hCrc         - Handle to the library instance.
 * @param[in]  pConfig      - The pointer to the configuration structure
 * @return  One of the return codes documented in #ADI_CRC_RESULT.
 *          Refer to #ADI_CRC_RESULT for details.
 */
ADI_CRC_RESULT adi_crc_SetConfigSw(ADI_CRC_HANDLE hCrc, ADI_CRC_CONFIG *pConfig);

/**
 * @brief Function to calculate the CRC.
 * @param[in] hCrc          - Handle to the library instance.
 * @param[in] pData         - Pointer to buffer.
 * @param[in] numBytes      - Number of bytes in the buffer.
 * @param[in] offset        - Offset in the buffer to data to calculate CRC.
 * @return  One of the return codes documented in #ADI_CRC_RESULT.
 *          Refer to #ADI_CRC_RESULT for details.
 */
ADI_CRC_RESULT adi_crc_CalculateSw(ADI_CRC_HANDLE hCrc, uint8_t *pData, uint32_t numBytes,
                                   uint16_t offset);

/**
 * @brief Function to get the computed CRC.
 * @param[in]  hCrc          - Handle to CRC instance.
 * @param[out] pData         - Pointer to buffer.
 * return status whether data is ready or not.
 */
ADI_CRC_RESULT adi_crc_GetCrcSw(ADI_CRC_HANDLE hCrc, uint32_t *pData);

#ifdef __cplusplus
}
#endif

#endif /* __ADI_CRC_SW_H__ */

/** @} */
