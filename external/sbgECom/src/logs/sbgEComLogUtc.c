// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Local headers
#include "sbgEComLogUtc.h"

//----------------------------------------------------------------------//
//- Private methods                                                    -//
//----------------------------------------------------------------------//

/*!
 * Clock status and UTC time status definitions.
 */
#define SBG_ECOM_LOG_UTC_CLOCK_STATE_SHIFT          (1u)                    /*!< Shift used to extract the clock state part. */
#define SBG_ECOM_LOG_UTC_CLOCK_STATE_MASK           (0x000Fu)               /*!< Mask used to keep only the clock state part. */
#define SBG_ECOM_LOG_UTC_TIME_STATUS_SHIFT          (6u)                    /*!< Shift used to extract the UTC status part. */
#define SBG_ECOM_LOG_UTC_TIME_STATUS_MASK           (0x000Fu)               /*!< Mask used to keep only the UTC status part. */

#define SBG_ECOM_LOG_UTC_STATUS_HAS_CLOCK_INPUT     (0x0001u << 0)          /*!< Set to 1 if a stable input clock could be used to synchronized the internal clock. */
#define SBG_ECOM_LOG_UTC_STATUS_UTC_IS_ACCURATE     (0x0001u << 5)          /*!< The UTC time information is accurate and correctly timestamped with a PPS. */

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

void sbgEComLogUtcZeroInit(SbgEComLogUtc *pLogData)
{
    assert(pLogData);

    memset(pLogData, 0x00, sizeof(*pLogData));

    pLogData->clkBiasStd        = NAN;
    pLogData->clkSfErrorStd     = NAN;
    pLogData->clkResidualError  = NAN;
}

SbgErrorCode sbgEComLogUtcReadFromStream(SbgEComLogUtc *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->timeStamp         = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status            = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pLogData->year              = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pLogData->month             = sbgStreamBufferReadInt8LE(pStreamBuffer);
    pLogData->day               = sbgStreamBufferReadInt8LE(pStreamBuffer);
    pLogData->hour              = sbgStreamBufferReadInt8LE(pStreamBuffer);
    pLogData->minute            = sbgStreamBufferReadInt8LE(pStreamBuffer);
    pLogData->second            = sbgStreamBufferReadInt8LE(pStreamBuffer);
    pLogData->nanoSecond        = sbgStreamBufferReadInt32LE(pStreamBuffer);
    pLogData->gpsTimeOfWeek     = sbgStreamBufferReadUint32LE(pStreamBuffer);

    if (sbgStreamBufferGetSpace(pStreamBuffer) >= 3*sizeof(float))
    {
        pLogData->clkBiasStd        = sbgStreamBufferReadFloatLE(pStreamBuffer);
        pLogData->clkSfErrorStd     = sbgStreamBufferReadFloatLE(pStreamBuffer);
        pLogData->clkResidualError  = sbgStreamBufferReadFloatLE(pStreamBuffer);
    }
    else
    {
        pLogData->clkBiasStd        = NAN;
        pLogData->clkSfErrorStd     = NAN;
        pLogData->clkResidualError  = NAN;
    }

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogUtcWriteToStream(const SbgEComLogUtc *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->year);
    sbgStreamBufferWriteInt8LE(pStreamBuffer,   pLogData->month);
    sbgStreamBufferWriteInt8LE(pStreamBuffer,   pLogData->day);
    sbgStreamBufferWriteInt8LE(pStreamBuffer,   pLogData->hour);
    sbgStreamBufferWriteInt8LE(pStreamBuffer,   pLogData->minute);
    sbgStreamBufferWriteInt8LE(pStreamBuffer,   pLogData->second);
    sbgStreamBufferWriteInt32LE(pStreamBuffer,  pLogData->nanoSecond);
    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->gpsTimeOfWeek);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->clkBiasStd);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->clkSfErrorStd);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->clkResidualError);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

void sbgEComLogUtcSetClockState(SbgEComLogUtc *pLogData, SbgEComClockState status)
{
    assert(pLogData);
    assert(status <= SBG_ECOM_LOG_UTC_CLOCK_STATE_MASK);

    pLogData->status &= ~(SBG_ECOM_LOG_UTC_CLOCK_STATE_MASK << SBG_ECOM_LOG_UTC_CLOCK_STATE_SHIFT);
    pLogData->status |= ((uint16_t)status&SBG_ECOM_LOG_UTC_CLOCK_STATE_MASK) << SBG_ECOM_LOG_UTC_CLOCK_STATE_SHIFT;
}

SbgEComClockState sbgEComLogUtcGetClockState(const SbgEComLogUtc *pLogData)
{
    assert(pLogData);

    return (SbgEComClockState)((pLogData->status >> SBG_ECOM_LOG_UTC_CLOCK_STATE_SHIFT)&SBG_ECOM_LOG_UTC_CLOCK_STATE_MASK);
}

const char *sbgEComLogUtcGetClockStateAsString(const SbgEComLogUtc *pLogData)
{
    static const char *clockStateStr[] =
    {
        [SBG_ECOM_CLOCK_STATE_ERROR]            = "error",
        [SBG_ECOM_CLOCK_STATE_FREE_RUNNING]     = "free",
        [SBG_ECOM_CLOCK_STATE_STEERING]         = "steering",
        [SBG_ECOM_CLOCK_STATE_VALID]            = "valid"
    };

    SbgEComClockState       clockState;
    
    assert(pLogData);

    clockState = sbgEComLogUtcGetClockState(pLogData);
    
    if (clockState < SBG_ARRAY_SIZE(clockStateStr))
    {
        return clockStateStr[clockState];
    }
    else
    {
        return "undefined";
    }
}

