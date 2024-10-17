/*!
 * \file			hpInsMinimal.c
 * \author			SBG Systems
 * \date			06/05/2015
 *
 * \brief			C example to read euler angles from a high performance INS over Ethernet
 *
 * This small C example demonstrates how to initialize the sbgECom library
 * with an Ethernet UDP interface and parse incoming SBG_ECOM_LOG_EKF_EULER logs.
 * 
 * The INS has to be correctly setup to output the SBG_ECOM_LOG_EKF_EULER log
 * on the selected UDP port.
 *
 * \copyright		Copyright (C) 2022, SBG Systems SAS. All rights reserved.
 * \beginlicense	The MIT license
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * \endlicense
 */

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgEComLib.h>

//----------------------------------------------------------------------//
//- Private methods                                                    -//
//----------------------------------------------------------------------//

/*!
 *	Callback definition called each time a new log is received.
 * 
 *	\param[in]	pHandle									Valid handle on the sbgECom instance that has called this callback.
 *	\param[in]	msgClass								Class of the message we have received
 *	\param[in]	msg										Message ID of the log received.
 *	\param[in]	pLogData								Contains the received log data as an union.
 *	\param[in]	pUserArg								Optional user supplied argument.
 *	\return												SBG_NO_ERROR if the received log has been used successfully.
 */
static SbgErrorCode onLogReceived(SbgEComHandle *pHandle, SbgEComClass msgClass, SbgEComMsgId msg, const SbgEComLogUnion *pLogData, void *pUserArg)
{
	SBG_UNUSED_PARAMETER(pHandle);
	SBG_UNUSED_PARAMETER(pUserArg);

	if (msgClass == SBG_ECOM_CLASS_LOG_ECOM_0)
	{
		//
		// Handle separately each received data according to the log ID
		//
		switch (msg)
		{
		case SBG_ECOM_LOG_EKF_EULER:
			//
			// Simply display euler angles in real time
			//
			printf("Euler Angles: %3.1f\t%3.1f\t%3.1f\tStd Dev:%3.1f\t%3.1f\t%3.1f   \n",
				sbgRadToDegf(pLogData->ekfEulerData.euler[0]), sbgRadToDegf(pLogData->ekfEulerData.euler[1]), sbgRadToDegf(pLogData->ekfEulerData.euler[2]),
				sbgRadToDegf(pLogData->ekfEulerData.eulerStdDev[0]), sbgRadToDegf(pLogData->ekfEulerData.eulerStdDev[1]), sbgRadToDegf(pLogData->ekfEulerData.eulerStdDev[2]));
			break;

		default:
			break;
		}
	}

	return SBG_NO_ERROR;
}

/*!
 * Execute the hpInsMinimal example given an opened and valid interface.
 * 
 * \param[in]	pInterface							Interface used to communicate with the device.
 * \return											SBG_NO_ERROR if successful.
 */
static SbgErrorCode hpInsMinimalProcess(SbgInterface *pInterface)
{
	SbgErrorCode			errorCode = SBG_NO_ERROR;
	SbgEComHandle			comHandle;
		
	assert(pInterface);

	//
	// Create the sbgECom library and associate it with the created interfaces
	//
	errorCode = sbgEComInit(&comHandle, pInterface);

	//
	// Test that the sbgECom has been initialized
	//
	if (errorCode == SBG_NO_ERROR)
	{
		//
		// Welcome message
		//
		printf("Welcome to the High Performance INS minimal example.\n");
		printf("sbgECom version %s\n\n", SBG_E_COM_VERSION_STR);

		printf("Euler Angles display with estimated standard deviation.\n");

		//
		// Define callbacks for received data
		//
		sbgEComSetReceiveLogCallback(&comHandle, onLogReceived, NULL);

		//
		// Loop until the user exist
		//
		while (1)
		{
			//
			// Try to read a frame
			//
			errorCode = sbgEComHandle(&comHandle);

			//
			// Test if we have to release some CPU (no frame received)
			//
			if (errorCode == SBG_NOT_READY)
			{
				//
				// Release CPU
				//
				sbgSleep(1);
			}
			else
			{
				SBG_LOG_ERROR(errorCode, "Unable to process incoming sbgECom logs");
			}
		}

		//
		// Close the sbgECom library
		//
		sbgEComClose(&comHandle);
	}
	else
	{
		SBG_LOG_ERROR(errorCode, "Unable to initialize the sbgECom library");
	}

	return errorCode;
}

//----------------------------------------------------------------------//
//  Main program                                                        //
//----------------------------------------------------------------------//

/*!
 * Program entry point usage: ekinoxMinimal REMOTE_ID_ADDR REMOTE_UDP_PORT HOST_UDP_PORT
 *
 * Establish an UDP connection with a High Performance INS product:
 *	REMOTE_ID_ADDR:		is the INS IP address to connect to in the form 192.168.1.1
 *	REMOTE_UDP_PORT:	is the UDP port on which the INS is listening
 *	HOST_UDP_PORT:		is the UDP port on which the INS is sending data
 * 
 * \param[in]	argc					Number of input arguments.
 * \param[in]	argv					Input arguments as an array of strings.
 * \return								EXIT_SUCCESS if successful.
 */
int main(int argc, char **argv)
{
	SbgErrorCode		errorCode =SBG_NO_ERROR;
	SbgInterface		sbgInterface;
	int					exitCode;

	SBG_UNUSED_PARAMETER(argc);
	SBG_UNUSED_PARAMETER(argv);

	if (argc == 4)
	{
		//
		// Create a serial interface to communicate with the PULSE
		//
		errorCode = sbgInterfaceUdpCreate(&sbgInterface, sbgNetworkIpFromString(argv[1]), atoi(argv[2]), atoi(argv[3]));

		if (errorCode == SBG_NO_ERROR)
		{
			errorCode = hpInsMinimalProcess(&sbgInterface);

			if (errorCode == SBG_NO_ERROR)
			{
				exitCode = EXIT_SUCCESS;
			}
			else
			{
				exitCode = EXIT_FAILURE;
			}

			sbgInterfaceDestroy(&sbgInterface);
		}
		else
		{
			SBG_LOG_ERROR(errorCode, "unable to open serial interface");
			exitCode = EXIT_FAILURE;
		}
	}
	else
	{
		printf("Invalid input arguments, usage: hpInsMinimal REMOTE_ID_ADDR REMOTE_UDP_PORT HOST_UDP_PORT\n");
		exitCode = EXIT_FAILURE;
	}
	
	return exitCode;
}
