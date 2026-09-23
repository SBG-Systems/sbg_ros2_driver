/*!
 * \file            sbgEComLogPosition.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            October 7, 2025
 *
 * \brief           Parse received generic position measurement logs.
 *
 * \copyright       Copyright (C) 2007-2026, SBG Systems SAS. All rights reserved.
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

#ifndef SBG_ECOM_LOG_POSITION_H
#define SBG_ECOM_LOG_POSITION_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <defs/sbgEComDefsAiding.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Enumeration definitions                                            -//
//----------------------------------------------------------------------//

/*!
 * Position types definitions.
 */
typedef enum _SbgEComPositionType
{
    SBG_ECOM_POSITION_TYPE_INVALID              = 0,                                /*!< Position is invalid. */
    SBG_ECOM_POSITION_TYPE_VALID                = 1,                                /*!< Position is valid. */
} SbgEComPositionType;

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * Log structure for position measurements.
 */
typedef struct _SbgEComLogPosition
{
    uint32_t                             timeStamp;                                 /*!< Time in us since the INS power up OR measurement delay in us OR GPS time of the week in ms. */
    uint16_t                             status;                                    /*!< Position status bitmask. */
    double                               position[3];                               /*!< Latitude (positive north), longitude (positive east), and height above ellipsoid, in degrees, degrees, and meters respectively. */
    float                                covLatLat;                                 /*!< Variance latitude, in m^2. */
    float                                covLonLon;                                 /*!< Variance longitude, in m^2. */
    float                                covHgtHgt;                                 /*!< Variance height, in m^2. */
    float                                covLatLon;                                 /*!< Covariance latitude-longitude, in m^2. */
    float                                covLatHgt;                                 /*!< Covariance latitude-height, in m^2. */
    float                                covLonHgt;                                 /*!< Covariance longitude-height, in m^2. */
} SbgEComLogPosition;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Parse data for the SBG_ECOM_LOG_POSITION_# message and fill the corresponding structure.
 *
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogPositionReadFromStream(SbgEComLogPosition *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_POSITION_# message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogPositionWriteToStream(const SbgEComLogPosition *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

/*!
 * Set the position type.
 *
 * \param[in]   pLogData                    Log instance.
 * \param[in]   posType                     The position type to set.
 */
void sbgEComLogPositionSetPositionType(SbgEComLogPosition *pLogData, SbgEComPositionType posType);

/*!
 * Returns the position type.
 *
 * \param[in]   pLogData                    Log instance.
 * \return                                  The position type.
 */
SbgEComPositionType sbgEComLogPositionGetPositionType(const SbgEComLogPosition *pLogData);

/*!
 * Set the position time type.
 *
 * \param[in]   pLogData                    Log instance.
 * \param[in]   timeType                    The time type to set.
 */
void sbgEComLogPositionSetTimeType(SbgEComLogPosition *pLogData, SbgEComAidingTimeType timeType);

/*!
 * Returns the position time type.
 *
 * \param[in]   pLogData                    Log instance.
 * \return                                  The time type.
 */
SbgEComAidingTimeType sbgEComLogPositionGetTimeType(const SbgEComLogPosition *pLogData);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_POSITION_H
