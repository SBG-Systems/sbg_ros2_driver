/*!
 * \file            sbgEComVibMonFft.h
 * \ingroup         vibMonFft
 * \author          SBG Systems
 * \date            May 5, 2025
 *
 * \brief           Vibration monitoring FFT management.
 *
 * Vibration monitoring FFT logs may be sent regularly by a device.
 * This module handles the reassembly of that information for the convenience of the user.
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

/*!
 * \defgroup    vibMonFft Vibration monitoring FFT management
 * \brief       Provides a convenient way to reassemble and work with vibration monitoring FFT logs.
 */

#ifndef SBG_ECOM_VIB_MON_FFT_H
#define SBG_ECOM_VIB_MON_FFT_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <logs/sbgEComLogVibMon.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Constant definitions                                               -//
//----------------------------------------------------------------------//

/*!
 * Maximum number of bins.
 */
#define SBG_ECOM_VIB_MON_FFT_MAX_BIN_COUNT                  (2048)

//----------------------------------------------------------------------//
//- Structure definitions                                              -//
//----------------------------------------------------------------------//

/*!
 * Vibration monitoring FFT, reconstructed.
 *
 * This is a public structure.
 */
typedef struct _SbgEComVibMonFft
{
    uint32_t                         timestampUs;                                   /*!< Timestamp, in us. */
    uint16_t                         status;                                        /*!< Axis and window status. */
    uint16_t                         fSampling;                                     /*!< Sampling frequency, in Hz. */
    uint16_t                         binCount;                                      /*!< Number of FFT bins. */
    float                            scaleFactor;                                   /*!< Scale factor. */
    float                            offset;                                        /*!< Offset, in m.s^-2. */
    float                            acf;                                           /*!< Amplitude correction factor. */
    float                            ecf;                                           /*!< Energy correction factor. */
    uint16_t                         buffer[SBG_ECOM_VIB_MON_FFT_MAX_BIN_COUNT];    /*!< Array of FFT magnitudes, in m.s^-2. */
} SbgEComVibMonFft;

//----------------------------------------------------------------------//
//- Public functions (SbgEComVibMonFft)                                -//
//----------------------------------------------------------------------//

/*!
 * Parse vibration monitoring FFT data and fill the corresponding structure.
 *
 * \param[out]  pVibMonFft                  Structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read from.
 * \return                                  SBG_NO_ERROR if valid vibration monitoring FFT data have been read from the stream buffer.
 */
SbgErrorCode sbgEComVibMonFftReadFromStream(SbgEComVibMonFft *pVibMonFft, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write vibration monitoring FFT data to the output stream buffer.
 *
 * \param[in]   pVibMonFft                  Structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write to.
 * \return                                  SBG_NO_ERROR if vibration monitoring FFT data have have been written to the stream buffer.
 */
SbgErrorCode sbgEComVibMonFftWriteToStream(const SbgEComVibMonFft *pVibMonFft, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

/*!
 * Returns the vibration monitoring axis status.
 *
 * \param[in]   pLogData            Log instance.
 * \return                          Axis.
 */
SbgEComVibMonAxis sbgEComVibMonFftGetAxis(const SbgEComVibMonFft *pLogData);

/*!
 * Returns the vibration monitoring window status.
 *
 * \param[in]   pLogData            Log instance.
 * \return                          Window.
 */
SbgEComVibMonWindow sbgEComVibMonFftGetWindow(const SbgEComVibMonFft *pLogData);

/*!
 * Get the first frequency of a bin, in Hz.
 *
 * \param[in]   pVibMonFft                  Vibration monitoring FFT.
 * \param[in]   binIndex                    Bin index.
 * \return                                  Bin frequency, in Hz.
 */
float sbgEComVibMonFftGetFrequencyFromBinIndex(const SbgEComVibMonFft *pVibMonFft, size_t binIndex);

/*!
 * Get the index of the bin that contains the frequency.
 *
 * The frequency must be valid (i.e. positive and smaller than the Nyquist frequency).
 *
 * \param[in]   pVibMonFft                  Vibration monitoring FFT.
 * \param[in]   frequency                   Bin frequency, in Hz.
 * \return                                  Bin index.
 */
size_t sbgEComVibMonFftGetBinIndexFromFrequency(const SbgEComVibMonFft *pVibMonFft, float frequency);

/*!
 * Get the magnitude of a bin, in m.s^-2.
 *
 * \param[in]   pVibMonFft                  Vibration monitoring FFT.
 * \param[in]   binIndex                    Bin index.
 * \return                                  Magnitude, in m.s^-2.
 */
float sbgEComVibMonFftGetMagnitudeFromBinIndex(const SbgEComVibMonFft *pVibMonFft, size_t binIndex);

/*!
 * Get the magnitude of a bin, with the amplitude correction factor applied, in m.s^-2.
 *
 * \param[in]   pVibMonFft                  Vibration monitoring FFT.
 * \param[in]   binIndex                    Bin index.
 * \return                                  Magnitude, in m.s^-2.
 */
float sbgEComVibMonFftGetMagnitudeFromBinIndexAcfCorrected(const SbgEComVibMonFft *pVibMonFft, size_t binIndex);

/*!
 * Get the magnitude of a bin, with the energy correction factor applied, in m.s^-2.
 *
 * \param[in]   pVibMonFft                  Vibration monitoring FFT.
 * \param[in]   binIndex                    Bin index.
 * \return                                  Magnitude, in m.s^-2.
 */
float sbgEComVibMonFftGetMagnitudeFromBinIndexEcfCorrected(const SbgEComVibMonFft *pVibMonFft, size_t binIndex);

/*!
 * Get the magnitude of a frequency, in m.s^-2.
 *
 * The frequency must be valid (i.e. positive and smaller than the Nyquist frequency).
 * The magnitude returned is the magnitude of the bin that contains the frequency.
 *
 * \param[in]   pVibMonFft                  Vibration monitoring FFT.
 * \param[in]   frequency                   Frequency, in Hz.
 * \return                                  Magnitude, in m.s^-2.
 */
float sbgEComVibMonFftGetMagnitudeFromFrequency(const SbgEComVibMonFft *pVibMonFft, float frequency);

/*!
 * Get the magnitude of a frequency, with the amplitude correction factor applied, in m.s^-2.
 *
 * The frequency must be valid (i.e. positive and smaller than the Nyquist frequency).
 * The magnitude returned is the magnitude of the bin that contains the frequency.
 *
 * \param[in]   pVibMonFft                  Vibration monitoring FFT.
 * \param[in]   frequency                   Frequency, in Hz.
 * \return                                  Magnitude, in m.s^-2.
 */
float sbgEComVibMonFftGetMagnitudeFromFrequencyAcfCorrected(const SbgEComVibMonFft *pVibMonFft, float frequency);

/*!
 * Get the magnitude of a frequency, with the energy correction factor applied, in m.s^-2.
 *
 * The frequency must be valid (i.e. positive and smaller than the Nyquist frequency).
 * The magnitude returned is the magnitude of the bin that contains the frequency.
 *
 * \param[in]   pVibMonFft                  Vibration monitoring FFT.
 * \param[in]   frequency                   Frequency, in Hz.
 * \return                                  Magnitude, in m.s^-2.
 */
float sbgEComVibMonFftGetMagnitudeFromFrequencyEcfCorrected(const SbgEComVibMonFft *pVibMonFft, float frequency);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_VIB_MON_FFT_H
