/*!
 * \file            sbgEComLogDvl.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            05 June 2013
 *
 * \brief           Parse received DVL (Doppler Velocity Logger) measurement logs.
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

#ifndef SBG_ECOM_LOG_DVL_H
#define SBG_ECOM_LOG_DVL_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Log DVL status definitions                                         -//
//----------------------------------------------------------------------//

/*!
 * DVL status mask definitions
 */
#define SBG_ECOM_DVL_VELOCITY_VALID     (0x0001u << 0)          /*!< Set to 1 if the DVL equipment was able to measure a valid velocity. */
#define SBG_ECOM_DVL_TIME_SYNC          (0x0001u << 1)          /*!< Set to 1 if the DVL data is correctly synchronized. */

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * \brief Log structure for DVL 3D velocity measurement.
 *
 * This structure contains data related to the DVL 3D velocity measurements,
 * including timestamp, status, and velocity information.
 *
 * \note The DVL quality indicator represents residual error information rather
 * than an actual standard deviation measurement.
 */
typedef struct _SbgEComLogDvl
{
    uint32_t    timeStamp;              /*!< Time in microseconds since the sensor power up. */
    uint16_t    status;                 /*!< DVL status bitmask. */
    float       velocity[3];            /*!< X, Y, Z velocities in m/s expressed in the DVL instrument frame. */
    float       velocityQuality[3];     /*!< Quality indicators for X, Y, Z velocities provided by the DVL sensor, expressed in m/s. */
} SbgEComLogDvl;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Parse data for the SBG_ECOM_LOG_DVL_BOTTOM_TRACK / SBG_ECOM_LOG_DVL_WATER_TRACK messages and fill the corresponding structure.
 * 
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogDvlReadFromStream(SbgEComLogDvl *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_DVL_BOTTOM_TRACK / SBG_ECOM_LOG_DVL_WATER_TRACK messages to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogDvlWriteToStream(const SbgEComLogDvl *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogDvl SbgLogDvlData);

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParseDvlData(SbgStreamBuffer *pStreamBuffer, SbgEComLogDvl *pLogData));
SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogWriteDvlData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogDvl *pLogData));

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_DVL_H
