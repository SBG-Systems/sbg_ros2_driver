// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <sbgECom.h>

// Local headers
#include "sbgEComCmdCommon.h"
#include "sbgEComCmdSensor.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComCmdSensorSetMotionProfileId(SbgEComHandle *pHandle, SbgEComMotionProfileStdIds modelId)
{
	assert(pHandle);

	return sbgEComCmdGenericSetModelId(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_MOTION_PROFILE_ID, modelId);
}


SbgErrorCode sbgEComCmdSensorGetMotionProfileId(SbgEComHandle *pHandle, SbgEComMotionProfileStdIds *pModelId)
{
	SbgErrorCode	errorCode = SBG_NO_ERROR;
	uint32_t		modelIdAsUint;

	assert(pHandle);
	assert(pModelId);

	errorCode = sbgEComCmdGenericGetModelId(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_MOTION_PROFILE_ID, &modelIdAsUint);

	if (errorCode == SBG_NO_ERROR)
	{
		*pModelId = (SbgEComMotionProfileStdIds)modelIdAsUint;
	}

	return errorCode;
}

SbgErrorCode sbgEComCmdSensorGetInitCondition(SbgEComHandle *pHandle, SbgEComInitConditionConf *pConf)
{
	SbgErrorCode			errorCode = SBG_NO_ERROR;
	SbgEComProtocolPayload	receivedPayload;
	uint32_t				trial;

	assert(pHandle);
	assert(pConf);

	sbgEComProtocolPayloadConstruct(&receivedPayload);

	//
	// Send the command three times
	//
	for (trial = 0; trial < pHandle->numTrials; trial++)
	{
		//
		// Send the command only since this is a no payload command
		//
		errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_INIT_PARAMETERS, NULL, 0);

		//
		// Make sure that the command has been sent
		//
		if (errorCode == SBG_NO_ERROR)
		{
			//
			// Try to read the device answer for 500 ms
			//
			errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_INIT_PARAMETERS, &receivedPayload, pHandle->cmdDefaultTimeOut);

			//
			// Test if we have received a SBG_ECOM_CMD_INIT_PARAMETERS command
			//
			if (errorCode == SBG_NO_ERROR)
			{
				SbgStreamBuffer		inputStream;

				//
				// Initialize stream buffer to read parameters
				//
				sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));

				//
				// Read parameters
				//
				pConf->latitude = sbgStreamBufferReadDoubleLE(&inputStream);
				pConf->longitude = sbgStreamBufferReadDoubleLE(&inputStream);
				pConf->altitude = sbgStreamBufferReadDoubleLE(&inputStream);
				pConf->year = sbgStreamBufferReadUint16LE(&inputStream);
				pConf->month = sbgStreamBufferReadUint8LE(&inputStream);
				pConf->day = sbgStreamBufferReadUint8LE(&inputStream);

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

SbgErrorCode sbgEComCmdSensorSetInitCondition(SbgEComHandle *pHandle, const SbgEComInitConditionConf *pConf)
{
	SbgErrorCode		errorCode = SBG_NO_ERROR;
	uint32_t			trial;
	uint8_t				outputBuffer[64];
	SbgStreamBuffer		outputStream;

	assert(pHandle);
	assert(pConf);

	//
	// Send the command three times
	//
	for (trial = 0; trial < pHandle->numTrials; trial++)
	{
		//
		// Init stream buffer for output
		//
		sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

		//
		// Build payload
		//
		sbgStreamBufferWriteDoubleLE(&outputStream, pConf->latitude);
		sbgStreamBufferWriteDoubleLE(&outputStream, pConf->longitude);
		sbgStreamBufferWriteDoubleLE(&outputStream, pConf->altitude);
		sbgStreamBufferWriteUint16LE(&outputStream, (uint16_t)pConf->year);
		sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pConf->month);
		sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pConf->day);

		//
		// Send the payload over ECom
		//
		errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_INIT_PARAMETERS, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

		//
		// Make sure that the command has been sent
		//
		if (errorCode == SBG_NO_ERROR)
		{
			//
			// Try to read the device answer for 500 ms
			//
			errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_INIT_PARAMETERS, pHandle->cmdDefaultTimeOut);

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

