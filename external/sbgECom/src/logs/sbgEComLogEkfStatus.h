/*!
 * \file            sbgEComLogEkfStatus.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            March 19, 2026
 *
 * \brief           Parse logs used to report EKF filter status.
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

#ifndef SBG_ECOM_LOG_EKF_STATUS_H
#define SBG_ECOM_LOG_EKF_STATUS_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- EKF status definitions                                             -//
//----------------------------------------------------------------------//

#define SBG_EKF_STATUS_ROLL_PITCH_VALID                 (0x0001u << 0)          /*!< Set if attitude data is reliable (Roll/Pitch error within defined criteria). */
#define SBG_EKF_STATUS_HEADING_VALID                    (0x0001u << 1)          /*!< Set if heading data is reliable (Heading error within defined criteria). */
#define SBG_EKF_STATUS_VELOCITY_VALID                   (0x0001u << 2)          /*!< Set if velocity data is reliable (Velocity error within defined criteria). */
#define SBG_EKF_STATUS_POSITION_VALID                   (0x0001u << 3)          /*!< Set if position data is reliable (Position error within defined criteria). */
#define SBG_EKF_STATUS_ALIGN_VALID                      (0x0001u << 4)          /*!< Set if the INS is aligned and sensor error estimation has converged enough to trust the solution. */

#define SBG_EKF_STATUS_GNSS_SPOOFING_SRC_TIME           (0x0001u << 3)          /*!< Set if GNSS reports inconsistent time information. */
#define SBG_EKF_STATUS_GNSS_SPOOFING_SRC_BEHAVIOR       (0x0001u << 4)          /*!< Set if GNSS exhibits abnormal behavior consistent with spoofing. */
#define SBG_EKF_STATUS_GNSS_SPOOFING_SRC_SOLUTION       (0x0001u << 5)          /*!< Set if the GNSS PVT solution is inconsistent with the INS solution. */

#define SBG_EKF_STATUS_AIDING_VERTICAL_REF_USED         (0x00000001u << 0)      /*!< Set if the vertical reference is used in the solution. */
#define SBG_EKF_STATUS_AIDING_ZUPT_USED                 (0x00000001u << 1)      /*!< Set if a Zero Velocity Update (ZUPT) is is used in the solution. */
#define SBG_EKF_STATUS_AIDING_ZARU_USED                 (0x00000001u << 2)      /*!< Set if a Zero Angular Rate Update (ZARU) is used in the solution. */
#define SBG_EKF_STATUS_AIDING_MOTION_CONSTRAINT_USED    (0x00000001u << 3)      /*!< Set if vehicle motion constraints are used in the solution. */
#define SBG_EKF_STATUS_AIDING_MAGNETOMETER_USED         (0x00000001u << 4)      /*!< Set if the magnetometer is used in the solution. */
#define SBG_EKF_STATUS_AIDING_GNSS1_VEL_USED            (0x00000001u << 5)      /*!< Set if GNSS 1 velocity is used in the solution. */
#define SBG_EKF_STATUS_AIDING_GNSS1_POS_USED            (0x00000001u << 6)      /*!< Set if GNSS 1 position is used in the solution. */
#define SBG_EKF_STATUS_AIDING_GNSS1_HDT_USED            (0x00000001u << 7)      /*!< Set if GNSS 1 true heading is used in the solution. */
#define SBG_EKF_STATUS_AIDING_GNSS2_VEL_USED            (0x00000001u << 8)      /*!< Set if GNSS 2 velocity is used in the solution. */
#define SBG_EKF_STATUS_AIDING_GNSS2_POS_USED            (0x00000001u << 9)      /*!< Set if GNSS 2 position is used in the solution. */
#define SBG_EKF_STATUS_AIDING_GNSS2_HDT_USED            (0x00000001u << 10)     /*!< Set if GNSS 2 true heading is used in the solution. */
#define SBG_EKF_STATUS_AIDING_ODO_USED                  (0x00000001u << 11)     /*!< Set if odometer velocity is used in the solution. */
#define SBG_EKF_STATUS_AIDING_DVL_BT_USED               (0x00000001u << 12)     /*!< Set if DVL bottom-track velocity is used in the solution. */
#define SBG_EKF_STATUS_AIDING_DVL_WT_USED               (0x00000001u << 13)     /*!< Set if DVL water-track velocity is used in the solution. */
#define SBG_EKF_STATUS_AIDING_USBL_USED                 (0x00000001u << 14)     /*!< Set if USBL position is used in the solution. */
#define SBG_EKF_STATUS_AIDING_DEPTH_USED                (0x00000001u << 15)     /*!< Set if depth is used in the solution. */
#define SBG_EKF_STATUS_AIDING_AIRSPEED_USED             (0x00000001u << 16)     /*!< Set if true airspeed is used in the solution. */
#define SBG_EKF_STATUS_AIDING_BARO_USED                 (0x00000001u << 17)     /*!< Set if barometric altitude is used in the solution. */
#define SBG_EKF_STATUS_AIDING_GENERIC_VEL1_USED         (0x00000001u << 18)     /*!< Set if generic velocity 1 is used in the solution. */
#define SBG_EKF_STATUS_AIDING_GENERIC_POS1_USED         (0x00000001u << 19)     /*!< Set if generic position 1 is used in the solution. */

