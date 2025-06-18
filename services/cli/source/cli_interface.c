/******************************************************************************
 Copyright (c) 2023 - 2025  Analog Devices Inc.
******************************************************************************/

/**
 * @file     cli_interface.c
 * @brief    This file contains API definitions for command line interface
 * @{
 */

/*==========================  I N C L U D E S   ==========================*/

#include "cli_interface.h"
#include "adi_cli.h"
#include "adi_cli_private.h"
#include "app_cfg.h"
#include "cli_dispatch.h"
#include "cli_history.h"
#include "cli_utils.h"
#include "message.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*==========================  D E F I N I T I O N S ==========================*/

/** @brief command argument type string  */
#define DATATYPE_STRING (0)
/** @brief command argument type float */
#define DATATYPE_FLOAT (1)
/** @brief command argument type integer*/
#define DATATYPE_INTEGER (2)
/** @brief command argument type char */
#define DATATYPE_CHAR (3)
/** @brief state machine case for no character */
#define CLI_MET_NO_CHAR (0)
/** @brief state machine case for no ESC character */
#define CLI_MET_ESC_CHAR (1)
/** @brief cstate machine case for cli control */
#define CLI_MET_CTRL_CHAR (2)
/** @brief cstate machine case for final character for some control sequences */
#define CLI_MET_FINAL_CHAR (3)
/** @brief state machine ID to move control UPWARD*/
#define CLI_MET_CTRL_UP ('A')
/** @brief state machine ID to move control DOWNWARD */
#define CLI_MET_CTRL_DOWN ('B')
/** @brief state machine ID to move control RIGHT */
#define CLI_MET_CTRL_RIGHT ('C')
/** @brief state machine ID to move control LEFT */
#define CLI_MET_CTRL_LEFT ('D')
/** @brief state machine ID to move control to the beginning of the line */
#define CLI_MET_CTRL_HOME ('1')
/** @brief state machine ID to move control to the end of the line */
#define CLI_MET_CTRL_END ('4')

/*========================== D A T A T Y P E S ==========================*/

/*========================== P R O T O T Y P E S ==========================*/

/**
 * @brief Insert character at current position, right shift text after cursor and update pointers.
 * @param[in] pEditLine    - handle to CLI editline.
 * @param[in] inputChar    - input character.
 */
static void CliInsertChar(EditLine *pEditLine, const char inputChar);

/**
 * @brief  Process single character (not an escape sequence).
 * @param[in] pEditLine   - handle to CLI edit line.
 * @param[in] inputChar   - input character.
 * @return 0 -  A complete line is received, 1 - still parsing.
 */
static int32_t CliProcessInputChar(EditLine *pEditLine, const char inputChar);

/**
 * @brief  Move cursor to beginning of line, and fill edit line with command string.
 * @param[in] pEditLine    - handle to CLI editline.
 * @param[in] pCommand     - pointer to command string.
 */
static void CliFillCommandLine(EditLine *pEditLine, const char *pCommand);

/**
 * @brief Put character at current position(overwrite mode) and update pointers.
 * @param[in] pEditLine    - handle to CLI editline.
 * @param[in] inputChar    - input character.
 */
static void CliPutChar(EditLine *pEditLine, const char inputChar);

/**
 * @brief Sends a bold string through UART
 * @param[in] pStr - String pointer
 * @return  0 - Success, 1 - Failed.
 */
static int32_t CliPutStringBold(char *pString);

/**
 * @brief  Move cursor back to beginning of line without disturbing command and leave indexCur=0.
 * @param[in] pEditLine    - handle to CLI editline.
 */
static void CliMoveCursorToStart(EditLine *pEditLine);

/**
 * @brief  Move cursor to end of line without disturbing command, and leave indexCur=indexEnd.
 * @param[in] pEditLine    - handle to CLI editline.
 */
static void CliMoveCursorToEnd(EditLine *pEditLine);

/**
 * @brief  Delete character at cursor position, and shift end of line to left.
 * @param[in] pEditLine    - handle to CLI editline.
 */
static void CliDeleteCharAtCursor(EditLine *pEditLine);

/**
 * @brief  Delete all characters to the right of cursor.
 * @param[in] pEditLine    - handle to CLI editline.
 */
static void CliDeleteAllChars(EditLine *pEditLine);

