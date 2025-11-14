// sbgCommonLib headers
#include <sbgCommon.h>

// Local headers
#include "sbgEComLogRawData.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogRawDataReadFromStream(SbgEComLogRawData *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    SbgErrorCode    errorCode = SBG_NO_ERROR;
    size_t          payloadSize;

    assert(pStreamBuffer);
    assert(pLogData);

    payloadSize = sbgStreamBufferGetSize(pStreamBuffer);

    if (payloadSize <= SBG_ECOM_RAW_DATA_MAX_BUFFER_SIZE)
    {
        errorCode = sbgStreamBufferReadBuffer(pStreamBuffer, pLogData->rawBuffer, payloadSize);
        pLogData->bufferSize = payloadSize;
    }
    else
    {
        errorCode = SBG_BUFFER_OVERFLOW;
    }

    return errorCode;
}

SbgErrorCode sbgEComLogRawDataWriteToStream(const SbgEComLogRawData *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    return sbgStreamBufferWriteBuffer(pStreamBuffer, pLogData->rawBuffer, pLogData->bufferSize);
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComBinaryLogParseRawData(SbgStreamBuffer *pStreamBuffer, SbgEComLogRawData *pLogData)
{
    return sbgEComLogRawDataReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteRawData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogRawData *pLogData)
{
    return sbgEComLogRawDataWriteToStream(pLogData, pStreamBuffer);
}
