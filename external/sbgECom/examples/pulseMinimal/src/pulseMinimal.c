/*!
 * \file			pulseMinimal.c
 * \author			SBG Systems
 * \date			July 27, 2021
 *
 * \brief			C example that simply opens a pulse interface and reads IMU data from it.
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
//- Constant definitions                                               -//
//----------------------------------------------------------------------//

/*!
 * IMU short period to configure, in ms.
 */
#define PULSE_MINIMAL_IMU_SHORT_PERIOD							(100)

//----------------------------------------------------------------------//
//- Private methods                                                    -//
//----------------------------------------------------------------------//

/*!
 * Callback used to handle received logs.
 *
 * \param[in]	pECom					SbgECom instance.
 * \param[in]	msgClass				Class of the received message.
 * \param[in]	msg						Received message ID.
 * \param[in]	pLogData				Received data.
 * \param[in]	pUserArg				Optional user argument.
 * \return								SBG_NO_ERROR if successful.
 */
static SbgErrorCode pulseMinimalOnLogReceived(SbgEComHandle *pECom, SbgEComClass msgClass, SbgEComMsgId msg, const SbgEComLogUnion *pLogData, void *pUserArg)
{
	SBG_UNUSED_PARAMETER(pECom);
	SBG_UNUSED_PARAMETER(pUserArg);

	assert(pLogData);

	if (msgClass == SBG_ECOM_CLASS_LOG_ECOM_0)
	{
		switch (msg)
		{
		case SBG_ECOM_LOG_IMU_SHORT:
			printf("%" PRIu32 "\t%" PRIu16 "\t%0.3lf\t%0.12lf\t%0.12lf\t%0.12lf\t%0.12lf\t%0.12lf\t%0.12lf\n",
				pLogData->imuShort.timeStamp,
				pLogData->imuShort.status,
				((double)(pLogData->imuShort.temperature) / 256.0),
				((double)pLogData->imuShort.deltaVelocity[0] / 1048576.0),
				((double)pLogData->imuShort.deltaVelocity[1] / 1048576.0 ),
				((double)pLogData->imuShort.deltaVelocity[2] / 1048576.0 ),
				sbgRadToDegd(((double)pLogData->imuShort.deltaAngle[0] / 67108864.0)),
				sbgRadToDegd(((double)pLogData->imuShort.deltaAngle[1] / 67108864.0 )),
				sbgRadToDegd(((double)pLogData->imuShort.deltaAngle[2] / 67108864.0 )));
			break;

		default:
			break;
		}
	}

	return SBG_NO_ERROR;
}

/*!
 * Receive logs.
 *
 * \param[in]	pECom					SbgECom instance.
 */
static void pulseMinimalReceive(SbgEComHandle *pECom)
{
	assert(pECom);

	sbgEComSetReceiveLogCallback(pECom, pulseMinimalOnLogReceived, NULL);

	while (1)
	{
		SbgErrorCode					 errorCode;

		errorCode = sbgEComHandle(pECom);

		if (errorCode == SBG_NOT_READY)
		{
			sbgSleep(1);
		}
	}
}

/*!
 * Get and print product info.
 *
 * \param[in]	pECom					SbgECom instance.
 * \return								SBG_NO_ERROR if successful.
 */
