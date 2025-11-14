// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Local headers
#include "sbgEComLogGnssPos.h"

//----------------------------------------------------------------------//
//- Private definitions for status field                               -//
//----------------------------------------------------------------------//
#define SBG_ECOM_LOG_GNSS_POS_STATUS_SHIFT      (0u)                    /*!< Shift used to extract the GNSS position status part. */
#define SBG_ECOM_LOG_GNSS_POS_STATUS_MASK       (0x0000003Fu)           /*!< Mask used to keep only the GNSS position status part. */

#define SBG_ECOM_LOG_GNSS_POS_TYPE_SHIFT        (6u)                    /*!< Shift used to extract the GNSS position type part. */
#define SBG_ECOM_LOG_GNSS_POS_TYPE_MASK         (0x0000003Fu)           /*!< Mask used to keep only the GNSS position type part. */

#define SBG_ECOM_LOG_GNSS_POS_SIGNALS_MASK      (0xFFFFF000u)           /*!< Mask used to keep only the signals used in solution part. */

//----------------------------------------------------------------------//
//- Private definitions for statusExt field                            -//
//----------------------------------------------------------------------//
#define SBG_ECOM_LOG_GNSS_POS_IFM_SHIFT         (0u)                    /*!< Shift used to extract the GNSS interference monitoring and mitigation indicator. */
#define SBG_ECOM_LOG_GNSS_POS_IFM_MASK          (0x0000000Fu)           /*!< Mask used to keep only the GNSS interference monitoring and mitigation indicator. */

#define SBG_ECOM_LOG_GNSS_POS_SPOOFING_SHIFT    (4u)                    /*!< Shift used to extract the GNSS spoofing indicator. */
#define SBG_ECOM_LOG_GNSS_POS_SPOOFING_MASK     (0x0000000Fu)           /*!< Mask used to keep only the GNSS spoofing indicator. */

#define SBG_ECOM_LOG_GNSS_POS_OSNMA_SHIFT       (8u)                    /*!< Shift used to extract the Galileo OSNMA status. */
#define SBG_ECOM_LOG_GNSS_POS_OSNMA_MASK        (0x0000000Fu)           /*!< Mask used to keep only the Galileo OSNMA status. */

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

void sbgEComLogGnssPosZeroInit(SbgEComLogGnssPos *pLogData)
{
    assert(pLogData);

    memset(pLogData, 0x00, sizeof(*pLogData));

    pLogData->latitudeAccuracy      = 9999.0f;
    pLogData->longitudeAccuracy     = 9999.0f;
    pLogData->altitudeAccuracy      = 9999.0f;
    pLogData->numSvTracked          = UINT8_MAX;
    pLogData->numSvUsed             = UINT8_MAX;
    pLogData->baseStationId         = UINT16_MAX;
    pLogData->differentialAge       = UINT16_MAX;

    sbgEComLogGnssPosSetStatus(pLogData,            SBG_ECOM_GNSS_POS_STATUS_INSUFFICIENT_OBS);
    sbgEComLogGnssPosSetType(pLogData,              SBG_ECOM_GNSS_POS_TYPE_NO_SOLUTION);

    sbgEComLogGnssPosSetIfmStatus(pLogData,         SBG_ECOM_GNSS_IFM_STATUS_UNKNOWN);
    sbgEComLogGnssPosSetSpoofingStatus(pLogData,    SBG_ECOM_GNSS_SPOOFING_STATUS_UNKNOWN);
    sbgEComLogGnssPosSetOsnmaStatus(pLogData,       SBG_ECOM_GNSS_OSNMA_STATUS_DISABLED);
}