/**
 * @brief  Move cursor forward one character(if exists), and increment indexCur.
 * @param[in] pEditLine    - handle to CLI editline.
 */
static void CliMoveCursorForward(EditLine *pEditLine);

/**
 * @brief  Move cursor backward one character (if exists), and decrement indexCur.
 * @param[in] pEditLine    - handle to CLI editline.
 */
static void CliMoveCursorBackward(EditLine *pEditLine);

/**
 * @brief  Reset command line.
 * @param[in] pEditLine    - handle to CLI editline
 */
static void CliReset(void);

/**
 * @brief  Emit a command prompt.
 */
static void CliDisplayPrompt(void);

/**
 * @brief Parse input string;get dispatch and execute command function.
 * @param[in] pString	- Pointer to input command string.
 * @return  0 - Success, 1 - Failed.
 */
static int32_t CliParse(char *pString);

/**
 * @brief Display help for a specific command.
 * @param[in] pCommandToken - Pointer to command.
 * @return  0 - Success, 1 - Failed.
 */
static int32_t CliCommandHelp(char *pCommandToken);

/**
 * @brief Function for CLI "help" command.
 * @param showHidden - true to display only hidden commands; false to display only visible commands
 * @return  0 - Success, 1 - Failed.
 */
static int32_t CliGenericHelp(bool showHidden);

/**
 * @brief  Function to control terminal cursor position.
 * @param[in] ctrlFuncID    - ctrlFuncID.
 * @return  0 - Success, 1 - Failed.
 */
static int32_t CliInsertControlChars(int32_t ctrlFuncID);

/**
 * @brief  Function to control terminal cursor position.
 * @param[in] ctrlFuncID    - ctrlFuncID.
 * @return  0 - Success, 1 - Failed.
 */
static int32_t CliInsertManualControlChars(int32_t ctrlFuncID);

/**
 * @brief  Scans each input char and form an input command/control char action.
 * @return  0 - Success, 1 - Failed.
 */
static int32_t CliScanInputChars(EditLine *pEditLine);

/**
 * @brief  Parse command parameters and call command functions.
 * @param[in] pDispatchRecord - handle to dispatch table.
 * @param[in] silent          - flag silent parse.
 * @param[out] pArgs          - pointer to command arguments storage.
 * @return  0 - Success, 1 - Failed.
 */
static int32_t CliDispatch(const Command *pDispatchRecord, Args *pArgs, bool silent);

/**
 * @brief  Parse the command arguments list based on their data type.
 * @param[in] pPattern      - pointer to user input command string.
 * @param[in] pArgs         - pointer to user input command string.
 * @param[in] argIndex      - argument index/position.
 * @param[in] dataType      - argument data type.
 * @return  0 - Success, 1 - Failed.
 */
static int32_t CliScanParams(char *pPattern, Args *pArgs, int32_t argIndex, int32_t dataType);

/**
 * @brief  State machine to parse arguments of a command.
 * @param[in] pParams	  - handle to dispatch table.
 * @param[in] silent      - flag silent parse.
 * @param[out] pArgs      - pointer to command arguments storage.
 * @return  0 - Success, 1 - Failed.
 */
static int32_t CliParseParams(const char *pParams, Args *pArgs, bool silent);

/**
 * @brief Resets the Edit line buffer.
 */
static void CliEditLineReset(void);

/*========================== C O D E ==========================*/

int32_t CliInit(void)
{
    int32_t status = 0;

    CliInsertControlChars(CLI_CTRL_CLS);
    HistoryInit();
    CliDisplayPrompt();
    CliEditLineReset();
    return status;
}

void CliDeferPrompt(bool enable)
{
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    pInfo->deferPrompt = enable;
    pInfo->displayPrompt = !enable;
}

int32_t CliInterface(void)
{
    int32_t status = 0;
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    char *pStringBuffer = &pInfo->cliString[0];
    char *pTrimBuffer = pInfo->pCliTrimString;
// #define DEBUG_CLI
#ifdef DEBUG_CLI
    {
        static FILE *fp = NULL;
        if (fp == NULL)
        {
            char *filename = "commands.txt";
            fp = fopen(filename, "r");
            if (fp == NULL)
            {
                printf("Not able to open %s", filename);
                exit(0);
            }
        }
        if (feof(fp) != 0)
        {
            exit(0);
        }

        fgets(pStringBuffer, MAX_COMMAND_LENGTH, fp);
    }
#else
    status = CliReadCommandLineInput(pStringBuffer);
#endif
    if (status == 0)
    {
        TrimWhiteSpaces(pStringBuffer, pTrimBuffer);
        status = CliParse(pTrimBuffer);
    }
    return status;
}

