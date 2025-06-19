/******************************************************************************
 Copyright (c) 2024 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file        adi_crc_ccitt16.h
 * @addtogroup CRCAPI
 * @{
 */

#ifndef __ADI_CRC_CCITT16_H__
#define __ADI_CRC_CCITT16_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============= I N C L U D E S =============*/
#include <stdint.h>

/** macro for crc error */
#define ADI_STATUS_CRC_ERROR 0x01

/** CRC Bytes*/
#define ADI_CRC_BYTES_LEN 2

/*======= P U B L I C   P R O T O T Y P E S ========*/

/**
 * @brief Add the crc
 *
 * @param[in]  pData  - pointer to data(error code)
 * @param[in]  numBytes  - number of bytes for calculating crc.
 */
void adi_crc_AddCCITT16(uint8_t *pData, uint16_t numBytes);
/**
 * @brief Verify the crc
 *
 * @param[in]  pData  - pointer to data(error code)
 * @param[in]  numBytes  - number of bytes for verifing crc.
 * @return  0 if it is success.
 */
int32_t adi_crc_VerifyCCITT16(uint8_t *pData, uint16_t numBytes);

/**
 * @brief Calculate crc
 *
 * @param[in]  pBuff  - pointer to data
 * @param[in]  numBytes  - number of bytesfrom pBuff to calculate crc upon
 * @return  CRC
 */
uint32_t adi_crc_CalculateCCITT16(uint8_t *pBuff, uint32_t numBytes);

#ifdef __cplusplus
}
#endif

#endif /* __ADI_CRC_CCITT16_H__  */

/** @} */
