/*!
 * \file            sbgEComLog.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            06 February 2013
 *
 * \brief           Parse incoming sbgECom logs and store result in an union.
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

/*!
 * \defgroup    binaryLogs Binary Logs
 * \brief       All messages and logs that can be output by the device.
 */

#ifndef SBG_ECOM_LOG_H
#define SBG_ECOM_LOG_H

 // sbgCommonLib headers
#include <sbgCommon.h>

// Project headers
#include <sbgEComIds.h>

// Local headers
#include "sbgEComLogAirData.h"
#include "sbgEComLogDepth.h"
#include "sbgEComLogDiag.h"
#include "sbgEComLogDvl.h"
#include "sbgEComLogEkf.h"
#include "sbgEComLogEkfRotAccel.h"
#include "sbgEComLogEvent.h"
#include "sbgEComLogGnssHdt.h"
#include "sbgEComLogGnssPos.h"
#include "sbgEComLogGnssVel.h"
#include "sbgEComLogImu.h"
#include "sbgEComLogMag.h"
#include "sbgEComLogMagCalib.h"
#include "sbgEComLogOdometer.h"
#include "sbgEComLogPtp.h"
#include "sbgEComLogRawData.h"
#include "sbgEComLogSat.h"
#include "sbgEComLogSessionInfo.h"
#include "sbgEComLogShipMotion.h"
#include "sbgEComLogStatus.h"
#include "sbgEComLogUsbl.h"
#include "sbgEComLogUtc.h"
#include "sbgEComLogVelocity.h"
#include "sbgEComLogVibMon.h"

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Public definitions                                                 -//
//----------------------------------------------------------------------//

/*!
 *  Union used to store received logs data.
 */
typedef union _SbgEComLogUnion
{
    SbgEComLogStatus                statusData;         /*!< Stores data for the SBG_ECOM_LOG_STATUS message. */
    SbgEComLogImuLegacy             imuData;            /*!< Stores data for the SBG_ECOM_LOG_IMU_DATA message. */
    SbgEComLogImuShort              imuShort;           /*!< Stores data for the SBG_ECOM_LOG_IMU_SHORT message. */
    SbgEComLogEkfEuler              ekfEulerData;       /*!< Stores data for the SBG_ECOM_LOG_EKF_EULER message. */
    SbgEComLogEkfQuat               ekfQuatData;        /*!< Stores data for the SBG_ECOM_LOG_EKF_QUAT message. */
    SbgEComLogEkfNav                ekfNavData;         /*!< Stores data for the SBG_ECOM_LOG_EKF_NAV message. */
    SbgEComLogEkfVelBody            ekfVelBody;         /*!< Stores data for the SBG_ECOM_LOG_EKF_VEL_BODY message. */
    SbgEComLogEkfRotAccel           ekfRotAccel;        /*!< Stores either SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY or SBG_ECOM_LOG_EKF_ROT_ACCEL_NED. */
    SbgEComLogShipMotion            shipMotionData;     /*!< Stores data for the SBG_ECOM_LOG_SHIP_MOTION or SBG_ECOM_LOG_SHIP_MOTION_HP message. */
    SbgEComLogOdometer              odometerData;       /*!< Stores data for the SBG_ECOM_LOG_ODO_VEL message. */
    SbgEComLogPtp                   ptpData;            /*!< Stores data for the SBG_ECOM_LOG_PTP_STATUS message. */
    SbgEComLogUtc                   utcData;            /*!< Stores data for the SBG_ECOM_LOG_UTC_TIME message. */
    SbgEComLogGnssPos               gpsPosData;         /*!< Stores data for the SBG_ECOM_LOG_GPS_POS message. */
    SbgEComLogGnssVel               gpsVelData;         /*!< Stores data for the SBG_ECOM_LOG_GPS#_VEL message. */
    SbgEComLogGnssHdt               gpsHdtData;         /*!< Stores data for the SBG_ECOM_LOG_GPS#_HDT message. */
    SbgEComLogRawData               gpsRawData;         /*!< Stores data for the SBG_ECOM_LOG_GPS#_RAW message. */
    SbgEComLogRawData               rtcmRawData;        /*!< Stores data for the SBG_ECOM_LOG_RTCM_RAW message. */
    SbgEComLogMag                   magData;            /*!< Stores data for the SBG_ECOM_LOG_MAG message. */
    SbgEComLogMagCalib              magCalibData;       /*!< Stores data for the SBG_ECOM_LOG_MAG_CALIB message. */
    SbgEComLogDvl                   dvlData;            /*!< Stores data for the SBG_ECOM_LOG_DVL_BOTTOM_TRACK message. */
    SbgEComLogAirData               airData;            /*!< Stores data for the SBG_ECOM_LOG_AIR_DATA message. */
    SbgEComLogUsbl                  usblData;           /*!< Stores data for the SBG_ECOM_LOG_USBL message. */
    SbgEComLogDepth                 depthData;          /*!< Stores data for the SBG_ECOM_LOG_DEPTH message */
    SbgEComLogEvent                 eventMarker;        /*!< Stores data for the SBG_ECOM_LOG_EVENT_# message. */
    SbgEComLogDiagData              diagData;           /*!< Stores data for the SBG_ECOM_LOG_DIAG message. */
    SbgEComLogSatList               satGroupData;       /*!< Stores data for the SBG_ECOM_LOG_SAT message. */
    SbgEComLogSessionInfo           sessionInfoData;    /*!< Stores data for the SBG_ECOM_LOG_SESSION_INFO message. */
    SbgEComLogVelocity              velocityData;       /*!< Stores data for the SBG_ECOM_LOG_VELOCITY_# message. */
    SbgEComLogVibMonFft             vibMonFft;          /*!< Stores data for the SBG_ECOM_LOG_VIB_MON_FFT message. */
    SbgEComLogVibMonReport          vibMonReport;       /*!< Stores data for the SBG_ECOM_LOG_VIB_MON_REPORT message. */

    /* Fast logs */
    SbgEComLogImuFastLegacy         fastImuData;        /*!< Stores Fast IMU Data for 1KHz output */

} SbgEComLogUnion;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Parse an incoming log and fill the output union.
 *
 * \param[in]   msgClass                    Received message class
 * \param[in]   msgId                       Received message ID
 * \param[in]   pPayload                    Read only pointer on the payload buffer.
 * \param[in]   payloadSize                 Payload size in bytes.
 * \param[out]  pLogData                    Pointer on the output union that stores parsed data.
 */
SbgErrorCode sbgEComLogParse(SbgEComClass msgClass, SbgEComMsgId msgId, const void *pPayload, size_t payloadSize, SbgEComLogUnion *pLogData);

/*!
 * Clean up resources allocated during parsing, if any.
 *
 * \param[in]   pLogData                    Log data.
 * \param[in]   msgClass                    Message class.
 * \param[in]   msgId                       Message ID.
 */
void sbgEComLogCleanup(SbgEComLogUnion *pLogData, SbgEComClass msgClass, SbgEComMsgId msgId);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SBG_DEPRECATED_TYPEDEF(typedef union _SbgEComLogUnion SbgBinaryLogData);

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParse(SbgEComClass msgClass, SbgEComMsgId msg, const void *pPayload, size_t payloadSize, SbgEComLogUnion *pLogData));

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_H