SbgErrorCode sbgEComLogGnssPosReadFromStream(SbgEComLogGnssPos *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    bool        shouldContinue;

    assert(pLogData);
    assert(pStreamBuffer);

    pLogData->timeStamp             = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status                = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->timeOfWeek            = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->latitude              = sbgStreamBufferReadDoubleLE(pStreamBuffer);
    pLogData->longitude             = sbgStreamBufferReadDoubleLE(pStreamBuffer);
    pLogData->altitude              = sbgStreamBufferReadDoubleLE(pStreamBuffer);
    pLogData->undulation            = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->latitudeAccuracy      = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->longitudeAccuracy     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->altitudeAccuracy      = sbgStreamBufferReadFloatLE(pStreamBuffer);

    //
    // Read additional fields added in version 1.4
    //
    if (sbgStreamBufferGetSpace(pStreamBuffer) >= 5)
    {
        pLogData->numSvUsed         = sbgStreamBufferReadUint8LE(pStreamBuffer);
        pLogData->baseStationId     = sbgStreamBufferReadUint16LE(pStreamBuffer);
        pLogData->differentialAge   = sbgStreamBufferReadUint16LE(pStreamBuffer);
        shouldContinue              = true;
    }
    else
    {
        pLogData->numSvUsed         = UINT8_MAX;
        pLogData->baseStationId     = UINT16_MAX;
        pLogData->differentialAge   = UINT16_MAX;
        shouldContinue              = false;
    }

    //
    // Read additional status added in version 4.0
    //
    if ( shouldContinue && (sbgStreamBufferGetSpace(pStreamBuffer) >= 5) )
    {
        pLogData->numSvTracked      = sbgStreamBufferReadUint8LE(pStreamBuffer);
        pLogData->statusExt         = sbgStreamBufferReadUint32LE(pStreamBuffer);
    }
    else
    {
        pLogData->numSvTracked      = UINT8_MAX;
        pLogData->statusExt         = 0;

        sbgEComLogGnssPosSetIfmStatus(pLogData,         SBG_ECOM_GNSS_IFM_STATUS_UNKNOWN);
        sbgEComLogGnssPosSetSpoofingStatus(pLogData,    SBG_ECOM_GNSS_SPOOFING_STATUS_UNKNOWN);
        sbgEComLogGnssPosSetOsnmaStatus(pLogData,       SBG_ECOM_GNSS_OSNMA_STATUS_DISABLED);
    }
    
    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogGnssPosWriteToStream(const SbgEComLogGnssPos *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint32LE(pStreamBuffer,     pLogData->timeStamp);
    sbgStreamBufferWriteUint32LE(pStreamBuffer,     pLogData->status);
    sbgStreamBufferWriteUint32LE(pStreamBuffer,     pLogData->timeOfWeek);

    sbgStreamBufferWriteDoubleLE(pStreamBuffer,     pLogData->latitude);
    sbgStreamBufferWriteDoubleLE(pStreamBuffer,     pLogData->longitude);
    sbgStreamBufferWriteDoubleLE(pStreamBuffer,     pLogData->altitude);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,      pLogData->undulation);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,      pLogData->latitudeAccuracy);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,      pLogData->longitudeAccuracy);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,      pLogData->altitudeAccuracy);

    sbgStreamBufferWriteUint8LE(pStreamBuffer,      pLogData->numSvUsed);
    sbgStreamBufferWriteUint16LE(pStreamBuffer,     pLogData->baseStationId);
    sbgStreamBufferWriteUint16LE(pStreamBuffer,     pLogData->differentialAge);

    sbgStreamBufferWriteUint8LE(pStreamBuffer,      pLogData->numSvTracked);
    sbgStreamBufferWriteUint32LE(pStreamBuffer,     pLogData->statusExt);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

void sbgEComLogGnssPosSetStatus(SbgEComLogGnssPos *pLogData, SbgEComGnssPosStatus status)
{
    assert(pLogData);
    assert(status <= SBG_ECOM_LOG_GNSS_POS_STATUS_MASK);

    pLogData->status &= ~(SBG_ECOM_LOG_GNSS_POS_STATUS_MASK << SBG_ECOM_LOG_GNSS_POS_STATUS_SHIFT);
    pLogData->status |= ((uint32_t)status&SBG_ECOM_LOG_GNSS_POS_STATUS_MASK) << SBG_ECOM_LOG_GNSS_POS_STATUS_SHIFT;
}

SbgEComGnssPosStatus sbgEComLogGnssPosGetStatus(const SbgEComLogGnssPos *pLogData)
{
    assert(pLogData);

    return (SbgEComGnssPosStatus)((pLogData->status >> SBG_ECOM_LOG_GNSS_POS_STATUS_SHIFT)&SBG_ECOM_LOG_GNSS_POS_STATUS_MASK);
}

