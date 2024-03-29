#include "sbgEComLogMag.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogMagReadFromStream(SbgEComLogMag *pLogData, SbgStreamBuffer *pStreamBuffer)
{
	assert(pStreamBuffer);
	assert(pLogData);

	pLogData->timeStamp			= sbgStreamBufferReadUint32LE(pStreamBuffer);
	pLogData->status				= sbgStreamBufferReadUint16LE(pStreamBuffer);

	pLogData->magnetometers[0]	= sbgStreamBufferReadFloatLE(pStreamBuffer);
	pLogData->magnetometers[1]	= sbgStreamBufferReadFloatLE(pStreamBuffer);
	pLogData->magnetometers[2]	= sbgStreamBufferReadFloatLE(pStreamBuffer);
				
	pLogData->accelerometers[0]	= sbgStreamBufferReadFloatLE(pStreamBuffer);
	pLogData->accelerometers[1]	= sbgStreamBufferReadFloatLE(pStreamBuffer);
	pLogData->accelerometers[2]	= sbgStreamBufferReadFloatLE(pStreamBuffer);

	return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogMagWriteToStream(const SbgEComLogMag *pLogData, SbgStreamBuffer *pStreamBuffer)
{
	assert(pStreamBuffer);
	assert(pLogData);

	sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
	sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);
	
	sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->magnetometers[0]);
	sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->magnetometers[1]);
	sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->magnetometers[2]);
		
	sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->accelerometers[0]);
	sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->accelerometers[1]);
	sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->accelerometers[2]);

	return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogMagCalibReadFromStream(SbgEComLogMagCalib *pLogData, SbgStreamBuffer *pStreamBuffer)
{
	assert(pStreamBuffer);
	assert(pLogData);

	pLogData->timeStamp	= sbgStreamBufferReadUint32LE(pStreamBuffer);
	pLogData->reserved	= sbgStreamBufferReadUint16LE(pStreamBuffer);

	return sbgStreamBufferReadBuffer(pStreamBuffer, pLogData->magData, sizeof(pLogData->magData));
}

SbgErrorCode sbgEComLogMagCalibWriteToStream(const SbgEComLogMagCalib *pLogData, SbgStreamBuffer *pStreamBuffer)
{
	assert(pStreamBuffer);
	assert(pLogData);

	sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
	sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->reserved);

	return sbgStreamBufferWriteBuffer(pStreamBuffer, pLogData->magData, sizeof(pLogData->magData));
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogMag		SbgLogMag);
SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogMagCalib	SbgLogMagCalib);

SbgErrorCode sbgEComBinaryLogParseMagData(SbgStreamBuffer *pStreamBuffer, SbgEComLogMag *pLogData)
{
	return sbgEComLogMagReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteMagData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogMag *pLogData)
{
	return sbgEComLogMagWriteToStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogParseMagCalibData(SbgStreamBuffer *pStreamBuffer, SbgEComLogMagCalib *pLogData)
{
	return sbgEComLogMagCalibReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteMagCalibData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogMagCalib *pLogData)
{
	return sbgEComLogMagCalibWriteToStream(pLogData, pStreamBuffer);
}
