// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <defs/sbgEComDefsAiding.h>

// Local headers
#include "sbgEComLogVelocity.h"

//----------------------------------------------------------------------//
//- Private definitions for status field                               -//
//----------------------------------------------------------------------//

#define SBG_ECOM_LOG_VELOCITY_TIME_TYPE_SHIFT       (0u)                /*!< Shift used to extract the velocity time type part. */

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

void sbgEComLogVelocityConstruct(SbgEComLogVelocity *pLogData)
{
    assert(pLogData);

    memset(pLogData, 0, sizeof(*pLogData));
}

SbgErrorCode sbgEComLogVelocityReadFromStream(SbgEComLogVelocity *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    pLogData->timeStamp         = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status            = sbgStreamBufferReadUint16LE(pStreamBuffer);

    pLogData->velocity[0]       = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocity[1]       = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocity[2]       = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->velocityStd[0]    = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocityStd[1]    = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocityStd[2]    = sbgStreamBufferReadFloatLE(pStreamBuffer);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogVelocityWriteToStream(const SbgEComLogVelocity *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);

    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->velocity[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->velocity[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->velocity[2]);

    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->velocityStd[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->velocityStd[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->velocityStd[2]);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

void sbgEComLogVelocitySetTimeType(SbgEComLogVelocity *pLogData, SbgEComAidingTimeType timeType)
{
    assert(pLogData);
    assert(timeType <= SBG_ECOM_AIDING_TIME_TYPE_MASK);

    pLogData->status &= ~(SBG_ECOM_AIDING_TIME_TYPE_MASK << SBG_ECOM_LOG_VELOCITY_TIME_TYPE_SHIFT);
    pLogData->status |= ((uint16_t)timeType & SBG_ECOM_AIDING_TIME_TYPE_MASK) << SBG_ECOM_LOG_VELOCITY_TIME_TYPE_SHIFT;
}

SbgEComAidingTimeType sbgEComLogVelocityGetTimeType(const SbgEComLogVelocity *pLogData)
{
    assert(pLogData);

    return (SbgEComAidingTimeType)((pLogData->status >> SBG_ECOM_LOG_VELOCITY_TIME_TYPE_SHIFT) & SBG_ECOM_AIDING_TIME_TYPE_MASK);
}