void sbgEComLogGnssPosSetType(SbgEComLogGnssPos *pLogData, SbgEComGnssPosType posType)
{
    assert(pLogData);
    assert(posType <= SBG_ECOM_LOG_GNSS_POS_TYPE_MASK);

    pLogData->status &= ~(SBG_ECOM_LOG_GNSS_POS_TYPE_MASK << SBG_ECOM_LOG_GNSS_POS_TYPE_SHIFT);
    pLogData->status |= ((uint32_t)posType&SBG_ECOM_LOG_GNSS_POS_TYPE_MASK) << SBG_ECOM_LOG_GNSS_POS_TYPE_SHIFT;
}

SbgEComGnssPosType sbgEComLogGnssPosGetType(const SbgEComLogGnssPos *pLogData)
{
    assert(pLogData);

    return (SbgEComGnssPosType)((pLogData->status >> SBG_ECOM_LOG_GNSS_POS_TYPE_SHIFT)&SBG_ECOM_LOG_GNSS_POS_TYPE_MASK);
}

void sbgEComLogGnssPosSetSignalsUsed(SbgEComLogGnssPos *pLogData, uint32_t signalMask)
{
    assert(pLogData);
    assert((signalMask&SBG_ECOM_LOG_GNSS_POS_SIGNALS_MASK) == signalMask);

    pLogData->status &= ~SBG_ECOM_LOG_GNSS_POS_SIGNALS_MASK;
    pLogData->status |= signalMask&SBG_ECOM_LOG_GNSS_POS_SIGNALS_MASK;
}