static int32_t CliScanInputChars(EditLine *pEditLine)
{
    char *pCommand = NULL;
    int32_t status = 1;
    /* State variable for building ANSI escape sequences */
    static int32_t escSeqState = 0;
    int32_t inputChar = adi_cli_GetChar();
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    if (inputChar != EOF)
    {
        pInfo->userIsTyping = true;
        if (pInfo->deferPrompt)
        {
            CliInsertControlChars(CLI_CTRL_NEWLINE);
            CliOverwriteLineWithPrompt();
        }
        /* State machine that constructs an ANSI ESC sequence from char */
        switch (escSeqState)
        {

        /* Initiate one/process character, if no ESC sequence is initiated yet */
        case CLI_MET_NO_CHAR:
            if (inputChar == 0x1B)
            {
                escSeqState++;
            }
            else
            {
                status = CliProcessInputChar(pEditLine, (const char)inputChar);
            }
            break;
        /* Received an ESC character, check for a '[' continuation, or reset state */
        case CLI_MET_ESC_CHAR:
            if (inputChar == '[')
            {
                escSeqState++;
            }
            else
            {
                escSeqState = 0;
            }
            break;
        /* So far we have an 'ESC[' sequence, and now check for A, B, C or D */
        case CLI_MET_CTRL_CHAR:
            switch (inputChar)
            {

            /* Up arrow */
            case CLI_MET_CTRL_UP:
                pCommand = HistoryScrollUp();
                if (pCommand != NULL)
                {
                    CliFillCommandLine(pEditLine, pCommand);
                }
                escSeqState = 0;
                break;

            /* Down arrow */
            case CLI_MET_CTRL_DOWN:
                pCommand = HistroyScrollDown();
                if (pCommand != NULL)
                {
                    CliFillCommandLine(pEditLine, pCommand);
                }
                else
                {
                    CliReset();
                }
                escSeqState = 0;
                break;

            /* Right arrow */
            case CLI_MET_CTRL_RIGHT:
                CliMoveCursorForward(pEditLine);
                escSeqState = 0;
                break;

            /* Left arrow */
            case CLI_MET_CTRL_LEFT:
                CliMoveCursorBackward(pEditLine);
                escSeqState = 0;
                break;

            /* Home */
            case CLI_MET_CTRL_HOME:
                CliMoveCursorToStart(pEditLine);
                ++escSeqState;
                break;

            /* End */
            case CLI_MET_CTRL_END:
                CliMoveCursorToEnd(pEditLine);
                ++escSeqState;
                break;

            /* Unrecognized escape sequence */
            default:
                ++escSeqState;
                break;
            }
            break;

        /* Check final character for some special control sequences */
        case CLI_MET_FINAL_CHAR:
            if (inputChar == '~')
            {
                escSeqState = 0;
            }
            break;

        /* The state variable is somehow out of whack, reset it */
        default:
            escSeqState = 0;
            break;
        }
    }

    return status;
}

int32_t CliReadCommandLineInput(char *pBuffer)
{
    int32_t status = 1;
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    /* First initialize an edit line structure */
    pInfo->editLine.pPrompt = APP_CFG_CLI_PROMPT;
    pInfo->editLine.pBuffer = pBuffer;

    if (pInfo->displayPrompt)
    {
        CliOverwriteLineWithPrompt();
    }
    /* Scan each character to identify the command/control char */
    status = CliScanInputChars(&pInfo->editLine);
    if (status == 0)
    {
        pInfo->displayPrompt = true;
        /* Got a complete (or empty) command line, drop to next terminal line */
        CliInsertControlChars(CLI_CTRL_NEWLINE);
    }

    return status;
}

void CliOverwriteLineWithPrompt(void)
{
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    pInfo->deferPrompt = false;
    pInfo->displayPrompt = false;
    /* Start on a new line and issue a prompt */
    CliClearLine();
    CliDisplayPrompt();
}

void CliNewLine(void)
{
    CliClearLine();
    CliInsertControlChars(CLI_CTRL_NEWLINE);
}

