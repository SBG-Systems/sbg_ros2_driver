/*!
 * \file            sbgEComLogVibMon.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            February 27, 2025
 *
 * \brief           Vibration monitoring related logs.
 *
 * \copyright       Copyright (C) 2007-2025, SBG Systems SAS. All rights reserved.
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

#ifndef SBG_ECOM_LOG_VIB_MON_H
#define SBG_ECOM_LOG_VIB_MON_H

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
 * Axes.
 */
typedef enum _SbgEComVibMonAxis
{
    SBG_ECOM_VIB_MON_AXIS_X                         = 0,                            /*!< X axis. */
    SBG_ECOM_VIB_MON_AXIS_Y                         = 1,                            /*!< Y axis. */
    SBG_ECOM_VIB_MON_AXIS_Z                         = 2,                            /*!< Z axis. */
} SbgEComVibMonAxis;

/*!
 * Windows.
 */
typedef enum _SbgEComVibMonWindow
{
    SBG_ECOM_VIB_MON_WINDOW_RECTANGULAR             = 0,                            /*!< Rectangular (Uniform) window. */
    SBG_ECOM_VIB_MON_WINDOW_HANNING                 = 1,                            /*!< Hanning (Hann) window. */
    SBG_ECOM_VIB_MON_WINDOW_FLAT_TOP                = 2,                            /*!< Flat Top window. */
} SbgEComVibMonWindow;

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * Log structure for vibration monitoring FFT data.
 */
typedef struct _SbgEComLogVibMonFft
{
    uint16_t                             pageIndex;                             /*!< Page index. */
    uint16_t                             nrPages;                               /*!< Total number of pages. */
    size_t                               size;                                  /*!< Buffer size, in bytes. */

    uint8_t                              buffer[64];                            /*!< Buffer. */
} SbgEComLogVibMonFft;

/*!
 * Structure for a frequency band.
 */
typedef struct _SbgEComLogVibMonBand
{
    float                                freq1;                                 /*!< First frequency of the band, in Hz. */
    float                                freq2;                                 /*!< Last frequency of the band, in Hz. */

    float                                rms;                                   /*!< RMS on the band, in m.s^-2. */
    float                                peakHarmonic;                          /*!< Frequency of the peak harmonic on the band, in Hz. */
    float                                peakMagnitude;                         /*!< Magnitude of the peak harmonic on the band, in m.s^-2. */
} SbgEComLogVibMonBand;

/*!
 * Log structure for vibration monitoring report information.
 */
typedef struct _SbgEComLogVibMonReport
{
    uint32_t                             timeStamp;                             /*!< Timestamp, in us. */
    uint16_t                             status;                                /*!< Axis and window status. */
    float                                mean;                                  /*!< Mean of the raw of signal, in m.s^-2. */
    float                                rms;                                   /*!< RMS on the full frequency band, from 0 Hz to the Nyquist frequency, in m.s^-2. */

    SbgEComLogVibMonBand                 bands[4];                              /*!< Frequency bands. */

} SbgEComLogVibMonReport;

//----------------------------------------------------------------------//
//- Public functions (SbgEComLogVibMonFft)                             -//
//----------------------------------------------------------------------//

/*!
 * Zero initialize the message struct.
 *
 * \param[out]  pLogData                    Structure instance to zero init.
 */
void sbgEComLogVibMonFftZeroInit(SbgEComLogVibMonFft *pLogData);

/*!
 * Parse data for the SBG_ECOM_LOG_VIB_MON_FFT message and fill the corresponding structure.
 *
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogVibMonFftReadFromStream(SbgEComLogVibMonFft *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_VIB_MON_FFT message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogVibMonFftWriteToStream(const SbgEComLogVibMonFft *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- Public functions (SbgEComLogVibMonReport)                          -//
//----------------------------------------------------------------------//

/*!
 * Zero initialize the message struct.
 *
 * \param[out]  pLogData                    Structure instance to zero init.
 */
void sbgEComLogVibMonReportZeroInit(SbgEComLogVibMonReport *pLogData);

/*!
 * Parse data for the SBG_ECOM_LOG_VIB_MON_REPORT message and fill the corresponding structure.
 *
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogVibMonReportReadFromStream(SbgEComLogVibMonReport *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_VIB_MON_REPORT message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogVibMonReportWriteToStream(const SbgEComLogVibMonReport *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- Public setters/getters (SbgEComLogVibMonReport)                    -//
//----------------------------------------------------------------------//

/*!
 * Returns the vibration monitoring axis status.
 *
 * \param[in]   pLogData            Log instance.
 * \return                          Axis.
 */
SbgEComVibMonAxis sbgEComLogVibMonReportGetAxis(const SbgEComLogVibMonReport *pLogData);

/*!
 * Returns the vibration monitoring window status.
 *
 * \param[in]   pLogData            Log instance.
 * \return                          Window.
 */
SbgEComVibMonWindow sbgEComLogVibMonReportGetWindow(const SbgEComLogVibMonReport *pLogData);

/*!
 * Returns a pointer to the frequency band with the highest peak amplitude.
 *
 * This function scans all frequency bands in the given vibration monitoring report
 * and returns a pointer to the band where the signal peak is the highest.
 *
 * \param[in]   pLogData            Pointer to the vibration monitoring report log.
 * \return                          Pointer to the frequency band with the highest peak amplitude.
 */
const SbgEComLogVibMonBand *sbgEComLogVibMonReportGetMaxPeakBand(const SbgEComLogVibMonReport *pLogData);

//----------------------------------------------------------------------//
//- Private setters/getters                                            -//
//----------------------------------------------------------------------//

/*!
 * Returns the vibration monitoring axis status.
 *
 * \param[in]   status              Vibration monitoring status.
 * \return                          Axis.
 */
SbgEComVibMonAxis sbgEComLogVibMonStatusGetAxis(uint16_t status);

/*!
 * Returns the vibration monitoring window status.
 *
 * \param[in]   status              Vibration monitoring status.
 * \return                          Window.
 */
SbgEComVibMonWindow sbgEComLogVibMonStatusGetWindow(uint16_t status);

/*!
 * Make a vibration monitoring status.
 *
 * \param[in]   axis                Axis.
 * \param[in]   window              Window.
 * \return                          Status.
 */
uint16_t sbgEComLogVibMonMakeStatus(SbgEComVibMonAxis axis, SbgEComVibMonWindow window);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_VIB_MON_H
