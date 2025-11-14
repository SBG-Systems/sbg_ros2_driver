#include "sbgEComLogUsbl.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogUsblReadFromStream(SbgEComLogUsbl *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->timeStamp         = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status            = sbgStreamBufferReadUint16LE(pStreamBuffer);

    pLogData->latitude          = sbgStreamBufferReadDoubleLE(pStreamBuffer);
    pLogData->longitude         = sbgStreamBufferReadDoubleLE(pStreamBuffer);

    pLogData->depth             = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->latitudeAccuracy  = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->longitudeAccuracy = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->depthAccuracy     = sbgStreamBufferReadFloatLE(pStreamBuffer);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogUsblWriteToStream(const SbgEComLogUsbl *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);

    sbgStreamBufferWriteDoubleLE(pStreamBuffer, pLogData->latitude);
    sbgStreamBufferWriteDoubleLE(pStreamBuffer, pLogData->longitude);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->depth);
    
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->latitudeAccuracy);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->longitudeAccuracy);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->depthAccuracy);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComBinaryLogParseUsblData(SbgStreamBuffer *pStreamBuffer, SbgEComLogUsbl *pLogData)
{
    return sbgEComLogUsblReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteUsblData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogUsbl *pLogData)
{
    return sbgEComLogUsblWriteToStream(pLogData, pStreamBuffer);
}
