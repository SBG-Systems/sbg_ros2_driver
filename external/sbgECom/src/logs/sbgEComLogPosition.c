// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Local headers
#include "sbgEComLogPosition.h"

//----------------------------------------------------------------------//
//- Private definitions for status field                               -//
//----------------------------------------------------------------------//

#define SBG_ECOM_LOG_POSITION_TYPE_SHIFT            (3u)                    /*!< Shift used to extract the position type part. */
#define SBG_ECOM_LOG_POSITION_TYPE_MASK             (0x0007u)               /*!< Mask used to keep only the position type part. */

#define SBG_ECOM_LOG_POSITION_TIME_TYPE_SHIFT       (0u)                    /*!< Shift used to extract the position time type part. */

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogPositionReadFromStream(SbgEComLogPosition *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    pLogData->timeStamp     = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status        = sbgStreamBufferReadUint16LE(pStreamBuffer);

    pLogData->position[0]   = sbgStreamBufferReadDoubleLE(pStreamBuffer);
    pLogData->position[1]   = sbgStreamBufferReadDoubleLE(pStreamBuffer);
    pLogData->position[2]   = sbgStreamBufferReadDoubleLE(pStreamBuffer);

    pLogData->covLatLat     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->covLonLon     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->covHgtHgt     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->covLatLon     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->covLatHgt     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->covLonHgt     = sbgStreamBufferReadFloatLE(pStreamBuffer);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogPositionWriteToStream(const SbgEComLogPosition *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);

    sbgStreamBufferWriteDoubleLE(pStreamBuffer, pLogData->position[0]);
    sbgStreamBufferWriteDoubleLE(pStreamBuffer, pLogData->position[1]);
    sbgStreamBufferWriteDoubleLE(pStreamBuffer, pLogData->position[2]);

    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->covLatLat);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->covLonLon);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->covHgtHgt);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->covLatLon);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->covLatHgt);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->covLonHgt);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

void sbgEComLogPositionSetPositionType(SbgEComLogPosition *pLogData, SbgEComPositionType posType)
{
    assert(pLogData);
    assert(posType <= SBG_ECOM_LOG_POSITION_TYPE_MASK);

    pLogData->status    &= ~(SBG_ECOM_LOG_POSITION_TYPE_MASK << SBG_ECOM_LOG_POSITION_TYPE_SHIFT);
    pLogData->status    |= ((uint16_t)posType & SBG_ECOM_LOG_POSITION_TYPE_MASK) << SBG_ECOM_LOG_POSITION_TYPE_SHIFT;
}

SbgEComPositionType sbgEComLogPositionGetPositionType(const SbgEComLogPosition *pLogData)
{
    assert(pLogData);

    return (SbgEComPositionType)((pLogData->status >> SBG_ECOM_LOG_POSITION_TYPE_SHIFT) & SBG_ECOM_LOG_POSITION_TYPE_MASK);
}

void sbgEComLogPositionSetTimeType(SbgEComLogPosition *pLogData, SbgEComAidingTimeType timeType)
{
    assert(pLogData);
    assert(timeType <= SBG_ECOM_AIDING_TIME_TYPE_MASK);

    pLogData->status    &= ~(SBG_ECOM_AIDING_TIME_TYPE_MASK << SBG_ECOM_LOG_POSITION_TIME_TYPE_SHIFT);
    pLogData->status    |= ((uint16_t)timeType & SBG_ECOM_AIDING_TIME_TYPE_MASK) << SBG_ECOM_LOG_POSITION_TIME_TYPE_SHIFT;
}

SbgEComAidingTimeType sbgEComLogPositionGetTimeType(const SbgEComLogPosition *pLogData)
{
    assert(pLogData);

    return (SbgEComAidingTimeType)((pLogData->status >> SBG_ECOM_LOG_POSITION_TIME_TYPE_SHIFT) & SBG_ECOM_AIDING_TIME_TYPE_MASK);
}
