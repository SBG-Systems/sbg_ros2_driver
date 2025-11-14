// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Local headers
#include "sbgEComLogGnssHdt.h"

//----------------------------------------------------------------------//
//- Private definitions                                                -//
//----------------------------------------------------------------------//
#define SBG_ECOM_LOG_GNSS_HDT_STATUS_SHIFT      (0u)                    /*!< Shift used to extract the GNSS HDT status part. */
#define SBG_ECOM_LOG_GNSS_HDT_STATUS_MASK       (0x003Fu)               /*!< Mask used to keep only the GNSS HDT status part. */

#define SBG_ECOM_LOG_GNSS_HDT_BASELINE_VALID    (0x0001 << 6)           /*!< Set to 1 if the baseline length field is filled and valid. */

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

void sbgEComLogGnssHdtZeroInit(SbgEComLogGnssHdt *pLogData)
{
    assert(pLogData);

    memset(pLogData, 0x00, sizeof(*pLogData));

    pLogData->headingAccuracy   = 180.0f;
    pLogData->pitchAccuracy     = 90.0f;
    pLogData->numSvTracked      = UINT8_MAX;
    pLogData->numSvUsed         = UINT8_MAX;

    sbgEComLogGnssHdtSetStatus(pLogData, SBG_ECOM_GNSS_HDT_STATUS_INSUFFICIENT_OBS);
}

SbgErrorCode sbgEComLogGnssHdtReadFromStream(SbgEComLogGnssHdt *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    pLogData->timeStamp         = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status            = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pLogData->timeOfWeek        = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->heading           = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->headingAccuracy   = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->pitch             = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->pitchAccuracy     = sbgStreamBufferReadFloatLE(pStreamBuffer);

    //
    // The baseline field have been added in version 2.0
    //
    if (sbgStreamBufferGetSpace(pStreamBuffer) > 0)
    {
        pLogData->baseline      = sbgStreamBufferReadFloatLE(pStreamBuffer);
    }
    else
    {
        pLogData->baseline      = 0.0f;
    }

    //
    // Read numSvTracked and numSvUsed added in version 4.0
    //
    if (sbgStreamBufferGetSpace(pStreamBuffer) > 0)
    {
        pLogData->numSvTracked  = sbgStreamBufferReadUint8LE(pStreamBuffer);
    }
    else
    {
        pLogData->numSvTracked  = UINT8_MAX;
    }

    if (sbgStreamBufferGetSpace(pStreamBuffer) > 0)
    {
        pLogData->numSvUsed     = sbgStreamBufferReadUint8LE(pStreamBuffer);
    }
    else
    {
        pLogData->numSvUsed     = UINT8_MAX;
    }

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogGnssHdtWriteToStream(const SbgEComLogGnssHdt *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);
    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeOfWeek);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->heading);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->headingAccuracy);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->pitch);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->pitchAccuracy);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->baseline);
    sbgStreamBufferWriteUint8LE(pStreamBuffer,  pLogData->numSvTracked);
    sbgStreamBufferWriteUint8LE(pStreamBuffer,  pLogData->numSvUsed);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

void sbgEComLogGnssHdtSetStatus(SbgEComLogGnssHdt *pLogData, SbgEComGnssHdtStatus status)
{
    assert(pLogData);
    assert(status <= SBG_ECOM_LOG_GNSS_HDT_STATUS_MASK);

    pLogData->status &= ~(SBG_ECOM_LOG_GNSS_HDT_STATUS_MASK << SBG_ECOM_LOG_GNSS_HDT_STATUS_SHIFT);
    pLogData->status |= ((uint16_t)status&SBG_ECOM_LOG_GNSS_HDT_STATUS_MASK) << SBG_ECOM_LOG_GNSS_HDT_STATUS_SHIFT;
}

SbgEComGnssHdtStatus sbgEComLogGnssHdtGetStatus(const SbgEComLogGnssHdt *pLogData)
{
    assert(pLogData);

    return (SbgEComGnssHdtStatus)((pLogData->status >> SBG_ECOM_LOG_GNSS_HDT_STATUS_SHIFT)&SBG_ECOM_LOG_GNSS_HDT_STATUS_MASK);
}

bool sbgEComLogGnssHdtHeadingIsValid(const SbgEComLogGnssHdt *pLogData)
{
    assert(pLogData);

    if ( (pLogData->headingAccuracy > 0.0f) && (pLogData->headingAccuracy < 180.0f) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool sbgEComLogGnssHdtPitchIsValid(const SbgEComLogGnssHdt *pLogData)
{
    assert(pLogData);

    if ( (pLogData->pitchAccuracy > 0.0f) && (pLogData->pitchAccuracy < 90.0f) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

void sbgEComLogGnssHdtSetBaseLineValid(SbgEComLogGnssHdt *pLogData, bool isValid)
{
    assert(pLogData);

    if (isValid)
    {
        pLogData->status |= SBG_ECOM_LOG_GNSS_HDT_BASELINE_VALID;
    }
    else
    {
        pLogData->status &= ~SBG_ECOM_LOG_GNSS_HDT_BASELINE_VALID;
    }
}

bool sbgEComLogGnssHdtBaseLineIsValid(const SbgEComLogGnssHdt *pLogData)
{
    assert(pLogData);

    if ( (pLogData->status & SBG_ECOM_LOG_GNSS_HDT_BASELINE_VALID) && (pLogData->baseline > 0.0f) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool sbgEComLogGnssHdtNumSvTrackedIsValid(const SbgEComLogGnssHdt *pLogData)
{
    assert(pLogData);

    if (pLogData->numSvTracked == UINT8_MAX)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool sbgEComLogGnssHdtNumSvUsedIsValid(const SbgEComLogGnssHdt *pLogData)
{
    assert(pLogData);

    if (pLogData->numSvUsed == UINT8_MAX)
    {
        return false;
    }
    else
    {
        return true;
    }
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

uint32_t sbgEComLogGpsHdtBuildStatus(SbgEComGnssHdtStatus status, uint32_t masks)
{
    return  (((uint32_t)status&SBG_ECOM_LOG_GNSS_HDT_STATUS_MASK) << SBG_ECOM_LOG_GNSS_HDT_STATUS_SHIFT) | masks;
}

SbgEComGnssHdtStatus sbgEComLogGpsHdtGetStatus(uint32_t status)
{
    return (SbgEComGnssHdtStatus)((status >> SBG_ECOM_LOG_GNSS_HDT_STATUS_SHIFT) & SBG_ECOM_LOG_GNSS_HDT_STATUS_MASK);
}

SbgErrorCode sbgEComBinaryLogParseGpsHdtData(SbgStreamBuffer *pStreamBuffer, SbgEComLogGnssHdt *pLogData)
{
    return sbgEComLogGnssHdtReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteGpsHdtData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogGnssHdt *pLogData)
{
    return sbgEComLogGnssHdtWriteToStream(pLogData, pStreamBuffer);
}
