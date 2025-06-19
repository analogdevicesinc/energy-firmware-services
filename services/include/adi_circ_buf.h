/******************************************************************************
 Copyright (c) 2024 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file   adi_circ_buf.h
 * @addtogroup ADI_UCOMM
 * @brief  Circular buffer routines
 * @{
 */

#ifndef __ADI_CIRC_BUF_H__
#define __ADI_CIRC_BUF_H__

/*============= I N C L U D E S =============*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============= D A T A  T Y P E S =============*/

/**
 * Circular buffer instance structure
 */
typedef struct __ADI_CIRC_BUF
{

    uint8_t *pBase;                /**< Base pointer */
    uint32_t nSize;                /**< Size of the buffer in bytes */
    volatile uint32_t nReadIndex;  /**< Read index */
    volatile uint32_t nWriteIndex; /**< Write index */

} ADI_CIRC_BUF;

/*======= INLINE FUNCTIONS  ========*/

/**
 * @brief Gets the number of data bytes present in the circular buffer
 * @param[in] pCircBuf - Pointer to circular buffer structure
 * @return nDataAvailable - Number of bytes available in buffer
 */
static inline int32_t ADICircBufGetNumBytesAvailable(volatile ADI_CIRC_BUF *pCircBuf)
{
    uint32_t nReadOffset = pCircBuf->nReadIndex;
    uint32_t nWriteOffset = pCircBuf->nWriteIndex;
    uint32_t nSize = pCircBuf->nSize;
    int32_t nDataAvailable;

    nDataAvailable = (int32_t)(nWriteOffset - nReadOffset);

    if (nDataAvailable < 0)
    {
        nDataAvailable += nSize;
    }

    return nDataAvailable;
}

/**
 * @brief Gets the free space available in the circular buffer
 * @param[in] pCircBuf - Pointer to circular buffer structure
 * @return nSpaceAvailable - space bytes available in buffer
 */
static inline int32_t ADICircBufGetSpaceAvailable(volatile ADI_CIRC_BUF *pCircBuf)
{
    uint32_t nReadOffset = pCircBuf->nReadIndex;
    uint32_t nWriteOffset = pCircBuf->nWriteIndex;
    uint32_t nSize = pCircBuf->nSize;
    int32_t nSpaceAvailable;

    nSpaceAvailable = (int32_t)(nReadOffset - nWriteOffset - 4u);

    if (nSpaceAvailable < 0)
    {
        nSpaceAvailable += nSize;
    }

    return nSpaceAvailable;
}

/**
 * @brief Reads data from buffer and updates read index
 * @param[in] pCircBuf - Pointer to circular buffer structure
 * @param[out] pDstAddr - Pointer to store the required bytes
 * @param[in] nReqBytes - Number of required bytes
 * @return 0 -  Success , 1 - Not enough bytes in buffer
 */
static inline int32_t ADICircBufRead(volatile ADI_CIRC_BUF *pCircBuf, volatile uint8_t *pDstAddr,
                                     uint32_t nReqBytes)
{
    int32_t nResult = 0;
    uint32_t i = 0;

    uint32_t nOffset = pCircBuf->nReadIndex;
    uint32_t nSize = pCircBuf->nSize;
    int32_t nNumBytesAvailable = ADICircBufGetNumBytesAvailable(pCircBuf);
#ifdef CIRC_BUFF_MEM_32_ALLIGN
    uint32_t *pBase = (uint32_t *)pCircBuf->pBase;
    uint32_t *pDst = (uint32_t *)pDstAddr;
    nReqBytes = nReqBytes / 4u;
#else
    volatile uint8_t *pBase = pCircBuf->pBase;
    volatile uint8_t *pDst = pDstAddr;
#endif

    if ((uint32_t)nNumBytesAvailable >= nReqBytes)
    {

        for (i = 0; i < nReqBytes; i++)
        {
            if (nOffset >= nSize)
            {
                nOffset -= nSize;
            }

            pDst[i] = pBase[nOffset];

            nOffset++;
        }
    }
    else
    {
        nResult = 1;
    }

    pCircBuf->nReadIndex = nOffset;

    return nResult;
}

