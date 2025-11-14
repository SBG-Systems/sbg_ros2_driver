// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Local headers
#include "sbgEComLogVibMon.h"

//----------------------------------------------------------------------//
//- Private definitions                                                -//
//----------------------------------------------------------------------//

#define SBG_ECOM_LOG_VIB_MON_AXIS_SHIFT                     (0u)                        /*!< Shift used to extract the axis part. */
#define SBG_ECOM_LOG_VIB_MON_AXIS_MASK                      ((uint16_t)0x03u)           /*!< Mask used to keep only the axis part. */

#define SBG_ECOM_LOG_VIB_MON_WINDOW_SHIFT                   (3u)                        /*!< Shift used to extract the window part. */
#define SBG_ECOM_LOG_VIB_MON_WINDOW_MASK                    ((uint16_t)0x03u)           /*!< Mask used to keep only the window part. */

//----------------------------------------------------------------------//
//- Public functions (SbgEComLogVibMonFft)                             -//
//----------------------------------------------------------------------//

void sbgEComLogVibMonFftZeroInit(SbgEComLogVibMonFft *pLogData)
{
    assert(pLogData);

    memset(pLogData, 0, sizeof(*pLogData));
}

SbgErrorCode sbgEComLogVibMonFftReadFromStream(SbgEComLogVibMonFft *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    SbgErrorCode                         errorCode;

    assert(pLogData);
    assert(pStreamBuffer);

    pLogData->pageIndex = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pLogData->nrPages   = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pLogData->size      = sbgStreamBufferReadUint16LE(pStreamBuffer);

    errorCode = sbgStreamBufferGetLastError(pStreamBuffer);

    if (errorCode == SBG_NO_ERROR)
    {
        if (pLogData->pageIndex >= pLogData->nrPages)
        {
            errorCode = SBG_INVALID_FRAME;
            SBG_LOG_ERROR(errorCode, "invalid fft page index %" PRIu16 "/%" PRIu16, pLogData->pageIndex, pLogData->nrPages);
        }
    }

    if (errorCode == SBG_NO_ERROR)
    {
        if (pLogData->size <= sizeof(pLogData->buffer))
        {
            errorCode = sbgStreamBufferReadBuffer(pStreamBuffer, pLogData->buffer, pLogData->size);

            if (errorCode != SBG_NO_ERROR)
            {
                SBG_LOG_ERROR(errorCode, "invalid fft size %zu, actual:%zu", pLogData->size, sbgStreamBufferGetSpace(pStreamBuffer));
            }
        }
        else
        {
            errorCode = SBG_INVALID_FRAME;
            SBG_LOG_ERROR(errorCode, "invalid fft size %zu", pLogData->size);
        }
    }

    return errorCode;
}

SbgErrorCode sbgEComLogVibMonFftWriteToStream(const SbgEComLogVibMonFft *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pLogData->size <= UINT16_MAX);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->pageIndex);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->nrPages);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, (uint16_t)pLogData->size);

    sbgStreamBufferWriteBuffer(pStreamBuffer, pLogData->buffer, pLogData->size);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public functions (SbgEComLogVibMonReport)                          -//
//----------------------------------------------------------------------//

void sbgEComLogVibMonReportZeroInit(SbgEComLogVibMonReport *pLogData)
{
    assert(pLogData);

    memset(pLogData, 0, sizeof(*pLogData));

    pLogData->status    = sbgEComLogVibMonMakeStatus(SBG_ECOM_VIB_MON_AXIS_X, SBG_ECOM_VIB_MON_WINDOW_RECTANGULAR);
    pLogData->mean      = NAN;
    pLogData->rms       = NAN;

    for (size_t i = 0; i < SBG_ARRAY_SIZE(pLogData->bands); i++)
    {
        pLogData->bands[i].freq1            = NAN;
        pLogData->bands[i].freq2            = NAN;
        pLogData->bands[i].rms              = NAN;
        pLogData->bands[i].peakHarmonic     = NAN;
        pLogData->bands[i].peakMagnitude    = NAN;
    }
}

