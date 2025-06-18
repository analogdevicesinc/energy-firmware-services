/******************************************************************************
 Copyright (c) 2024 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file     cli_dispatch.h
 * @brief    This file contains API declarations for dispatch table
 * @addtogroup ADI_CLI
 * @{
 */

#ifndef __CLI_DISPATCH_H__
#define __CLI_DISPATCH_H__

#include "cli_interface.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief typedef for one record of the command dispatch table
 */

typedef struct commandData
{
    /** command name */
    char *pName;
    /** list of arguments-type in sequence */
    char *pParamList;
    /** pointer to command function*/
    int32_t (*pfFunc)(Args *);
    /** flag hidden/non-hidden command */
    bool hide;
    /** pointer to summary of command */
    char *pSummary;
    /** pointer to synopsis for command */
    char *pSynopsis;
    /** pointer to command description*/
    char *pDescription;
    /** Function pointer to additional description function*/
    void (*pfDesc)(void);
} Command;

/**
 * @brief typedef for one record of the command dispatch table
 */

typedef struct
{
    /** pointer to store the command in lower case */
    char *pCmdLower;
    /** pointer to store the command in lower case from dispatch table */
    char *pDispatchCmd;
    /** pointer for the dispatch table */
    const Command *pDispatchTable;
    /** number of records in dispatch table */
    int32_t numDispatchRecords;

} CLI_DISPATCH_DATA;

/** @brief HIDE command */
#define HIDE true
/** @brief UNHIDE command */
#define NOHIDE false

/**
 * @brief Get number of records in dispatch table.
 * \ref DispatchGetNumRecords
 * @return total number of dispatch table records.
 */
int32_t DispatchGetNumRecords(void);

/**
 * @brief Convert input as lowercase and get matching dispatch record.
 * \ref DispatchMatchCommand
 * @param[in]  pCommandToken	- parsed user command.
 * @return const Command*       - pointer to dispatch table record.
 */
const Command *DispatchMatchCommand(const char *pCommandToken);

/**
 * @brief Get handle for dispatch table.
 * \ref DispatchGetRecords
 * @return const Command*  - pointer to dispatch table record.
 */
const Command *DispatchGetRecords(void);

/**
 * @brief Get matching dispatch table record.
 * \ref DispatchGetCommandDetails
 * @param [in] pCommandToken	- parsed user command in lowercase.
 * @return const Command*       - pointer to dispatch table record.
 */
const Command *DispatchGetCommandDetails(char *pCommandToken);

/**
 * @brief Initialize dispatch table.
 * @param [in] pRecords		- pointer to command dispatch table.
 * @param [in] numRecords	- number of records in dispatch table.
 */
void DispatchInit(const Command *pRecords, int32_t numRecords);

#ifdef __cplusplus
}
#endif

#endif /* __CLI_DISPATCH_H__ */

/**
 * @}
 */
