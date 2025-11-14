/*!
 * \file            main.c
 * \author          SBG Systems
 * \date            October 27, 2020
 *
 * \brief           Tool to perform REST GET/POST queries through sbgECom commands.
 *
 * \copyright       Copyright (C) 2007-2024, SBG Systems SAS. All rights reserved.
 * \beginlicense    Proprietary license
 * 
 * This source code is intended for use only by SBG Systems SAS and
 * those that have explicit written permission to use it from
 * SBG Systems SAS.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 * 
 * \endlicense
 */

// sbgCommonLib headers
#include <sbgCommon.h>
#include <interfaces/sbgInterface.h>
#include <interfaces/sbgInterfaceSerial.h>
#include <interfaces/sbgInterfaceUdp.h>
#include <network/sbgNetwork.h>
#include <string/sbgString.h>

// sbgECom headers
#include <sbgECom.h>
#include <sbgEComGetVersion.h>
#include <commands/sbgEComCmdApi.h>

// Argtable3 headers
#include <argtable3.h>

//----------------------------------------------------------------------//
//- Constant definitions                                               -//
//----------------------------------------------------------------------//

/*!
 * Program name.
 */
#define PROGRAM_NAME                                        "sbgEComApi"

/*!
 * Default number of attempts per command execution.
 */
#define DEFAULT_CMD_NR_ATTEMPTS                             (3)

/*!
 * Default time-out per command execution attempt, in seconds.
 */
#define DEFAULT_CMD_TIMEOUT                                 (5)

//----------------------------------------------------------------------//
//- Structure definitions                                              -//
//----------------------------------------------------------------------//

/*!
 * Exit code descriptor.
 */
typedef struct _ExitCodeDesc
{
    int                                  exitCode;                                  /*!< Exit code, on 7 bits. */
    uint16_t                             statusCode;                                /*!< Status code. */
    const char                          *pMessage;                                  /*!< Message. */
} ExitCodeDesc;

//----------------------------------------------------------------------//
//- Private variables                                                  -//
//----------------------------------------------------------------------//

/*!
 * Table of exit code descriptors.
 */
static const ExitCodeDesc                gExitCodeDescs[] =
{
    {   .exitCode = 0,      .statusCode = 200,  .pMessage = "OK"                    },
    {   .exitCode = 60,     .statusCode = 400,  .pMessage = "BAD REQUEST"           },
    {   .exitCode = 61,     .statusCode = 401,  .pMessage = "UNAUTHORIZED"          },
    {   .exitCode = 63,     .statusCode = 403,  .pMessage = "FORBIDDEN"             },
    {   .exitCode = 64,     .statusCode = 404,  .pMessage = "NOT FOUND"             },
    {   .exitCode = 69,     .statusCode = 409,  .pMessage = "CONFLICT"              },
    {   .exitCode = 82,     .statusCode = 422,  .pMessage = "UNPROCESSABLE ENTITY"  },
    {   .exitCode = 100,    .statusCode = 200,  .pMessage = "INTERNAL SERVER ERROR" },
};

//----------------------------------------------------------------------//
//- Private functions                                                  -//
//----------------------------------------------------------------------//

/*!
 * Look up the exit code descriptor matching the given status code.
 *
 * \param[in]   statusCode                  Status code.
 * \return                                  Exit code descriptor.
 */
static const ExitCodeDesc *lookupExitCodeDesc(uint16_t statusCode)
{
    const ExitCodeDesc                  *pDesc = NULL;

    for (size_t i = 0; i < SBG_ARRAY_SIZE(gExitCodeDescs); i++)
    {
        const ExitCodeDesc              *pTmpDesc;

        pTmpDesc = &gExitCodeDescs[i];

        if (pTmpDesc->statusCode == statusCode)
        {
            pDesc = pTmpDesc;
            break;
        }
    }

    return pDesc;
}

/*!
 * Convert a status code into an exit code.
 *
 * \param[in]   statusCode                  Status code.
 * \return                                  Exit code.
 */