void CliClearLine(void)
{
    CliEditLineReset();
    CliInsertControlChars(CLI_CTRL_CR);
}

static void CliEditLineReset()
{
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    if (pInfo->editLine.pBuffer)
    {
        pInfo->editLine.pBuffer[0] = '\0';
    }
    pInfo->editLine.indexCur = 0;
    pInfo->editLine.indexEnd = 0;
    pInfo->editLine.numCharsToPrint = 0;
}

static int32_t CliDispatch(const Command *pDispatchRecord, Args *pArgs, bool silent)
{
    /* Initializing status to error*/
    int32_t status = 1;
    status = CliParseParams(pDispatchRecord->pParamList, pArgs, silent);
    if (status == 0)
    {
        /* Call respective command function */
        status = (int32_t)pDispatchRecord->pfFunc(pArgs);
    }

    return status;
}

static int32_t CliParse(char *pString)
{
    bool silent;
    char *pCommandToken = NULL;
    const Command *pDispatchRecord;
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    // Initialize sArgs to 0
    memset(&pInfo->sArgs, 0, sizeof(pInfo->sArgs));
    int32_t status = 0;

    /* Parse off the first token from command line in pString,
     * should be the command name.
     */
    pCommandToken = strtok(pString, " ,;\t");

    if (pCommandToken != NULL)
    {
        /* Match pCommandToken with a dispatch table record,
         * return a record pointer if matched; else NULL
         */
        pDispatchRecord = (const Command *)DispatchGetCommandDetails(pCommandToken);
        if (pDispatchRecord != NULL)
        {
            silent = false;
            /* Got a matching command record */
            /* It's a good command, and not hidden (or we are in unlock
             * mode anyway) parse parameters required for this command */
            status = CliDispatch(pDispatchRecord, &pInfo->sArgs, silent);
            if (status != 0)
            {
                INFO_MSG("Incorrect usage: Enter 'help %s' for details", pCommandToken)
            }
        }
        else
        {
            WARN_MSG("Command '%s' not found", pCommandToken)
        }
    }
    return status;
}

static int32_t CliScanParams(char *pPattern, Args *pArgs, int32_t argIndex, int32_t dataType)
{
    int32_t status = 0;
    char *pParamToken = NULL;
    pParamToken = strtok(NULL, pPattern);

    if (pParamToken != NULL)
    {
        switch (dataType)
        {
        case DATATYPE_STRING:
            pArgs->v[argIndex].pS = pParamToken;
            break;
        case DATATYPE_FLOAT:
            if (sscanf(pParamToken, "%lf", &pArgs->v[argIndex].f) != 1)
            {
                status = 1;
            }
            break;
        case DATATYPE_INTEGER:
            if (sscanf(pParamToken, "%li", &pArgs->v[argIndex].d) != 1)
            {
                status = 1;
            }
            break;
        case DATATYPE_CHAR:
            if (sscanf(pParamToken, "%c", &pArgs->v[argIndex].c) != 1)
            {
                status = 1;
            }
            break;
        default:
            status = 1;
            break;
        }

        if (status != 1)
        {
            pArgs->c++;
        }
    }

    return status;
}

static int32_t CliParseParams(const char *pParamList, Args *pArgs, bool silent)
{
    char *pParamToken;
    int32_t paramCount;
    int32_t i = 0;
    int32_t status = 0;

    paramCount = (int32_t)StrnLen(pParamList, APP_CFG_CLI_MAX_PARAM_COUNT);
    if (paramCount <= APP_CFG_CLI_MAX_PARAM_COUNT)
    {
        for (i = 0; i < paramCount; i++)
        {
            switch (pParamList[i])
            {
            /** FIXME: Currently supported datatypes in dispatch are
             * s,f,d,D,x,X. c,S,F are not used; Fix their support when needed */
            case 's':
            case 'S':
                status = CliScanParams(" \"'", pArgs, i, DATATYPE_STRING);
                break;
            case 'f':
            case 'F':
                status = CliScanParams(" ,;\t", pArgs, i, DATATYPE_FLOAT);
                break;
            case 'd':
            case 'x':
            case 'D':
            case 'X':
                status = CliScanParams(" ,;\t", pArgs, i, DATATYPE_INTEGER);
                break;
            case 'c':
            case 'C':
                status = CliScanParams(" ,;\t", pArgs, i, DATATYPE_CHAR);
                break;
            default:
                break;
            }
        }
    }
    else
    {
        status = 1;
    }

    if (status != 0)
    {
        if (!silent)
        {
            INFO_MSG("Invalid Arguments")
        }
    }

    while (1)
    {
        pParamToken = strtok(NULL, " ,;\t");
        if (!pParamToken)
        {
            break;
        }

        if (!silent)
        {
            WARN_MSG("Extra parameter '%s' ignored", pParamToken)
        }
    }
    return status;
}

