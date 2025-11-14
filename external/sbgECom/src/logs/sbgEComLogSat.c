/*!
 * \file            sbgEComLogSat.c
 * \author          SBG Systems
 * \date            1 March 2022
 *
 * \brief           Handle binary satellite logs.
 *
 * \copyright       Copyright (C) 2007-2024, SBG Systems SAS. All rights reserved.
 * \beginlicense    The MIT license
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * \endlicense
 */

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <defs/sbgEComDefsGnss.h>

// Local headers
#include "sbgEComLogSat.h"

//----------------------------------------------------------------------//
//- Constant definitions                                               -//
//----------------------------------------------------------------------//

#define SBG_ECOM_LOG_SAT_TRACKING_STATUS_OFFSET             (0)                                                     /*!< Offset of the tracking status field, in bits. */
#define SBG_ECOM_LOG_SAT_TRACKING_STATUS_WIDTH              (3)                                                     /*!< Width of the tracking status field, in bits. */
#define SBG_ECOM_LOG_SAT_TRACKING_STATUS_MASK               ((1u << SBG_ECOM_LOG_SAT_TRACKING_STATUS_WIDTH) - 1)    /*!< Tracking status field mask. */

#define SBG_ECOM_LOG_SAT_HEALTH_STATUS_OFFSET               (3)                                                     /*!< Offset of the health status field, in bits. */
#define SBG_ECOM_LOG_SAT_HEALTH_STATUS_WIDTH                (2)                                                     /*!< Width of the health status field, in bits. */
#define SBG_ECOM_LOG_SAT_HEALTH_STATUS_MASK                 ((1u << SBG_ECOM_LOG_SAT_HEALTH_STATUS_WIDTH) - 1)      /*!< Health status field mask. */

#define SBG_ECOM_LOG_SAT_ELEVATION_STATUS_OFFSET            (5)                                                     /*!< Offset of the elevation status field, in bits. */
#define SBG_ECOM_LOG_SAT_ELEVATION_STATUS_WIDTH             (2)                                                     /*!< Width of the elevation status field, in bits. */
#define SBG_ECOM_LOG_SAT_ELEVATION_STATUS_MASK              ((1u << SBG_ECOM_LOG_SAT_ELEVATION_STATUS_WIDTH) - 1)   /*!< Elevation status field mask. */

#define SBG_ECOM_LOG_SAT_CONSTELLATION_ID_OFFSET            (7)                                                     /*!< Offset of the constellation ID field, in bits. */
#define SBG_ECOM_LOG_SAT_CONSTELLATION_ID_WIDTH             (4)                                                     /*!< Width of the constellation ID field, in bits. */
#define SBG_ECOM_LOG_SAT_CONSTELLATION_ID_MASK              ((1u << SBG_ECOM_LOG_SAT_CONSTELLATION_ID_WIDTH) - 1)   /*!< Constellation ID field mask. */

#define SBG_ECOM_LOG_SAT_SIGNAL_SNR_VALID                   (1u << 5)                                               /*!< Set if the SNR value is valid. */

//----------------------------------------------------------------------//
//- Private functions                                                  -//
//----------------------------------------------------------------------//

/*!
 * Get a bit field from a set of flags.
 *
 * \param[in]   flags                       Flags.
 * \param[in]   offset                      Field offset, in bits.
 * \param[in]   mask                        Field mask.
 * \return                                  Field value.
 */
#define sbgEComLogSatGetField(flags, offset, mask)                  (((flags) >> (offset)) & (mask))

/*!
 * Set a bit field from a set of flags.
 *
 * \param[in/out]   flags                       Flags.
 * \param[in]       value                       Field value.
 * \param[in]       offset                      Field offset, in bits.
 * \param[in]       mask                        Field mask.
 */
#define sbgEComLogSatSetField(flags, value, offset, mask)           (flags) &= ~((mask) << (offset)); (flags) |= ((value) & (mask)) << (offset)

/*!
 * Returns tracking status string from enum value
 *
 * \param[in]   trackingStatus                      Tracking status enum to convert
 * \return                                          Tracking status as a read only C string.
 */