static SbgErrorCode pulseMinimalGetAndPrintProductInfo(SbgEComHandle *pECom)
{
	SbgEComCmdApiReply				 reply;
	SbgErrorCode					 errorCode;

	assert(pECom);

	sbgEComCmdApiReplyConstruct(&reply);

	errorCode = sbgEComCmdApiGet(pECom, "/api/v1/info", NULL, &reply);

	if (errorCode == SBG_NO_ERROR)
	{
		char							 calibVersion[32];
		char							 productCode[32];
		char							 serialNumber[32];
		char							 hwRevision[32];
		char							 btVersion[32];
		char							 fmwVersion[32];
		int								 ret;

		//
		// This is a naive and simplistic way to parse a JSON content.
		// It is recommended to use a true JSON parser.
		// The cJson library can help you with this.
		//
		ret = sscanf(reply.pContent, "{"									\
										"\"productCode\":\"%[^\"]\","		\
										"\"serialNumber\":\"%[^\"]\","		\
										"\"hwRevision\":\"%[^\"]\","		\
										"\"calibVersion\":\"%[^\"]\","		\
										"\"fmwVersion\":\"%[^\"]\","		\
										"\"btVersion\":\"%[^\"]\","			\
									"}", productCode, serialNumber, hwRevision, calibVersion, fmwVersion, btVersion);
		
		if (ret == 6)
		{
			printf("       product code: %s\n", productCode);
			printf("      serial number: %s\n", serialNumber);
			printf("  hardware revision: %s\n", hwRevision);
			printf("   firmware version: %s\n", fmwVersion);
			printf(" bootLoader version: %s\n", btVersion);
			printf("calibration version: %s\n", calibVersion);
			printf("\n");
		}
		else
		{
			errorCode = SBG_INVALID_PARAMETER;
			SBG_LOG_ERROR(errorCode, "Received JSON is malformed");
		}
	}
	else
	{
		SBG_LOG_ERROR(errorCode, "unable to retrieve product info");
	}

	sbgEComCmdApiReplyDestroy(&reply);

	return errorCode;
}

/*!
 * Parse an error JSON payload to extract each field.
 *
 * This is a naive implementation only for demonstration purposes.
 * It should NOT be used in production environments.
 * 
 * \param[in]	pContent					The JSON payload to parse
 * \param[out]	pStatus						Extracted 'status' field from the JSON payload
 * \param[out]	pTitle						Extracted 'title' field from the JSON payload 
 * \param[out]	pDetail						Extracted 'detail' field from the JSON payload.
 */
static SbgErrorCode pulseMinimalJsonParseError(const char* pContent, uint32_t *pStatus, char *pTitle, size_t titleMaxSize, char *pDetail, size_t detailMaxSize)
{
	SbgErrorCode	errorCode = SBG_NO_ERROR;
	char			formatStr[128];
	int				ret;

	assert(pContent);
	assert(pStatus);
	assert(pTitle);
	assert(titleMaxSize > 0);
	assert(pDetail);	
	assert(detailMaxSize > 0);

	//
	// Create a sscanf format string with built in width to avoid buffer overflows
	// This is a naive implementation and sscanf should not be used to correctly address buffer overflows.
	//
	ret = sprintf(formatStr,	"{"									\
									"\"status\": %%"PRIu32","		\
									"\"title\":\"%%%zu[^\"]\","		\
									"\"detail\":\"%%%zu[^\"]\","	\
								"}", titleMaxSize, detailMaxSize);

	if (ret > 0)
	{		
		//
		// This is a naive and simplistic way to parse a JSON content.
		// It is recommended to use a true JSON parser.
		// The cJson library can help you with this.
		//
		ret = sscanf(pContent, formatStr, pStatus, pTitle,  pDetail);

		if (ret != 3)
		{
			errorCode = SBG_INVALID_PARAMETER;
			SBG_LOG_ERROR(errorCode, "JSON payload malformed");
		}
	}
	else
	{
		errorCode = SBG_ERROR;
		SBG_LOG_ERROR(errorCode, "Unable to generate sscanf format string");
	}

	return errorCode;
}

/*!
 * Change IMU short output rate.
 *
 * \param[in]	pECom					SbgECom instance.
 * \param[in]	period					IMU short period.
 * \return								SBG_NO_ERROR if successful.
 */