static int32_t CliCommandHelp(char *pCommandToken)
{
    bool lock = false;
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    char *pString = &pInfo->cliString[0];
    const Command *pDispatchRecord;
    int32_t status = 0;

    /* Specific command help */
    pDispatchRecord = (const Command *)DispatchGetCommandDetails(pCommandToken);
    if (pDispatchRecord != NULL)
    {
        /* Found a matching command as argument to help;
        check to see if it is a hidden command and if so then complain */
        if (!(pDispatchRecord->hide && lock))
        {

            /* Valid command as argument to help, and not a hidden command (or
            we are in unlock state anyway).start sending out the help text */
            snprintf(pString, APP_CFG_CLI_MAX_CMD_LENGTH, "\tCOMMAND:\r\n");
            CliInsertControlChars(CLI_CTRL_NEWLINE);
            CliPutStringBold(pString);

            snprintf(pString, APP_CFG_CLI_MAX_CMD_LENGTH, "\t  %s - %s\r\n", pDispatchRecord->pName,
                     pDispatchRecord->pSummary);
            adi_cli_PutStringNb(pString);
            CliInsertControlChars(CLI_CTRL_CR);

            snprintf(pString, APP_CFG_CLI_MAX_CMD_LENGTH, "\n\tSYNOPSIS:\r\n");
            CliPutStringBold(pString);
            snprintf(pString, APP_CFG_CLI_MAX_CMD_LENGTH, "\t  %s %s", pDispatchRecord->pName,
                     pDispatchRecord->pSynopsis);
            adi_cli_PutStringNb(pString);
            CliInsertControlChars(CLI_CTRL_NEWLINE);
            if ((pDispatchRecord->pDescription != NULL) || (pDispatchRecord->pfDesc != NULL))
            {
                snprintf(pString, APP_CFG_CLI_MAX_CMD_LENGTH, "\n\tDESCRIPTION:\r\n");
                CliPutStringBold(pString);
                if (pDispatchRecord->pDescription != NULL)
                {
                    adi_cli_PutString(pDispatchRecord->pDescription);
                }
                if (pDispatchRecord->pfDesc != NULL)
                {
                    pDispatchRecord->pfDesc();
                }
                CliInsertControlChars(CLI_CTRL_NEWLINE);
            }
        }
    }
    else
    {
        status = 1;
    }

    return status;
}

static int32_t CliGenericHelp(bool showHidden)
{
    int32_t i;
    int32_t status = 0;
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    char *pString = &pInfo->cliString[0];
    int32_t numRecords = 0;
    int32_t cmdLength = 0;
    int32_t maxCmdLength = 0;
    const Command *pDispatchRecord;

    /* General help for all commands */
    /* Number of records in the dispatch table that we need to loop through */
    numRecords = DispatchGetNumRecords();
    pDispatchRecord = (const Command *)DispatchGetRecords();
    if (pDispatchRecord != NULL)
    {
        for (i = 0; i < numRecords; i++)
        {
            char *pName = pDispatchRecord[i].pName;
            if (pName == NULL)
            {
                pName = "";
            }
            cmdLength = strlen(pDispatchRecord[i].pName);
            if (cmdLength > maxCmdLength)
            {
                maxCmdLength = cmdLength;
            }
        }

        snprintf(pString, APP_CFG_CLI_MAX_CMD_LENGTH, "\r\n\t %-*s  %s\r\n",
                 (int)(maxCmdLength + 1), "COMMANDS", "PARAMETERS");
        CliPutStringBold(pString);

        for (i = 0; i < numRecords; i++)
        {
            char *pName = pDispatchRecord[i].pName;
            char *pSynopsis = pDispatchRecord[i].pSynopsis;
            if (pName == NULL)
            {
                pName = "";
            }
            if (pSynopsis == NULL)
            {
                pSynopsis = "";
            }
            snprintf(pString, APP_CFG_CLI_MAX_CMD_LENGTH, "\t  %-*s  %s\r\n",
                     (int)(maxCmdLength + 1), pName, pSynopsis);

            if (pDispatchRecord[i].hide == showHidden)
            {
                adi_cli_PutString(pString);
            }
        }
    }
    else
    {
        status = 1;
    }

    return status;
}

