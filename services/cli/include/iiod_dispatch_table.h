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

#include "cli_commands.h"
#include "cli_dispatch.h"
#include "cli_interface.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
    {"gettrig", "ss", CmdGetRig, NOHIDE, "Get the name of the trigger used by the specified device",
     "<device>", NULL, NULL},
    {"settrig", "sss", CmdSetRig, NOHIDE, "Set the trigger to use for the specified device",
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
