#include "sbgEComLogEvent.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogEventReadFromStream(SbgEComLogEvent *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->timeStamp     = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status        = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pLogData->timeOffset0   = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pLogData->timeOffset1   = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pLogData->timeOffset2   = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pLogData->timeOffset3   = sbgStreamBufferReadUint16LE(pStreamBuffer);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogEventWriteToStream(const SbgEComLogEvent *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->timeOffset0);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->timeOffset1);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->timeOffset2);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->timeOffset3);
    
    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComBinaryLogParseEvent(SbgStreamBuffer *pStreamBuffer, SbgEComLogEvent *pLogData)
{
    return sbgEComLogEventReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteEvent(SbgStreamBuffer *pStreamBuffer, const SbgEComLogEvent *pLogData)
{
    return sbgEComLogEventWriteToStream(pLogData, pStreamBuffer);
}
