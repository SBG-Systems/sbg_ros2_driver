/*!
 * \file            sbgEComLogUtc.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            20 February 2013
 *
 * \brief           Parse logs used to report device UTC time.
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

#ifndef SBG_ECOM_LOG_UTC_H
#define SBG_ECOM_LOG_UTC_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Public definitions                                                 -//
//----------------------------------------------------------------------//

/*!
 * Internal clock alignment to PPS algorithm state.
 */
typedef enum _SbgEComClockState
{
    SBG_ECOM_CLOCK_STATE_ERROR              = 0,                        /*!< An error has occurred on the clock estimation. */
    SBG_ECOM_CLOCK_STATE_FREE_RUNNING       = 1,                        /*!< The clock is only based on the internal crystal using latest known clock bias and scale factor. */
    SBG_ECOM_CLOCK_STATE_STEERING           = 2,                        /*!< A PPS has been detected and the clock is converging to it. */
    SBG_ECOM_CLOCK_STATE_VALID              = 3                         /*!< The internal clock is converged to the PPS signal or is still considered to be accurate. */
} SbgEComClockState;

/*!
 * Status for the GNSS UTC time information.
 */
typedef enum _SbgEComUtcStatus
{
    SBG_ECOM_UTC_STATUS_INVALID             = 0,                        /*!< UTC time is not known and invalid. The INS uses the firmware default initial date/time. */
    SBG_ECOM_UTC_STATUS_NO_LEAP_SEC         = 1,                        /*!< UTC time is initialized but the leap second is not known - the default firmware value is used. */
    SBG_ECOM_UTC_STATUS_INITIALIZED         = 2                         /*!< UTC time is initialized with a fully resolved leap second information. */
} SbgEComUtcStatus;

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * Structure that stores data for the SBG_ECOM_LOG_UTC_TIME message.
 */
typedef struct _SbgEComLogUtc
{
    uint32_t    timeStamp;                  /*!< Time in us since the sensor power up. */
    uint16_t    status;                     /*!< Clock state and UTC time status. */
    uint16_t    year;                       /*!< Year for example: 2013. */
    int8_t      month;                      /*!< Month in year [1 .. 12]. */
    int8_t      day;                        /*!< Day in month [1 .. 31]. */
    int8_t      hour;                       /*!< Hour in day [0 .. 23]. */
    int8_t      minute;                     /*!< Minute in hour [0 .. 59]. */
    int8_t      second;                     /*!< Second in minute [0 .. 60]. (60 is used only when a leap second is added) */
    int32_t     nanoSecond;                 /*!< Nanosecond of current second in ns. */
    uint32_t    gpsTimeOfWeek;              /*!< GPS time of week in ms. */
    float       clkBiasStd;                 /*!< Estimated internal clock bias standard deviation in seconds - set to NaN if not available. (added in 4.0) */
    float       clkSfErrorStd;              /*!< Estimated internal clock scale factor error standard deviation - set to NaN if not available. (added in 4.0) */
    float       clkResidualError;           /*!< Latest residual clock error from the GNSS PPS signal in seconds - set to NaN if not available. (added in 4.0) */
} SbgEComLogUtc;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Zero initialize the message struct.
 * 
 * \param[out]  pLogData                    Structure instance to zero init.
 */
void sbgEComLogUtcZeroInit(SbgEComLogUtc *pLogData);

/*!
 * Parse data for the SBG_ECOM_LOG_UTC_DATA message and fill the corresponding structure.
 * 
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogUtcReadFromStream(SbgEComLogUtc *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_UTC_DATA message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogUtcWriteToStream(const SbgEComLogUtc *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

/*!
 * Set the clock alignment state.
 * 
 * \param[in]   pLogData            Log instance.
 * \param[in]   state               The status to set.
 */
void sbgEComLogUtcSetClockState(SbgEComLogUtc *pLogData, SbgEComClockState state);

/*!
 * Returns the clock alignment state.
 * 
 * \param[in]   pLogData                    Log instance.
 * \return                                  The clock status.
 */
SbgEComClockState sbgEComLogUtcGetClockState(const SbgEComLogUtc *pLogData);

/*!
 * Returns the clock alignment state as a NULL terminated C string.
 *
 * \param[in]   pLogData                    Log instance.
 * \return                                  The clock state as a C string.
 */
const char *sbgEComLogUtcGetClockStateAsString(const SbgEComLogUtc *pLogData);

/*!
 * Set the UTC time information status.
 * 
 * \param[in]   pLogData            Log instance.
 * \param[in]   status              The status to set.
 */
void sbgEComLogUtcSetUtcStatus(SbgEComLogUtc *pLogData, SbgEComUtcStatus status);

/*!
 * Returns the GNSS UTC time information status.
 * 
 * \param[in]   pLogData                    Log instance.
 * \return                                  The UTC time information status.
 */
SbgEComUtcStatus sbgEComLogUtcGetUtcStatus(const SbgEComLogUtc *pLogData);

/*!
 * Returns the GNSS UTC time information status as a NULL terminated C string.
 *
 * \param[in]   pLogData                    Log instance.
 * \return                                  The UTC time information status as a C string.
 */
const char *sbgEComLogUtcGetUtcStatusAsString(const SbgEComLogUtc *pLogData);