static const char *sbgEComLogSatTrackingStatusToStr(SbgEComSatTrackingStatus trackingStatus)
{
    static const char               *enumToStrLut[] = 
    {
        [SBG_ECOM_SAT_TRACKING_STATUS_UNKNOWN]              = "unknown",
        [SBG_ECOM_SAT_TRACKING_STATUS_SEARCHING]            = "searching",
        [SBG_ECOM_SAT_TRACKING_STATUS_TRACKING_UNKNOWN]     = "tracking",
        [SBG_ECOM_SAT_TRACKING_STATUS_TRACKING_NOT_USED]    = "unused",
        [SBG_ECOM_SAT_TRACKING_STATUS_TRACKING_REJECTED]    = "rejected",
        [SBG_ECOM_SAT_TRACKING_STATUS_TRACKING_USED]        = "used",
    };

    if (trackingStatus < SBG_ARRAY_SIZE(enumToStrLut))
    {
        return enumToStrLut[trackingStatus];
    }
    else
    {
        return enumToStrLut[SBG_ECOM_SAT_TRACKING_STATUS_UNKNOWN];
    }
}

/*!
 * Returns the best tracking status between the two inputs.
 *
 * \param[in]   trackingStatus1             First tracking status.
 * \param[in]   trackingStatus2             Second tracking status.
 */
static SbgEComSatTrackingStatus sbgEComLogSatSelectTrackingStatus(SbgEComSatTrackingStatus trackingStatus1, SbgEComSatTrackingStatus trackingStatus2)
{
    SbgEComSatTrackingStatus             trackingStatus;

    if (trackingStatus2 > trackingStatus1)
    {
        trackingStatus = trackingStatus2;
    }
    else
    {
        trackingStatus = trackingStatus1;
    }

    return trackingStatus;
}

/*!
 * Returns health status string from enum value
 *
 * \param[in]   trackingStatus                      Tracking status enum to convert
 * \return                                          Tracking status as a read only C string.
 */
static const char *sbgEComLogSatHealthStatusToStr(SbgEComSatHealthStatus healthStatus)
{
    static const char               *enumToStrLut[] = 
    {
        [SBG_ECOM_SAT_HEALTH_STATUS_UNKNOWN]                = "unknown",
        [SBG_ECOM_SAT_HEALTH_STATUS_HEALTHY]                = "healthy",
        [SBG_ECOM_SAT_HEALTH_STATUS_UNHEALTHY]              = "unhealthy",
    };

    if (healthStatus < SBG_ARRAY_SIZE(enumToStrLut))
    {
        return enumToStrLut[healthStatus];
    }
    else
    {
        return enumToStrLut[SBG_ECOM_SAT_HEALTH_STATUS_UNKNOWN];
    }
}

/*!
 * Returns the worst health status between the two inputs.
 *
 * \param[in]   healthStatus1               First health status.
 * \param[in]   healthStatus2               Second health status.
 */
static SbgEComSatHealthStatus sbgEComLogSatSelectHealthStatus(SbgEComSatHealthStatus healthStatus1, SbgEComSatHealthStatus healthStatus2)
{
    SbgEComSatHealthStatus               healthStatus;

    if (healthStatus2 > healthStatus1)
    {
        healthStatus = healthStatus2;
    }
    else
    {
        healthStatus = healthStatus1;
    }

    return healthStatus;
}

/*!
 * Returns the elevation status string from enum value
 *
 * \param[in]   elevationStatus                     Elevation status enum to convert
 * \return                                          Elevation status as a read only C string.
 */
static const char *sbgEComLogSatElevationStatusToStr(SbgEComSatElevationStatus elevationStatus)
{
    static const char               *enumToStrLut[] = 
    {
        [SBG_ECOM_SAT_ELEVATION_STATUS_UNKNOWN]             = "unknown",
        [SBG_ECOM_SAT_ELEVATION_STATUS_SETTING]             = "setting",
        [SBG_ECOM_SAT_ELEVATION_STATUS_RISING]              = "rising",
    };

    if (elevationStatus < SBG_ARRAY_SIZE(enumToStrLut))
    {
        return enumToStrLut[elevationStatus];
    }
    else
    {
        return enumToStrLut[SBG_ECOM_SAT_ELEVATION_STATUS_UNKNOWN];
    }
}

