/*!
 * \file			main.cpp
 * \author			SBG Systems
 * \date			March 22, 2023
 *
 * \brief			Simple console application that parse and output sbgECom logs to CSV like files.
 *
 * \copyright		Copyright (C) 2007-2024, SBG Systems SAS. All rights reserved.
 * \beginlicense	Proprietary license
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

// Standard Library
#include <signal.h>

// STL headers
#include <iostream>
#include <iomanip>
#include <thread>

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgECom.h>
#include <sbgEComGetVersion.h>


// Local headers
#include "loggerApp.h"

//----------------------------------------------------------------------//
//- Global variables                                                   -//
//----------------------------------------------------------------------//

/*!
 * Clear this flag to cancel current processing.
 */
std::atomic_flag	gShouldContinue	= ATOMIC_FLAG_INIT;

/*!
 * Flag cleared once the processing has been terminated (either canceled or completed).
 */
std::atomic_flag	gProgramRunning	= ATOMIC_FLAG_INIT;

/*!
 * Stores the main running thread to detect if signal handlers are called from an other thread.
 */
std::thread::id		gMainThreadId;

//----------------------------------------------------------------------//
//- Private functions                                                  -//
//----------------------------------------------------------------------//

/*!
 * Callback definition used to route log error messages.
 *
 * \param[in]	pFileName				The file in which the log was triggered.
 * \param[in]	pFunctionName			The function where the log was triggered.
 * \param[in]	line					The line in the file where the log was triggered.
 * \param[in]	pCategory				Category for this log or "None" if no category has been specified.
 * \param[in]	type					Associated log message type.
 * \param[in]	errorCode				Associated error code or SBG_NO_ERROR for INFO & DEBUG level logs.
 * \param[in]	pMessage				The message to log.
 */
static void onMessageCallback(const char *pFileName, const char *pFunctionName, uint32_t line, const char *pCategory, SbgDebugLogType logType, SbgErrorCode errorCode, const char *pMessage)
{
	assert(pFunctionName);
	assert(pCategory);
	assert(pMessage);

	SBG_UNUSED_PARAMETER(pFileName);
	SBG_UNUSED_PARAMETER(pCategory);

	switch (logType)
	{
	case SBG_DEBUG_LOG_TYPE_ERROR:
		std::cout << "*ERR * " << std::setw(20) << std::left << sbgErrorCodeToString(errorCode) << " " << pFunctionName << "(" << line << "): " << pMessage << std::endl;
		break;
	case SBG_DEBUG_LOG_TYPE_WARNING:
		std::cout << "*WARN* " << std::setw(20) << std::left << sbgErrorCodeToString(errorCode) << " " << pFunctionName << "(" << line << "): " << pMessage << std::endl;
		break;
	case SBG_DEBUG_LOG_TYPE_INFO:
		std::cout << "*INFO* " << pFunctionName << "(" << line << "): " << pMessage << std::endl;
		break;
	case SBG_DEBUG_LOG_TYPE_DEBUG:
		std::cout << "*DBG * " << pFunctionName << "(" << line << "): " << pMessage << std::endl;
		break;
	default:
		std::cout << "*UKN * " << std::setw(20) << std::left << sbgErrorCodeToString(errorCode) << " " << pFunctionName << "(" << line << "): " << pMessage << std::endl;
		break;
	}
}

/*!
 * Intercept Ctrl+C events and Terminal close events to exit gracefully.
 * 
 * \param[in]	signalId				Signal identifier that has triggered the event.
 */
static void signalHandler(int signalId)
{
	SBG_UNUSED_PARAMETER(signalId);

	//
	// Notify processing (main thread) to stop processing through continueProcessingCb
	//
	gShouldContinue.clear();

	//
	// On Windows platforms the signal handler is called from an other thread
	// than the main running thread.
	// 
	// Once the flag is cleared, we should wait until the application exits gracefully.
	// This is required to properly flush and close all opened files.
	// 
	// However, on Unix platforms, the signal handler is called from the same thread.
	// In this case, we must skip this test as it will wait forever
	//
	if (gMainThreadId != std::this_thread::get_id())
	{
		//
		// Wait until the program has exited gracefully
		//
		while (gProgramRunning.test_and_set());
	}
}

/*!
 * Register all signals events used to exit the logger gracefully
 */
static void registerSignalsEvents()
{	
	signal(SIGINT,		signalHandler);
	signal(SIGABRT,		signalHandler);
	signal(SIGTERM,		signalHandler);

#ifdef SIGHUP
	signal(SIGHUP,		signalHandler);
#endif
	
#ifdef SIGBREAK
	signal(SIGBREAK,	signalHandler);
#endif
}

/*!
 * Function periodically called by the logger to continue or cancel processing.
 * 
 * \return								true to continue the processing
 *										false to immediately stop the processing.
 */
static bool continueProcessingCb()
{
	return gShouldContinue.test_and_set();
}

//----------------------------------------------------------------------//
// Public functions                                                     //
//----------------------------------------------------------------------//

/*!
 * Program entry point.
 *
 * \param[in]	argc					Number of input arguments.
 * \param[in]	argv					Input arguments as an array of strings.
 * \return								EXIT_SUCCESS if successful.
 */
int main(int argc, char **argv)
{
	sbg::CLoggerApp		loggerApp;
	int					retCode;

	//
	// Register sbgECom generic error logger to print to cout
	//
	sbgCommonLibSetLogCallback(onMessageCallback);

	//
	// Register console signal events to intercept exit / CTRL-C signals
	// and exit the application gracefully.
	//
	// This is required to properly flush and close output file. 
	//
	gMainThreadId = std::this_thread::get_id();
	registerSignalsEvents();

	//
	// Register a callback with a atomic flag used to cancel processing at any time
	//
	gShouldContinue.test_and_set();
	loggerApp.setContinueCallback(continueProcessingCb);

	//
	// Start the main console application and notify when processing has exited
	//
	gProgramRunning.test_and_set();
	retCode = loggerApp.exec(argc, argv);
	gProgramRunning.clear();

	return retCode;
}
