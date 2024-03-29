#include "sbgEComLogImu.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogImuLegacyReadFromStream(SbgEComLogImuLegacy *pLogData, SbgStreamBuffer *pStreamBuffer)
{
	assert(pStreamBuffer);
	assert(pLogData);

	pLogData->timeStamp			= sbgStreamBufferReadUint32LE(pStreamBuffer);
	pLogData->status			= sbgStreamBufferReadUint16LE(pStreamBuffer);
				
	pLogData->accelerometers[0]	= sbgStreamBufferReadFloatLE(pStreamBuffer);
	pLogData->accelerometers[1]	= sbgStreamBufferReadFloatLE(pStreamBuffer);
	pLogData->accelerometers[2]	= sbgStreamBufferReadFloatLE(pStreamBuffer);

	pLogData->gyroscopes[0]		= sbgStreamBufferReadFloatLE(pStreamBuffer);
	pLogData->gyroscopes[1]		= sbgStreamBufferReadFloatLE(pStreamBuffer);
	pLogData->gyroscopes[2]		= sbgStreamBufferReadFloatLE(pStreamBuffer);

	pLogData->temperature		= sbgStreamBufferReadFloatLE(pStreamBuffer);

	pLogData->deltaVelocity[0]	= sbgStreamBufferReadFloatLE(pStreamBuffer);
	pLogData->deltaVelocity[1]	= sbgStreamBufferReadFloatLE(pStreamBuffer);
	pLogData->deltaVelocity[2]	= sbgStreamBufferReadFloatLE(pStreamBuffer);
				
	pLogData->deltaAngle[0]		= sbgStreamBufferReadFloatLE(pStreamBuffer);
	pLogData->deltaAngle[1]		= sbgStreamBufferReadFloatLE(pStreamBuffer);
	pLogData->deltaAngle[2]		= sbgStreamBufferReadFloatLE(pStreamBuffer);

	return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogImuLegacyWriteToStream(const SbgEComLogImuLegacy *pLogData, SbgStreamBuffer *pStreamBuffer)
{
	assert(pStreamBuffer);
	assert(pLogData);

	sbgStreamBufferWriteUint32LE(pStreamBuffer,	pLogData->timeStamp);
	sbgStreamBufferWriteUint16LE(pStreamBuffer,	pLogData->status);
				
	sbgStreamBufferWriteFloatLE(pStreamBuffer,	pLogData->accelerometers[0]);
	sbgStreamBufferWriteFloatLE(pStreamBuffer,	pLogData->accelerometers[1]);
	sbgStreamBufferWriteFloatLE(pStreamBuffer,	pLogData->accelerometers[2]);

	sbgStreamBufferWriteFloatLE(pStreamBuffer,	pLogData->gyroscopes[0]);
	sbgStreamBufferWriteFloatLE(pStreamBuffer,	pLogData->gyroscopes[1]);
	sbgStreamBufferWriteFloatLE(pStreamBuffer,	pLogData->gyroscopes[2]);

	sbgStreamBufferWriteFloatLE(pStreamBuffer,	pLogData->temperature);

	sbgStreamBufferWriteFloatLE(pStreamBuffer,	pLogData->deltaVelocity[0]);
	sbgStreamBufferWriteFloatLE(pStreamBuffer,	pLogData->deltaVelocity[1]);
	sbgStreamBufferWriteFloatLE(pStreamBuffer,	pLogData->deltaVelocity[2]);
				
	sbgStreamBufferWriteFloatLE(pStreamBuffer,	pLogData->deltaAngle[0]);
	sbgStreamBufferWriteFloatLE(pStreamBuffer,	pLogData->deltaAngle[1]);
	sbgStreamBufferWriteFloatLE(pStreamBuffer,	pLogData->deltaAngle[2]);

	return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogImuShortReadFromStream(SbgEComLogImuShort *pLogData, SbgStreamBuffer *pStreamBuffer)
{
	assert(pStreamBuffer);
	assert(pLogData);

	pLogData->timeStamp			= sbgStreamBufferReadUint32LE(pStreamBuffer);
	pLogData->status			= sbgStreamBufferReadUint16LE(pStreamBuffer);

	pLogData->deltaVelocity[0]	= sbgStreamBufferReadInt32LE(pStreamBuffer);
	pLogData->deltaVelocity[1]	= sbgStreamBufferReadInt32LE(pStreamBuffer);
	pLogData->deltaVelocity[2]	= sbgStreamBufferReadInt32LE(pStreamBuffer);

	pLogData->deltaAngle[0]		= sbgStreamBufferReadInt32LE(pStreamBuffer);
	pLogData->deltaAngle[1]		= sbgStreamBufferReadInt32LE(pStreamBuffer);
	pLogData->deltaAngle[2]		= sbgStreamBufferReadInt32LE(pStreamBuffer);

	pLogData->temperature		= sbgStreamBufferReadInt16LE(pStreamBuffer);

	return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogImuShortWriteToStream(const SbgEComLogImuShort *pLogData, SbgStreamBuffer *pStreamBuffer)
{
	assert(pStreamBuffer);
	assert(pLogData);

	sbgStreamBufferWriteUint32LE(pStreamBuffer,	pLogData->timeStamp);
	sbgStreamBufferWriteUint16LE(pStreamBuffer,	pLogData->status);

	sbgStreamBufferWriteInt32LE(pStreamBuffer,	pLogData->deltaVelocity[0]);
	sbgStreamBufferWriteInt32LE(pStreamBuffer,	pLogData->deltaVelocity[1]);
	sbgStreamBufferWriteInt32LE(pStreamBuffer,	pLogData->deltaVelocity[2]);

	sbgStreamBufferWriteInt32LE(pStreamBuffer,	pLogData->deltaAngle[0]);
	sbgStreamBufferWriteInt32LE(pStreamBuffer,	pLogData->deltaAngle[1]);
	sbgStreamBufferWriteInt32LE(pStreamBuffer,	pLogData->deltaAngle[2]);

	sbgStreamBufferWriteInt16LE(pStreamBuffer,	pLogData->temperature);

	return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogImuFastLegacyReadFromStream(SbgEComLogImuFastLegacy *pLogData, SbgStreamBuffer *pStreamBuffer)
{
	assert(pStreamBuffer);
	assert(pLogData);

	pLogData->timeStamp			= sbgStreamBufferReadUint32LE(pStreamBuffer);
	pLogData->status			= sbgStreamBufferReadUint16LE(pStreamBuffer);
				
	pLogData->accelerometers[0]	= (float)sbgStreamBufferReadInt16LE(pStreamBuffer) * 0.01f;
	pLogData->accelerometers[1]	= (float)sbgStreamBufferReadInt16LE(pStreamBuffer) * 0.01f;
	pLogData->accelerometers[2]	= (float)sbgStreamBufferReadInt16LE(pStreamBuffer) * 0.01f;

	pLogData->gyroscopes[0]		= (float)sbgStreamBufferReadInt16LE(pStreamBuffer) * 0.001f;
	pLogData->gyroscopes[1]		= (float)sbgStreamBufferReadInt16LE(pStreamBuffer) * 0.001f;
	pLogData->gyroscopes[2]		= (float)sbgStreamBufferReadInt16LE(pStreamBuffer) * 0.001f;

	return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogImuFastLegacyWriteToStream(const SbgEComLogImuFastLegacy *pLogData, SbgStreamBuffer *pStreamBuffer)
{
	assert(pStreamBuffer);
	assert(pLogData);

	sbgStreamBufferWriteUint32LE(pStreamBuffer,	pLogData->timeStamp);
	sbgStreamBufferWriteUint16LE(pStreamBuffer,	pLogData->status);
				
	sbgStreamBufferWriteInt16LE(pStreamBuffer,	(int16_t)(pLogData->accelerometers[0] * 100.0f));
	sbgStreamBufferWriteInt16LE(pStreamBuffer,	(int16_t)(pLogData->accelerometers[1] * 100.0f));
	sbgStreamBufferWriteInt16LE(pStreamBuffer,	(int16_t)(pLogData->accelerometers[2] * 100.0f));
		
	sbgStreamBufferWriteInt16LE(pStreamBuffer,	(int16_t)(pLogData->gyroscopes[0] * 1000.0f));
	sbgStreamBufferWriteInt16LE(pStreamBuffer,	(int16_t)(pLogData->gyroscopes[1] * 1000.0f));
	sbgStreamBufferWriteInt16LE(pStreamBuffer,	(int16_t)(pLogData->gyroscopes[2] * 1000.0f));

	return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

float sbgEComLogImuShortGetDeltaAngle(const SbgEComLogImuShort *pImuShort, size_t idx)
{
	assert(pImuShort);
	assert(idx < 3);

	return pImuShort->deltaAngle[idx] / 67108864.0f;
}

float sbgEComLogImuShortGetDeltaVelocity(const SbgEComLogImuShort *pImuShort, size_t idx)
{
	assert(pImuShort);
	assert(idx < 3);

	return pImuShort->deltaVelocity[idx] / 1048576.0f;
}

float sbgEComLogImuShortGetTemperature(const SbgEComLogImuShort *pImuShort)
{
	assert(pImuShort);

	return pImuShort->temperature / 256.0f;
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComBinaryLogParseImuData(SbgStreamBuffer *pStreamBuffer, SbgEComLogImuLegacy *pLogData)
{
	return sbgEComLogImuLegacyReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteImuData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogImuLegacy *pLogData)
{
	return sbgEComLogImuLegacyWriteToStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogParseImuShort(SbgStreamBuffer *pStreamBuffer, SbgEComLogImuShort *pLogData)
{
	return sbgEComLogImuShortReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteImuShort(SbgStreamBuffer *pStreamBuffer, const SbgEComLogImuShort *pLogData)
{
	return sbgEComLogImuShortWriteToStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogParseFastImuData(SbgStreamBuffer *pStreamBuffer, SbgEComLogImuFastLegacy *pLogData)
{
	return sbgEComLogImuFastLegacyReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteFastImuData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogImuFastLegacy *pLogData)
{
	return sbgEComLogImuFastLegacyWriteToStream(pLogData, pStreamBuffer);
}

float sbgLogImuShortGetDeltaAngle(const SbgEComLogImuShort *pImuShort, size_t idx)
{
	return sbgEComLogImuShortGetDeltaAngle(pImuShort, idx);
}

float sbgLogImuShortGetDeltaVelocity(const SbgEComLogImuShort *pImuShort, size_t idx)
{
	return sbgEComLogImuShortGetDeltaVelocity(pImuShort, idx);
}

float sbgLogImuShortGetTemperature(const SbgEComLogImuShort *pImuShort)
{
	return sbgEComLogImuShortGetTemperature(pImuShort);
}