//----------------------------------------------------------------------//
//- Private functions (SbgLogSatSignalData)                            -//
//----------------------------------------------------------------------//

/*!
 * Parse a satellite signal instance from a stream buffer.
 *
 * \param[out]  pSatSignal                  Satellite signal instance.
 * \param[in]   pStreamBuffer               Stream buffer.
 * \return                                  SBG_NO_ERROR if successful.
 */
static SbgErrorCode sbgEComLogSatSignalReadFromStream(SbgEComLogSatSignal *pSatSignal, SbgStreamBuffer *pStreamBuffer)
{
    assert(pSatSignal);
    assert(pStreamBuffer);

    pSatSignal->id      = sbgStreamBufferReadUint8LE(pStreamBuffer);
    pSatSignal->flags   = sbgStreamBufferReadUint8LE(pStreamBuffer);
    pSatSignal->snr     = sbgStreamBufferReadUint8LE(pStreamBuffer);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

/*!
 * Write a satellite signal instance to a stream buffer.
 *
 * \param[in]   pSatSignal                  Satellite signal instance.
 * \param[out]  pStreamBuffer               Stream buffer.
 * \return                                  SBG_NO_ERROR if successful.
 */
static SbgErrorCode sbgEComLogSatSignalWriteToStream(const SbgEComLogSatSignal *pSatSignal, SbgStreamBuffer *pStreamBuffer)
{
    assert(pSatSignal);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint8LE(pStreamBuffer,  pSatSignal->id);
    sbgStreamBufferWriteUint8LE(pStreamBuffer,  pSatSignal->flags);
    sbgStreamBufferWriteUint8LE(pStreamBuffer,  pSatSignal->snr);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Private functions (SbgEComLogSatEntry)                             -//
//----------------------------------------------------------------------//

/*!
 * Satellite entry constructor.
 *
 * \param[in]   pSatData                    Satellite entry.
 * \param[in]   id                          Satellite ID.
 * \param[in]   elevation                   Elevation, in degrees.
 * \param[in]   azimuth                     Azimuth, in degrees.
 * \param[in]   constellationId             Constellation ID.
 * \param[in]   elevationStatus             Elevation status.
 * \param[in]   healthStatus                Health status.
 * \param[in]   trackingStatus              Tracking status.
 */
static void sbgEComLogSatEntryConstruct(SbgEComLogSatEntry *pSatData, uint8_t id, int8_t elevation, uint16_t azimuth, SbgEComConstellationId constellationId, SbgEComSatElevationStatus elevationStatus, SbgEComSatHealthStatus healthStatus, SbgEComSatTrackingStatus trackingStatus)
{
    uint16_t                             flags = 0;

    assert(pSatData);

    sbgEComLogSatSetField(flags, constellationId,   SBG_ECOM_LOG_SAT_CONSTELLATION_ID_OFFSET,   SBG_ECOM_LOG_SAT_CONSTELLATION_ID_MASK);
    sbgEComLogSatSetField(flags, elevationStatus,   SBG_ECOM_LOG_SAT_ELEVATION_STATUS_OFFSET,   SBG_ECOM_LOG_SAT_ELEVATION_STATUS_MASK);
    sbgEComLogSatSetField(flags, healthStatus,      SBG_ECOM_LOG_SAT_HEALTH_STATUS_OFFSET,      SBG_ECOM_LOG_SAT_HEALTH_STATUS_MASK);
    sbgEComLogSatSetField(flags, trackingStatus,    SBG_ECOM_LOG_SAT_TRACKING_STATUS_OFFSET,    SBG_ECOM_LOG_SAT_TRACKING_STATUS_MASK);

    pSatData->id                    = id;
    pSatData->elevation             = elevation;
    pSatData->azimuth               = azimuth;
    pSatData->flags                 = flags;
    pSatData->nrSignals             = 0;
}

/*!
 * Update the health and tracking summary statuses of satellite data.
 * 
 * The SbgEComLogSatEntry stores a synthetic health and tracking statues.
 * This synthetic status is computed from each signal status using priority rules.
 * 
 * For example:
 *  - if at least one signal reports an unhealthy signal, the whole SV is flagged unhealthy.
 *  - if at least one signal is tracked and used in the solution, the whole SV is flagged as tracked+used.
 *
 * \param[in]   pSatData                    Satellite data.
 * \param[in]   healthStatus                Health status.
 * \param[in]   trackingStatus              Tracking status.
 */
static void sbgEComLogSatEntryUpdateStatus(SbgEComLogSatEntry *pSatData, SbgEComSatHealthStatus healthStatus, SbgEComSatTrackingStatus trackingStatus)
{
    uint16_t                             flags;

    assert(pSatData);

    flags = pSatData->flags;

    healthStatus    = sbgEComLogSatSelectHealthStatus(sbgEComLogSatEntryGetHealthStatus(pSatData),      healthStatus);
    trackingStatus  = sbgEComLogSatSelectTrackingStatus(sbgEComLogSatEntryGetTrackingStatus(pSatData),  trackingStatus);

    sbgEComLogSatSetField(flags, healthStatus,      SBG_ECOM_LOG_SAT_HEALTH_STATUS_OFFSET,      SBG_ECOM_LOG_SAT_HEALTH_STATUS_MASK);
    sbgEComLogSatSetField(flags, trackingStatus,    SBG_ECOM_LOG_SAT_TRACKING_STATUS_OFFSET,    SBG_ECOM_LOG_SAT_TRACKING_STATUS_MASK);

    pSatData->flags = flags;
}

/*!
 * Read a satellite entry instance from a stream buffer.
 *
 * \param[out]  pSatData                    Satellite entry instance.
 * \param[in]   pStreamBuffer               Stream buffer.
 * \return                                  SBG_NO_ERROR if successful.
 */
static SbgErrorCode sbgEComLogSatEntryReadFromStream(SbgEComLogSatEntry *pSatData, SbgStreamBuffer *pStreamBuffer)
{
    SbgErrorCode                         errorCode;

    assert(pSatData);
    assert(pStreamBuffer);

    pSatData->id                    = sbgStreamBufferReadUint8LE(pStreamBuffer);
    pSatData->elevation             = sbgStreamBufferReadInt8LE(pStreamBuffer);
    pSatData->azimuth               = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pSatData->flags                 = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pSatData->nrSignals             = sbgStreamBufferReadUint8LE(pStreamBuffer);
    
    errorCode = sbgStreamBufferGetLastError(pStreamBuffer);

    if (errorCode == SBG_NO_ERROR)
    {
        if (pSatData->nrSignals <= SBG_ARRAY_SIZE(pSatData->signalData))
        {
            for (size_t i = 0; i < pSatData->nrSignals; i++)
            {
                errorCode = sbgEComLogSatSignalReadFromStream(&pSatData->signalData[i], pStreamBuffer);

                if (errorCode != SBG_NO_ERROR)
                {
                    break;
                }
            }
        }
        else
        {
            errorCode = SBG_INVALID_FRAME;
            SBG_LOG_ERROR(errorCode, "invalid number of signals: %zu", pSatData->nrSignals);
        }
    }

    return errorCode;
}

/*!
 * Write a satellite entry instance to a stream buffer.
 *
 * \param[in]   pSatData                    Satellite entry instance.
 * \param[out]  pStreamBuffer               Stream buffer.
 * \return                                  SBG_NO_ERROR if successful.
 */
static SbgErrorCode sbgEComLogSatEntryWriteToStream(const SbgEComLogSatEntry *pSatData, SbgStreamBuffer *pStreamBuffer)
{
    SbgErrorCode                         errorCode;

    assert(pSatData);   
    assert(pStreamBuffer);
    assert(pSatData->nrSignals <= UINT8_MAX);

    sbgStreamBufferWriteUint8LE(pStreamBuffer,      pSatData->id);
    sbgStreamBufferWriteInt8LE(pStreamBuffer,       pSatData->elevation);
    sbgStreamBufferWriteUint16LE(pStreamBuffer,     pSatData->azimuth);
    sbgStreamBufferWriteUint16LE(pStreamBuffer,     pSatData->flags);
    sbgStreamBufferWriteUint8LE(pStreamBuffer,      (uint8_t)pSatData->nrSignals);

    errorCode = sbgStreamBufferGetLastError(pStreamBuffer);

    if (errorCode == SBG_NO_ERROR)
    {
        for (size_t i = 0; i < pSatData->nrSignals; i++)
        {
            errorCode = sbgEComLogSatSignalWriteToStream(&pSatData->signalData[i], pStreamBuffer);

            if (errorCode != SBG_NO_ERROR)
            {
                break;
            }
        }
    }

    return errorCode;
}

//----------------------------------------------------------------------//
//- Public setters/getters (SbgEComLogSatSignal)                       -//
//----------------------------------------------------------------------//

const char *sbgEComLogSatSignalGetSignalIdAsStr(const SbgEComLogSatSignal *pSignalData)
{
    assert(pSignalData);

    return sbgEComSignalToStr(pSignalData->id);
}

bool sbgEComLogSatSignalSnrIsValid(const SbgEComLogSatSignal *pSignalData)
{
    assert(pSignalData);

    return (pSignalData->flags&SBG_ECOM_LOG_SAT_SIGNAL_SNR_VALID?true:false);
}

SbgEComSatHealthStatus sbgEComLogSatSignalGetHealthStatus(const SbgEComLogSatSignal *pSignalData)
{
    assert(pSignalData);

    return (SbgEComSatHealthStatus)sbgEComLogSatGetField(pSignalData->flags, SBG_ECOM_LOG_SAT_HEALTH_STATUS_OFFSET, SBG_ECOM_LOG_SAT_HEALTH_STATUS_MASK);
}

const char *sbgEComLogSatSignalGetHealthStatusAsStr(const SbgEComLogSatSignal *pSignalData)
{
    assert(pSignalData);

    return sbgEComLogSatHealthStatusToStr(sbgEComLogSatSignalGetHealthStatus(pSignalData));
}

SbgEComSatTrackingStatus sbgEComLogSatSignalGetTrackingStatus(const SbgEComLogSatSignal *pSignalData)
{
    assert(pSignalData);

    return (SbgEComSatTrackingStatus)sbgEComLogSatGetField(pSignalData->flags, SBG_ECOM_LOG_SAT_TRACKING_STATUS_OFFSET, SBG_ECOM_LOG_SAT_TRACKING_STATUS_MASK);
}

const char *sbgEComLogSatSignalGetTrackingStatusAsStr(const SbgEComLogSatSignal *pSignalData)
{
    assert(pSignalData);

    return sbgEComLogSatTrackingStatusToStr(sbgEComLogSatSignalGetTrackingStatus(pSignalData));
}

//----------------------------------------------------------------------//
//- Public methods (SbgEComLogSatEntry)                                -//
//----------------------------------------------------------------------//

SbgEComLogSatSignal *sbgEComLogSatEntryAdd(SbgEComLogSatEntry *pSatData, SbgEComSignalId id, SbgEComSatHealthStatus healthStatus, SbgEComSatTrackingStatus trackingStatus, bool snrValid, uint8_t snr)
{
    SbgEComLogSatSignal                 *pSignalData = NULL;

    assert(pSatData);

    if (pSatData->nrSignals < SBG_ARRAY_SIZE(pSatData->signalData))
    {
        uint8_t                              flags = 0;

        sbgEComLogSatSetField(flags, healthStatus,      SBG_ECOM_LOG_SAT_HEALTH_STATUS_OFFSET,      SBG_ECOM_LOG_SAT_HEALTH_STATUS_MASK);
        sbgEComLogSatSetField(flags, trackingStatus,    SBG_ECOM_LOG_SAT_TRACKING_STATUS_OFFSET,    SBG_ECOM_LOG_SAT_TRACKING_STATUS_MASK);

        if (snrValid)
        {
            flags |= SBG_ECOM_LOG_SAT_SIGNAL_SNR_VALID;
        }

        pSignalData = &pSatData->signalData[pSatData->nrSignals];
        pSatData->nrSignals++;

        pSignalData->id             = id;
        pSignalData->flags          = flags;
        pSignalData->snr            = snr;

        sbgEComLogSatEntryUpdateStatus(pSatData, healthStatus, trackingStatus);
    }
    else
    {
        SBG_LOG_ERROR(SBG_BUFFER_OVERFLOW, "not free slot to add a new signal");
    }

    return pSignalData;
}

//----------------------------------------------------------------------//
//- Public setters/getters (SbgEComLogSatEntry)                        -//
//----------------------------------------------------------------------//

SbgEComLogSatSignal *sbgEComLogSatEntryGet(SbgEComLogSatEntry *pSatData, SbgEComSignalId id)
{
    SbgEComLogSatSignal                 *pSignalData = NULL;

    assert(pSatData);

    for (size_t i = 0; i < pSatData->nrSignals; i++)
    {
        if (pSatData->signalData[i].id == id)
        {
            pSignalData = &pSatData->signalData[i];
            break;
        }
    }

    return pSignalData;
}

SbgEComConstellationId sbgEComLogSatEntryGetConstellationId(const SbgEComLogSatEntry *pSatData)
{
    assert(pSatData);

    return (SbgEComConstellationId)sbgEComLogSatGetField(pSatData->flags, SBG_ECOM_LOG_SAT_CONSTELLATION_ID_OFFSET, SBG_ECOM_LOG_SAT_CONSTELLATION_ID_MASK);
}

const char *sbgEComLogSatEntryGetConstellationIdAsStr(const SbgEComLogSatEntry *pSatData)
{
    return sbgEComConstellationToStr(sbgEComLogSatEntryGetConstellationId(pSatData));
}

SbgEComSatElevationStatus sbgEComLogSatEntryGetElevationStatus(const SbgEComLogSatEntry *pSatData)
{
    assert(pSatData);

    return (SbgEComSatElevationStatus)sbgEComLogSatGetField(pSatData->flags, SBG_ECOM_LOG_SAT_ELEVATION_STATUS_OFFSET, SBG_ECOM_LOG_SAT_ELEVATION_STATUS_MASK);
}

const char *sbgEComLogSatEntryGetElevationStatusAsStr(const SbgEComLogSatEntry *pSatData)
{
    assert(pSatData);
    
    return sbgEComLogSatElevationStatusToStr(sbgEComLogSatEntryGetElevationStatus(pSatData));
}

SbgEComSatHealthStatus sbgEComLogSatEntryGetHealthStatus(const SbgEComLogSatEntry *pSatData)
{
    assert(pSatData);

    return (SbgEComSatHealthStatus) sbgEComLogSatGetField(pSatData->flags, SBG_ECOM_LOG_SAT_HEALTH_STATUS_OFFSET, SBG_ECOM_LOG_SAT_HEALTH_STATUS_MASK);
}

const char *sbgEComLogSatEntryGetHealthStatusAsStr(const SbgEComLogSatEntry *pSatData)
{
    assert(pSatData);

    return sbgEComLogSatHealthStatusToStr(sbgEComLogSatEntryGetHealthStatus(pSatData));
}

SbgEComSatTrackingStatus sbgEComLogSatEntryGetTrackingStatus(const SbgEComLogSatEntry *pSatData)
{
    assert(pSatData);

    return (SbgEComSatTrackingStatus)sbgEComLogSatGetField(pSatData->flags, SBG_ECOM_LOG_SAT_TRACKING_STATUS_OFFSET, SBG_ECOM_LOG_SAT_TRACKING_STATUS_MASK);
}

const char *sbgEComLogSatEntryGetTrackingStatusAsStr(const SbgEComLogSatEntry *pSatData)
{
    assert(pSatData);

    return sbgEComLogSatTrackingStatusToStr(sbgEComLogSatEntryGetTrackingStatus(pSatData));
}

//----------------------------------------------------------------------//
//- Public methods (SbgEComLogSatList)                                 -//
//----------------------------------------------------------------------//

void sbgEComLogSatListConstruct(SbgEComLogSatList *pSatList, uint32_t timeStamp)
{
    assert(pSatList);
    
    pSatList->timeStamp                 = timeStamp;
    pSatList->reserved                  = 0;
    pSatList->nrSatellites              = 0;
}

SbgEComLogSatEntry *sbgEComLogSatListAdd(SbgEComLogSatList *pSatList, uint8_t id, int8_t elevation, uint16_t azimuth, SbgEComConstellationId constellationId, SbgEComSatElevationStatus elevationStatus, SbgEComSatHealthStatus healthStatus, SbgEComSatTrackingStatus trackingStatus)
{
    SbgEComLogSatEntry                      *pSatData = NULL;

    assert(pSatList);

    if (pSatList->nrSatellites < SBG_ARRAY_SIZE(pSatList->satData))
    {
        pSatData = &pSatList->satData[pSatList->nrSatellites];
        pSatList->nrSatellites++;

        sbgEComLogSatEntryConstruct(pSatData, id, elevation, azimuth, constellationId, elevationStatus, healthStatus, trackingStatus);
    }
    else
    {
        SBG_LOG_ERROR(SBG_BUFFER_OVERFLOW, "no free slot to add a new satellite.");
    }

    return pSatData;
}

SbgErrorCode sbgEComLogSatListReadFromStream(SbgEComLogSatList *pSatList, SbgStreamBuffer *pStreamBuffer)
{
    SbgErrorCode                         errorCode;

    assert(pSatList);
    assert(pStreamBuffer);

    pSatList->timeStamp                 = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pSatList->reserved                  = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pSatList->nrSatellites              = sbgStreamBufferReadUint8LE(pStreamBuffer);
    
    errorCode = sbgStreamBufferGetLastError(pStreamBuffer);

    if (errorCode == SBG_NO_ERROR)
    {
        if (pSatList->nrSatellites <= SBG_ARRAY_SIZE(pSatList->satData))
        {
            for (size_t i = 0; i < pSatList->nrSatellites; i++)
            {
                errorCode = sbgEComLogSatEntryReadFromStream(&pSatList->satData[i], pStreamBuffer);

                if (errorCode != SBG_NO_ERROR)
                {
                    break;
                }
            }
        }
        else
        {
            errorCode = SBG_INVALID_FRAME;
            SBG_LOG_ERROR(errorCode, "invalid number of satellites: %zu", pSatList->nrSatellites);
        }
    }

    return errorCode;
}

SbgErrorCode sbgEComLogSatListWriteToStream(const SbgEComLogSatList *pSatList, SbgStreamBuffer *pStreamBuffer)
{
    SbgErrorCode                            errorCode;

    assert(pSatList);
    assert(pSatList->nrSatellites <= UINT8_MAX);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint32LE(pStreamBuffer,     pSatList->timeStamp);
    sbgStreamBufferWriteUint32LE(pStreamBuffer,     pSatList->reserved);
    sbgStreamBufferWriteUint8LE(pStreamBuffer,      (uint8_t)pSatList->nrSatellites);

    errorCode = sbgStreamBufferGetLastError(pStreamBuffer);

    if (errorCode == SBG_NO_ERROR)
    {
        for (size_t i = 0; i < pSatList->nrSatellites; i++)
        {
            errorCode = sbgEComLogSatEntryWriteToStream(&pSatList->satData[i], pStreamBuffer);

            if (errorCode != SBG_NO_ERROR)
            {
                break;
            }
        }
    }

    return errorCode;
}

//----------------------------------------------------------------------//
//- Public setters/getters (SbgEComLogSatList)                         -//
//----------------------------------------------------------------------//

SbgEComLogSatEntry *sbgEComLogSatListGet(SbgEComLogSatList *pSatList, uint8_t id)
{
    SbgEComLogSatEntry                      *pSatData = NULL;

    assert(pSatList);

    for (size_t i = 0; i < pSatList->nrSatellites; i++)
    {
        if (pSatList->satData[i].id == id)
        {
            pSatData = &pSatList->satData[i];
            break;
        }
    }

    return pSatData;
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComBinaryLogParseSatGroupData(SbgStreamBuffer *pStreamBuffer, SbgEComLogSatList *pSatList)
{
    return sbgEComLogSatListReadFromStream(pSatList, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteSatGroupData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogSatList *pSatList)
{
    return sbgEComLogSatListWriteToStream(pSatList, pStreamBuffer);
}
