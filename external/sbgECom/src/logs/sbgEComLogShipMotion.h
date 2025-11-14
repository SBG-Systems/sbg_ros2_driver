/*!
 * \file            sbgEComLogShipMotion.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            30 March 2013
 *
 * \brief           Parse logs that returns ship motion values such as heave.
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

#ifndef SBG_ECOM_LOG_SHIP_MOTION_H
#define SBG_ECOM_LOG_SHIP_MOTION_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Heave status definitions                                           -//
//----------------------------------------------------------------------//

#define SBG_ECOM_SHIP_MOTION_HEAVE_VALID            (0x0001u << 0)      /*!< Set if the heave filter has converged and is delivering valid results. */
#define SBG_ECOM_SHIP_MOTION_VEL_AIDED              (0x0001u << 1)      /*!< Set if the heave algorithm uses transient accelerations to improve accuracy. */
#define SBG_ECOM_SHIP_MOTION_SURGE_SWAY_VALID       (0x0001u << 2)      /*!< Set if surge, sway, and velocity (velX, velY) channels are valid and available. */
#define SBG_ECOM_SHIP_MOTION_RESERVED               (0x0001u << 3)      /*!< Reserved. Deprecated status flag, subject to potential reuse. */
#define SBG_ECOM_SHIP_MOTION_HEAVE_PERIOD_VALID     (0x0001u << 4)      /*!< Set if the main wave or swell period is available and valid. */
#define SBG_ECOM_SHIP_MOTION_SWELL_MODE             (0x0001u << 5)      /*!< Set if the heave filter is operating in swell computation mode. */
#define SBG_ECOM_SHIP_MOTION_ACCEL_VALID            (0x0001u << 6)      /*!< Set if X, Y, and Z accelerations are valid and available. */

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * Structure storing data for the SBG_ECOM_LOG_SHIP_MOTION or SBG_ECOM_LOG_SHIP_MOTION_HP message.
 * 
 * Ship motion measurements are defined in a vessel-specific coordinate frame:
 * - Surge: Longitudinal displacement (positive toward the bow/forward).
 * - Sway: Transverse displacement (positive toward the starboard side/right).
 * - Heave: Vertical displacement (positive downward).
 * 
 * Note: Always check the status flags before using the data, as they indicate the validity of each output.
 * Some data fields, such as the heave period or surge/sway components, may be unavailable in certain frames.
 */
typedef struct _SbgEComLogShipMotion
{
    uint32_t    timeStamp;                  /*!< Time in microseconds since sensor power-up. */
    uint16_t    status;                     /*!< Status bitmask indicating validity of ship motion data. */
    float       mainHeavePeriod;            /*!< Dominant heave period in seconds. */
    float       shipMotion[3];              /*!< Surge, sway, and heave positions in meters. */
    float       shipAccel[3];               /*!< Surge, sway, and heave accelerations in m.s^-2. */
    float       shipVel[3];                 /*!< Surge, sway, and heave velocities in m.s^-1. */
} SbgEComLogShipMotion;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Parse data for the SBG_ECOM_LOG_SHIP_MOTION or SBG_ECOM_LOG_SHIP_MOTION_HP message and fill the corresponding structure.
 * 
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogShipMotionReadFromStream(SbgEComLogShipMotion *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_SHIP_MOTION or SBG_ECOM_LOG_SHIP_MOTION_HP message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogShipMotionWriteToStream(const SbgEComLogShipMotion *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

#ifdef SBG_ECOM_USE_DEPRECATED_MACROS
    #define SBG_ECOM_HEAVE_VALID                    SBG_ECOM_SHIP_MOTION_HEAVE_VALID
    #define SBG_ECOM_HEAVE_VEL_AIDED                SBG_ECOM_SHIP_MOTION_VEL_AIDED
    #define SBG_ECOM_HEAVE_SURGE_SWAY_INCLUDED      SBG_ECOM_SHIP_MOTION_SURGE_SWAY_VALID
    #define SBG_ECOM_HEAVE_PERIOD_INCLUDED          SBG_ECOM_SHIP_MOTION_HEAVE_PERIOD_VALID
    #define SBG_ECOM_HEAVE_PERIOD_VALID             SBG_ECOM_SHIP_MOTION_HEAVE_PERIOD_VALID
    #define SBG_ECOM_HEAVE_SWELL_MODE               SBG_ECOM_SHIP_MOTION_SWELL_MODE
#endif

SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogShipMotion SbgLogShipMotionData);

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParseShipMotionData(SbgStreamBuffer *pStreamBuffer, SbgEComLogShipMotion *pLogData));
SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogWriteShipMotionData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogShipMotion *pLogData));

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_SHIP_MOTION_H
