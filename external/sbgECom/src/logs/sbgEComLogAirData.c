#include "sbgEComLogAirData.h"

//----------------------------------------------------------------------//
//- Operations                                                         -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogAirDataReadFromStream(SbgEComLogAirData *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    pLogData->timeStamp             = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status                = sbgStreamBufferReadUint16LE(pStreamBuffer);

    pLogData->pressureAbs           = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->altitude              = sbgStreamBufferReadFloatLE(pStreamBuffer);

    //
    // The true airspeed fields have been added in version 2.0
    //
    if (sbgStreamBufferGetSpace(pStreamBuffer) > 0)
    {
        pLogData->pressureDiff      = sbgStreamBufferReadFloatLE(pStreamBuffer);
        pLogData->trueAirspeed      = sbgStreamBufferReadFloatLE(pStreamBuffer);
        pLogData->airTemperature    = sbgStreamBufferReadFloatLE(pStreamBuffer);
    }
    else
    {
        pLogData->pressureDiff      = 0.0f;
        pLogData->trueAirspeed      = 0.0f;
        pLogData->airTemperature    = 0.0f;
    }

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogAirDataWriteToStream(const SbgEComLogAirData *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->pressureAbs);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->altitude);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->pressureDiff);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->trueAirspeed);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->airTemperature);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComBinaryLogParseAirData(SbgStreamBuffer *pStreamBuffer, SbgEComLogAirData *pLogData)
{
    return sbgEComLogAirDataReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteAirData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogAirData *pLogData)
{
    return sbgEComLogAirDataWriteToStream(pLogData, pStreamBuffer);
}
