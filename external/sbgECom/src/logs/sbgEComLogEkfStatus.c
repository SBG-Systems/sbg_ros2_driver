// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Local headers
#include "sbgEComLogEkfStatus.h"

//----------------------------------------------------------------------//
//- Private definitions                                                -//
//----------------------------------------------------------------------//

#define SBG_ECOM_LOG_EKF_STATUS_NAV_SAFETY_SHIFT            (8u)                /*!< Shift used to extract the navigation safety indicator part. */
#define SBG_ECOM_LOG_EKF_STATUS_NAV_SAFETY_MASK             (0x0007u)           /*!< Mask used to keep only the navigation safety indicator part. */

#define SBG_ECOM_LOG_EKF_STATUS_GNSS_SPOOFING_SHIFT         (0u)                /*!< Shift used to extract the GNSS spoofing indicator part. */
#define SBG_ECOM_LOG_EKF_STATUS_GNSS_SPOOFING_MASK          (0x07u)             /*!< Mask used to keep only the GNSS spoofing indicator part. */

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogEkfStatusReadFromStream(SbgEComLogEkfStatus *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->timeStamp             = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->ekfStatus             = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->solutionType          = sbgStreamBufferReadUint8LE(pStreamBuffer);
    pLogData->alignmentProgress     = sbgStreamBufferReadUint8LE(pStreamBuffer);
    pLogData->gnss1SpoofingStatus   = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pLogData->gnss2SpoofingStatus   = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pLogData->aidingsUsed           = sbgStreamBufferReadUint32LE(pStreamBuffer);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogEkfStatusWriteToStream(const SbgEComLogEkfStatus *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->ekfStatus);
    sbgStreamBufferWriteUint8LE(pStreamBuffer,  pLogData->solutionType);
    sbgStreamBufferWriteUint8LE(pStreamBuffer,  pLogData->alignmentProgress);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->gnss1SpoofingStatus);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->gnss2SpoofingStatus);
    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->aidingsUsed);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

void sbgEComLogEkfStatusConstruct(SbgEComLogEkfStatus *pLogData)
{
    assert(pLogData);

    memset(pLogData, 0, sizeof(*pLogData));
}

void sbgEComLogEkfStatusSetNavSafety(SbgEComLogEkfStatus *pLogData, SbgEComEkfStatusNavSafety navSafety)
{
    assert(pLogData);
    assert(navSafety <= SBG_ECOM_LOG_EKF_STATUS_NAV_SAFETY_MASK);

    pLogData->ekfStatus &= ~(SBG_ECOM_LOG_EKF_STATUS_NAV_SAFETY_MASK << SBG_ECOM_LOG_EKF_STATUS_NAV_SAFETY_SHIFT);
    pLogData->ekfStatus |= ((uint32_t)navSafety & SBG_ECOM_LOG_EKF_STATUS_NAV_SAFETY_MASK) << SBG_ECOM_LOG_EKF_STATUS_NAV_SAFETY_SHIFT;
}

SbgEComEkfStatusNavSafety sbgEComLogEkfStatusGetNavSafety(const SbgEComLogEkfStatus *pLogData)
{
    assert(pLogData);

    return (SbgEComEkfStatusNavSafety)((pLogData->ekfStatus >> SBG_ECOM_LOG_EKF_STATUS_NAV_SAFETY_SHIFT) & SBG_ECOM_LOG_EKF_STATUS_NAV_SAFETY_MASK);
}

void sbgEComLogEkfStatusSetGnss1SpoofingIndicator(SbgEComLogEkfStatus *pLogData, SbgEComEkfStatusSpoofing spoofing)
{
    assert(pLogData);
    assert(spoofing <= SBG_ECOM_LOG_EKF_STATUS_GNSS_SPOOFING_MASK);

    pLogData->gnss1SpoofingStatus &= ~(SBG_ECOM_LOG_EKF_STATUS_GNSS_SPOOFING_MASK << SBG_ECOM_LOG_EKF_STATUS_GNSS_SPOOFING_SHIFT);
    pLogData->gnss1SpoofingStatus |= ((uint8_t)spoofing & SBG_ECOM_LOG_EKF_STATUS_GNSS_SPOOFING_MASK) << SBG_ECOM_LOG_EKF_STATUS_GNSS_SPOOFING_SHIFT;
}

SbgEComEkfStatusSpoofing sbgEComLogEkfStatusGetGnss1SpoofingIndicator(const SbgEComLogEkfStatus *pLogData)
{
    assert(pLogData);

    return (SbgEComEkfStatusSpoofing)((pLogData->gnss1SpoofingStatus >> SBG_ECOM_LOG_EKF_STATUS_GNSS_SPOOFING_SHIFT) & SBG_ECOM_LOG_EKF_STATUS_GNSS_SPOOFING_MASK);
}

void sbgEComLogEkfStatusSetGnss2SpoofingIndicator(SbgEComLogEkfStatus *pLogData, SbgEComEkfStatusSpoofing spoofing)
{
    assert(pLogData);
    assert(spoofing <= SBG_ECOM_LOG_EKF_STATUS_GNSS_SPOOFING_MASK);

    pLogData->gnss2SpoofingStatus &= ~(SBG_ECOM_LOG_EKF_STATUS_GNSS_SPOOFING_MASK << SBG_ECOM_LOG_EKF_STATUS_GNSS_SPOOFING_SHIFT);
    pLogData->gnss2SpoofingStatus |= ((uint8_t)spoofing & SBG_ECOM_LOG_EKF_STATUS_GNSS_SPOOFING_MASK) << SBG_ECOM_LOG_EKF_STATUS_GNSS_SPOOFING_SHIFT;
}

SbgEComEkfStatusSpoofing sbgEComLogEkfStatusGetGnss2SpoofingIndicator(const SbgEComLogEkfStatus *pLogData)
{
    assert(pLogData);

    return (SbgEComEkfStatusSpoofing)((pLogData->gnss2SpoofingStatus >> SBG_ECOM_LOG_EKF_STATUS_GNSS_SPOOFING_SHIFT) & SBG_ECOM_LOG_EKF_STATUS_GNSS_SPOOFING_MASK);
}
