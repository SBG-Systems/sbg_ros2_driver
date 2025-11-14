#include "sbgEComLogDvl.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogDvlReadFromStream(SbgEComLogDvl *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    pLogData->timeStamp             = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status                = sbgStreamBufferReadUint16LE(pStreamBuffer);

    pLogData->velocity[0]           = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocity[1]           = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocity[2]           = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->velocityQuality[0]    = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocityQuality[1]    = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocityQuality[2]    = sbgStreamBufferReadFloatLE(pStreamBuffer);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogDvlWriteToStream(const SbgEComLogDvl *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocity[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocity[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocity[2]);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocityQuality[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocityQuality[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocityQuality[2]);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComBinaryLogParseDvlData(SbgStreamBuffer *pStreamBuffer, SbgEComLogDvl *pLogData)
{
    return sbgEComLogDvlReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteDvlData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogDvl *pLogData)
{
    return sbgEComLogDvlWriteToStream(pLogData, pStreamBuffer);
}
