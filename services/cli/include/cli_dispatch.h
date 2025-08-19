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

#include "app_cfg.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Holds parameter value
 */
typedef union
{
    char *pS; /**< holds string value */
    char c;   /**< holds char type value */
    double f; /**< holds double type value */
    long d;   /**< holds long type value */
} PARAM;

/**
 *  Holds a full set of parsed parameter values for passing to cli_commands
 */
typedef struct
{
    int c;                                /**< arguments count value */
    PARAM v[APP_CFG_CLI_MAX_PARAM_COUNT]; /**< buffer to hold parameter values */
} Args;

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

typedef struct internalCommandData
{
    /** command name */
    char *pName;
    /** list of arguments-type in sequence */
    char *pParamList;
    /** pointer to command function*/
    int32_t (*pfFunc)(void *, const Command *, Args *, int32_t);
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
} InternalCommand;

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
    /** Holds a full set of parsed parameter values for passing to commands */
    Args sArgs;

} CLI_DISPATCH_DATA;

/** @brief HIDE command */
#define HIDE true
/** @brief UNHIDE command */
#define NOHIDE false

/**
 * @brief Get matching dispatch table record.
 * \ref DispatchGetCommandDetails
 * @param [in] pInfo		    - pointer to dispatch data
 * @param [in] pCommandToken	- parsed user command in lowercase.
 * @param [in] pDispatchTable	- pointer to dispatch table
 * @param [in] numRecords	    - number of records in dispatch table
 * @return const Command*       - pointer to dispatch table record.
 */
const Command *DispatchGetCommandDetails(CLI_DISPATCH_DATA *pInfo, char *pCommandToken,
                                         const Command *pDispatchTable, int32_t numRecords);

#ifdef __cplusplus
}
#endif

#endif /* __CLI_DISPATCH_H__ */

/**
 * @}
 */