/*!
 * Set if a valid PPS signal is received and can be used to align the internal clock.
 * 
 * \param[in]   pLogData                    Log instance.
 * \param[in]   hasClockInput               true if a valid PPS signal is received.
 */
void sbgEComLogUtcSetHasClockInput(SbgEComLogUtc *pLogData, bool hasClockInput);

/*!
 * Returns true if a valid and stable PPS signal is being received to align the INS clock.
 * 
 * Use this status to determine if the INS is constantly correcting the clock with a PPS signal
 * or propagating the clock with its internal crystal using latest estimated bias and scale factor corrections.
 * 
 * During nominal INS operations with good GNSS reception, this method should return true.
 * During GNSS outages, no PPS signal is received and this method should return false.
 * 
 * \param[in]   pLogData                    Log instance.
 * \return                                  true if a valid PPS signal is received and false if no PPS signal is received.
 */
bool sbgEComLogUtcHasClockInput(const SbgEComLogUtc *pLogData);

/*!
 * Set if the reported UTC time is accurate or not (ie timestamped with a PPS).
 * 
 * \param[in]   pLogData                    Log instance.
 * \param[in]   utcIsAccurate               true if the UTC time is accurate or false otherwise.
 */
void sbgEComLogUtcTimeSetIsAccurate(SbgEComLogUtc *pLogData, bool utcIsAccurate);

/*!
 * Returns true if the UTC time information is considered to be accurate.
 * 
 * The PPS signal is used to compensate for processing and transmission delays to
 * precisely timestamp the received UTC time information.
 * 
 * During nominal INS operations with good GNSS reception, this method should return true.
 * 
 * If the PPS signal is lost for too much time, this method should return false indicating
 * a degraded UTC time information accuracy.
 * 
 * \param[in]   pLogData                    Log instance.
 * \return                                  true if the UTC time information is accurate.
 */
bool sbgEComLogUtcTimeIsAccurate(const SbgEComLogUtc *pLogData);

/*!
 * Returns true if the clkBiasStd field is filled and valid.
 * 
 * \param[in]   pLogData                    Log instance.
 * \return                                  true if the field is valid.
 */
bool sbgEComLogUtcTimeClkBiasStdIsValid(const SbgEComLogUtc *pLogData);

/*!
 * Returns true if the clkSfErrorStd field is filled and valid.
 * 
 * \param[in]   pLogData                    Log instance.
 * \return                                  true if the field is valid.
 */
bool sbgEComLogUtcTimeClkSfErrorStdIsValid(const SbgEComLogUtc *pLogData);

/*!
 * Returns true if the clkResidualError field is filled and valid.
 * 
 * \param[in]   pLogData                    Log instance.
 * \return                                  true if the field is valid.
 */
bool sbgEComLogUtcTimeClkResidualErrorIsValid(const SbgEComLogUtc *pLogData);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

#ifdef SBG_ECOM_USE_DEPRECATED_MACROS
    #define SBG_ECOM_CLOCK_STABLE_INPUT         (0x0001u << 0)
    #define SBG_ECOM_CLOCK_UTC_SYNC             (0x0001u << 5)
    
    #define SBG_ECOM_CLOCK_ERROR                (SBG_ECOM_CLOCK_STATE_ERROR)
    #define SBG_ECOM_CLOCK_FREE_RUNNING         (SBG_ECOM_CLOCK_STATE_FREE_RUNNING)
    #define SBG_ECOM_CLOCK_STEERING             (SBG_ECOM_CLOCK_STATE_STEERING)
    #define SBG_ECOM_CLOCK_VALID                (SBG_ECOM_CLOCK_STATE_VALID)

    #define SBG_ECOM_UTC_INVALID                (SBG_ECOM_UTC_STATUS_INVALID)
    #define SBG_ECOM_UTC_NO_LEAP_SEC            (SBG_ECOM_UTC_STATUS_NO_LEAP_SEC)
    #define SBG_ECOM_UTC_VALID                  (SBG_ECOM_UTC_STATUS_INITIALIZED)
#endif

SBG_DEPRECATED_TYPEDEF(typedef enum _SbgEComClockState  SbgEComClockStatus);
SBG_DEPRECATED_TYPEDEF(typedef enum _SbgEComUtcStatus   SbgEComClockUtcStatus);
SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogUtc    SbgLogUtcData);

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParseUtcData(SbgStreamBuffer *pStreamBuffer, SbgEComLogUtc *pLogData));
SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogWriteUtcData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogUtc *pLogData));

SBG_DEPRECATED(SbgEComClockState sbgEComLogUtcGetClockStatus(uint16_t status));
SBG_DEPRECATED(const char *sbgEcomLogUtcGetClockStatusAsString(const SbgEComLogUtc *pLogUtc));

SBG_DEPRECATED(SbgEComUtcStatus sbgEComLogUtcGetClockUtcStatus(uint16_t status));
SBG_DEPRECATED(const char *sbgEcomLogUtcGetUtcStatusAsString(const SbgEComLogUtc *pLogUtc));

SBG_DEPRECATED(uint16_t sbgEComLogUtcBuildClockStatus(SbgEComClockState clockState, SbgEComUtcStatus utcStatus, uint16_t masks));

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_UTC_H
