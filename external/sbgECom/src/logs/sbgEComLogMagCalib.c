// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Local headers
#include "sbgEComLogMagCalib.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogMagCalibReadFromStream(SbgEComLogMagCalib *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->timeStamp = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->reserved  = sbgStreamBufferReadUint16LE(pStreamBuffer);

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

SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogMagCalib   SbgLogMagCalib);

SbgErrorCode sbgEComBinaryLogParseMagCalibData(SbgStreamBuffer *pStreamBuffer, SbgEComLogMagCalib *pLogData)
{
    return sbgEComLogMagCalibReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteMagCalibData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogMagCalib *pLogData)
{
    return sbgEComLogMagCalibWriteToStream(pLogData, pStreamBuffer);
}