static int convertStatusCodeToExitCode(uint16_t statusCode)
{
    const ExitCodeDesc                  *pDesc;
    int                                  exitCode;

    pDesc = lookupExitCodeDesc(statusCode);

    if (pDesc)
    {
        exitCode = pDesc->exitCode;
    }
    else
    {
        exitCode = EXIT_FAILURE;
    }

    return exitCode;
}

/*!
 * Print help about a single exit code / status code mapping.
 *
 * \param[in]   pDesc                       Exit code descriptor.
 */
static void printExitCodeMapping(const ExitCodeDesc *pDesc)
{
    printf("  %3u: %3u %s\n", pDesc->exitCode, pDesc->statusCode, pDesc->pMessage);
}

/*!
 * Print help about exit codes.
 */
static void printExitCodeHelp(void)
{
    printf("Exit codes :\n");

    for (size_t i = 0; i < SBG_ARRAY_SIZE(gExitCodeDescs); i++)
    {
        printExitCodeMapping(&gExitCodeDescs[i]);
    }

    puts("");
    printf("EXIT_FAILURE for a general error unrelated to the status code or if the status code is unknown.\n\n");
}

/*!
 * Callback definition used to route log error messages.
 *
 * \param[in]   pFileName                   The file in which the log was triggered.
 * \param[in]   pFunctionName               The function where the log was triggered.
 * \param[in]   line                        The line in the file where the log was triggered.
 * \param[in]   pCategory                   Category for this log or "None" if no category has been specified.
 * \param[in]   logType                     Associated log message level.
 * \param[in]   errorCode                   Associated error code or SBG_NO_ERROR for INFO & DEBUG level logs.
 * \param[in]   pMessage                    The message to log.
 */
static void onLogCallback(const char *pFileName, const char *pFunctionName, uint32_t line, const char *pCategory, SbgDebugLogType type, SbgErrorCode errorCode, const char *pMessage)
{
    const char                          *pTypeStr;
    const char                          *pBaseName;

    assert(pFileName);
    assert(pFunctionName);
    assert(pCategory);
    assert(pMessage);

    pTypeStr    = sbgDebugLogTypeToStr(type);
    pBaseName   = strrchr(pFileName, '/');

    if (!pBaseName)
    {
        pBaseName = pFileName;
    }
    else
    {
        //
        // Skip the slash.
        //
        pBaseName++;
    }

    if (errorCode == SBG_NO_ERROR)
    {
        fprintf(stderr, "%-7s %s (%s:%" PRIu32 ") %s\n", pTypeStr, pFunctionName, pBaseName, line, pMessage);
    }
    else
    {
        fprintf(stderr, "%-7s err:%s %s (%s:%" PRIu32 ") %s\n", pTypeStr, sbgErrorCodeToString(errorCode), pFunctionName, pBaseName, line, pMessage);
    }
}

/*!
 * Read a string from a file.
 *
 * \param[out]  pString                     String.
 * \param[in]   pPath                       File path.
 * \return                                  SBG_NO_ERROR if successful.
 */