int32_t CliHelp(Args *pArgs)
{
    int32_t status = 0;

    if (pArgs->c > 0)
    {
        status = CliCommandHelp(pArgs->v[0].pS);
    }
    else
    {
        status = CliGenericHelp(false);
        INFO_MSG("\r\nCommand specific help is displayed with 'help <command>'")
    }

    if (status != 0)
    {
        WARN_MSG("Command '%s' not found", pArgs->v[0].pS)
    }

    return status;
}

int32_t CliExpertHelp(Args *pArgs)
{
    int32_t status = 0;

    if (pArgs->c == 0)
    {
        status = CliGenericHelp(true);
        INFO_MSG("\r\nCommand specific help is displayed with 'help <command>'")
    }

    return status;
}

int32_t CliCmdManual(Args *pArgs)
{
    int32_t status = 0;
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    if (pArgs->c > 0)
    {
        if (strcmp(pArgs->v[0].pS, "on") == 0)
        {

            pInfo->echo = true;
            INFO_MSG("echo on")
        }
        else if (strcmp(pArgs->v[0].pS, "off") == 0)
        {

            pInfo->echo = false;
            if (pArgs->c == 2)
            {
                if (strcmp(pArgs->v[1].pS, "off") == 0)
                {
                    pInfo->displayCtrlChars = false;
                }
            }
            INFO_MSG("echo off")
        }
        else
        {
            WARN_MSG("Invalid configuration choice. Usage: manual on/off")
        }
    }
    else
    {
        if (pInfo->echo == true)
        {
            INFO_MSG("echo on")
        }
        else
        {
            INFO_MSG("echo off")
        }
    }

    return status;
}

#ifdef ENABLE_X86_BUILD
int32_t CliExit(Args *pArgs)
{

    int32_t status = 0;

    if (pArgs->c > 0)
    {
        WARN_MSG("Incorrect usage")
    }
    else
    {
        exit(0);
    }

    CliInsertControlChars(CLI_CTRL_NEWLINE);

    return status;
}
#endif

bool CliUserIsTyping(void)
{
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    return pInfo->userIsTyping;
}

static int32_t CliProcessInputChar(EditLine *pEditLine, const char inputChar)
{
    int32_t status = 1;
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    if (!iscntrl((unsigned char)inputChar))
    {
        CliInsertChar(pEditLine, inputChar);
    }
    else
    {
        switch (inputChar)
        {

        /* ^A       move cursor to beginning of line */
        case 0x01:
            CliMoveCursorToStart(pEditLine);
            break;
        /* ^E       move cursor to end of line */
        case 0x05:
            CliMoveCursorToEnd(pEditLine);
            break;
        /* ^B or ^P move cursor back (previous)*/
        case 0x02:
        case 0x10:
            CliMoveCursorBackward(pEditLine);
            break;
        /* ^F or ^N move cursor forward (next) */
        case 0x06:
        case 0x0e:
            CliMoveCursorForward(pEditLine);
            break;
        /* ^K       kill to end of line */
        case 0x0b:
            CliDeleteAllChars(pEditLine);
            break;
        /* ^H or ^? delete character to left of cursor */
        case 0x08:
        case 0x7f:
            CliDeleteCharAtCursor(pEditLine);
            break;
        /* ^J or ^M carriage return */
        case 0x0d:
        case 0x0a:
            pEditLine->pBuffer[pEditLine->indexEnd] = '\0';
            HistoryAppend(pEditLine->pBuffer);
            /* parent will not scan for more characters after <CR> */
            status = 0;
            pInfo->userIsTyping = false;
            break;
        /* ^L  start over */
        case 0x0c:
            CliReset();
            break;
        /* ^C break (probably not a command we want to keep for target
             application) */
        case 0x03:
            pEditLine->pBuffer[0] = '\0';
            /* Parent will not scan for more characters after ^C */
            status = 0;
            break;
        default:
            CliInsertControlChars(CLI_CTRL_ALERT);
        }
    }
    /* Tell parent to continue/stop scanning for characters **/
    return status;
}

