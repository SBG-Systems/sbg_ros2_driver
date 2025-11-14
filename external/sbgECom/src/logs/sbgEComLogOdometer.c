#include "sbgEComLogOdometer.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogOdometerReadFromStream(SbgEComLogOdometer *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->timeStamp = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status    = sbgStreamBufferReadUint16LE(pStreamBuffer);

    pLogData->velocity  = sbgStreamBufferReadFloatLE(pStreamBuffer);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogOdometerWriteToStream(const SbgEComLogOdometer *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);

    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->velocity);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComBinaryLogParseOdometerData(SbgStreamBuffer *pStreamBuffer, SbgEComLogOdometer *pLogData)
{
    return sbgEComLogOdometerReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteOdometerData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogOdometer *pLogData)
{
    return sbgEComLogOdometerWriteToStream(pLogData, pStreamBuffer);
}
