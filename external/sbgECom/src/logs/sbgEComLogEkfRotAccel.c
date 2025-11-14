// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Local headers
#include "sbgEComLogEkfRotAccel.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

void sbgEComLogEkfRotAccelConstruct(SbgEComLogEkfRotAccel *pLogData)
{
    assert(pLogData);

    memset(pLogData, 0x00, sizeof(*pLogData));
}

SbgErrorCode sbgEComLogEkfRotAccelReadFromStream(SbgEComLogEkfRotAccel *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->timeStamp             = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status                = sbgStreamBufferReadUint32LE(pStreamBuffer);

    pLogData->rate[0]               = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->rate[1]               = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->rate[2]               = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->acceleration[0]       = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->acceleration[1]       = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->acceleration[2]       = sbgStreamBufferReadFloatLE(pStreamBuffer);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogEkfRotAccelWriteToStream(const SbgEComLogEkfRotAccel *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->status);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->rate[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->rate[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->rate[2]);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->acceleration[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->acceleration[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->acceleration[2]);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}
