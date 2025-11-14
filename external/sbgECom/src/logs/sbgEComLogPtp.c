// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Local headers
#include "sbgEComLogPtp.h"

//----------------------------------------------------------------------//
//- Private definitions for state field                                -//
//----------------------------------------------------------------------//

#define SBG_ECOM_LOG_PTP_STATE_SHIFT                        (0u)                    /*!< Shift used to extract the PTP state part. */
#define SBG_ECOM_LOG_PTP_STATE_MASK                         ((uint16_t)0x07u)       /*!< Mask used to keep only the PTP state part. */

#define SBG_ECOM_LOG_PTP_TRANSPORT_SHIFT                    (3u)                    /*!< Shift used to extract the PTP transport part. */
#define SBG_ECOM_LOG_PTP_TRANSPORT_MASK                     ((uint16_t)0x07u)       /*!< Mask used to keep only the PTP transport part. */

#define SBG_ECOM_LOG_PTP_TIMESCALE_SHIFT                    (8u)                    /*!< Shift used to extract the PTP time scale part. */
#define SBG_ECOM_LOG_PTP_TIMESCALE_MASK                     ((uint16_t)0x07u)       /*!< Mask used to keep only the PTP time scale part. */

//----------------------------------------------------------------------//
//- Public functions                                                   -//
//----------------------------------------------------------------------//

void sbgEComLogPtpZeroInit(SbgEComLogPtp *pLogData)
{
    assert(pLogData);

    memset(pLogData, 0, sizeof(*pLogData));

    sbgEComLogPtpSetState(pLogData,     SBG_ECOM_LOG_PTP_STATE_FAULTY);
    sbgEComLogPtpSetTransport(pLogData, SBG_ECOM_LOG_PTP_TRANSPORT_UDP);
    sbgEComLogPtpSetTimeScale(pLogData, SBG_ECOM_LOG_PTP_TIME_SCALE_TAI);

    pLogData->timeScaleOffset               = 0.0;
    pLogData->localClockIdentity            = UINT64_MAX;
    pLogData->masterClockIdentity           = UINT64_MAX;
    pLogData->masterIpAddress               = UINT32_MAX;
    pLogData->meanPathDelay                 = NAN;
    pLogData->meanPathDelayStdDev           = NAN;
    pLogData->clockOffset                   = NAN;
    pLogData->clockOffsetStdDev             = NAN;
    pLogData->clockFreqOffset               = NAN;
    pLogData->clockFreqOffsetStdDev         = NAN;

    memset(pLogData->masterMacAddress, UINT8_MAX, sizeof(pLogData->masterMacAddress));
}

SbgErrorCode sbgEComLogPtpReadFromStream(SbgEComLogPtp *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    SbgErrorCode                         errorCode;
    uint16_t                             status;

    assert(pLogData);
    assert(pStreamBuffer);

    pLogData->timeStamp                 = sbgStreamBufferReadUint32LE(pStreamBuffer);
    status                              = sbgStreamBufferReadUint16LE(pStreamBuffer);

    pLogData->timeScaleOffset           = sbgStreamBufferReadDoubleLE(pStreamBuffer);

    pLogData->localClockIdentity        = sbgStreamBufferReadUint64LE(pStreamBuffer);
    pLogData->localClockPriority1       = sbgStreamBufferReadUint8(pStreamBuffer);
    pLogData->localClockPriority2       = sbgStreamBufferReadUint8(pStreamBuffer);
    pLogData->localClockClass           = sbgStreamBufferReadUint8(pStreamBuffer);
    pLogData->localClockAccuracy        = sbgStreamBufferReadUint8(pStreamBuffer);
    pLogData->localClockLog2Variance    = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pLogData->localClockTimeSource      = sbgStreamBufferReadUint8(pStreamBuffer);

    pLogData->masterClockIdentity       = sbgStreamBufferReadUint64LE(pStreamBuffer);
    pLogData->masterClockPriority1      = sbgStreamBufferReadUint8(pStreamBuffer);
    pLogData->masterClockPriority2      = sbgStreamBufferReadUint8(pStreamBuffer);
    pLogData->masterClockClass          = sbgStreamBufferReadUint8(pStreamBuffer);
    pLogData->masterClockAccuracy       = sbgStreamBufferReadUint8(pStreamBuffer);
    pLogData->masterClockLog2Variance   = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pLogData->masterClockTimeSource     = sbgStreamBufferReadUint8(pStreamBuffer);
    pLogData->masterIpAddress           = sbgStreamBufferReadUint32LE(pStreamBuffer);

    pLogData->meanPathDelay             = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->meanPathDelayStdDev       = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->clockOffset               = sbgStreamBufferReadDoubleLE(pStreamBuffer);
    pLogData->clockOffsetStdDev         = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->clockFreqOffset           = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->clockFreqOffsetStdDev     = sbgStreamBufferReadFloatLE(pStreamBuffer);

    //
    // Added in sbgECom 5.2
    //
    if (sbgStreamBufferGetSpace(pStreamBuffer) >= sizeof(pLogData->masterMacAddress))
    {
        for (size_t i = 0; i < SBG_ARRAY_SIZE(pLogData->masterMacAddress); i++)
        {
            pLogData->masterMacAddress[i] = sbgStreamBufferReadUint8(pStreamBuffer);
        }
    }
    else
    {
        memset(pLogData->masterMacAddress, UINT8_MAX, sizeof(pLogData->masterMacAddress));
    }

    errorCode = sbgStreamBufferGetLastError(pStreamBuffer);

    if (errorCode == SBG_NO_ERROR)
    {
        pLogData->status = status;
    }

    return errorCode;
}