SbgErrorCode sbgEComCmdSensorGetAidingAssignment(SbgEComHandle *pHandle, SbgEComAidingAssignConf *pConf)
{
	SbgErrorCode			errorCode = SBG_NO_ERROR;
	SbgEComProtocolPayload	receivedPayload;
	uint32_t				trial;	

	assert(pHandle);
	assert(pConf);

	sbgEComProtocolPayloadConstruct(&receivedPayload);

	//
	// Send the command three times
	//
	for (trial = 0; trial < pHandle->numTrials; trial++)
	{
		//
		// Send the command only since this is a no payload command
		//
		errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_AIDING_ASSIGNMENT, NULL, 0);

		//
		// Make sure that the command has been sent
		//
		if (errorCode == SBG_NO_ERROR)
		{
			//
			// Try to read the device answer for 500 ms
			//
			errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_AIDING_ASSIGNMENT, &receivedPayload, pHandle->cmdDefaultTimeOut);

			//
			// Test if we have received a SBG_ECOM_CMD_AIDING_ASSIGNMENT command
			//
			if (errorCode == SBG_NO_ERROR)
			{
				SbgStreamBuffer		inputStream;

				//
				// Initialize stream buffer to read parameters
				//
				sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));

				//
				// Read parameters
				//
				pConf->gps1Port			= (SbgEComModulePortAssignment)sbgStreamBufferReadUint8LE(&inputStream);
				pConf->gps1Sync			= (SbgEComModuleSyncAssignment)sbgStreamBufferReadUint8LE(&inputStream);
					
				sbgStreamBufferSeek(&inputStream, 4*sizeof(uint8_t), SB_SEEK_CUR_INC);						/*!< Reserved fields to ignore */

				pConf->dvlPort			= (SbgEComModulePortAssignment)sbgStreamBufferReadUint8LE(&inputStream);
				pConf->dvlSync			= (SbgEComModuleSyncAssignment)sbgStreamBufferReadUint8LE(&inputStream);

				pConf->rtcmPort			= (SbgEComModulePortAssignment)sbgStreamBufferReadUint8LE(&inputStream);
				pConf->airDataPort		= (SbgEComModulePortAssignment)sbgStreamBufferReadUint8LE(&inputStream);
				pConf->odometerPinsConf	= (SbgEComOdometerPinAssignment)sbgStreamBufferReadUint8LE(&inputStream);

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

SbgErrorCode sbgEComCmdSensorSetAidingAssignment(SbgEComHandle *pHandle, const SbgEComAidingAssignConf *pConf)
{
	SbgErrorCode		errorCode = SBG_NO_ERROR;
	uint32_t			trial;
	uint8_t				outputBuffer[16];
	SbgStreamBuffer		outputStream;

	assert(pHandle);
	assert(pConf);

	//
	// Send the command three times
	//
	for (trial = 0; trial < pHandle->numTrials; trial++)
	{
		//
		// Init stream buffer for output
		//
		sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

		//
		// Build payload
		//
		sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pConf->gps1Port);
		sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pConf->gps1Sync);

		//
		// Skip the 4 reserved bytes
		//
		sbgStreamBufferWriteUint8LE(&outputStream, 0);
		sbgStreamBufferWriteUint8LE(&outputStream, 0);
		sbgStreamBufferWriteUint8LE(&outputStream, 0);
		sbgStreamBufferWriteUint8LE(&outputStream, 0);

		sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pConf->dvlPort);
		sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pConf->dvlSync);			
		sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pConf->rtcmPort);
		sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pConf->airDataPort);
		sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pConf->odometerPinsConf);

		//
		// Send the payload over ECom
		//
		errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_AIDING_ASSIGNMENT, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

		//
		// Make sure that the command has been sent
		//
		if (errorCode == SBG_NO_ERROR)
		{
			//
			// Try to read the device answer for 500 ms
			//
			errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_AIDING_ASSIGNMENT, pHandle->cmdDefaultTimeOut);

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

