/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file    nvm_device.h
 * @brief   Configuration of non volatile memory device
 * @addtogroup  ADI_NVM
 */

#ifndef __NVM_DEVICE_H__
#define __NVM_DEVICE_H__

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#include "adi_nvm_private.h"
#include "adi_nvm_status.h"
#include "nvm_device.h"
#include <stdint.h>

/**
 *  @brief Format the non volatile memory device
 *
 */
void NVMFormat(ADI_NVM_INFO *pInfo);

/**
 * @brief Initializes NVM Service.
 * @param[in] pInfo 		- pointer to NVM data
 * @return  #ADI_NVM_STATUS_SUCCESS\n
 * #ADI_NVM_STATUS_INVALID_PRODUCT_ID \n
 * #ADI_NVM_STATUS_COMM_ERROR
 */
ADI_NVM_STATUS NvmInit(ADI_NVM_INFO *pInfo);

/**
 *  @brief Performs a write operation in the non volatile memory device
 *
 *  @param[in] pInfo 		- pointer to NVM data
 * 	@param[in]  pData     	Pointer to data
 * 	@param[in]  addr     	  address to where data to be written
 * 	@param[in]  numBytes    number of bytes to write
 *
 * @return  #ADI_NVM_STATUS_SUCCESS\n
 * #ADI_NVM_STATUS_COMM_ERROR
 */
ADI_NVM_STATUS NvmWrite(ADI_NVM_INFO *pInfo, uint8_t *pData, uint32_t addr, uint32_t numBytes);

/**
 *  @brief Performs a read operation in the non volatile memory device
 *
 * @param[in] pInfo 		- pointer to NVM data
 * @param[in]  addr     	  address to where data to be written
 * @param[in] 	numBytes    number of bytes to write
 * @param[out]  pData     	Pointer to read data from NVM
 *
 * @return  #ADI_NVM_STATUS_SUCCESS\n
 * #ADI_NVM_STATUS_COMM_ERROR \n
 * #ADI_NVM_STATUS_CRC_MISMATCH
 */
ADI_NVM_STATUS NvmRead(ADI_NVM_INFO *pInfo, uint32_t addr, uint32_t numBytes, uint8_t *pData);

#ifdef __cplusplus
}
#endif

#endif /* __NVM_DEVICE_H__ */

/*
** EOF
*/