SbgErrorCode sbgEComLogPtpWriteToStream(const SbgEComLogPtp *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);

    sbgStreamBufferWriteDoubleLE(pStreamBuffer, pLogData->timeScaleOffset);

    sbgStreamBufferWriteUint64LE(pStreamBuffer, pLogData->localClockIdentity);
    sbgStreamBufferWriteUint8(pStreamBuffer,    pLogData->localClockPriority1);
    sbgStreamBufferWriteUint8(pStreamBuffer,    pLogData->localClockPriority2);
    sbgStreamBufferWriteUint8(pStreamBuffer,    pLogData->localClockClass);
    sbgStreamBufferWriteUint8(pStreamBuffer,    pLogData->localClockAccuracy);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->localClockLog2Variance);
    sbgStreamBufferWriteUint8(pStreamBuffer,    pLogData->localClockTimeSource);

    sbgStreamBufferWriteUint64LE(pStreamBuffer, pLogData->masterClockIdentity);
    sbgStreamBufferWriteUint8(pStreamBuffer,    pLogData->masterClockPriority1);
    sbgStreamBufferWriteUint8(pStreamBuffer,    pLogData->masterClockPriority2);
    sbgStreamBufferWriteUint8(pStreamBuffer,    pLogData->masterClockClass);
    sbgStreamBufferWriteUint8(pStreamBuffer,    pLogData->masterClockAccuracy);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->masterClockLog2Variance);
    sbgStreamBufferWriteUint8(pStreamBuffer,    pLogData->masterClockTimeSource);
    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->masterIpAddress);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->meanPathDelay);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->meanPathDelayStdDev);
    sbgStreamBufferWriteDoubleLE(pStreamBuffer, pLogData->clockOffset);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->clockOffsetStdDev);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->clockFreqOffset);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->clockFreqOffsetStdDev);

    //
    // Added in sbgECom 5.2
    //
    for (size_t i = 0; i < SBG_ARRAY_SIZE(pLogData->masterMacAddress); i++)
    {
        sbgStreamBufferWriteUint8(pStreamBuffer, pLogData->masterMacAddress[i]);
    }

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

void sbgEComLogPtpSetState(SbgEComLogPtp *pLogData, SbgEComLogPtpState state)
{
    assert(pLogData);
    assert(state <= SBG_ECOM_LOG_PTP_STATE_MASK);

    pLogData->status &= ~(SBG_ECOM_LOG_PTP_STATE_MASK << SBG_ECOM_LOG_PTP_STATE_SHIFT);
    pLogData->status |= ((uint16_t)state & SBG_ECOM_LOG_PTP_STATE_MASK) << SBG_ECOM_LOG_PTP_STATE_SHIFT;
}

SbgEComLogPtpState sbgEComLogPtpGetState(const SbgEComLogPtp *pLogData)
{
    assert(pLogData);

    return (SbgEComLogPtpState)((pLogData->status >> SBG_ECOM_LOG_PTP_STATE_SHIFT) & SBG_ECOM_LOG_PTP_STATE_MASK);
}

void sbgEComLogPtpSetTransport(SbgEComLogPtp *pLogData, SbgEComLogPtpTransport transport)
{
    assert(pLogData);
    assert(transport <= SBG_ECOM_LOG_PTP_TRANSPORT_MASK);

    pLogData->status &= ~(SBG_ECOM_LOG_PTP_TRANSPORT_MASK << SBG_ECOM_LOG_PTP_TRANSPORT_SHIFT);
    pLogData->status |= ((uint16_t)transport & SBG_ECOM_LOG_PTP_TRANSPORT_MASK) << SBG_ECOM_LOG_PTP_TRANSPORT_SHIFT;
}

SbgEComLogPtpTransport sbgEComLogPtpGetTransport(const SbgEComLogPtp *pLogData)
{
    assert(pLogData);

    return (SbgEComLogPtpTransport)((pLogData->status >> SBG_ECOM_LOG_PTP_TRANSPORT_SHIFT) & SBG_ECOM_LOG_PTP_TRANSPORT_MASK);
}

void sbgEComLogPtpSetTimeScale(SbgEComLogPtp *pLogData, SbgEComLogPtpTimeScale timescale)
{
    assert(pLogData);
    assert(timescale <= SBG_ECOM_LOG_PTP_TIMESCALE_MASK);

    pLogData->status &= ~(SBG_ECOM_LOG_PTP_TIMESCALE_MASK << SBG_ECOM_LOG_PTP_TIMESCALE_SHIFT);
    pLogData->status |= ((uint16_t)timescale & SBG_ECOM_LOG_PTP_TIMESCALE_MASK) << SBG_ECOM_LOG_PTP_TIMESCALE_SHIFT;
}

SbgEComLogPtpTimeScale sbgEComLogPtpGetTimeScale(const SbgEComLogPtp *pLogData)
{
    assert(pLogData);

    return (SbgEComLogPtpTimeScale)((pLogData->status >> SBG_ECOM_LOG_PTP_TIMESCALE_SHIFT) & SBG_ECOM_LOG_PTP_TIMESCALE_MASK);
}