void CliReset(void)
{
    CliEditLineReset();
    CliInsertControlChars(CLI_CTRL_CR);
    CliInsertControlChars(CLI_CTRL_KILL);
    CliDisplayPrompt();
}

void CliMoveCursorToStart(EditLine *pEditLine)
{
    uint32_t i;
    for (i = pEditLine->indexCur; i > 0; i--)
    {
        CliInsertControlChars(CLI_CTRL_PREV);
    }
    pEditLine->indexCur = 0;
}

void CliMoveCursorToEnd(EditLine *pEditLine)
{
    uint32_t i;
    for (i = pEditLine->indexCur; i != pEditLine->indexEnd; i++)
    {
        CliInsertControlChars(CLI_CTRL_NEXT);
    }
    pEditLine->indexCur = pEditLine->indexEnd;
}

void CliPutChar(EditLine *pEditLine, const char inputChar)
{
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    volatile bool echo = pInfo->echo;
    pEditLine->pBuffer[pEditLine->indexCur] = inputChar;

    if (echo == true)
    {
        adi_cli_PutChar(pEditLine->pBuffer[pEditLine->indexCur]);
    }

    if (pEditLine->indexCur < (APP_CFG_CLI_MAX_CMD_LENGTH - 1))
    {
        pEditLine->indexCur++;
    }

    if (pEditLine->indexCur > pEditLine->indexEnd)
    {
        pEditLine->indexEnd = pEditLine->indexCur;
    }

    return;
}

static void CliInsertChar(EditLine *pEditLine, const char inputChar)
{
    uint32_t i;
    uint32_t indexInputChar;
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    volatile bool echo = pInfo->echo;
    /* If there is no room, the last character will be overwritten */
    if (pEditLine->indexEnd < APP_CFG_CLI_MAX_CMD_LENGTH - 1)
    {
        indexInputChar = pEditLine->indexCur + pEditLine->numCharsToPrint;
        /* Incrementing the end index indicates a character has been added to the buffer */
        pEditLine->indexEnd++;

        /* Shift any characters after the cursor to make room for the new character(s) to insert */
        for (i = pEditLine->indexEnd; i > indexInputChar; i--)
        {
            pEditLine->pBuffer[i] = pEditLine->pBuffer[i - 1];
        }

        /* Add new character to the edit line buffer */
        pEditLine->pBuffer[indexInputChar] = inputChar;
        pEditLine->numCharsToPrint++;
    }
    /* Check if there are any more characters waiting to be echoed (e.g. when several characters are
     * buffered when copy-pasting) */
    if (adi_cli_GetNumCharsWaiting() == 0 && echo)
    {
        /* Print inputChar and all buffered characters */
        if (pEditLine->indexCur + pEditLine->numCharsToPrint < APP_CFG_CLI_MAX_CMD_LENGTH)
        {
            snprintf(pInfo->pCliPrintString, pEditLine->numCharsToPrint + 1, "%s",
                     pEditLine->pBuffer + pEditLine->indexCur);
            adi_cli_PutStringNb(pInfo->pCliPrintString);
            pEditLine->indexCur = pEditLine->indexCur + pEditLine->numCharsToPrint;
        }
        pEditLine->numCharsToPrint = 0;

        /* Save the current cursor position */
        CliInsertControlChars(CLI_CTRL_SAVE);

        /* Print any characters that were after the cursor */
        if (pEditLine->indexCur < pEditLine->indexEnd)
        {
            snprintf(pInfo->pCliPrintString, pEditLine->indexEnd - pEditLine->indexCur + 1, "%s",
                     pEditLine->pBuffer + pEditLine->indexCur);
            adi_cli_PutStringNb(pInfo->pCliPrintString);
        }
        else
        {
            pEditLine->indexEnd = pEditLine->indexCur;
        }

        /* Restore the cursor to its saved position */
        CliInsertControlChars(CLI_CTRL_RESTORE);
    }
}

void CliFillCommandLine(EditLine *pEditLine, const char *pCommand)
{
    int32_t i;
    CliReset();
    for (i = 0; ((pCommand[i]) && (i < APP_CFG_CLI_MAX_CMD_LENGTH)); i++)
    {
        CliPutChar(pEditLine, pCommand[i]);
    }
    return;
}

