// sbgCommonLib headers
#include <sbgCommon.h>

// Project headers
#include <defs/sbgEComDefsAiding.h>

// Local headers
#include "sbgEComLogAirData.h"

//----------------------------------------------------------------------//
//- Private definitions for status field                               -//
//----------------------------------------------------------------------//

//
// Timestamp time type has been added in 5.4.
//
#define SBG_ECOM_LOG_AIR_DATA_TIME_TYPE_SHIFT               (6u)                /*!< Shift used to extract the air data time type part. */

//----------------------------------------------------------------------//
//- Operations                                                         -//
//----------------------------------------------------------------------//

void sbgEComLogAirDataConstruct(SbgEComLogAirData *pLogData)
{
    assert(pLogData);

    memset(pLogData, 0, sizeof(*pLogData));
}

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
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

void sbgEComLogAirDataSetTimeType(SbgEComLogAirData *pLogData, SbgEComAidingTimeType timeType)
{
    assert(pLogData);
    assert(timeType <= SBG_ECOM_AIDING_TIME_TYPE_MASK);

    pLogData->status &= ~(SBG_ECOM_AIDING_TIME_TYPE_MASK << SBG_ECOM_LOG_AIR_DATA_TIME_TYPE_SHIFT);
    pLogData->status |= ((uint16_t)timeType & SBG_ECOM_AIDING_TIME_TYPE_MASK) << SBG_ECOM_LOG_AIR_DATA_TIME_TYPE_SHIFT;
}

SbgEComAidingTimeType sbgEComLogAirDataGetTimeType(const SbgEComLogAirData *pLogData)
{
    assert(pLogData);

    return (SbgEComAidingTimeType)((pLogData->status >> SBG_ECOM_LOG_AIR_DATA_TIME_TYPE_SHIFT) & SBG_ECOM_AIDING_TIME_TYPE_MASK);
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
