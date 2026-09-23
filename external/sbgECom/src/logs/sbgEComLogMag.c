// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <defs/sbgEComDefsAiding.h>

// Local headers
#include "sbgEComLogMag.h"

//----------------------------------------------------------------------//
//- Private definitions for status field                               -//
//----------------------------------------------------------------------//

//
// Timestamp time type has been added in 5.4.
//
#define SBG_ECOM_LOG_MAG_TIME_TYPE_SHIFT                    (9u)                /*!< Shift used to extract the magnetometer time type part. */

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogMagReadFromStream(SbgEComLogMag *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->timeStamp         = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status            = sbgStreamBufferReadUint16LE(pStreamBuffer);

    pLogData->magnetometers[0]  = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->magnetometers[1]  = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->magnetometers[2]  = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->accelerometers[0] = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->accelerometers[1] = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->accelerometers[2] = sbgStreamBufferReadFloatLE(pStreamBuffer);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogMagWriteToStream(const SbgEComLogMag *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);

    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->magnetometers[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->magnetometers[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->magnetometers[2]);

    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->accelerometers[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->accelerometers[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->accelerometers[2]);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

void sbgEComLogMagSetTimeType(SbgEComLogMag *pLogData, SbgEComAidingTimeType timeType)
{
    assert(pLogData);
    assert(timeType <= SBG_ECOM_AIDING_TIME_TYPE_MASK);

    pLogData->status &= ~(SBG_ECOM_AIDING_TIME_TYPE_MASK << SBG_ECOM_LOG_MAG_TIME_TYPE_SHIFT);
    pLogData->status |= ((uint16_t)timeType & SBG_ECOM_AIDING_TIME_TYPE_MASK) << SBG_ECOM_LOG_MAG_TIME_TYPE_SHIFT;
}

SbgEComAidingTimeType sbgEComLogMagGetTimeType(const SbgEComLogMag *pLogData)
{
    assert(pLogData);

    return (SbgEComAidingTimeType)((pLogData->status >> SBG_ECOM_LOG_MAG_TIME_TYPE_SHIFT) & SBG_ECOM_AIDING_TIME_TYPE_MASK);
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogMag        SbgLogMag);

SbgErrorCode sbgEComBinaryLogParseMagData(SbgStreamBuffer *pStreamBuffer, SbgEComLogMag *pLogData)
{
    return sbgEComLogMagReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteMagData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogMag *pLogData)
{
    return sbgEComLogMagWriteToStream(pLogData, pStreamBuffer);
}
