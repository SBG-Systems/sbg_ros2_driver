/*!
 * \file            sbgEComLogVelocity.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            March 12, 2025
 *
 * \brief           Parse received generic velocity measurement logs.
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

#ifndef SBG_ECOM_LOG_VELOCITY_H
#define SBG_ECOM_LOG_VELOCITY_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <defs/sbgEComDefsAiding.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Public definitions                                                 -//
//----------------------------------------------------------------------//

/*!
 * Velocity status mask definitions.
 */
#define SBG_ECOM_VELOCITY_0_VALID                   (0x0001 << 3)                   /*!< Set to 1 if the X or North velocity information is valid. */
#define SBG_ECOM_VELOCITY_1_VALID                   (0x0001 << 4)                   /*!< Set to 1 if the Y or East velocity information is valid. */
#define SBG_ECOM_VELOCITY_2_VALID                   (0x0001 << 5)                   /*!< Set to 1 if the Z or Down velocity information is valid. */
#define SBG_ECOM_VELOCITY_STD_VALID                 (0x0001 << 6)                   /*!< Set to 1 if the velocity standard deviation information is valid. */

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * Log structure for velocity measurements.
 */
typedef struct _SbgEComLogVelocity
{
    uint32_t                             timeStamp;                                 /*!< Time in us since the INS power up OR measurement delay in us OR GPS time of the week in ms. */
    uint16_t                             status;                                    /*!< Velocity status bitmask. */
    float                                velocity[3];                               /*!< X,Y,Z or North/East/Down velocities in m/s. */
    float                                velocityStd[3];                            /*!< X,Y,Z or North/East/Down velocities standard deviation in m/s. */
} SbgEComLogVelocity;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Construct an empty / zero initialized instance.
 *
 * \param[in]   pLogData                    Log instance.
 */
void sbgEComLogVelocityConstruct(SbgEComLogVelocity *pLogData);

/*!
 * Parse data for the SBG_ECOM_LOG_VELOCITY_# message and fill the corresponding structure.
 *
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogVelocityReadFromStream(SbgEComLogVelocity *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_VELOCITY_# message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogVelocityWriteToStream(const SbgEComLogVelocity *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

/*!
 * Set the velocity time type.
 *
 * \param[in]   pLogData            Log instance.
 * \param[in]   timeType            The time type to set.
 */
void sbgEComLogVelocitySetTimeType(SbgEComLogVelocity *pLogData, SbgEComAidingTimeType timeType);

/*!
 * Returns the velocity time type.
 *
 * \param[in]   pLogData            Log instance.
 * \return                          The time type.
 */
SbgEComAidingTimeType sbgEComLogVelocityGetTimeType(const SbgEComLogVelocity *pLogData);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_VELOCITY_H
