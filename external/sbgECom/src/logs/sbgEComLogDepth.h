/*!
 * \file            sbgEComLogDepth.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            20 February 2019
 *
 * \brief           Parse received sub-sea depth measurement logs.
 *
 * Depth sensor are used for sub-sea navigation to improve height.
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

#ifndef SBG_ECOM_LOG_DEPTH_H
#define SBG_ECOM_LOG_DEPTH_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Log Air Data status definitions                                    -//
//----------------------------------------------------------------------//

/*!
 * Air Data sensor status mask definitions
 */
#define SBG_ECOM_DEPTH_TIME_IS_DELAY                (0x0001u << 0)      /*!< Set to 1 if the timeStamp field represents a delay instead of an absolute timestamp. */
#define SBG_ECOM_DEPTH_PRESSURE_ABS_VALID           (0x0001u << 1)      /*!< Set to 1 if the pressure field is filled and valid. */
#define SBG_ECOM_DEPTH_ALTITUDE_VALID               (0x0001u << 2)      /*!< Set to 1 if the depth altitude field is filled and valid. */

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * Log structure for Depth sensor measurement (sub-sea).
 */
typedef struct _SbgEComLogDepth
{
    uint32_t    timeStamp;                      /*!< Time in us since the sensor power up OR measurement delay in us. */
    uint16_t    status;                         /*!< Airdata sensor status bitmask. */
    float       pressureAbs;                    /*!< Raw absolute pressure measured by the depth sensor in Pascals. */
    float       altitude;                       /*!< Altitude computed from depth sensor in meters and positive upward. */
} SbgEComLogDepth;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Parse data for the SBG_ECOM_LOG_DEPTH message and fill the corresponding structure.
 * 
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogDepthReadFromStream(SbgEComLogDepth *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_DEPTH message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogDepthWriteToStream(const SbgEComLogDepth *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogDepth SbgLogDepth);

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParseDepth(SbgStreamBuffer *pStreamBuffer, SbgEComLogDepth *pLogData));
SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogWriteDepth(SbgStreamBuffer *pStreamBuffer, const SbgEComLogDepth *pLogData));

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_DEPTH_H