void CliMoveCursorBackward(EditLine *pEditLine)
{
    if (pEditLine->indexCur > 0)
    {
        pEditLine->indexCur--;
        CliInsertControlChars(CLI_CTRL_PREV);
    }
}

void CliMoveCursorForward(EditLine *pEditLine)
{
    if ((pEditLine->indexCur < APP_CFG_CLI_MAX_CMD_LENGTH - 1) &&
        (pEditLine->indexCur < pEditLine->indexEnd))
    {
        pEditLine->indexCur++;
        CliInsertControlChars(CLI_CTRL_NEXT);
    }
}

void CliDeleteAllChars(EditLine *pEditLine)
{

    CliInsertControlChars(CLI_CTRL_KILL);
    pEditLine->indexEnd = pEditLine->indexCur;
}

void CliDeleteCharAtCursor(EditLine *pEditLine)
{
    uint32_t i;
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    volatile bool echo = pInfo->echo;
    if (pEditLine->indexCur > 0)
    {
        pEditLine->indexCur--;
        CliInsertControlChars(CLI_CTRL_PREV);
        CliInsertControlChars(CLI_CTRL_SAVE);
        for (i = pEditLine->indexCur; i < pEditLine->indexEnd - 1; i++)
        {
            pEditLine->pBuffer[i] = pEditLine->pBuffer[i + 1];
            if (echo == true)
            {
                /* Echoed  for all commands in both host/cli mode */
                adi_cli_PutChar(pEditLine->pBuffer[i]);
            }
        }
        adi_cli_PutChar(' ');
        CliInsertControlChars(CLI_CTRL_RESTORE);
        pEditLine->indexEnd--;
    }
}

void CliDisplayPrompt()
{
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    if (pInfo->displayCtrlChars == true)
    {
        CliInsertControlChars(CLI_CTRL_CR);
        CliPutStringBold(APP_CFG_CLI_PROMPT);
    }
}

static int32_t CliInsertControlChars(int32_t ctrlFuncID)
{
    int32_t status = 0;
    ADI_CLI_INFO *pCliInfo = GetCliInfo();
    CLI_INTERFACE *pInfo = &pCliInfo->cliIfData;
    volatile bool echo = pInfo->echo;
    if (pInfo->displayCtrlChars == true)
    {
        switch (ctrlFuncID)
        {
        case CLI_CTRL_CR:
            adi_cli_PutChar('\r');
            break;
        case CLI_CTRL_NEWLINE:
            adi_cli_PutStringNb("\r\n");
            break;
        default:
            if (echo == true)
            {
                status = CliInsertManualControlChars(ctrlFuncID);
            }
            break;
        }
    }

    return status;
}

static int32_t CliInsertManualControlChars(int32_t ctrlFuncID)
{
    int32_t status = 0;
    switch (ctrlFuncID)
    {
    case CLI_CTRL_ALERT:
        adi_cli_PutChar('\a');
        break;
    case CLI_CTRL_BOLD:
        adi_cli_PutStringNb("\x1B[1m");
        break;
    case CLI_CTRL_CLS:
        adi_cli_PutStringNb("\x1B[2J\x1B[H");
        break;
    case CLI_CTRL_KILL:
        adi_cli_PutStringNb("\x1B[K");
        break;
    case CLI_CTRL_PREV:
        adi_cli_PutStringNb("\x1B[1D");
        break;
    case CLI_CTRL_NEXT:
        adi_cli_PutStringNb("\x1B[1C");
        break;
    case CLI_CTRL_NORMAL:
        adi_cli_PutStringNb("\x1B[0m");
        break;
    case CLI_CTRL_RED:
        adi_cli_PutStringNb("\x1B[0;31m");
        break;
    case CLI_CTRL_RESTORE:
        adi_cli_PutStringNb("\x1B"
                            "8");
        break;
    case CLI_CTRL_SAVE:
        adi_cli_PutStringNb("\x1B"
                            "7");
        break;
    default:
        status = 1;
        break;
    }

    return status;
}

static int32_t CliPutStringBold(char *pString)
{
    int32_t status = 0;
    if (CliInsertControlChars(CLI_CTRL_BOLD) || adi_cli_PutStringNb(pString) ||
        CliInsertControlChars(CLI_CTRL_NORMAL))
    {
        status = 1;
    }
    return status;
}

/**
 * @}
 */
