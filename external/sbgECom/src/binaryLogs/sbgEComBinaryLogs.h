/*!
 *	\file		sbgEComBinaryLogs.h
 *  \author		SBG Systems (Raphael Siryani)
 *	\date		06 February 2013
 *
 *	\brief		This file is used to parse received binary logs.
 *
 *	\section CodeCopyright Copyright Notice 
 *  The MIT license
 *  
 *  Copyright (C) 2007-2020, SBG Systems SAS. All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */
#ifndef __BINARY_LOGS_H__
#define __BINARY_LOGS_H__

#include <sbgCommon.h>
#include "../sbgEComIds.h"
#include "sbgEComBinaryLogAirData.h"
#include "sbgEComBinaryLogDebug.h"
#include "sbgEComBinaryLogDepth.h"
#include "sbgEComBinaryLogDiag.h"
#include "sbgEComBinaryLogDvl.h"
#include "sbgEComBinaryLogEkf.h"
#include "sbgEComBinaryLogEvent.h"
#include "sbgEComBinaryLogGps.h"
#include "sbgEComBinaryLogImu.h"
#include "sbgEComBinaryLogImuRaw.h"
#include "sbgEComBinaryLogMag.h"
#include "sbgEComBinaryLogOdometer.h"
#include "sbgEComBinaryLogShipMotion.h"
#include "sbgEComBinaryLogStatus.h"
#include "sbgEComBinaryLogUsbl.h"
#include "sbgEComBinaryLogUtc.h"

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 *	Union used to store received logs data.
 */
typedef union _SbgBinaryLogData
{
	SbgLogStatusData		statusData;			/*!< Stores data for the SBG_ECOM_LOG_STATUS message. */
	SbgLogImuData			imuData;			/*!< Stores data for the SBG_ECOM_LOG_IMU_DATA message. */
	SbgLogImuShort			imuShort;			/*!< Stores data for the SBG_ECOM_LOG_IMU_SHORT message. */
	SbgLogEkfEulerData		ekfEulerData;		/*!< Stores data for the SBG_ECOM_LOG_EKF_EULER message. */
	SbgLogEkfQuatData		ekfQuatData;		/*!< Stores data for the SBG_ECOM_LOG_EKF_QUAT message. */
	SbgLogEkfNavData		ekfNavData;			/*!< Stores data for the SBG_ECOM_LOG_EKF_NAV message. */
	SbgLogShipMotionData	shipMotionData;		/*!< Stores data for the SBG_ECOM_LOG_SHIP_MOTION or SBG_ECOM_LOG_SHIP_MOTION_HP message. */
	SbgLogOdometerData		odometerData;		/*!< Stores data for the SBG_ECOM_LOG_ODO_VEL message. */
	SbgLogUtcData			utcData;			/*!< Stores data for the SBG_ECOM_LOG_UTC_TIME message. */
	SbgLogGpsPos			gpsPosData;			/*!< Stores data for the SBG_ECOM_LOG_GPS_POS message. */
	SbgLogGpsVel			gpsVelData;			/*!< Stores data for the SBG_ECOM_LOG_GPS#_VEL message. */
	SbgLogGpsHdt			gpsHdtData;			/*!< Stores data for the SBG_ECOM_LOG_GPS#_HDT message. */
	SbgLogGpsRaw			gpsRawData;			/*!< Stores data for the SBG_ECOM_LOG_GPS#_RAW message. */
	SbgLogMag				magData;			/*!< Stores data for the SBG_ECOM_LOG_MAG message. */
	SbgLogMagCalib			magCalibData;		/*!< Stores data for the SBG_ECOM_LOG_MAG_CALIB message. */
	SbgLogDvlData			dvlData;			/*!< Stores data for the SBG_ECOM_LOG_DVL_BOTTOM_TRACK message. */
	SbgLogAirData			airData;			/*!< Stores data for the SBG_ECOM_LOG_AIR_DATA message. */
	SbgLogUsblData			usblData;			/*!< Stores data for the SBG_ECOM_LOG_USBL message. */
	SbgLogDepth				depthData;			/*!< Stores data for the SBG_ECOM_LOG_DEPTH message */
	SbgLogEvent				eventMarker;		/*!< Stores data for the SBG_ECOM_LOG_EVENT_# message. */
	SbgLogDebugData			debugData;			/*!< Stores debug information */
	SbgLogImuRawData		imuRawData;			/*!< Stores data for the SBG_ECOM_LOG_IMU_RAW_DATA message. */
	SbgLogFastImuData		fastImuData;		/*!< Stores Fast Imu Data for 1KHz output */
	SbgLogDiagData			diagData;			/*!< Stores data for the SBG_ECOM_LOG_DIAG message. */
} SbgBinaryLogData;

//----------------------------------------------------------------------//
//- Communication protocol operations                                  -//
//----------------------------------------------------------------------//

/*!
 *	Parse an incoming log and fill the output union.
 *	\param[in]	msgClass					Received message class
 *	\param[in]	msg							Received message ID
 *	\param[in]	pPayload					Read only pointer on the payload buffer.
 *	\param[in]	payloadSize					Payload size in bytes.
 *	\param[out]	pOutputData					Pointer on the output union that stores parsed data.
 */
SbgErrorCode sbgEComBinaryLogParse(SbgEComClass msgClass, SbgEComMsgId msg, const void *pPayload, size_t payloadSize, SbgBinaryLogData *pOutputData);

#endif
