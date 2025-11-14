/*!
 * \file            sbgEComLogOdometer.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            25 February 2013
 *
 * \brief           Parse received odometer/DMI velocity measurement logs.
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

#ifndef SBG_ECOM_LOG_ODOMETER_H
#define SBG_ECOM_LOG_ODOMETER_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Log odometer status definitions                                    -//
//----------------------------------------------------------------------//

/*!
 * Odometer / velocity status mask definitions.
 */
#define SBG_ECOM_ODO_REAL_MEAS              (0x0001 << 0)           /*!< Set to 1 if this log comes from a real pulse measurement or from a timeout. */
#define SBG_ECOM_ODO_TIME_SYNC              (0x0001 << 1)           /*!< Set to 1 if the velocity information is correctly time synchronized. */

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * Log structure for odometer/DMI velocity measurements.
 */
typedef struct _SbgEComLogOdometer
{
    uint32_t    timeStamp;              /*!< Time in us since the sensor power up. */
    uint16_t    status;                 /*!< Odometer velocity status bitmask. */
    float       velocity;               /*!< Velocity in m.s^-1 in the odometer direction. */
} SbgEComLogOdometer;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Parse data for the SBG_ECOM_LOG_ODO_VEL message and fill the corresponding structure.
 * 
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogOdometerReadFromStream(SbgEComLogOdometer *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_ODO_VEL message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogOdometerWriteToStream(const SbgEComLogOdometer *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogOdometer SbgLogOdometerData);

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParseOdometerData(SbgStreamBuffer *pStreamBuffer, SbgEComLogOdometer *pLogData));
SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogWriteOdometerData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogOdometer *pLogData));

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_ODOMETER_H