/**
 * @brief Writes data to buffer and updates write index
 * @param[in] pCircBuf - Pointer to circular buffer structure
 * @param[in] pSrcAddr - Pointer to the bytes
 * @param[in] nNumBytes - Number of bytes to write
 * @return 0 -  Success , 1 - Not enough space in buffer
 */
static inline int32_t ADICircBufWrite(volatile ADI_CIRC_BUF *pCircBuf,
                                      const volatile uint8_t *pSrcAddr, uint32_t nNumBytes)
{
    int32_t nResult = 0;
    uint32_t i = 0;

    uint32_t nSpaceAvailable = (uint32_t)ADICircBufGetSpaceAvailable(pCircBuf);
    uint32_t nOffset = pCircBuf->nWriteIndex;
    uint32_t nSize = pCircBuf->nSize;
#ifdef CIRC_BUFF_MEM_32_ALLIGN
    uint32_t *pBase = (uint32_t *)pCircBuf->pBase;
    uint32_t *pSrc = (uint32_t *)pSrcAddr;
    nNumBytes = nNumBytes / 4u;
#else
    volatile uint8_t *pBase = pCircBuf->pBase;
    const volatile uint8_t *pSrc = pSrcAddr;
#endif

    if (nSpaceAvailable >= nNumBytes)
    {
        for (i = 0; i < nNumBytes; i++)
        {
            if (nOffset >= nSize)
            {
                nOffset -= nSize;
            }
            pBase[nOffset] = pSrc[i];
            nOffset++;
        }
        pCircBuf->nWriteIndex = nOffset;
    }
    else
    {
        nResult = 1;
    }
    return nResult;
}

/**
 * @brief Reads data from buffer but do not update read index
 * @param[in] pCircBuf - Pointer to circular buffer structure
 * @param[out] pDstAddr - Pointer to store the require dbytes
 * @param[in] nReqBytes - Number of required bytes
 * @return 0 -  Success , 1 - Not enough bytes in buffer
 */
static inline int32_t ADICircBufQuery(volatile ADI_CIRC_BUF *pCircBuf, volatile uint8_t *pDstAddr,
                                      uint32_t nReqBytes)
{
    int32_t nResult = 0;
    uint32_t i = 0;

    uint32_t nSize = pCircBuf->nSize;
    uint32_t nOffset = pCircBuf->nReadIndex;
#ifdef CIRC_BUFF_MEM_32_ALLIGN
    uint32_t *pBase = (uint32_t *)pCircBuf->pBase;
    uint32_t *pDst = (uint32_t *)pDstAddr;
    nReqBytes = nReqBytes / 4u;
#else
    volatile uint8_t *pBase = pCircBuf->pBase;
    volatile uint8_t *pDst = pDstAddr;
#endif
    uint32_t nNumBytesAvailable = (uint32_t)ADICircBufGetNumBytesAvailable(pCircBuf);

    if (nNumBytesAvailable >= nReqBytes)
    {
        for (i = 0; i < nReqBytes; i++)
        {
            if (nOffset >= nSize)
            {
                nOffset -= nSize;
            }
            pDst[i] = pBase[nOffset];
            nOffset++;
        }
    }
    else
    {
        nResult = 1;
    }

    return nResult;
}

/**
 * @brief Flushes nNumBytes of data from the circular buffer
 * @param[in] pCircBuf - Pointer to circular buffer structure
 * @param[in] nNumBytes  - Number of bytes to be flsuhed
 * @return 0 -  Success , 1 - Not enough bytes in buffer
 */
static inline int32_t ADICircBufFlush(ADI_CIRC_BUF *pCircBuf, uint32_t nNumBytes)
{
    int32_t nResult = 0;

    uint32_t nNumBytesAvailable = (uint32_t)ADICircBufGetNumBytesAvailable(pCircBuf);

    if (nNumBytesAvailable < nNumBytes)
    {
        nNumBytes = nNumBytesAvailable;
    }

    pCircBuf->nReadIndex += nNumBytes;

    if (pCircBuf->nReadIndex >= pCircBuf->nSize)
    {
        pCircBuf->nReadIndex -= pCircBuf->nSize;
    }

    return nResult;
}

/*======= P U B L I C   P R O T O T Y P E S ========*/
/* (globally-scoped functions)  */

#ifdef __cplusplus
}
#endif

#endif /* __ADI_CIRC_BUF_H__ */
/**
 * @}
 */
