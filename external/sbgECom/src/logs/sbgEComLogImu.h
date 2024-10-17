﻿/*!
 * \file			sbgEComLogImu.h
 * \ingroup			binaryLogs
 * \author			SBG Systems
 * \date			25 February 2013
 *
 * \brief			Parse IMU (Inertial Measurement Unit) measurement logs.
 *
 * \copyright		Copyright (C) 2022, SBG Systems SAS. All rights reserved.
 * \beginlicense	The MIT license
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

#ifndef SBG_ECOM_LOG_IMU_H
#define SBG_ECOM_LOG_IMU_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Log Inertial Data definitions                                      -//
//----------------------------------------------------------------------//

/*!
 * Log inertial data status mask definitions
 */
#define	SBG_ECOM_IMU_COM_OK				(0x00000001u << 0)		/*!< Set to 1 if the communication with the IMU is ok. */
#define	SBG_ECOM_IMU_STATUS_BIT			(0x00000001u << 1)		/*!< Set to 1 if the IMU passes general Built in Tests (calibration, CPU, ...). */

#define	SBG_ECOM_IMU_ACCEL_X_BIT		(0x00000001u << 2)		/*!< Set to 1 if the accelerometer X passes Built In Test. */
#define	SBG_ECOM_IMU_ACCEL_Y_BIT		(0x00000001u << 3)		/*!< Set to 1 if the accelerometer Y passes Built In Test. */
#define	SBG_ECOM_IMU_ACCEL_Z_BIT		(0x00000001u << 4)		/*!< Set to 1 if the accelerometer Z passes Built In Test. */

#define	SBG_ECOM_IMU_GYRO_X_BIT			(0x00000001u << 5)		/*!< Set to 1 if the gyroscope X passes Built In Test. */
#define	SBG_ECOM_IMU_GYRO_Y_BIT			(0x00000001u << 6)		/*!< Set to 1 if the gyroscope Y passes Built In Test. */
#define	SBG_ECOM_IMU_GYRO_Z_BIT			(0x00000001u << 7)		/*!< Set to 1 if the gyroscope Z passes Built In Test. */

#define	SBG_ECOM_IMU_ACCELS_IN_RANGE	(0x00000001u << 8)		/*!< Set to 1 if all accelerometers are within operating range. */
#define SBG_ECOM_IMU_GYROS_IN_RANGE		(0x00000001u << 9)		/*!< Set to 1 if all gyroscopes are within operating range. */

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * Structure that stores data for the SBG_ECOM_LOG_IMU_DATA message.
 *
 * This message returns synchronous IMU measurements that are aligned to UTC time.
 * 
 * All SBG Systems INS, except ELLIPSE products, use an IMU that provides asynchronous measurements.
 * The INS has to extrapolate the IMU measurements to the current EKF processing loop.
 * 
 * For a 200Hz processing loop, the extrapolation can be as high as 5ms.
 * The extrapolation process introduces artificial noise in the measurements.
 * 
 * This message is thus not suitable for post processing for example.
 * 
 * If you would like IMU measurements correctly time stamped to UTC/GPS time
 * but as output by the IMU (asynchronously), please use SBG_ECOM_LOG_IMU_SHORT
 * 
 * WARNING: This message is not recommended for new designs and will be deprecated in sbgECom 5.x
 */
typedef struct _SbgEComLogImuLegacy
{
	uint32_t	timeStamp;					/*!< Time in us since the sensor power up. */
	uint16_t	status;						/*!< IMU status bitmask. */
	float		accelerometers[3];			/*!< X, Y, Z accelerometers in m.s^-2. */
	float		gyroscopes[3];				/*!< X, Y, Z gyroscopes in rad.s^-1. */
	float		temperature;				/*!< Internal temperature in °C. */
	float		deltaVelocity[3];			/*!< X, Y, Z delta velocity in m.s^-2. (Exact same value as accelerometers field) */
	float		deltaAngle[3];				/*!< X, Y, Z delta angle in rad.s^-1. (Exact same value as gyroscopes field) */
} SbgEComLogImuLegacy;

/*!
 * Structure that stores data for the SBG_ECOM_LOG_IMU_SHORT message.
 * 
 * This message is sent asynchronously and must be used for post processing.
 */
typedef struct _SbgEComLogImuShort
{
	uint32_t	timeStamp;					/*!< Time in us since the sensor power up. */
	uint16_t	status;						/*!< IMU status bitmask. */
	int32_t		deltaVelocity[3];			/*!< X, Y, Z delta velocity. Unit is 1048576 LSB for 1 m.s^-2. */
	int32_t		deltaAngle[3];				/*!< X, Y, Z delta angle. Unit is 67108864 LSB for 1 rad.s^-1. */
	int16_t		temperature;				/*!< IMU average temperature. Unit is 256 LSB for 1°C. */
} SbgEComLogImuShort;