void sbgEComLogUtcSetUtcStatus(SbgEComLogUtc *pLogData, SbgEComUtcStatus status)
{
    assert(pLogData);
    assert(status <= SBG_ECOM_LOG_UTC_TIME_STATUS_MASK);

    pLogData->status &= ~(SBG_ECOM_LOG_UTC_TIME_STATUS_MASK << SBG_ECOM_LOG_UTC_TIME_STATUS_SHIFT);
    pLogData->status |= ((uint16_t)status&SBG_ECOM_LOG_UTC_TIME_STATUS_MASK) << SBG_ECOM_LOG_UTC_TIME_STATUS_SHIFT;
}

SbgEComUtcStatus sbgEComLogUtcGetUtcStatus(const SbgEComLogUtc *pLogData)
{
    assert(pLogData);

    return (SbgEComUtcStatus)((pLogData->status >> SBG_ECOM_LOG_UTC_TIME_STATUS_SHIFT)&SBG_ECOM_LOG_UTC_TIME_STATUS_MASK);
}

const char *sbgEComLogUtcGetUtcStatusAsString(const SbgEComLogUtc *pLogData)
{
    static const char *utcStatusStr[] =
    {
        [SBG_ECOM_UTC_STATUS_INVALID]           = "invalid",
        [SBG_ECOM_UTC_STATUS_NO_LEAP_SEC]       = "noLeapSec",
        [SBG_ECOM_UTC_STATUS_INITIALIZED]       = "initialized",
    };

    SbgEComUtcStatus    utcStatus;

    assert(pLogData);

    utcStatus = sbgEComLogUtcGetUtcStatus(pLogData);
        
    if (utcStatus < SBG_ARRAY_SIZE(utcStatusStr))
    {
        return utcStatusStr[utcStatus];
    }
    else
    {
        return "undefined";
    }
}

void sbgEComLogUtcSetHasClockInput(SbgEComLogUtc *pLogData, bool hasClockInput)
{
    assert(pLogData);
    
    if (hasClockInput)
    {
        pLogData->status |= SBG_ECOM_LOG_UTC_STATUS_HAS_CLOCK_INPUT;
    }
    else
    {
        pLogData->status &= ~SBG_ECOM_LOG_UTC_STATUS_HAS_CLOCK_INPUT;
    }
}

bool sbgEComLogUtcHasClockInput(const SbgEComLogUtc *pLogData)
{
    assert(pLogData);

    return (pLogData->status&SBG_ECOM_LOG_UTC_STATUS_HAS_CLOCK_INPUT?true:false);
}

void sbgEComLogUtcTimeSetIsAccurate(SbgEComLogUtc *pLogData, bool utcIsAccurate)
{
    assert(pLogData);
    
    if (utcIsAccurate)
    {
        pLogData->status |= SBG_ECOM_LOG_UTC_STATUS_UTC_IS_ACCURATE;
    }
    else
    {
        pLogData->status &= ~SBG_ECOM_LOG_UTC_STATUS_UTC_IS_ACCURATE;
    }
}

bool sbgEComLogUtcTimeIsAccurate(const SbgEComLogUtc *pLogData)
{
    assert(pLogData);

    return (pLogData->status&SBG_ECOM_LOG_UTC_STATUS_UTC_IS_ACCURATE?true:false);
}

bool sbgEComLogUtcTimeClkBiasStdIsValid(const SbgEComLogUtc *pLogData)
{
    assert(pLogData);

    return (isnan(pLogData->clkBiasStd)?false:true);
}

bool sbgEComLogUtcTimeClkSfErrorStdIsValid(const SbgEComLogUtc *pLogData)
{
    assert(pLogData);

    return (isnan(pLogData->clkSfErrorStd)?false:true);
}

bool sbgEComLogUtcTimeClkResidualErrorIsValid(const SbgEComLogUtc *pLogData)
{
    assert(pLogData);

    return (isnan(pLogData->clkResidualError)?false:true);
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComBinaryLogParseUtcData(SbgStreamBuffer *pStreamBuffer, SbgEComLogUtc *pLogData)
{
    return sbgEComLogUtcReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteUtcData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogUtc *pLogData)
{
    return sbgEComLogUtcWriteToStream(pLogData, pStreamBuffer);
}

SbgEComClockState sbgEComLogUtcGetClockStatus(uint16_t status)
{
    return (SbgEComClockState)((status >> SBG_ECOM_LOG_UTC_CLOCK_STATE_SHIFT) & SBG_ECOM_LOG_UTC_CLOCK_STATE_MASK);
}

const char *sbgEcomLogUtcGetClockStatusAsString(const SbgEComLogUtc *pLogUtc)
{
    return sbgEComLogUtcGetClockStateAsString(pLogUtc);
}

SbgEComUtcStatus sbgEComLogUtcGetClockUtcStatus(uint16_t status)
{
    return (SbgEComUtcStatus)((status >> SBG_ECOM_LOG_UTC_TIME_STATUS_SHIFT) & SBG_ECOM_LOG_UTC_TIME_STATUS_MASK);
}

const char *sbgEcomLogUtcGetUtcStatusAsString(const SbgEComLogUtc *pLogUtc)
{
    return sbgEComLogUtcGetUtcStatusAsString(pLogUtc);
}

uint16_t sbgEComLogUtcBuildClockStatus(SbgEComClockState clockState, SbgEComUtcStatus utcStatus, uint16_t masks)
{
    return  ((((uint16_t)clockState)&SBG_ECOM_LOG_UTC_CLOCK_STATE_MASK) << SBG_ECOM_LOG_UTC_CLOCK_STATE_SHIFT) |
            ((((uint16_t)utcStatus)&SBG_ECOM_LOG_UTC_TIME_STATUS_MASK) << SBG_ECOM_LOG_UTC_TIME_STATUS_SHIFT) | masks;
}