SbgErrorCode sbgEComCmdSensorGetAlignmentAndLeverArm(SbgEComHandle *pHandle, SbgEComSensorAlignmentInfo *pAlignConf, float *pLeverArm)
{
	SbgErrorCode			errorCode = SBG_NO_ERROR;
	SbgEComProtocolPayload	receivedPayload;
	uint32_t				trial;

	assert(pHandle);
	assert(pAlignConf);
	assert(pLeverArm);

	sbgEComProtocolPayloadConstruct(&receivedPayload);

	//
	// Send the command three times
	//
	for (trial = 0; trial < pHandle->numTrials; trial++)
	{
		//
		// Send the command only since this is a no-payload command
		//
		errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_IMU_ALIGNMENT_LEVER_ARM, NULL, 0);

		//
		// Make sure that the command has been sent
		//
		if (errorCode == SBG_NO_ERROR)
		{
			//
			// Try to read the device answer for 500 ms
			//
			errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_IMU_ALIGNMENT_LEVER_ARM, &receivedPayload, pHandle->cmdDefaultTimeOut);

			//
			// Test if we have received a SBG_ECOM_CMD_IMU_ALIGNMENT command
			//
			if (errorCode == SBG_NO_ERROR)
			{
				SbgStreamBuffer		inputStream;

				//
				// Initialize stream buffer to read parameters
				//
				sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));

				//
				// Read parameters
				//
				pAlignConf->axisDirectionX = (SbgEComAxisDirection)sbgStreamBufferReadUint8LE(&inputStream);
				pAlignConf->axisDirectionY = (SbgEComAxisDirection)sbgStreamBufferReadUint8LE(&inputStream);
				pAlignConf->misRoll = sbgStreamBufferReadFloatLE(&inputStream);
				pAlignConf->misPitch = sbgStreamBufferReadFloatLE(&inputStream);
				pAlignConf->misYaw = sbgStreamBufferReadFloatLE(&inputStream);
				pLeverArm[0] = sbgStreamBufferReadFloatLE(&inputStream);
				pLeverArm[1] = sbgStreamBufferReadFloatLE(&inputStream);
				pLeverArm[2] = sbgStreamBufferReadFloatLE(&inputStream);

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

SbgErrorCode sbgEComCmdSensorSetAlignmentAndLeverArm(SbgEComHandle *pHandle, const SbgEComSensorAlignmentInfo *pAlignConf, const float *pLeverArm)
{
	SbgErrorCode		errorCode = SBG_NO_ERROR;
	uint32_t			trial;
	uint8_t				outputBuffer[32];
	SbgStreamBuffer		outputStream;

	assert(pHandle);
	assert(pAlignConf);
	assert(pLeverArm);
	
	//
	// Send the command three times
	//
	for (trial = 0; trial < pHandle->numTrials; trial++)
	{
		//
		// Init stream buffer for output
		//
		sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

		//
		// Build payload
		//
		sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pAlignConf->axisDirectionX);
		sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pAlignConf->axisDirectionY);
		sbgStreamBufferWriteFloatLE(&outputStream, pAlignConf->misRoll);
		sbgStreamBufferWriteFloatLE(&outputStream, pAlignConf->misPitch);
		sbgStreamBufferWriteFloatLE(&outputStream, pAlignConf->misYaw);
		sbgStreamBufferWriteFloatLE(&outputStream, pLeverArm[0]);
		sbgStreamBufferWriteFloatLE(&outputStream, pLeverArm[1]);
		sbgStreamBufferWriteFloatLE(&outputStream, pLeverArm[2]);

		//
		// Send the payload over ECom
		//
		errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_IMU_ALIGNMENT_LEVER_ARM, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

		//
		// Make sure that the command has been sent
		//
		if (errorCode == SBG_NO_ERROR)
		{
			//
			// Try to read the device answer for 500 ms
			//
			errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_IMU_ALIGNMENT_LEVER_ARM, pHandle->cmdDefaultTimeOut);

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