/*!
 * Structure that stores the data for SBG_ECOM_LOG_FAST_IMU_DATA message
 * 
 * WARNING: This message is not recommended for new designs and will be deprecated in sbgECom 5.x
 */
typedef struct _SbgEComLogImuFastLegacy
{
	uint32_t	timeStamp;					/*!< Time in us since the sensor power up. */
	uint16_t	status;						/*!< IMU status bitmask. */
	float		accelerometers[3];			/*!< X, Y, Z accelerometers in m.s^-2. */
	float		gyroscopes[3];				/*!< X, Y, Z gyroscopes in rad.s^-1. */
} SbgEComLogImuFastLegacy;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Parse data for the SBG_ECOM_LOG_IMU_DATA message and fill the corresponding structure.
 * 
 * \param[out]	pLogData					Log structure instance to fill.
 * \param[in]	pStreamBuffer				Input stream buffer to read the log from.
 * \return									SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogImuLegacyReadFromStream(SbgEComLogImuLegacy *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_IMU_DATA message to the output stream buffer from the provided structure.
 *
 * \param[in]	pLogData					Log structure instance to write.
 * \param[out]	pStreamBuffer				Output stream buffer to write the log to.
 * \return									SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogImuLegacyWriteToStream(const SbgEComLogImuLegacy *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Parse data for the SBG_ECOM_LOG_IMU_SHORT message and fill the corresponding structure.
 * 
 * \param[out]	pLogData					Log structure instance to fill.
 * \param[in]	pStreamBuffer				Input stream buffer to read the log from.
 * \return									SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogImuShortReadFromStream(SbgEComLogImuShort *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_IMU_SHORT message to the output stream buffer from the provided structure.
 *
 * \param[in]	pLogData					Log structure instance to write.
 * \param[out]	pStreamBuffer				Output stream buffer to write the log to.
 * \return									SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogImuShortWriteToStream(const SbgEComLogImuShort *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Parse data for the SBG_ECOM_LOG_FAST_IMU_DATA message and fill the corresponding structure.
 * 
 * \param[out]	pLogData					Log structure instance to fill.
 * \param[in]	pStreamBuffer				Input stream buffer to read the log from.
 * \return									SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogImuFastLegacyReadFromStream(SbgEComLogImuFastLegacy *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_FAST_IMU_DATA message to the output stream buffer from the provided structure.
 *
 * \param[in]	pLogData					Log structure instance to write.
 * \param[out]	pStreamBuffer				Output stream buffer to write the log to.
 * \return									SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogImuFastLegacyWriteToStream(const SbgEComLogImuFastLegacy *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

/*!
 * Return from an IMU Short log, the X, Y or Z delta angle value in rad.s^-1
 * 
 * \param[in]	pImuShort					Input IMU short message instance.
 * \param[in]	idx							The component to return from 0 to 2.
 * \return									The delta angle value converted in rad.s^-1.
 */
float sbgEComLogImuShortGetDeltaAngle(const SbgEComLogImuShort *pImuShort, size_t idx);

/*!
 * Return from an IMU Short log, the X, Y or Z delta velocity value in m.s^-2
 * 
 * \param[in]	pImuShort					Input IMU short message instance.
 * \param[in]	idx							The component to return from 0 to 2.
 * \return									The delta velocity value converted in m.s^-2.
 */
float sbgEComLogImuShortGetDeltaVelocity(const SbgEComLogImuShort *pImuShort, size_t idx);

/*!
 * Return from an IMU Short log, the temperature in °C
 * 
 * \param[in]	pImuShort					Input IMU short message instance.
 * \return									The converted temperature in °C
 */
float sbgEComLogImuShortGetTemperature(const SbgEComLogImuShort *pImuShort);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogImuLegacy		SbgLogImuData);
SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogImuShort		SbgLogImuShort);
SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogImuFastLegacy	SbgLogFastImuData);

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParseImuData(SbgStreamBuffer *pStreamBuffer, SbgEComLogImuLegacy *pLogData));
SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogWriteImuData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogImuLegacy *pLogData));

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParseImuShort(SbgStreamBuffer *pStreamBuffer, SbgEComLogImuShort *pLogData));
SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogWriteImuShort(SbgStreamBuffer *pStreamBuffer, const SbgEComLogImuShort *pLogData));

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParseFastImuData(SbgStreamBuffer *pStreamBuffer, SbgEComLogImuFastLegacy *pLogData));
SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogWriteFastImuData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogImuFastLegacy *pLogData));

SBG_DEPRECATED(float sbgLogImuShortGetDeltaAngle(const SbgEComLogImuShort *pImuShort, size_t idx));
SBG_DEPRECATED(float sbgLogImuShortGetDeltaVelocity(const SbgEComLogImuShort *pImuShort, size_t idx));
SBG_DEPRECATED(float sbgLogImuShortGetTemperature(const SbgEComLogImuShort *pImuShort));

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_IMU_H