static SbgErrorCode readStringFromFile(SbgString *pString, const char *pPath)
{
    SbgErrorCode                         errorCode;
    FILE                                *pFile;

    assert(pString);
    assert(pPath);

    pFile = fopen(pPath, "r");

    if (pFile)
    {
        for (;;)
        {
            size_t                       size;
            char                         buffer[4096];

            size = fread(buffer, 1, sizeof(buffer) - 1, pFile);

            if (size != 0)
            {
                assert(size < sizeof(buffer));
                buffer[size] = '\0';

                errorCode = sbgStringAppendCString(pString, buffer);

                if (errorCode == SBG_NO_ERROR)
                {
                    char                *pPtr;

                    pPtr = memchr(buffer, '\0', size);

                    if (pPtr)
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                if (ferror(pFile))
                {
                    errorCode = SBG_READ_ERROR;
                    SBG_LOG_ERROR(errorCode, "unable to read file %s", pPath);
                }
                else
                {
                    errorCode = SBG_NO_ERROR;
                }

                break;
            }
        }

        fclose(pFile);
    }
    else
    {
        errorCode = SBG_INVALID_PARAMETER;
        SBG_LOG_ERROR(errorCode, "unable to open file %s", pPath);
    }

    return errorCode;
}

/*!
 * Write a reply to an output file.
 *
 * \param[in]   pReply                      Reply.
 * \param[in]   writeStatus                 If true, print the status to the output file.
 * \param[out]  pOutputFile                 Output file.
 */
static SbgErrorCode writeReply(const SbgEComCmdApiReply *pReply, bool writeStatus, FILE *pOutputFile)
{
    SbgErrorCode                         errorCode;

    if (writeStatus)
    {
        int                              nrChars;

        nrChars = fprintf(pOutputFile, "%"PRIu16"\n", pReply->statusCode);

        if (nrChars >= 0)
        {
            errorCode = SBG_NO_ERROR;
        }
        else
        {
            errorCode = SBG_WRITE_ERROR;
            SBG_LOG_ERROR(errorCode, "unable to write status code");
        }
    }
    else
    {
        errorCode = SBG_NO_ERROR;
    }

    if (errorCode == SBG_NO_ERROR)
    {
        int                              nrChars;

        nrChars = fprintf(pOutputFile, "%s", pReply->pContent);

        if (nrChars >= 0)
        {
            errorCode = SBG_NO_ERROR;
        }
        else
        {
            errorCode = SBG_WRITE_ERROR;
            SBG_LOG_ERROR(errorCode, "unable to write content");
        }
    }

    return errorCode;
}

/*!
 * Execute a GET or POST ECom command.
 *
 * \param[in]   pInterface                  Interface.
 * \param[in]   methodIsGet                 The GET command is executed if true, the POST command otherwise.
 * \param[in]   pPath                       URI path component.
 * \param[in]   nrAttempts                  Number of attempts.
 * \param[in]   timeout                     Time-out per attempt, in seconds.
 * \param[in]   pQuery                      Query string, may be NULL.
 * \param[in]   pBody                       Body, may be NULL.
 * \param[in]   writeStatus                 If true, print the status to the output file.
 * \param[out]  pStatusCode                 Status code.
 * \param[in]   pOutputFile                 Output file.
 * \return                                  SBG_NO_ERROR if successful.
 */
static SbgErrorCode execute(SbgInterface *pInterface, bool methodIsGet, const char *pPath, uint32_t nrAttempts, uint32_t timeout,
                            const char *pQuery, const char *pBody, bool writeStatus, uint16_t *pStatusCode, FILE *pOutputFile)
{
    SbgErrorCode                         errorCode;
    SbgEComHandle                        ecomHandle;

    assert(pPath);
    assert(pOutputFile);

    errorCode = sbgEComInit(&ecomHandle, pInterface);

    if (errorCode == SBG_NO_ERROR)
    {
        SbgEComCmdApiReply               reply;

        sbgEComSetCmdTrialsAndTimeOut(&ecomHandle, nrAttempts, timeout * 1000);

        sbgEComCmdApiReplyConstruct(&reply);

        if (methodIsGet)
        {
            errorCode = sbgEComCmdApiGet(&ecomHandle, pPath, pQuery, &reply);
        }
        else
        {
            errorCode = sbgEComCmdApiPost(&ecomHandle, pPath, pQuery, pBody, &reply);
        }

        if (errorCode == SBG_NO_ERROR)
        {
            errorCode = writeReply(&reply, writeStatus, pOutputFile);

            if (errorCode == SBG_NO_ERROR)
            {
                *pStatusCode = reply.statusCode;
            }
        }
        else
        {
            SBG_LOG_ERROR(errorCode, "unable to execute command");
        }

        sbgEComCmdApiReplyDestroy(&reply);

        if (errorCode == SBG_NO_ERROR)
        {
            errorCode = sbgEComClose(&ecomHandle);
        }
        else
        {
            sbgEComClose(&ecomHandle);
        }
    }

    return errorCode;
}

//----------------------------------------------------------------------//
// Public functions                                                     //
//----------------------------------------------------------------------//

/*!
 * Program entry point.
 *
 * \param[in]   argc                        Number of input arguments.
 * \param[in]   argv                        Input arguments as an array of strings.
 * \return                                  EXIT_SUCCESS if successful.
 */
int main(int argc, char **argv)
{
    int                                  exitCode = EXIT_SUCCESS;
    bool                                 printHelp = false;

    struct arg_lit                      *pHelpArg;
    struct arg_lit                      *pVersionArg;

    struct arg_str                      *pUdpAddrArg;
    struct arg_int                      *pUdpPortInArg;
    struct arg_int                      *pUdpPortOutArg;

    struct arg_str                      *pSerialDeviceArg;
    struct arg_int                      *pSerialBaudrateArg;

    struct arg_int                      *pNrAttemptsArg;
    struct arg_int                      *pTimeoutArg;

    struct arg_lit                      *pGetMethodArg;
    struct arg_lit                      *pPostMethodArg;
    struct arg_str                      *pPathArg;
    struct arg_str                      *pQueryArg;
    struct arg_str                      *pBodyArg;
    struct arg_file                     *pBodyFileArg;
    struct arg_lit                      *pPrintStatus;
    struct arg_file                     *pOutputFileArg;
    struct arg_end                      *pEndArg;

    //
    // TODO: add support for network interfaces.
    //
    void                                *argTable[] =
    {
        pHelpArg            = arg_lit0(     NULL,   "help",                                 "display this help and exit"),
        pVersionArg         = arg_lit0(     NULL,   "version",                              "display version info and exit"),
        
        pUdpAddrArg         = arg_str0(     "a",    "addr-ip",          "IP address",       "open an UDP interface"),
        pUdpPortInArg       = arg_int0(     "I",    "udp-port-in",      "UDP port in",      "UDP port to receive data from (local)"),
        pUdpPortOutArg      = arg_int0(     "O",    "udp-port-out",     "UDP port out",     "UDP port to send data to (remote)"),

        pSerialDeviceArg    = arg_str0(     "s",    "serial-device",    "SERIAL_DEVICE",    "open a serial interface"),
        pSerialBaudrateArg  = arg_int0(     "r",    "serial-baudrate",  "SERIAL_BAUDRATE",  "serial baudrate"),

        pNrAttemptsArg      = arg_int0(     "n",    "nr-attempts",      "NR_ATTEMPTS",      "number of transaction attempts"),
        pTimeoutArg         = arg_int0(     "t",    "timeout",          "TIMEOUT",          "reply time-out, in seconds"),

        pGetMethodArg       = arg_lit0(     "g",    "method-get",                           "use the GET method (default)"),
        pPostMethodArg      = arg_lit0(     "p",    "method-post",                          "use the POST method"),
        pQueryArg           = arg_str0(     "q",    "query",            "QUERY",            "query string, format=pretty&delta=true, format and delta options are optional"),
        pBodyArg            = arg_str0(     "b",    "body",             "BODY",             "body (POST method only)"),
        pBodyFileArg        = arg_file0(    "B",    "body-file",        "BODY_FILE",        "file containing the body (POST method only)"),
        pPrintStatus        = arg_lit0(     "S",    "print-status",                         "print the status code on the output stream"),
        pOutputFileArg      = arg_file0(    "o",    "output-file",      "OUTPUT_FILE",      "output file"),
        pPathArg            = arg_str1(     NULL,   NULL,               "PATH",             "GET or POST request path endpoint"),

        pEndArg             = arg_end(20),
    };

    sbgCommonLibSetLogCallback(onLogCallback);

    if (arg_nullcheck(argTable) == 0)
    {
        int                              argError;

        argError = arg_parse(argc, argv, argTable);

        if (pHelpArg->count != 0)
        {
            printf("Usage: %s", PROGRAM_NAME);
            arg_print_syntax(stdout, argTable, "\n\n");
            

            printf("Access a RESTful SBG ECom server.\n\n");
            printf("    Serial example: %s -s <SERIAL-PORT> -r <BAUDRATE> api/v1/settings -g\n", PROGRAM_NAME);
            printf("    UDP example:    %s -a <IP_ADDR> -I <UDP_PORT_IN> -O <UDP_PORT_OUT> api/v1/settings -g\n", PROGRAM_NAME);

            puts("");
            
            arg_print_glossary(stdout, argTable, "  %-50s %s\n");

            puts("");
            printf("BODY or BODY_FILE may only be provided when using the POST method.\n");

            puts("");
            printf("If provided, BODY_FILE may not contain binary data.\n");

            puts("");
            printf("PATH is a URI path component such as api/v1/settings\n");

            puts("");
            printExitCodeHelp();
        }
        else if (pVersionArg->count != 0)
        {
            printf("%s\n", sbgEComGetVersionAsString());
        }
        else if (argError == 0)
        {
            SbgInterface                 ecomInterface;
            bool                         methodIsGet = true;
            int                          nrAttempts = DEFAULT_CMD_NR_ATTEMPTS;
            int                          timeout = DEFAULT_CMD_TIMEOUT;
            bool                         writeStatus = false;
            SbgString                    bodyStorage;
            SbgString                   *pBody = NULL;
            const char                  *pQuery = NULL;
            const char                  *pPath = NULL;

            if (exitCode == EXIT_SUCCESS)
            {
                sbgStringConstructEmpty(&bodyStorage);
                pBody = &bodyStorage;
            }

            if (exitCode == EXIT_SUCCESS)
            {
                bool                hasSerialConf   = false;
                bool                hasUdpConf      = false;
                
                //
                // Can't open at the same time a serial and UDP interface so check it
                //
                if ( (pSerialDeviceArg->count != 0) && (pSerialBaudrateArg->count != 0) )
                {
                    hasSerialConf = true;
                }

                if ( (pUdpAddrArg->count != 0) && (pUdpPortInArg->count != 0) && (pUdpPortOutArg->count != 0) )
                {
                    hasUdpConf = true;
                }

                if ( (hasSerialConf && !hasUdpConf) || (!hasSerialConf && hasUdpConf) )
                {
                    SbgErrorCode         errorCode;

                    if (hasSerialConf)
                    {
                        errorCode = sbgInterfaceSerialCreate(&ecomInterface, pSerialDeviceArg->sval[0], pSerialBaudrateArg->ival[0]);
                    }
                    else if (hasUdpConf)
                    {
                        errorCode = sbgInterfaceUdpCreate(&ecomInterface, sbgNetworkIpFromString(pUdpAddrArg->sval[0]), pUdpPortOutArg->ival[0], pUdpPortInArg->ival[0]);

                        if (errorCode == SBG_NO_ERROR)
                        {
                            //
                            // Enable connected mode to only send/receive commands to the designed host
                            //
                            sbgInterfaceUdpSetConnectedMode(&ecomInterface, true);
                        }
                    }
                    else
                    {
                        errorCode = SBG_INVALID_PARAMETER;
                    }

                    if (errorCode == SBG_NO_ERROR)
                    {
                        FILE                        *pOutputFile = NULL;

                        if ((pGetMethodArg->count != 0) && (pPostMethodArg->count == 0))
                        {
                            methodIsGet = true;
                        }
                        else if ((pGetMethodArg->count == 0) && (pPostMethodArg->count != 0))
                        {
                            methodIsGet = false;
                        }
                        else if ((pGetMethodArg->count != 0) && (pPostMethodArg->count != 0))
                        {
                            exitCode    = EXIT_FAILURE;
                            printHelp   = true;
                        }

                        if (exitCode == EXIT_SUCCESS)
                        {
                            if (pNrAttemptsArg->count != 0)
                            {
                                nrAttempts = pNrAttemptsArg->ival[0];

                                if (nrAttempts <= 0)
                                {
                                    exitCode    = EXIT_FAILURE;
                                    printHelp   = true;
                                }
                            }

                            if (pTimeoutArg->count != 0)
                            {
                                timeout = pTimeoutArg->ival[0];

                                if (timeout <= 0)
                                {
                                    exitCode    = EXIT_FAILURE;
                                    printHelp   = true;
                                }
                            }

                            if (pQueryArg->count != 0)
                            {
                                pQuery = pQueryArg->sval[0];
                            }
                        }

                        if (exitCode == EXIT_SUCCESS)
                        {
                            if (!methodIsGet && (pBodyArg->count != 0) && (pBodyFileArg->count == 0))
                            {
                                errorCode = sbgStringAssignCString(pBody, pBodyArg->sval[0]);

                                if (errorCode != SBG_NO_ERROR)
                                {
                                    exitCode = EXIT_FAILURE;
                                }
                            }
                            else if (!methodIsGet && (pBodyArg->count == 0) && (pBodyFileArg->count != 0))
                            {
                                errorCode = readStringFromFile(&bodyStorage, pBodyFileArg->filename[0]);

                                if (errorCode != SBG_NO_ERROR)
                                {
                                    exitCode = EXIT_FAILURE;
                                }
                            }
                            else if ((pBodyArg->count != 0) && (pBodyFileArg->count != 0))
                            {
                                exitCode    = EXIT_FAILURE;
                                printHelp   = true;
                            }
                        }

                        if (exitCode == EXIT_SUCCESS)
                        {
                            if (pPrintStatus->count != 0)
                            {
                                writeStatus = true;
                            }
                        }

                        if (exitCode == EXIT_SUCCESS)
                        {
                            if (pOutputFileArg->count == 0)
                            {
                                pOutputFile = stdout;
                            }
                            else
                            {
                                pOutputFile = fopen(pOutputFileArg->filename[0], "w");

                                if (!pOutputFile)
                                {
                                    SBG_LOG_ERROR(SBG_ERROR, "unable to open %s", pOutputFileArg->filename[0]);
                                    exitCode = EXIT_FAILURE;
                                }
                            }
                        }

                        if (exitCode == EXIT_SUCCESS)
                        {
                            assert(pPathArg->count != 0);

                            pPath = pPathArg->sval[0];
                        }

                        if (exitCode == EXIT_SUCCESS)
                        {
                            const char              *pBodyCString;
                            uint16_t                 statusCode;

                            if (pBody)
                            {
                                pBodyCString = sbgStringGetCString(pBody);
                            }
                            else
                            {
                                pBodyCString = NULL;
                            }

                            errorCode = execute(&ecomInterface, methodIsGet, pPath, nrAttempts, timeout, pQuery, pBodyCString, writeStatus, &statusCode, pOutputFile);

                            if (errorCode == SBG_NO_ERROR)
                            {
                                exitCode = convertStatusCodeToExitCode(statusCode);
                            }
                            else
                            {
                                exitCode = EXIT_FAILURE;
                            }
                        }

                        if (pOutputFile && (pOutputFile != stdout))
                        {
                            int                      result;

                            result = fclose(pOutputFile);

                            if (result != 0)
                            {
                                SBG_LOG_ERROR(SBG_WRITE_ERROR, "unable to close %s", pOutputFileArg->filename[0]);
                                exitCode = EXIT_FAILURE;
                            }
                        }

                        sbgInterfaceDestroy(&ecomInterface);
                    }
                    else
                    {
                        SBG_LOG_ERROR(errorCode, "unable to open the serial or UDP interface");
                        exitCode = EXIT_FAILURE;
                    }
                }
                else if (hasSerialConf && hasUdpConf)
                {
                    SBG_LOG_ERROR(SBG_ERROR, "please select either a serial or an UDP interface no both");
                    exitCode    = EXIT_FAILURE;
                }
                else
                {
                    exitCode    = EXIT_FAILURE;
                    printHelp   = true;
                }
            }

            if (pBody)
            {
                sbgStringDestroy(pBody);
            }
        }
        else
        {
            printHelp = true;
        }

        if (printHelp)
        {
            arg_print_errors(stderr, pEndArg, PROGRAM_NAME);
            fprintf(stderr, "Try '%s --help' for more information.\n", PROGRAM_NAME);
            exitCode = EXIT_FAILURE;
        }

        arg_freetable(argTable, SBG_ARRAY_SIZE(argTable));
    }
    else
    {
        SBG_LOG_ERROR(SBG_MALLOC_FAILED, "unable to allocate memory");
        exitCode = EXIT_FAILURE;
    }

    return exitCode;
}
