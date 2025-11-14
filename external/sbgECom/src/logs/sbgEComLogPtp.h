/*!
 * \file            sbgEComLogPtp.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            2024-06-17
 *
 * \brief           Parse logs used to report device PTP status.
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

#ifndef SBG_ECOM_LOG_PTP_H
#define SBG_ECOM_LOG_PTP_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Enumeration definitions                                            -//
//----------------------------------------------------------------------//

/*!
 * PTP states.
 */
typedef enum _SbgEComLogPtpState
{
    SBG_ECOM_LOG_PTP_STATE_DISABLED         = 0,                        /*!< PTP is disabled. */
    SBG_ECOM_LOG_PTP_STATE_FAULTY           = 1,                        /*!< The device is in the faulty state. */
    SBG_ECOM_LOG_PTP_STATE_MASTER           = 2,                        /*!< The device is the domain master. */
    SBG_ECOM_LOG_PTP_STATE_PASSIVE          = 3,                        /*!< The device is passive. */
} SbgEComLogPtpState;

/*!
 * Transport.
 */
typedef enum _SbgEComLogPtpTransport
{
    SBG_ECOM_LOG_PTP_TRANSPORT_UDP          = 0,                        /*!< UDP. */
    SBG_ECOM_LOG_PTP_TRANSPORT_ETHERNET     = 1,                        /*!< Ethernet (IEEE 802.3). */
} SbgEComLogPtpTransport;

/*!
 * Time scales.
 */
typedef enum _SbgEComLogPtpTimeScale
{
    SBG_ECOM_LOG_PTP_TIME_SCALE_TAI         = 0,                        /*!< TAI. */
    SBG_ECOM_LOG_PTP_TIME_SCALE_UTC         = 1,                        /*!< UTC. */
    SBG_ECOM_LOG_PTP_TIME_SCALE_GPS         = 2,                        /*!< GPS. */
} SbgEComLogPtpTimeScale;

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * Structure that stores data for the SBG_ECOM_LOG_PTP_STATUS message.
 *
 * The local clock members are valid only if the local clock identity is valid.
 * Similarly, the master clock members are valid only if the master clock identity is valid.
 *
 * The mean path delay, clock offset, and clock frequency offset, as well as their standard
 * deviations, are set to NAN if invalid or unknown.
 */
typedef struct _SbgEComLogPtp
{
    uint32_t                             timeStamp;                     /*!< Timestamp, in us. */
    uint16_t                             status;                        /*!< State, transport and internal time scale status. */

    double                               timeScaleOffset;               /*!< Internal time scale offset, in seconds. */

    uint64_t                             localClockIdentity;            /*!< Local clock identity, UINT64_MAX if invalid. */
    uint8_t                              localClockPriority1;           /*!< Local clock priority1 attribute. */
    uint8_t                              localClockPriority2;           /*!< Local clock priority2 attribute. */
    uint8_t                              localClockClass;               /*!< Local clock class attribute. */
    uint8_t                              localClockAccuracy;            /*!< Local clock accuracy. */
    uint16_t                             localClockLog2Variance;        /*!< Local clock PTP variance. */
    uint8_t                              localClockTimeSource;          /*!< Local clock time source. */

    uint64_t                             masterClockIdentity;           /*!< Master clock identity, UINT64_MAX if invalid. */
    uint8_t                              masterClockPriority1;          /*!< Master clock priority1 attribute. */
    uint8_t                              masterClockPriority2;          /*!< Master clock priority2 attribute. */
    uint8_t                              masterClockClass;              /*!< Master clock class attribute. */
    uint8_t                              masterClockAccuracy;           /*!< Master clock accuracy. */
    uint16_t                             masterClockLog2Variance;       /*!< Master clock PTP variance. */
    uint8_t                              masterClockTimeSource;         /*!< Master clock time source. */
    uint32_t                             masterIpAddress;               /*!< Master clock IP address, UINT32_MAX if invalid. */

    float                                meanPathDelay;                 /*!< Mean path delay to/from the master clock, in seconds. */
    float                                meanPathDelayStdDev;           /*!< Mean path delay standard deviation, in seconds. */
    double                               clockOffset;                   /*!< Offset between the local and master clocks, in seconds. */
    float                                clockOffsetStdDev;             /*!< Master clock offset standard deviation, in seconds. */
    float                                clockFreqOffset;               /*!< Offset between the frequency of the local and master clocks, in Hz. */
    float                                clockFreqOffsetStdDev;         /*!< Frequency offset standard deviation, in Hz. */

    uint8_t                              masterMacAddress[6];           /*!< Master clock 48 bits mac address, all field at UINT8_MAX if invalid. Added in sbgECom v5.2.*/
} SbgEComLogPtp;

//----------------------------------------------------------------------//
//- Public functions                                                   -//
//----------------------------------------------------------------------//

/*!
 * Zero initialize the message struct.
 *
 * \param[out]  pLogData                    Structure instance to zero init.
 */
void sbgEComLogPtpZeroInit(SbgEComLogPtp *pLogData);

/*!
 * Parse data for the SBG_ECOM_LOG_PTP_STATUS message and fill the corresponding structure.
 *
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogPtpReadFromStream(SbgEComLogPtp *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_PTP_STATUS message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogPtpWriteToStream(const SbgEComLogPtp *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

/*!
 * Set the PTP state.
 *
 * \param[in]   pLogData            Log instance.
 * \param[in]   state               The solution state to set.
 */
void sbgEComLogPtpSetState(SbgEComLogPtp *pLogData, SbgEComLogPtpState state);

/*!
 * Returns the PTP solution state.
 *
 * \param[in]   pLogData            Log instance.
 * \return                          The solution state.
 */
SbgEComLogPtpState sbgEComLogPtpGetState(const SbgEComLogPtp *pLogData);

/*!
 * Set the PTP transport.
 *
 * \param[in]   pLogData            Log instance.
 * \param[in]   transport           The solution transport to set.
 */
void sbgEComLogPtpSetTransport(SbgEComLogPtp *pLogData, SbgEComLogPtpTransport transport);

/*!
 * Returns the PTP solution transport.
 *
 * \param[in]   pLogData            Log instance.
 * \return                          The solution transport.
 */
SbgEComLogPtpTransport sbgEComLogPtpGetTransport(const SbgEComLogPtp *pLogData);

/*!
 * Set the PTP time scale.
 *
 * \param[in]   pLogData            Log instance.
 * \param[in]   timeScale           The solution time scale to set.
 */
void sbgEComLogPtpSetTimeScale(SbgEComLogPtp *pLogData, SbgEComLogPtpTimeScale timeScale);

/*!
 * Returns the PTP solution time scale.
 *
 * \param[in]   pLogData            Log instance.
 * \return                          The solution time scale.
 */
SbgEComLogPtpTimeScale sbgEComLogPtpGetTimeScale(const SbgEComLogPtp *pLogData);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_PTP_H
