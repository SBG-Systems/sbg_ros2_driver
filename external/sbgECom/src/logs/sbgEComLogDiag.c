// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Local headers
#include "sbgEComLogDiag.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogDiagReadFromStream(SbgEComLogDiagData *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    pLogData->timestamp     = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->type          = (SbgDebugLogType)sbgStreamBufferReadUint8(pStreamBuffer);
    pLogData->errorCode     = (SbgErrorCode)sbgStreamBufferReadUint8(pStreamBuffer);

    sbgStreamBufferReadBuffer(pStreamBuffer, pLogData->string, sbgStreamBufferGetSpace(pStreamBuffer));
    pLogData->string[sizeof(pLogData->string) - 1] = '\0';

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogDiagWriteToStream(const SbgEComLogDiagData *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    size_t                               length;

    assert(pLogData);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timestamp);
    sbgStreamBufferWriteUint8(pStreamBuffer,    pLogData->type);
    sbgStreamBufferWriteUint8(pStreamBuffer,    pLogData->errorCode);

    length = strlen(pLogData->string);

    if (length >= sizeof(pLogData->string))
    {
        length = sizeof(pLogData->string) - 1;
    }

    sbgStreamBufferWriteBuffer(pStreamBuffer, pLogData->string, length);
    sbgStreamBufferWriteUint8(pStreamBuffer, 0);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComBinaryLogParseDiagData(SbgStreamBuffer *pStreamBuffer, SbgEComLogDiagData *pLogData)
{
    return sbgEComLogDiagReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteDiagData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogDiagData *pLogData)
{
    return sbgEComLogDiagWriteToStream(pLogData, pStreamBuffer);
}
