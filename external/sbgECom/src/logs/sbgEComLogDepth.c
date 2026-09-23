#include "sbgEComLogDepth.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogDepthReadFromStream(SbgEComLogDepth *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    pLogData->timeStamp     = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status        = sbgStreamBufferReadUint16LE(pStreamBuffer);

    pLogData->pressure      = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->depth         = sbgStreamBufferReadFloatLE(pStreamBuffer);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogDepthWriteToStream(const SbgEComLogDepth *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint32LE(pStreamBuffer,     pLogData->timeStamp);
    sbgStreamBufferWriteUint16LE(pStreamBuffer,     pLogData->status);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,      pLogData->pressure);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,      pLogData->depth);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComBinaryLogParseDepth(SbgStreamBuffer *pStreamBuffer, SbgEComLogDepth *pLogData)
{
    return sbgEComLogDepthReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteDepth(SbgStreamBuffer *pStreamBuffer, const SbgEComLogDepth *pLogData)
{
    return sbgEComLogDepthWriteToStream(pLogData, pStreamBuffer);
}
