/******************************************************************************
 Copyright (c) 2024 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file        adi_cli_memory.h
 * @brief       Defines memory required for CLI. This file
 * includes internal library defines to calculate memory size required. Include
 * this file only if Create fuction to be called.
 * @{
 */

#ifndef __ADI_CLI_MEMORY_H__
#define __ADI_CLI_MEMORY_H__

#include "adi_cli_private.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** State memory required in bytes for the CLI service. Allocate a buffer aligned
 * to 32 bit boundary */
#define ADI_CLI_STATE_MEM_NUM_BYTES sizeof(ADI_CLI_INFO)
/** Temporary memory required in bytes for the CLI service. Allocate a buffer aligned
 * to 32 bit boundary */
#define ADI_CLI_TEMP_MEM_NUM_BYTES 0x2000

/** @} */
#ifdef __cplusplus
}
#endif

#endif /* __ADI_CLI_MEMORY_H__ */
