/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @defgroup    ADI_CRC CRC Service
 * @ingroup     SERVICES
 * @brief Definitions needed for CRC service
 * @details
 * ## Overview
 * The CRC service uses a handle and configuration structure to calculate 8-bit, 16-bit, and 32-bit
 * CRCs. It provides APIs for software LUT-based, non-LUT-based, and hardware-based CRC
 * calculations.
 *
 * Typical API usage:
 * 1. **Create the CRC Instance**: Call adi_crc_OpenSw() or adi_crc_OpenHw() depending on whether
 * software or hardware CRC is required.
 * 2. **Configure the NVM**: Populate an ADI_CRC_CONFIG structure with the required function
 * pointers.
 * 3. Use the provided APIs to calculate the CRC.
 * @{
 */

#ifndef __ADI_CRC_H__
#define __ADI_CRC_H__

/*============= I N C L U D E S =============*/
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*=============  D E F I N I T I O N S  =============*/

/** @defgroup CRCSTATUS Return Codes
 * @brief Enums of return codes for CRC APIs. All APIs return one of these codes.
 * If multiple errors are there, the API will return the codes corresponding to
 * first encountered error. The user should run and check the return codes after
 * fixing the error to know about other errors.
 * @{
 */

/**
 * Enums of return codes for CRC APIs. All APIs return one of these codes.
 * If multiple errors are there, the API will return the codes corresponding to
 * first encountered error. The user should run and check the return codes after
 * fixing the error to know about other errors.
 */
typedef enum
{
    /** No error, API succeeded. */
    ADI_CRC_RESULT_SUCCESS = 0u,
    /** Failure in CRC initialization. */
    ADI_CRC_RESULT_INIT_FAILURE,
    /** One of the pointers passed to the API is NULL. */
    ADI_CRC_RESULT_NULL_PTR,
    /** State memory provided to the Open API is less than required memory
     * for the configuration.*/
    ADI_CRC_RESULT_INSUFFICIENT_MEMORY,
    /** CRC result not ready. */
    ADI_CRC_RESULT_NOT_READY,
    /** CRC result not ready. */
    ADI_CRC_RESULT_RUN_TIME_ERROR,
    /** CRC result not ready. */
    ADI_CRC_RESULT_FAILURE,
} ADI_CRC_RESULT;

/** @} */

/** @defgroup CRCINIT Handle and Configurations
 * @brief This section contains the configurations of the CRC Service.
 * The function pointers must be initialized from the application; they should be of the same type
 * to avoid warnings during the build process.
 * @{
 */

/**
 * @brief Look up table buffer size.
 */
#define LOOK_UP_TABLE_SIZE 256
/** State memory required in bytes for the library. Allocate a buffer aligned to
 * 32 bit boundary */
#define ADI_CRC_HW_STATE_MEM_NUM_BYTES sizeof(ADI_CRC_DATA)
/** State memory required in bytes for the library. Allocate a buffer aligned to
 *  32 bit boundary */
#define ADI_CRC_SW_STATE_MEM_NUM_BYTES (sizeof(ADI_CRC_DATA) + LOOK_UP_TABLE_SIZE * 2)

/** A device handle used in all API functions to identify the instance.
 *  It is obtained from the open API. */
typedef void *ADI_CRC_HANDLE;
/** callback of CRC service. */
typedef void (*ADI_CRC_CALLBACK_FUNC)(void *);

/**
 * Enums of CRC type.
 */
typedef enum
{
    /** CRC-8. */
    ADI_CRC_TYPE_CRC8 = 0u,
    /** CRC-16. */
    ADI_CRC_TYPE_CRC16,
    /** CRC-32. */
    ADI_CRC_TYPE_CRC32,

} ADI_CRC_TYPE;

/**
 * Function pointer to CRC implementation.
 */
typedef uint32_t (*ADI_CRC_FUNC)(ADI_CRC_HANDLE hCrc, uint8_t *pBuff, uint32_t numBytes,
                                 uint16_t offset);

/**
 * Structure for CRC configuration parameters.
 */
typedef struct
{
    /** Byte Swap CRC Data Input. */
    bool reversed;
    /** Most Significant Bit Order. */
    bool bigEndian;
    /** Seed for CRC. */
    uint32_t seed;
    /** CRC polynomial. */
    uint32_t poly;
    /** Final XOR value. */
    uint32_t xorOut;
    /** CRC Type. */
    ADI_CRC_TYPE crcType;
    /** Pointer to CRC callback function */
    ADI_CRC_CALLBACK_FUNC pfCallback;
    /** data returned in callback function */
    void *pCBData;
} ADI_CRC_CONFIG;

/** CRC Calculation function */
typedef uint32_t (*ADI_CRC_CALC_FUNC)(ADI_CRC_HANDLE hCrc, uint8_t *pData, uint32_t numBytes,
                                      uint16_t offset);
/** CRC Calculation function */
typedef ADI_CRC_RESULT (*ADI_CRC_CALC_API_FUNC)(ADI_CRC_HANDLE hCrc, uint8_t *pData,
                                                uint32_t numBytes, uint16_t offset);
/** CRC config function */
typedef ADI_CRC_RESULT (*ADI_CRC_CONFIG_FUNC)(ADI_CRC_HANDLE hCrc, ADI_CRC_CONFIG *pConfig);
/** CRC config close function */
typedef ADI_CRC_RESULT (*ADI_CRC_CLOSE_FUNC)(ADI_CRC_HANDLE hCrc);
/** CRC config close function */
typedef ADI_CRC_RESULT (*ADI_CRC_GET_FUNC)(ADI_CRC_HANDLE hCrc, uint32_t *pData);

/**
 * Structure for CRC data.
 */
typedef struct
{
    /** Input data configuration */
    ADI_CRC_CONFIG crcCfg;
    /** Pointer to CRC look up table */
    uint16_t *pLookUpTable;
    /** CRC calculation function */
    ADI_CRC_CALC_API_FUNC pfCalc;
    /**  CRC Configuration function*/
    ADI_CRC_CONFIG_FUNC pfConfig;
    /**  CRC Configuration function*/
    ADI_CRC_GET_FUNC pfGetCrc;
    /** Close Function */
    ADI_CRC_CLOSE_FUNC pfReset;
    /** CRC value of last call to calculate */
    uint32_t crcValue;
    /** Core CRC calculation function*/
    ADI_CRC_CALC_FUNC pFunc;
} ADI_CRC_DATA;

/** @} */

/** @defgroup CRCAPI Service API
 * @brief This section covers API functions required to initialize the CLI Service, scan the
 * commands received from the user, call the appropriate application functions. All the APIs return
 * enumeration codes in #ADI_CRC_RESULT. Refer to #ADI_CRC_RESULT for detailed documentation on
 * return codes.
 * @{
 */

/**
 * @brief Function to assign CRC configuration parameters.
 * This API can be called whenever the user wants to reconfigure
 * library. This API will not reset the internal states of the
 * library.
 *
 * @param[in]  hCrc         - Handle to the library instance.
 * @param[in]  pConfig      - The pointer to the configuration structure
 * @return  One of the return codes documented in #ADI_CRC_RESULT.
 *  Refer to #ADI_CRC_RESULT for details.
 */
ADI_CRC_RESULT adi_crc_SetConfig(ADI_CRC_HANDLE hCrc, ADI_CRC_CONFIG *pConfig);

/**
 * @brief Function to calculate CRC .
 * @param hCrc          - Handle to CRC instance.
 * @param pData         - Pointer to buffer.
 * @param numBytes      - Number of bytes in the buffer.
 * @param offset        - Buffer offset.
 * return status of calculation.
 */
ADI_CRC_RESULT adi_crc_Calculate(ADI_CRC_HANDLE hCrc, uint8_t *pData, uint16_t numBytes,
                                 uint16_t offset);

/**
 * @brief Function to get the computed CRC.
 * @param hCrc          - Handle to CRC instance.
 * @param pData         - Pointer to buffer.
 * return status whether data is ready or not.
 */
ADI_CRC_RESULT adi_crc_GetCrc(ADI_CRC_HANDLE hCrc, uint32_t *pData);

/**
 * @brief Function to reset CRC.
 * @param hCrc          - Handle to CRC instance.
 */
void adi_crc_Reset(ADI_CRC_HANDLE hCrc);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __ADI_CRC_H__ */

/** @} */
