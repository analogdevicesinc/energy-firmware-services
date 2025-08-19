/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file     iiod_dispatch_table.h
 * @brief    This file contains API declarations for dispatch table
 * @{
 */

#ifndef __IIOD_DISPATCH_TABLE_H__
#define __IIOD_DISPATCH_TABLE_H__

#include "cli_dispatch.h"
#include "cli_private.h"
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Command to display help.
 * @param pArgs - pointer to command arguments storage.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t CmdHelp(Args *pArgs);

/**
 * @brief Command to exit the program.
 * @param pArgs - pointer to command arguments storage.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t CmdExit(Args *pArgs);

/**
 * @brief Command to print the XML file.
 * @param pArgs - pointer to command arguments storage.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t CmdPrint(Args *pArgs);

/**
 * @brief Command to print the Version
 * @param pArgs - pointer to command arguments storage.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t CmdVersion(Args *pArgs);

/**
 * @brief Command to set the timeout
 * @param pArgs - pointer to command arguments storage.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t CmdTimeout(Args *pArgs);

/**
 * @brief Command to Open the device
 * @param pArgs - pointer to command arguments storage.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t CmdOpen(Args *pArgs);

/**
 * @brief Command to Close the device
 * @param pArgs - pointer to command arguments storage.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t CmdClose(Args *pArgs);

/**
 * @brief Command to Read from a register
 * @param pArgs - pointer to command arguments storage.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t CmdRead(Args *pArgs);

/**
 * @brief Command to Write to a register
 * @param pArgs - pointer to command arguments storage.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t CmdWrite(Args *pArgs);

/**
 * @brief Command to Read from a buffer
 * @param pArgs - pointer to command arguments storage.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t CmdReadBuf(Args *pArgs);

/**
 * @brief Command to Write to a buffer
 * @param pArgs - pointer to command arguments storage.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t CmdWriteBuf(Args *pArgs);

/**
 * @brief Command to get the name of the trigger
 * @param pArgs - pointer to command arguments storage.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t CmdGetTrig(Args *pArgs);

/**
 * @brief Command to set the trigger
 * @param pArgs - pointer to command arguments storage.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t CmdSetTrig(Args *pArgs);

/**
 * @brief Command to set the number of kernel buffers
 * @param pArgs - pointer to command arguments storage.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t CmdSet(Args *pArgs);

/**
 * @brief Command dispatch table
 * Add commands table here with function description
 */
static const Command dispatchTable[] = {
    {"help", "s", CmdHelp, NOHIDE, "command help", "Print this help message", NULL, NULL},
    {"exit", "s", CmdExit, NOHIDE, "Exit", "Close the current session", NULL, NULL},
    {"print", "s", CmdPrint, NOHIDE, "Prints",
     "Display a XML string corresponding to the current IIO context", NULL, NULL},
    {"version", "s", CmdVersion, NOHIDE, "Get the version of libiio in use", "", NULL, NULL},
    {"timeout", "ss", CmdTimeout, NOHIDE, "Set the timeout (in ms) for I/O operations", "", NULL,
     NULL},
    {"open", "sss", CmdOpen, NOHIDE, "Open the specified device with the given mask of channels",
     "<device> <samples_count> <mask>", NULL, NULL},
    {"close", "ss", CmdClose, NOHIDE, "Close the specified device", "<device>", NULL, NULL},
    {"read", "ssss", CmdRead, NOHIDE, "Read the value of an attribute",
     "<device> DEBUG|[INPUT|OUTPUT <channel>] [<attribute>]", NULL, NULL},
    {"write", "sssss", CmdWrite, NOHIDE, "Set the value of an attribute",
     "<device> DEBUG|[INPUT|OUTPUT <channel>] [<attribute>] <bytes_count>", NULL, NULL},
    {"readbuf", "ss", CmdReadBuf, NOHIDE, "Read raw data from the specified device",
     "<device> <bytes_count>", NULL, NULL},
    {"writebuf", "ss", CmdWriteBuf, NOHIDE, "Write raw data to the specified device",
     "<device> <bytes_count>", NULL, NULL},
    {"gettrig", "ss", CmdGetTrig, NOHIDE,
     "Get the name of the trigger used by the specified device", "<device>", NULL, NULL},
    {"settrig", "sss", CmdSetTrig, NOHIDE, "Set the trigger to use for the specified device",
     "<device> [<trigger>]", NULL, NULL},
    // Isuue with the set command.
    {"set", "sss", CmdSet, NOHIDE, "Set the number of kernel buffers for the specified device",
     "<device> BUFFERS_COUNT <count>", NULL, NULL},
};

/**
 * @brief Get the number of commands in the dispatch table
 */
#define NUM_COMMANDS (sizeof(dispatchTable) / sizeof(Command))

#ifdef __cplusplus
}
#endif

#endif /* __IIOD_DISPATCH_TABLE_H__ */

/**
 * @}
 */