//----------------------------------------------------------------------//
//- Structure definitions                                              -//
//----------------------------------------------------------------------//

/*!
 * Solution types.
 */
typedef enum _SbgEComEkfStatusSolutionType
{
    SBG_EKF_STATUS_SOLUTION_TYPE_INVALID                = 0,                    /*!< EKF is not initialized. All outputs are invalid. */
    SBG_EKF_STATUS_SOLUTION_TYPE_VG                     = 1,                    /*!< Vertical gyro mode with roll and pitch estimated from the vertical reference. */
    SBG_EKF_STATUS_SOLUTION_TYPE_AHRS                   = 2,                    /*!< AHRS mode with roll, pitch, and heading estimated from magnetometers, dual-GNSS heading, or gyro-compassing. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_INERTIAL           = 3,                    /*!< Full INS solution using inertial data only. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_MOTION_CONSTRAINT  = 4,                    /*!< Full INS solution using vehicle motion constraints. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_ZUPT               = 5,                    /*!< Full INS solution using Zero Velocity Updates (ZUPT). */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_GNSS_VEL           = 6,                    /*!< Full INS solution using GNSS velocity. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_GNSS_UNKNOWN       = 7,                    /*!< Full INS solution using GNSS position of unknown type. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_SINGLE_POINT       = 8,                    /*!< Full INS solution using GNSS single-point position. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_DGPS               = 9,                    /*!< Full INS solution using GNSS DGPS position. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_SBAS               = 10,                   /*!< Full INS solution using GNSS SBAS position. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_RTK_FLOAT          = 11,                   /*!< Full INS solution using GNSS RTK with float position. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_RTK_FIXED          = 12,                   /*!< Full INS solution using GNSS RTK with fixed position. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_PPP_FLOAT          = 13,                   /*!< Full INS solution using GNSS PPP with float position. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_PPP_FIXED          = 14,                   /*!< Full INS solution using GNSS PPP with fixed position. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_ODO                = 15,                   /*!< Full INS solution using odometer velocity. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_DVL                = 16,                   /*!< Full INS solution using DVL velocity. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_USBL               = 17,                   /*!< Full INS solution using USBL position. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_AIRSPEED           = 18,                   /*!< Full INS solution using true airspeed. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_VEL1               = 19,                   /*!< Full INS solution using generic velocity 1. */
    SBG_EKF_STATUS_SOLUTION_TYPE_NAV_POS1               = 20,                   /*!< Full INS solution using generic position 1. */
} SbgEComEkfStatusSolutionType;

/*!
 * Navigation safety indicator.
 */
typedef enum _SbgEComEkfStatusNavSafety
{
    SBG_EKF_STATUS_NAV_SAFETY_UNKNOWN       = 0,                                /*!< Safety cannot be assessed. Anti-spoofing is disabled, or navigation uncertainty is too high to reliably detect or mitigate spoofing. */
    SBG_EKF_STATUS_NAV_NOMINAL              = 1,                                /*!< No spoofing is detected, and navigation operates under nominal conditions. */
    SBG_EKF_STATUS_NAV_MITIGATED            = 2,                                /*!< Spoofing is detected or suspected, but navigation remains protected thanks to mitigation using inertial and/or other aiding sources. */
    SBG_EKF_STATUS_NAV_UNPROTECTED          = 3,                                /*!< Navigation is operating with limited or no spoofing mitigation capability (e.g. due to high uncertainty, or insufficient aiding). */
} SbgEComEkfStatusNavSafety;

