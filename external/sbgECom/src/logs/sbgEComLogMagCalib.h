/*!
 * \file            sbgEComLogMagCalib.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            15 Mai 2024
 *
 * \brief           Parse magnetic calibration logs.
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

#ifndef SBG_ECOM_LOG_MAG_CALIB_H
#define SBG_ECOM_LOG_MAG_CALIB_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * Represents data from the SBG_ECOM_LOG_MAG_CALIB message.
 * 
 * This structure contains raw magnetic field data that has not been compensated for soft and hard iron effects. It is intended
 * to be used with SBG Systems' magnetic calibration algorithms, which calculate the necessary compensation parameters to correct
 * for these distortions.
 * 
 * Note: An effective strategy for utilizing this data involves storing the raw data stream that includes this message and later 
 *       processing it with the sbgCenter software to perform offline soft and hard iron magnetic compensation.
 */
typedef struct _SbgEComLogMagCalib
{
    uint32_t    timeStamp;                  /*!< Time in us since the sensor power up. */
    uint16_t    reserved;                   /*!< Reserved for future use, not currently utilized. */
    uint8_t     magData[16];                /*!< Raw magnetometer calibration data array, essential for compensation calculations. */
} SbgEComLogMagCalib;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Parse data for the SBG_ECOM_LOG_MAG_CALIB message and fill the corresponding structure.
 * 
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogMagCalibReadFromStream(SbgEComLogMagCalib *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_MAG_CALIB message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogMagCalibWriteToStream(const SbgEComLogMagCalib *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogMagCalib   SbgLogMagCalib);

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParseMagCalibData(SbgStreamBuffer *pStreamBuffer, SbgEComLogMagCalib *pLogData));
SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogWriteMagCalibData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogMagCalib *pLogData));

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_MAG_CALIB_H