SbgErrorCode sbgEComLogVibMonReportReadFromStream(SbgEComLogVibMonReport *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    pLogData->timeStamp = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status    = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pLogData->mean      = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->rms       = sbgStreamBufferReadFloatLE(pStreamBuffer);

    for (size_t i = 0; i < SBG_ARRAY_SIZE(pLogData->bands); i++)
    {
        pLogData->bands[i].freq1            = sbgStreamBufferReadFloatLE(pStreamBuffer);
        pLogData->bands[i].freq2            = sbgStreamBufferReadFloatLE(pStreamBuffer);
        pLogData->bands[i].rms              = sbgStreamBufferReadFloatLE(pStreamBuffer);
        pLogData->bands[i].peakHarmonic     = sbgStreamBufferReadFloatLE(pStreamBuffer);
        pLogData->bands[i].peakMagnitude    = sbgStreamBufferReadFloatLE(pStreamBuffer);
    }

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogVibMonReportWriteToStream(const SbgEComLogVibMonReport *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->mean);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->rms);

    for (size_t i = 0; i < SBG_ARRAY_SIZE(pLogData->bands); i++)
    {
        sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->bands[i].freq1);
        sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->bands[i].freq2);
        sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->bands[i].rms);
        sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->bands[i].peakHarmonic);
        sbgStreamBufferWriteFloatLE(pStreamBuffer, pLogData->bands[i].peakMagnitude);
    }

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public setters/getters (SbgEComLogVibMonReport)                    -//
//----------------------------------------------------------------------//

SbgEComVibMonAxis sbgEComLogVibMonReportGetAxis(const SbgEComLogVibMonReport *pLogData)
{
    assert(pLogData);

    return (SbgEComVibMonAxis)((pLogData->status >> SBG_ECOM_LOG_VIB_MON_AXIS_SHIFT) & SBG_ECOM_LOG_VIB_MON_AXIS_MASK);
}

SbgEComVibMonWindow sbgEComLogVibMonReportGetWindow(const SbgEComLogVibMonReport *pLogData)
{
    assert(pLogData);

    return (SbgEComVibMonWindow)((pLogData->status >> SBG_ECOM_LOG_VIB_MON_WINDOW_SHIFT) & SBG_ECOM_LOG_VIB_MON_WINDOW_MASK);
}

const SbgEComLogVibMonBand *sbgEComLogVibMonReportGetMaxPeakBand(const SbgEComLogVibMonReport *pLogData)
{
    float   maxPeakMagnitude;
    size_t  maxBandIndex;

    assert(pLogData);
    
    maxPeakMagnitude    = 0.0f;
    maxBandIndex        = 0;

    for (size_t i = 0; i < SBG_ARRAY_SIZE(pLogData->bands); i++)
    {
        if (pLogData->bands[i].peakMagnitude > maxPeakMagnitude)
        {
            maxPeakMagnitude    = pLogData->bands[i].peakMagnitude;
            maxBandIndex        = i;
        }
    }

    return &pLogData->bands[maxBandIndex];
}

//----------------------------------------------------------------------//
//- Private setters/getters                                            -//
//----------------------------------------------------------------------//

SbgEComVibMonAxis sbgEComLogVibMonStatusGetAxis(uint16_t status)
{
    return (SbgEComVibMonAxis)((status >> SBG_ECOM_LOG_VIB_MON_AXIS_SHIFT) & SBG_ECOM_LOG_VIB_MON_AXIS_MASK);
}

SbgEComVibMonWindow sbgEComLogVibMonStatusGetWindow(uint16_t status)
{
    return (SbgEComVibMonWindow)((status >> SBG_ECOM_LOG_VIB_MON_WINDOW_SHIFT) & SBG_ECOM_LOG_VIB_MON_WINDOW_MASK);
}

uint16_t sbgEComLogVibMonMakeStatus(SbgEComVibMonAxis axis, SbgEComVibMonWindow window)
{
    uint16_t                             status = 0;

    status &= ~(SBG_ECOM_LOG_VIB_MON_AXIS_MASK << SBG_ECOM_LOG_VIB_MON_AXIS_SHIFT);
    status |= ((uint16_t)axis & SBG_ECOM_LOG_VIB_MON_AXIS_MASK) << SBG_ECOM_LOG_VIB_MON_AXIS_SHIFT;

    status &= ~(SBG_ECOM_LOG_VIB_MON_WINDOW_MASK << SBG_ECOM_LOG_VIB_MON_WINDOW_SHIFT);
    status |= ((uint16_t)window & SBG_ECOM_LOG_VIB_MON_WINDOW_MASK) << SBG_ECOM_LOG_VIB_MON_WINDOW_SHIFT;

    return status;
}
