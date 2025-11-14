// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Local headers
#include "sbgEComLogSessionInfo.h"

//----------------------------------------------------------------------//
//- Public functions                                                   -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogSessionInfoReadFromStream(SbgEComLogSessionInfo *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    SbgErrorCode                         errorCode;

    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->pageIndex     = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pLogData->nrPages       = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pLogData->size          = sbgStreamBufferReadUint16LE(pStreamBuffer);

    errorCode = sbgStreamBufferGetLastError(pStreamBuffer);

    if (errorCode == SBG_NO_ERROR)
    {
        if (pLogData->pageIndex >= pLogData->nrPages)
        {
            errorCode = SBG_INVALID_FRAME;
            SBG_LOG_ERROR(errorCode, "invalid session information page index %" PRIu16 "/%" PRIu16, pLogData->pageIndex, pLogData->nrPages);
        }
    }

    if (errorCode == SBG_NO_ERROR)
    {
        if (pLogData->size <= sizeof(pLogData->buffer))
        {
            errorCode = sbgStreamBufferReadBuffer(pStreamBuffer, pLogData->buffer, pLogData->size);

            if (errorCode != SBG_NO_ERROR)
            {
                SBG_LOG_ERROR(errorCode, "invalid session information size %zu, actual:%zu", pLogData->size, sbgStreamBufferGetSpace(pStreamBuffer));
            }
        }
        else
        {
            errorCode = SBG_INVALID_FRAME;
            SBG_LOG_ERROR(errorCode, "invalid session information size %zu", pLogData->size);
        }
    }

    return errorCode;
}

SbgErrorCode sbgEComLogSessionInfoWriteToStream(const SbgEComLogSessionInfo *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pLogData->size <= UINT16_MAX);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->pageIndex);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->nrPages);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, (uint16_t)pLogData->size);

    sbgStreamBufferWriteBuffer(pStreamBuffer, pLogData->buffer, pLogData->size);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}
