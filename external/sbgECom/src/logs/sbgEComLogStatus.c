#include "sbgEComLogStatus.h"

//----------------------------------------------------------------------//
//- Private definitions                                                -//
//----------------------------------------------------------------------//

/*!
 * CAN bus status definitions for comStatus field.
 */
#define SBG_ECOM_LOG_STATUS_CAN_BUS_STATUS_SHIFT		(28u)			/*!< Shift used to access the CAN status part. */
#define SBG_ECOM_LOG_STATUS_CAN_BUS_STATUS_MASK			(0x00000007u)	/*!< Mask used to keep only the CAN status part. */

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogStatusReadFromStream(SbgEComLogStatus *pLogData, SbgStreamBuffer *pStreamBuffer)
{
	assert(pStreamBuffer);
	assert(pLogData);

	pLogData->timeStamp			= sbgStreamBufferReadUint32LE(pStreamBuffer);
	pLogData->generalStatus		= sbgStreamBufferReadUint16LE(pStreamBuffer);
	pLogData->comStatus2		= sbgStreamBufferReadUint16LE(pStreamBuffer);
	pLogData->comStatus			= sbgStreamBufferReadUint32LE(pStreamBuffer);
	pLogData->aidingStatus		= sbgStreamBufferReadUint32LE(pStreamBuffer);
	pLogData->reserved2			= sbgStreamBufferReadUint32LE(pStreamBuffer);
	pLogData->reserved3			= sbgStreamBufferReadUint16LE(pStreamBuffer);
	
	//
	// Test if we have a additional information such as uptime (since version 1.7)
	//
	if (sbgStreamBufferGetSpace(pStreamBuffer) >= sizeof(uint32_t))
	{
		pLogData->uptime		= sbgStreamBufferReadUint32LE(pStreamBuffer);
	}
	else
	{
		pLogData->uptime = 0;
	}
	
	return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogStatusWriteToStream(const SbgEComLogStatus *pLogData, SbgStreamBuffer *pStreamBuffer)
{
	assert(pStreamBuffer);
	assert(pLogData);

	sbgStreamBufferWriteUint32LE(pStreamBuffer,	pLogData->timeStamp);
	sbgStreamBufferWriteUint16LE(pStreamBuffer,	pLogData->generalStatus);
	sbgStreamBufferWriteUint16LE(pStreamBuffer,	pLogData->comStatus2);
	sbgStreamBufferWriteUint32LE(pStreamBuffer,	pLogData->comStatus);
	sbgStreamBufferWriteUint32LE(pStreamBuffer,	pLogData->aidingStatus);
	sbgStreamBufferWriteUint32LE(pStreamBuffer,	pLogData->reserved2);
	sbgStreamBufferWriteUint16LE(pStreamBuffer,	pLogData->reserved3);
	
	//
	// Write the additional information added in version 1.7
	//
	sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->uptime);

	return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

void sbgEComLogStatusSetCanBusStatus(SbgEComLogStatus *pLogData, SbgEComCanBusStatus status)
{
	assert(pLogData);
	assert(status <= SBG_ECOM_LOG_STATUS_CAN_BUS_STATUS_MASK);

	pLogData->comStatus &= ~(SBG_ECOM_LOG_STATUS_CAN_BUS_STATUS_MASK << SBG_ECOM_LOG_STATUS_CAN_BUS_STATUS_SHIFT);
	pLogData->comStatus |= ((uint32_t)status&SBG_ECOM_LOG_STATUS_CAN_BUS_STATUS_MASK) << SBG_ECOM_LOG_STATUS_CAN_BUS_STATUS_SHIFT;
}

SbgEComCanBusStatus sbgEComLogStatusGetCanBusStatus(const SbgEComLogStatus *pLogData)
{
	assert(pLogData);

	return (SbgEComCanBusStatus)((pLogData->comStatus >> SBG_ECOM_LOG_STATUS_CAN_BUS_STATUS_SHIFT)&SBG_ECOM_LOG_STATUS_CAN_BUS_STATUS_MASK);
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComBinaryLogParseStatusData(SbgStreamBuffer *pStreamBuffer, SbgEComLogStatus *pLogData)
{
	return sbgEComLogStatusReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteStatusData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogStatus *pLogData)
{
	return sbgEComLogStatusWriteToStream(pLogData, pStreamBuffer);
}

SbgEComCanBusStatus sbgEComLogStatusGetCanStatus(uint32_t status)
{
	return (SbgEComCanBusStatus)((status >> SBG_ECOM_LOG_STATUS_CAN_BUS_STATUS_SHIFT) & SBG_ECOM_LOG_STATUS_CAN_BUS_STATUS_MASK);
}

uint32_t sbgEComLogStatusBuildCommunicationStatus(SbgEComCanBusStatus canStatus, uint32_t masks)
{
	return	((((uint32_t)canStatus)&SBG_ECOM_LOG_STATUS_CAN_BUS_STATUS_MASK) << SBG_ECOM_LOG_STATUS_CAN_BUS_STATUS_SHIFT) | masks;
}
