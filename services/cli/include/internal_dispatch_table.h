/******************************************************************************
 Copyright (c) 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file     internal_dispatch_table.h
 * @brief    This file contains API declarations for dispatch table
 * @{
 */

#ifndef __INTERNAL_DISPATCH_TABLE_H__
#define __INTERNAL_DISPATCH_TABLE_H__

#include "cli_dispatch.h"
#include "cli_private.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Command dispatch table
 * Add commands table here with function description
 */
static const InternalCommand internalDispatchTable[] = {
    {"help", "s", CliHelp, NOHIDE, "command help", "<command>",
     "\t  Display command line help summary. command specific help is "
     "displayed  if argument <command> is "
     "given.\n\r",
     NULL},
#ifdef ENABLE_X86_BUILD
    {"exit", "", CliExit, NOHIDE, "Exits the CLI program", "", NULL, NULL},
#endif /* ENABLE_X86_BUILD */
    {"echo", "s", CliCmdEcho, NOHIDE, "Enables or disables echo mode", "", NULL, NULL},
};

#ifdef __cplusplus
}
#endif

#endif /* __INTERNAL_DISPATCH_TABLE_H__ */

/**
 * @}
 */
