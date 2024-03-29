// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <sbgECom.h>

// Local headers
#include "sbgEComCmdCommon.h"
#include "sbgEComCmdMag.h"

//----------------------------------------------------------------------//
//- Magnetometer commands                                              -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComCmdMagSetModelId(SbgEComHandle *pHandle, SbgEComMagModelsStdId modelId)
{
	assert(pHandle);
	
	return sbgEComCmdGenericSetModelId(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_MAGNETOMETER_MODEL_ID, modelId);
}

SbgErrorCode sbgEComCmdMagGetModelId(SbgEComHandle *pHandle, SbgEComMagModelsStdId *pModelId)
{
	SbgErrorCode	errorCode = SBG_NO_ERROR;
	uint32_t		modelIdAsUint;

	assert(pHandle);
	assert(pModelId);

	errorCode = sbgEComCmdGenericGetModelId(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_MAGNETOMETER_MODEL_ID, &modelIdAsUint);

	if (errorCode == SBG_NO_ERROR)
	{
		*pModelId = (SbgEComMagModelsStdId)modelIdAsUint;
	}

	return errorCode;
}

SbgErrorCode sbgEComCmdMagSetCalibData(SbgEComHandle *pHandle, const float *pOffset, const float *pMatrix)
{
	SbgErrorCode		errorCode = SBG_NO_ERROR;
	SbgStreamBuffer		outputStream;
	uint8_t				outputBuffer[12 * sizeof(float)];
	uint32_t			trial;
	uint32_t			i;

	assert(pHandle);
	assert(pOffset);
	assert(pMatrix);

	//
	// Initialize a stream buffer to write the command payload
	//
	errorCode = sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

	//
	// Write the offset vector
	//
	sbgStreamBufferWriteFloatLE(&outputStream, pOffset[0]);
	sbgStreamBufferWriteFloatLE(&outputStream, pOffset[1]);
	sbgStreamBufferWriteFloatLE(&outputStream, pOffset[2]);

	//
	// Write the matrix
	//
	for (i = 0; i < 9; i++)
	{
		sbgStreamBufferWriteFloatLE(&outputStream, pMatrix[i]);
	}

	//
	// Make sure that the stream buffer has been initialized
	//
	if (errorCode == SBG_NO_ERROR)
	{
		//
		// Send the command three times
		//
		for (trial = 0; trial < pHandle->numTrials; trial++)
		{
			//
			// Send the command
			//
			errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_SET_MAG_CALIB, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

			//
			// Make sure that the command has been sent
			//
			if (errorCode == SBG_NO_ERROR)
			{
				//
				// Try to read the device answer for 500 ms
				//
				errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_SET_MAG_CALIB, pHandle->cmdDefaultTimeOut);

				//
				// Test if we have received a valid ACK
				//
				if (errorCode == SBG_NO_ERROR)
				{
					//
					// The command has been executed successfully so return
					//
					break;
				}
			}
			else
			{
				//
				// We have a write error so exit the try loop
				//
				break;
			}
		}
	}

	return errorCode;
}

SbgErrorCode sbgEComCmdMagGetRejection(SbgEComHandle *pHandle, SbgEComMagRejectionConf *pRejectConf)
{
	SbgErrorCode			errorCode = SBG_NO_ERROR;
	SbgEComProtocolPayload	receivedPayload;
	uint32_t				trial;
	
	assert(pHandle);
	assert(pRejectConf);

	sbgEComProtocolPayloadConstruct(&receivedPayload);

	//
	// Send the command three times
	//
	for (trial = 0; trial < pHandle->numTrials; trial++)
	{
		//
		// Send the command only since this is a no-payload command
		//
		errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_MAGNETOMETER_REJECT_MODE, NULL, 0);

		//
		// Make sure that the command has been sent
		//
		if (errorCode == SBG_NO_ERROR)
		{
			SbgStreamBuffer		inputStream;

			//
			// Try to read the device answer for 500 ms
			//
			errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_MAGNETOMETER_REJECT_MODE, &receivedPayload, pHandle->cmdDefaultTimeOut);

			//
			// Test if we have received frame was OK
			//
			if (errorCode == SBG_NO_ERROR)
			{
				//
				// Initialize stream buffer to read parameters
				//
				sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));

				//
				// Read parameters
				//
				pRejectConf->magneticField = (SbgEComRejectionMode)sbgStreamBufferReadUint8LE(&inputStream);

				//
				// The command has been executed successfully so return
				//
				break;
			}
		}
		else
		{
			//
			// We have a write error so exit the try loop
			//
			break;
		}
	}

	sbgEComProtocolPayloadDestroy(&receivedPayload);

	return errorCode;
}

SbgErrorCode sbgEComCmdMagSetRejection(SbgEComHandle *pHandle, const SbgEComMagRejectionConf *pRejectConf)
{
	SbgErrorCode		errorCode = SBG_NO_ERROR;
	uint32_t			trial;
	uint8_t				outputBuffer[64];
	SbgStreamBuffer		outputStream;

	assert(pHandle);
	assert(pRejectConf);

	//
	// Send the command three times
	//
	for (trial = 0; trial < pHandle->numTrials; trial++)
	{
		//
		// Init stream buffer for output
		// Build payload
		//
		sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));
		sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pRejectConf->magneticField);

		//
		// Send the payload over ECom
		//
		errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_MAGNETOMETER_REJECT_MODE, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

		//
		// Make sure that the command has been sent
		//
		if (errorCode == SBG_NO_ERROR)
		{
			//
			// Try to read the device answer for 500 ms
			//
			errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_MAGNETOMETER_REJECT_MODE, pHandle->cmdDefaultTimeOut);

			//
			// Test if we have received a valid ACK
			//
			if (errorCode == SBG_NO_ERROR)
			{
				//
				// The command has been executed successfully so return
				//
				break;
			}
		}
		else
		{
			//
			// We have a write error so exit the try loop
			//
			break;
		}
	}
	
	return errorCode;
}