static SbgErrorCode pulseMinimalChangeImuShortOutputRate(SbgEComHandle *pECom, uint32_t period)
{
	SbgEComCmdApiReply				 reply;
	SbgErrorCode 					 errorCode;
	char							 periodStr[32];
	int								 ret;

	assert(pECom);

	sbgEComCmdApiReplyConstruct(&reply);

	//
	// This is a naive and simplistic way to build a JSON string.
	// It is recommended to use a true JSON generator.
	// The cJson library can help you with this.
	//
	ret = snprintf(periodStr, sizeof(periodStr), "\"%"PRIu32"ms\"", period);

	if (ret >= 0)
	{
		errorCode = sbgEComCmdApiPost(pECom, "/api/v1/settings/output/comA/messages/imuShort", NULL, periodStr, &reply);

		if (errorCode == SBG_NO_ERROR)
		{
			//
			// Check the reply status code that is based on HTTP codes (200 - OK)
			//
			if (reply.statusCode == 200)
			{
				SBG_LOG_INFO("IMU Short output period successfully configured");
			}
			else
			{
				uint32_t		status;
				char			title[64];
				char			details[128];

				//
				// Parse an error payload using a naive and simplistic approach
				// You should rather use a true JSON parser such as cJson library
				//
				errorCode = pulseMinimalJsonParseError(reply.pContent, &status, title, sizeof(title), details, sizeof(details));

				if (errorCode == SBG_NO_ERROR)
				{
					errorCode = SBG_INVALID_PARAMETER;
					SBG_LOG_ERROR(errorCode, "unable to configure IMU short log each %"PRIu32" ms", period);

					printf("     status: %"PRIu32"\n",	status);
					printf("      title: %s\n",			title);
					printf("    details: %s\n",			details);
					printf("\n");
				}
			}
		}
		else
		{
			SBG_LOG_ERROR(errorCode, "unable to configure IMU short log at 10 Hz");
		}

		sbgEComCmdApiReplyDestroy(&reply);
	}
	else
	{
		errorCode = SBG_INVALID_PARAMETER;
		SBG_LOG_ERROR(errorCode, "unable to parse period (%"PRIu32")", period);
	}	

	return errorCode;
}

/*!
 * Process.
 *
 * \param[in]	pInterface				Interface.
 * \return								SBG_NO_ERROR if successful.
 */
static SbgErrorCode pulseMinimalProcess(SbgInterface *pInterface)
{
	SbgErrorCode						 errorCode;
	SbgEComHandle						 comHandle;

	assert(pInterface);

	errorCode = sbgEComInit(&comHandle, pInterface);

	if (errorCode == SBG_NO_ERROR)
	{
		errorCode = pulseMinimalGetAndPrintProductInfo(&comHandle);

		if (errorCode == SBG_NO_ERROR)
		{
			errorCode = pulseMinimalChangeImuShortOutputRate(&comHandle, PULSE_MINIMAL_IMU_SHORT_PERIOD);
		}

		if (errorCode == SBG_NO_ERROR)
		{
			pulseMinimalReceive(&comHandle);
		}

		sbgEComClose(&comHandle);
	}

	return errorCode;
}

//----------------------------------------------------------------------//
//  Main program                                                        //
//----------------------------------------------------------------------//

/*!
 * Program entry point usage: pulseMinimal COM1 921600
 *
 * \param[in]	argc					Number of input arguments.
 * \param[in]	argv					Input arguments as an array of strings.
 * \return								EXIT_SUCCESS if successful.
 */
int main(int argc, char **argv)
{
	SbgErrorCode		errorCode = SBG_NO_ERROR;
	SbgInterface		sbgInterface;
	int					exitCode;

	SBG_UNUSED_PARAMETER(argc);
	SBG_UNUSED_PARAMETER(argv);

	if (argc == 3)
	{
		//
		// Create a serial interface to communicate with the PULSE
		//
		errorCode = sbgInterfaceSerialCreate(&sbgInterface, argv[1], atoi(argv[2]));

		if (errorCode == SBG_NO_ERROR)
		{
			errorCode = pulseMinimalProcess(&sbgInterface);

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
		printf("Invalid input arguments, usage: pulseMinimal SERIAL_DEVICE SERIAL_BAUDRATE\n");
		exitCode = EXIT_FAILURE;
	}
	
	return exitCode;
}
