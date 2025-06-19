/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file      cli_utils.h
 * @addtogroup ADI_CLI
 * @brief     Prototypes for cli_utils
 * @{
 */

#ifndef __CLI_UTILS_H__
#define __CLI_UTILS_H__

/*============= I N C L U D E S =============*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============= D E F I N E S =============*/

/**
 * @brief  structure to hold the pointers required for utils
 */
typedef struct
{
    /** pointer to store the parameter choice parameter */
    char *pMatchParam0;
    /** pointer to store the parameter choice parameter */
    char *pMatchParam1;
} CLI_UTILS_DATA;

/*============= F U N C T I O N  P R O T O T Y P E S =============*/

/**
 * @details Returns choice.
 * @param[in] pChoices   - Array of available choices.
 * @param[in] pArg       - Argument to compare with available choices.
 * @param[in] numChoices - Number of choices.
 * @param[in] pParam     - Parameter read from command line.
 * @return Choice        - Index if it matches.
 *                       - -1 if it doesn't match.
 */
int32_t GetChoice(char *pChoices[], char *pArg, int32_t numChoices, char *pParam);

/**
 * @details Compares parameters.
 * @param[in] pParam     - parameter read from command line.
 * @param[in] maxLength  - parameter length.
 * @param[in] pString    - Local parameter to check.
 * @return status        - true - if it matches.
 *                       - false -  if it doesn't match.
 */
bool CheckParams(char *pParam, int32_t maxLength, char *pString);

/**
 * @details Convert string to lower case.
 * @param[in] pSrc    - string that needs to  be converted.
 * @param[in] maxSize - size of buffer.
 * @param[out] pDst   - buffer to store converted string.
 */
void ConvertToLower(const char *pSrc, int32_t maxSize, char *pDst);

/**
 * @brief Convert string to upper case.
 * @param[in] pSrc    - string that needs to  be converted.
 * @param[in] maxSize - size of buffer.
 * @param[out] pDst   - buffer to store converted string.
 */
void ConvertToUpper(const char *pSrc, int32_t maxSize, char *pDst);

/**
 * @details Copies string from one buffer to other.
 * @param[in] pSrc      - source buffer.
 * @param[in] maxLength - length of string to be copied.
 * @param[out] pDst     - destination buffer.
 */
void StrCopy(const char *pSrc, int32_t maxLength, char *pDst);

/**
 * @brief  trim whitespace from front and back of command.
 * @param[in] pCommand        - pointer to command
 * @param[out] pTrimCommand   - storage pointer for trimmed command
 * @return size of trimmed command.
 */
size_t TrimWhiteSpaces(const char *pCommand, char *pTrimCommand);

/**
 * @brief  returns the number of characters in the string pointed to by
 * pStr, not including the terminating '\0' character, but at most maxlen.
 * @param[in] pStr          - pointer to command.
 * @param[in] maxLen        - maximum length.
 * @return size of string.
 */
size_t StrnLen(const char *pStr, size_t maxLen);

#ifdef __cplusplus
}
#endif

#endif /* __CLI_UTILS_H__ */
/**
 * @}
 */
