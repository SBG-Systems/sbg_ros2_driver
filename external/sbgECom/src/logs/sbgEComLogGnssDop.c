// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Local headers
#include "sbgEComLogGnssDop.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogGnssDopReadFromStream(SbgEComLogGnssDop *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    pLogData->timeStamp = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->hDop      = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->pDop      = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->vDop      = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->tDop      = sbgStreamBufferReadFloatLE(pStreamBuffer);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogGnssDopWriteToStream(const SbgEComLogGnssDop *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->hDop);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->pDop);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->vDop);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->tDop);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}