bool sbgEComLogGnssPosSignalsAreUsed(const SbgEComLogGnssPos *pLogData, uint32_t signalsMask)
{
    assert(pLogData);

    if ((pLogData->status & signalsMask) == signalsMask)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool sbgEComLogGnssPosNumSvTrackedIsValid(const SbgEComLogGnssPos *pLogData)
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

bool sbgEComLogGnssPosNumSvUsedIsValid(const SbgEComLogGnssPos *pLogData)
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

bool sbgEComLogGnssPosBaseStationIdIsValid(const SbgEComLogGnssPos *pLogData)
{
    assert(pLogData);

    if (pLogData->baseStationId == UINT16_MAX)
    {
        return false;
    }
    else
    {
        return true;
    }
}
bool sbgEComLogGnssPosDifferentialAgeIsValid(const SbgEComLogGnssPos *pLogData)
{
    assert(pLogData);

    if (pLogData->differentialAge == UINT16_MAX)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void sbgEComLogGnssPosSetDifferentialAge(SbgEComLogGnssPos *pLogData, float differentialAge)
{
    assert(pLogData);
    assert((isnan(differentialAge) != 0) || (differentialAge >= 0.0f));

    if (isnan(differentialAge) == 0)
    {
        float   diffAgeScaled;

        diffAgeScaled = differentialAge / 100.0f;

        if (diffAgeScaled >= (float)UINT16_MAX)
        {
            pLogData->differentialAge = UINT16_MAX;
        }
        else
        {
            pLogData->differentialAge = (uint16_t)diffAgeScaled;
        }
    }
    else
    {
        pLogData->differentialAge = UINT16_MAX;
    }
}

float sbgEComLogGnssPosGetDifferentialAge(const SbgEComLogGnssPos *pLogData)
{
    float   diffAgeSeconds;

    assert(pLogData);

    if (pLogData->differentialAge == UINT16_MAX)
    {
        diffAgeSeconds = NAN;
    }
    else
    {
        diffAgeSeconds = pLogData->differentialAge / 100.0f;
    }

    return diffAgeSeconds;
}

void sbgEComLogGnssPosSetIfmStatus(SbgEComLogGnssPos *pLogData, SbgEComGnssIfmStatus status)
{
    assert(pLogData);
    assert(status <= SBG_ECOM_LOG_GNSS_POS_IFM_MASK);

    pLogData->statusExt &= ~(SBG_ECOM_LOG_GNSS_POS_IFM_MASK << SBG_ECOM_LOG_GNSS_POS_IFM_SHIFT);
    pLogData->statusExt |= ((uint32_t)status&SBG_ECOM_LOG_GNSS_POS_IFM_MASK) << SBG_ECOM_LOG_GNSS_POS_IFM_SHIFT;
}

SbgEComGnssIfmStatus sbgEComLogGnssPosGetIfmStatus(const SbgEComLogGnssPos *pLogData)
{
    assert(pLogData);

    return (SbgEComGnssIfmStatus)((pLogData->statusExt >> SBG_ECOM_LOG_GNSS_POS_IFM_SHIFT)&SBG_ECOM_LOG_GNSS_POS_IFM_MASK);
}

void sbgEComLogGnssPosSetSpoofingStatus(SbgEComLogGnssPos *pLogData, SbgEComGnssSpoofingStatus status)
{
    assert(pLogData);
    assert(status <= SBG_ECOM_LOG_GNSS_POS_SPOOFING_MASK);

    pLogData->statusExt &= ~(SBG_ECOM_LOG_GNSS_POS_SPOOFING_MASK << SBG_ECOM_LOG_GNSS_POS_SPOOFING_SHIFT);
    pLogData->statusExt |= ((uint32_t)status&SBG_ECOM_LOG_GNSS_POS_SPOOFING_MASK) << SBG_ECOM_LOG_GNSS_POS_SPOOFING_SHIFT;
}

SbgEComGnssSpoofingStatus sbgEComLogGnssPosGetSpoofingStatus(const SbgEComLogGnssPos *pLogData)
{
    return (SbgEComGnssSpoofingStatus)((pLogData->statusExt >> SBG_ECOM_LOG_GNSS_POS_SPOOFING_SHIFT)&SBG_ECOM_LOG_GNSS_POS_SPOOFING_MASK);
}

void sbgEComLogGnssPosSetOsnmaStatus(SbgEComLogGnssPos *pLogData, SbgEComGnssOsnmaStatus status)
{
    assert(pLogData);
    assert(status <= SBG_ECOM_LOG_GNSS_POS_OSNMA_MASK);

    pLogData->statusExt &= ~(SBG_ECOM_LOG_GNSS_POS_OSNMA_MASK << SBG_ECOM_LOG_GNSS_POS_OSNMA_SHIFT);
    pLogData->statusExt |= ((uint32_t)status&SBG_ECOM_LOG_GNSS_POS_OSNMA_MASK) << SBG_ECOM_LOG_GNSS_POS_OSNMA_SHIFT;
}

SbgEComGnssOsnmaStatus sbgEComLogGnssPosGetOsnmaStatus(const SbgEComLogGnssPos *pLogData)
{
    return (SbgEComGnssOsnmaStatus)((pLogData->statusExt >> SBG_ECOM_LOG_GNSS_POS_OSNMA_SHIFT)&SBG_ECOM_LOG_GNSS_POS_OSNMA_MASK);
}
//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

uint32_t sbgEComLogGpsPosBuildStatus(SbgEComGnssPosStatus status, SbgEComGnssPosType type, uint32_t masks)
{
    return  ((((uint32_t)status)&SBG_ECOM_LOG_GNSS_POS_STATUS_MASK) << SBG_ECOM_LOG_GNSS_POS_STATUS_SHIFT) |
            ((((uint32_t)type)&SBG_ECOM_LOG_GNSS_POS_TYPE_MASK) << SBG_ECOM_LOG_GNSS_POS_TYPE_SHIFT) | masks;
}

SbgEComGnssPosStatus sbgEComLogGpsPosGetStatus(uint32_t status)
{
    return (SbgEComGnssPosStatus)((status >> SBG_ECOM_LOG_GNSS_POS_STATUS_SHIFT) & SBG_ECOM_LOG_GNSS_POS_STATUS_MASK);
}

SbgEComGnssPosType sbgEComLogGpsPosGetType(uint32_t status)
{
    return (SbgEComGnssPosType)((status >> SBG_ECOM_LOG_GNSS_POS_TYPE_SHIFT) & SBG_ECOM_LOG_GNSS_POS_TYPE_MASK);
}

SbgErrorCode sbgEComBinaryLogParseGpsPosData(SbgStreamBuffer *pStreamBuffer, SbgEComLogGnssPos *pLogData)
{
    return sbgEComLogGnssPosReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteGpsPosData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogGnssPos *pLogData)
{
    return sbgEComLogGnssPosWriteToStream(pLogData, pStreamBuffer);
}