//----------------------------------------------------------------------//
//- Magnetometer on-board calibration commands                         -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComCmdMagStartCalib(SbgEComHandle *pHandle, SbgEComMagCalibMode mode, SbgEComMagCalibBandwidth bandwidth)
{
	SbgErrorCode		errorCode = SBG_NO_ERROR;
	SbgStreamBuffer		outputStream;
	uint8_t				outputBuffer[2];
	uint32_t			trial;

	assert(pHandle);
	
	//
	// Initialize a stream buffer to write the command payload
	//
	errorCode = sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

	//
	// Write the calibration mode and bandwidth
	//
	sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)mode);
	sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)bandwidth);
		
	//
	// Make sure that the stream buffer has been initialized
	//
	if (errorCode == SBG_NO_ERROR)
	{
		//
		// Send the command three times
		//
		for (trial = 0; trial < pHandle->numTrials; trial++)
		{
			//
			// Send the command
			//
			errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_START_MAG_CALIB, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

			//
			// Make sure that the command has been sent
			//
			if (errorCode == SBG_NO_ERROR)
			{
				//
				// Try to read the device answer for 500 ms
				//
				errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_START_MAG_CALIB, pHandle->cmdDefaultTimeOut);

				//
				// Test if we have received a valid ACK
				//
				if (errorCode == SBG_NO_ERROR)
				{
					//
					// The command has been executed successfully so return
					//
					break;
				}
			}
			else
			{
				//
				// We have a write error so exit the try loop
				//
				break;
			}
		}
	}
	
	return errorCode;
}

SbgErrorCode sbgEComCmdMagComputeCalib(SbgEComHandle *pHandle, SbgEComMagCalibResults *pCalibResults)
{
	SbgErrorCode			errorCode = SBG_NO_ERROR;
	SbgEComProtocolPayload	receivedPayload;
	uint32_t				trial;
	
	assert(pHandle);
	assert(pCalibResults);

	sbgEComProtocolPayloadConstruct(&receivedPayload);

	//
	// Send the command three times
	//
	for (trial = 0; trial < pHandle->numTrials; trial++)
	{
		//
		// Send the command only since this is a no-payload command
		//
		errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_COMPUTE_MAG_CALIB, NULL, 0);

		//
		// Make sure that the command has been sent
		//
		if (errorCode == SBG_NO_ERROR)
		{
			//
			// Try to read the device answer for 5 s because the on-board magnetic computation can take some time
			//
			errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_COMPUTE_MAG_CALIB, &receivedPayload, 5000);

			//
			// Test if we have received the correct command
			//
			if (errorCode == SBG_NO_ERROR)
			{
				SbgStreamBuffer		inputStream;
				size_t				i;

				//
				// Initialize stream buffer to read parameters
				//
				sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));

				//
				// Read quality and status parameters
				//
				pCalibResults->quality			= (SbgEComMagCalibQuality)sbgStreamBufferReadUint8LE(&inputStream);
				pCalibResults->confidence		= (SbgEComMagCalibConfidence)sbgStreamBufferReadUint8LE(&inputStream);
				pCalibResults->advancedStatus	= sbgStreamBufferReadUint16LE(&inputStream);

				pCalibResults->beforeMeanError	= sbgStreamBufferReadFloatLE(&inputStream);
				pCalibResults->beforeStdError	= sbgStreamBufferReadFloatLE(&inputStream);
				pCalibResults->beforeMaxError	= sbgStreamBufferReadFloatLE(&inputStream);

				pCalibResults->afterMeanError	= sbgStreamBufferReadFloatLE(&inputStream);
				pCalibResults->afterStdError	= sbgStreamBufferReadFloatLE(&inputStream);
				pCalibResults->afterMaxError	= sbgStreamBufferReadFloatLE(&inputStream);

				pCalibResults->meanAccuracy		= sbgStreamBufferReadFloatLE(&inputStream);
				pCalibResults->stdAccuracy		= sbgStreamBufferReadFloatLE(&inputStream);
				pCalibResults->maxAccuracy		= sbgStreamBufferReadFloatLE(&inputStream);

				pCalibResults->numPoints		= sbgStreamBufferReadUint16LE(&inputStream);
				pCalibResults->maxNumPoints		= sbgStreamBufferReadUint16LE(&inputStream);

				//
				// Read the computed hard iron offset vector
				//
				pCalibResults->offset[0]		= sbgStreamBufferReadFloatLE(&inputStream);
				pCalibResults->offset[1]		= sbgStreamBufferReadFloatLE(&inputStream);
				pCalibResults->offset[2]		= sbgStreamBufferReadFloatLE(&inputStream);

				//
				// Read the computed soft iron matrix
				//
				for (i = 0; i < 9; i++)
				{
					pCalibResults->matrix[i]	= sbgStreamBufferReadFloatLE(&inputStream);
				}

				//
				// The command has been executed successfully so return
				//
				break;
			}
		}
		else
		{
			//
			// We have a write error so exit the try loop
			//
			break;
		}
	}

	sbgEComProtocolPayloadDestroy(&receivedPayload);

	return errorCode;
}