/*!
 * Spoofing indicator.
 */
typedef enum _SbgEComEkfStatusSpoofing
{
    SBG_EKF_STATUS_SPOOFING_UNKNOWN         = 0,                                /*!< Undetermined spoofing status. */
    SBG_EKF_STATUS_SPOOFING_RELIABLE        = 1,                                /*!< GNSS solution is considered as reliable. */
    SBG_EKF_STATUS_SPOOFING_INDICATED       = 2,                                /*!< One spoofing detector indicated potential spoofing event. */
    SBG_EKF_STATUS_SPOOFING_CONFIRMED       = 3,                                /*!< Multiple indications of spoofing. */
} SbgEComEkfStatusSpoofing;

/*!
 * Stores EKF filter status data.
 */
typedef struct _SbgEComLogEkfStatus
{
    uint32_t                             timeStamp;                             /*!< Time since the sensor power up, in microseconds. */
    uint32_t                             ekfStatus;                             /*!< EKF general status. */
    uint8_t                              solutionType;                          /*!< INS solution type. */
    uint8_t                              alignmentProgress;                     /*!< Alignment progress, in percent from 0 to 100. */
    uint16_t                             gnss1SpoofingStatus;                   /*!< GNSS 1 extended spoofing status. */
    uint16_t                             gnss2SpoofingStatus;                   /*!< GNSS 2 extended spoofing status. */
    uint32_t                             aidingsUsed;                           /*!< Aiding sources currently used by the EKF solution. */
} SbgEComLogEkfStatus;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Parse data for the SBG_ECOM_LOG_EKF_STATUS message and fill the corresponding structure.
 *
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogEkfStatusReadFromStream(SbgEComLogEkfStatus *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_EKF_STATUS message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogEkfStatusWriteToStream(const SbgEComLogEkfStatus *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- Public getters & helpers                                           -//
//----------------------------------------------------------------------//

/*!
 * Construct an empty / zero initialized instance.
 *
 * \param[in]   pLogData                    Log instance.
 */
void sbgEComLogEkfStatusConstruct(SbgEComLogEkfStatus *pLogData);

/*!
 * Set the navigation safety indicator.
 *
 * \param[in]   pLogData                    Log instance.
 * \param[in]   navSafety                   The navigation safety indicator to set.
 */
void sbgEComLogEkfStatusSetNavSafety(SbgEComLogEkfStatus *pLogData, SbgEComEkfStatusNavSafety navSafety);

/*!
 * Returns the navigation safety indicator.
 *
 * \param[in]   pLogData                    Log instance.
 * \return                                  The navigation safety indicator.
 */
SbgEComEkfStatusNavSafety sbgEComLogEkfStatusGetNavSafety(const SbgEComLogEkfStatus *pLogData);

/*!
 * Set the GNSS 1 spoofing indicator.
 *
 * \param[in]   pLogData                    Log instance.
 * \param[in]   spoofing                    The GNSS 1 spoofing indicator to set.
 */
void sbgEComLogEkfStatusSetGnss1SpoofingIndicator(SbgEComLogEkfStatus *pLogData, SbgEComEkfStatusSpoofing spoofing);

/*!
 * Returns the GNSS 1 spoofing indicator.
 *
 * \param[in]   pLogData                    Log instance.
 * \return                                  The GNSS 1 spoofing indicator.
 */
SbgEComEkfStatusSpoofing sbgEComLogEkfStatusGetGnss1SpoofingIndicator(const SbgEComLogEkfStatus *pLogData);

/*!
 * Set the GNSS 2 spoofing indicator.
 *
 * \param[in]   pLogData                    Log instance.
 * \param[in]   spoofing                    The GNSS 2 spoofing indicator to set.
 */
void sbgEComLogEkfStatusSetGnss2SpoofingIndicator(SbgEComLogEkfStatus *pLogData, SbgEComEkfStatusSpoofing spoofing);

/*!
 * Returns the GNSS 2 spoofing indicator.
 *
 * \param[in]   pLogData                    Log instance.
 * \return                                  The GNSS 2 spoofing indicator.
 */
SbgEComEkfStatusSpoofing sbgEComLogEkfStatusGetGnss2SpoofingIndicator(const SbgEComLogEkfStatus *pLogData);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_EKF_STATUS_H
