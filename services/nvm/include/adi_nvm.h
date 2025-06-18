/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @defgroup    ADI_NVM NVM Service
 * @ingroup     SERVICES
 * @brief       APIs and definitions for the Non-Volatile Memory (NVM) service.
 * @details
 * ## Overview
 * The NVM service uses a handle and configuration structure to access non-volatile memory devices.
 * It provides APIs for reading, writing, and erasing data in NVM, with built-in CRC verification to
 * ensure data integrity. Current support includes the MB85RS FRAM series and flash memory for the
 * MAX32670 controller.
 *
 * Typical API usage:
 * 1. **Create the NVM Instance**: Call adi_nvm_Create() to create an NVM service instance.
 * 2. **Configure the NVM**: Populate an ADI_NVM_CONFIG structure with required function pointers.
 * 3. **Initialize the NVM**: Call adi_nvm_Init() to configure the service.
 * 4. Use APIs to read, write and erase the data of NVM.
 * @{
 */

#ifndef __ADI_NVM_H__
#define __ADI_NVM_H__

/*=============  I N C L U D E S   =============*/

#include "adi_nvm_status.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*=============  D E F I N E S  =============*/

/** @defgroup    NVMINIT Handle and Configurations
 * @brief This section describes the handle and configuration for the NVM Service.
 *
 * The handle (`ADI_NVM_HANDLE`) is used to keep track of the NVM service instance and must be
 * passed to all NVM API functions. The configuration (`ADI_NVM_CONFIG`) allows you to set up how
 * the NVM service communicates with the hardware by providing function pointers. Make sure the
 * function pointers are initialized in the application and match the expected types to avoid build
 * warnings.
 * @{
 */

/** A device handle used in all API functions to identify the instance.
 *  It is obtained from the Create API. */
typedef void *ADI_NVM_HANDLE;

/** Function pointer definition for transmission */
typedef int32_t (*ADI_NVM_TX_FUNC)(void *, uint8_t *, uint32_t);
/** Function pointer definition for receive */
typedef int32_t (*ADI_NVM_TXRX_FUNC)(void *, uint8_t *, uint32_t, uint32_t, uint8_t *);

/**
 * NVM configurations
 */
typedef struct
{
    /** Function Pointer to start transmission */
    ADI_NVM_TX_FUNC pfStartTx;
    /** Function Pointer to start transmission and get response */
    ADI_NVM_TXRX_FUNC pfStartTxRx;
    /** user handle*/
    void *hUser;

} ADI_NVM_CONFIG;

/**
 * Structure to store the NVM info. This structure is useful when the data to be sent in blocks.
 */
typedef struct
{
    /** Pointer to data */
    uint8_t *pData;
    /** data to be sent or receive for every incremented address */
    uint32_t incrAddress;
    /** Number of bytes to send/receive */
    uint32_t numBytes;
    /** Number of blocks to be sent / receive */
    int32_t numBlocks;
} ADI_NVM_BLOCK_DATA;

/** @} */

/** @defgroup   NVMAPI Service API
 * @brief This section covers API functions required to read, write to FRAM.
 * All the APIs return enumeration codes in #ADI_NVM_STATUS. Refer to #ADI_NVM_STATUS for
 * detailed documentation on return codes
 * @{
 */

/** Number of bytes required to store the CRC */
#define ADI_NVM_NUM_BYTES_CRC 2

/**
 * @brief Function to create instance for NVM Service.
 * Assigns memory and sets up the internal structures of the service.
 * @param[in] phNvm      -   Pointer to a location where the handle to
 * the service is written. This handle is required in all other service APIs.
 * @param[in]  pStateMemory   -       The pointer to the memory for the
 * service.
 * This pointer must be 32-bit aligned. This memory must be persistent
 * in the application. So it is recommended that it is not allocated in the
 * stack.
 * @param[in]  stateMemorySize -  Size of the memory pointed by pStateMemory
 * This must be at least #ADI_NVM_STATE_MEM_NUM_BYTES bytes.
 * #adi_nvm_SetConfig can also be used to set the configuration at a later point.
 * @return  #ADI_NVM_STATUS_SUCCESS\n
 * #ADI_NVM_STATUS_NULL_PTR \n
 * #ADI_NVM_STATUS_INSUFFICIENT_STATE_MEMORY \n
 */
ADI_NVM_STATUS adi_nvm_Create(ADI_NVM_HANDLE *phNvm, void *pStateMemory, uint32_t stateMemorySize);

/**
 * @brief Initializes NVM Service.
 * Before calling this API, its recommended to call #adi_nvm_Create to create the instance and also
 * to populate configurations #ADI_NVM_CONFIG from application accordingly.
 * @param[in] hNvm 		- NVM service handle
 * @param[in] pConfig - Pointer to NVM service configuration.
 * @return  #ADI_NVM_STATUS_SUCCESS\n
 * #ADI_NVM_STATUS_NULL_PTR \n
 * #ADI_NVM_STATUS_INVALID_PRODUCT_ID \n
 * #ADI_NVM_STATUS_COMM_ERROR
 */
ADI_NVM_STATUS adi_nvm_Init(ADI_NVM_HANDLE hNvm, ADI_NVM_CONFIG *pConfig);

/**
 * @brief Sets Configuration to NVM.
 * Before calling this API, its recommended to call #adi_nvm_Create to create the instance and also
 * to populate configurations #ADI_NVM_CONFIG from application accordingly.
 * @param[in] hNvm 		- NVM service handle
 * @param[in] pConfig - Pointer to NVM service configuration.
 * @return  #ADI_NVM_STATUS_SUCCESS\n
 * #ADI_NVM_STATUS_NULL_PTR \n
 */
ADI_NVM_STATUS adi_nvm_SetConfig(ADI_NVM_HANDLE hNvm, ADI_NVM_CONFIG *pConfig);

/**
 *  @brief Write data into NVM with CRC.
 *
 * @param[in] hNvm     -   	NVM handle
 * @param[in]  pData    - 	Pointer to data
 * @param[in]  addr     -	  address to where data to be written
 * @param[in]  numBytes  -  number of bytes to write
 *
 * @return  #ADI_NVM_STATUS_SUCCESS\n
 * #ADI_NVM_STATUS_NULL_PTR \n
 * #ADI_NVM_STATUS_INVALID_MEMORY \n
 * #ADI_NVM_STATUS_COMM_ERROR
 */
ADI_NVM_STATUS adi_nvm_Write(ADI_NVM_HANDLE hNvm, uint8_t *pData, uint32_t addr, uint32_t numBytes);

/**
 *  @brief Writes a block of data to the NVM with CRC in a continuous memory region.
 *
 * @param[in] hNvm         - NVM handle
 * @param[in] pBlockData   - Pointer to the block data structure containing the data to be written
 * @param[in] addr         - Starting address in the NVM where the data will be written
 *
 * @return  #ADI_NVM_STATUS_SUCCESS\n
 * #ADI_NVM_STATUS_NULL_PTR \n
 * #ADI_NVM_STATUS_INVALID_MEMORY \n
 * #ADI_NVM_STATUS_COMM_ERROR
 */
ADI_NVM_STATUS adi_nvm_WriteBlock(ADI_NVM_HANDLE hNvm, ADI_NVM_BLOCK_DATA *pBlockData,
                                  uint32_t addr);
/**
 *  @brief Read data + CRC from NVM and verifies CRC.
 *
 * @param[in] hNvm       - 	NVM handle
 * @param[in]  addr     -	  address to where data to be written
 * @param[in] 	numBytes -   number of bytes to write
 * @param[out]  pData     -	Pointer to read data from NVM
 *
 * @return  #ADI_NVM_STATUS_SUCCESS\n
 * #ADI_NVM_STATUS_NULL_PTR \n
 * #ADI_NVM_STATUS_INVALID_MEMORY \n
 * #ADI_NVM_STATUS_COMM_ERROR \n
 * #ADI_NVM_STATUS_CRC_MISMATCH
 */
ADI_NVM_STATUS adi_nvm_Read(ADI_NVM_HANDLE hNvm, uint32_t addr, uint32_t numBytes, uint8_t *pData);

/**
 *  @brief Reads a block of data in a continuous memory region from the NVM and verifies CRC.
 *
 * @param[in] hNvm       - 	NVM handle
 * @param[in] addr       -	Start address from where the data is to be read
 * @param[out] pBlockData - Pointer to the block data structure where the read data will be stored
 *
 * @return  #ADI_NVM_STATUS_SUCCESS\n
 * #ADI_NVM_STATUS_NULL_PTR \n
 * #ADI_NVM_STATUS_INVALID_MEMORY \n
 * #ADI_NVM_STATUS_COMM_ERROR \n
 * #ADI_NVM_STATUS_CRC_MISMATCH
 */
ADI_NVM_STATUS adi_nvm_ReadBlock(ADI_NVM_HANDLE hNvm, uint32_t addr,
                                 ADI_NVM_BLOCK_DATA *pBlockData);

/**
 *  @brief Erases contents in the FRAM by corrupting CRC.
 *
 * @param[in] hNvm      -  	NVM handle
 * @param[in]  addr     -	  address to where data to be written
 * @param[in]  numBytes  -  number of bytes to write
 *
 * @return  #ADI_NVM_STATUS_SUCCESS\n
 * #ADI_NVM_STATUS_NULL_PTR \n
 * #ADI_NVM_STATUS_INVALID_MEMORY \n
 * #ADI_NVM_STATUS_COMM_ERROR
 */
ADI_NVM_STATUS adi_nvm_Erase(ADI_NVM_HANDLE hNvm, uint32_t addr, uint32_t numBytes);

/**
 *  @brief Erases block of data in the FRAM by corrupting CRC. This API finds the CRC position based
 * on the start address given and the numBlocks and numBytes fields in the pBlockData.
 *
 * @param[in] hNvm        -	NVM handle
 * @param[in]  addr     	-  start address from where data to be erased
 * @param[out]  pBlockData   - pointer to Block Data. This API uses only the numBlocks and numBytes
 * fields to clear the CRC.
 *
 * @return  #ADI_NVM_STATUS_SUCCESS\n
 * #ADI_NVM_STATUS_NULL_PTR \n
 * #ADI_NVM_STATUS_INVALID_MEMORY \n
 * #ADI_NVM_STATUS_COMM_ERROR
 */
ADI_NVM_STATUS adi_nvm_EraseBlock(ADI_NVM_HANDLE hNvm, uint32_t addr,
                                  ADI_NVM_BLOCK_DATA *pBlockData);

/**
 *  @brief TX Callback.
 * @param[in] hNvm    -    	NVM handle
 *
 * @return  #ADI_NVM_STATUS_SUCCESS\n
 * #ADI_NVM_STATUS_NULL_PTR \n
 */
ADI_NVM_STATUS adi_nvm_TxCallBack(ADI_NVM_HANDLE hNvm);

/**
 *  @brief RX Callback.
 * @param[in] hNvm     -   	NVM handle
 *
 * @return  #ADI_NVM_STATUS_SUCCESS\n
 * #ADI_NVM_STATUS_NULL_PTR \n
 */
ADI_NVM_STATUS adi_nvm_RxCallBack(ADI_NVM_HANDLE hNvm);

/**
 *  @brief Closes NVM service.
 *
 */
ADI_NVM_STATUS adi_nvm_Close(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __ADI_NVM_H__ */

/**
 * @}
 */
