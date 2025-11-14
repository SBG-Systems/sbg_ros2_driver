/*!
 * \file            sbgEComLogImu.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            25 February 2013
 *
 * \brief           Parse IMU (Inertial Measurement Unit) measurement logs.
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
#define SBG_ECOM_IMU_COM_OK                 (0x00000001u << 0)      /*!< Set to 1 if the communication with the IMU is ok. */
#define SBG_ECOM_IMU_STATUS_BIT             (0x00000001u << 1)      /*!< Set to 1 if the IMU passes general Built in Tests (calibration, CPU, ...). */

#define SBG_ECOM_IMU_ACCEL_X_BIT            (0x00000001u << 2)      /*!< Set to 1 if the accelerometer X passes Built In Test. */
#define SBG_ECOM_IMU_ACCEL_Y_BIT            (0x00000001u << 3)      /*!< Set to 1 if the accelerometer Y passes Built In Test. */
#define SBG_ECOM_IMU_ACCEL_Z_BIT            (0x00000001u << 4)      /*!< Set to 1 if the accelerometer Z passes Built In Test. */

#define SBG_ECOM_IMU_GYRO_X_BIT             (0x00000001u << 5)      /*!< Set to 1 if the gyroscope X passes Built In Test. */
#define SBG_ECOM_IMU_GYRO_Y_BIT             (0x00000001u << 6)      /*!< Set to 1 if the gyroscope Y passes Built In Test. */
#define SBG_ECOM_IMU_GYRO_Z_BIT             (0x00000001u << 7)      /*!< Set to 1 if the gyroscope Z passes Built In Test. */

#define SBG_ECOM_IMU_ACCELS_IN_RANGE        (0x00000001u << 8)      /*!< Set to 1 if all accelerometers are within operating range. */
#define SBG_ECOM_IMU_GYROS_IN_RANGE         (0x00000001u << 9)      /*!< Set to 1 if all gyroscopes are within operating range. */
#define SBG_ECOM_IMU_GYROS_USE_HIGH_SCALE   (0x00000001u << 10)     /*!< Set if the gyroscope scale factor range is high. Applicable only for SBG_ECOM_LOG_IMU_SHORT logs. */

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * \brief Structure that stores data for the SBG_ECOM_LOG_IMU_DATA message.
 *
 * This message provides synchronous IMU measurements aligned to UTC time.  
 * Most SBG Systems INS, except ELLIPSE products, use IMUs with asynchronous measurements.
 * 
 * These measurements are extrapolated to match the EKF processing loop, which can introduce  
 * artificial noise, particularly in high-frequency processing loops (e.g., 200Hz with up to 5ms extrapolation).
 *
 * \note This message is not suitable for post-processing due to the introduced noise.
 * For accurate UTC/GPS-timestamped asynchronous IMU measurements, use SBG_ECOM_LOG_IMU_SHORT.
 *
 * \warning This message is not recommended for new designs and will be deprecated in sbgECom 5.x.
 */
typedef struct _SbgEComLogImuLegacy
{
    uint32_t    timeStamp;                  /*!< Time in microseconds since the sensor power up. */
    uint16_t    status;                     /*!< IMU status bitmask. */
    float       accelerometers[3];          /*!< X, Y, Z accelerometer readings in m/s². */
    float       gyroscopes[3];              /*!< X, Y, Z gyroscope readings in rad/s. */
    float       temperature;                /*!< Internal temperature in °C. */
    float       deltaVelocity[3];           /*!< X, Y, Z delta velocity in m/s² (same as accelerometers field). */
    float       deltaAngle[3];              /*!< X, Y, Z delta angle in rad/s (same as gyroscopes field). */
} SbgEComLogImuLegacy;

/*!
 * Structure that stores data for the SBG_ECOM_LOG_IMU_SHORT message.
 *
 * This message is sent asynchronously and must be used for post processing.
 *
 * The delta angle values are scaled based on the gyroscopes output. If any output exceeds
 * a predefined limit, the scale factor switches from standard to high range to prevent saturation.
 */
typedef struct _SbgEComLogImuShort
{
    uint32_t    timeStamp;                  /*!< Time in us since the sensor power up. */
    uint16_t    status;                     /*!< IMU status bitmask. */
    int32_t     deltaVelocity[3];           /*!< X, Y, Z delta velocity. Unit is 1048576 LSB for 1 m.s^-2. */
    int32_t     deltaAngle[3];              /*!< X, Y, Z delta angle. Unit is either 67108864 LSB for 1 rad.s^-1 (standard) or 12304174 LSB for 1 rad.s^-1 (high range), managed automatically. */
    int16_t     temperature;                /*!< IMU average temperature. Unit is 256 LSB for 1°C. */
} SbgEComLogImuShort;

/*!
 * Structure that stores the data for SBG_ECOM_LOG_FAST_IMU_DATA message
 * 
 * \note This message is not recommended for new designs and will be deprecated in sbgECom 5.x
 */
typedef struct _SbgEComLogImuFastLegacy
{
    uint32_t    timeStamp;                  /*!< Time in us since the sensor power up. */
    uint16_t    status;                     /*!< IMU status bitmask. */
    float       accelerometers[3];          /*!< X, Y, Z accelerometers in m.s^-2. */
    float       gyroscopes[3];              /*!< X, Y, Z gyroscopes in rad.s^-1. */
} SbgEComLogImuFastLegacy;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Parse data for the SBG_ECOM_LOG_IMU_DATA message and fill the corresponding structure.
 *
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogImuLegacyReadFromStream(SbgEComLogImuLegacy *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_IMU_DATA message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogImuLegacyWriteToStream(const SbgEComLogImuLegacy *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Parse data for the SBG_ECOM_LOG_IMU_SHORT message and fill the corresponding structure.
 *
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogImuShortReadFromStream(SbgEComLogImuShort *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_IMU_SHORT message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogImuShortWriteToStream(const SbgEComLogImuShort *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Parse data for the SBG_ECOM_LOG_FAST_IMU_DATA message and fill the corresponding structure.
 *
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogImuFastLegacyReadFromStream(SbgEComLogImuFastLegacy *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_FAST_IMU_DATA message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogImuFastLegacyWriteToStream(const SbgEComLogImuFastLegacy *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

/*!
 * Return from an IMU Short log, the X, Y or Z delta angle value in rad.s^-1
 *
 * \param[in]   pImuShort                   Input IMU short message instance.
 * \param[in]   idx                         The component to return from 0 to 2.
 * \return                                  The delta angle value converted in rad.s^-1.
 */
float sbgEComLogImuShortGetDeltaAngle(const SbgEComLogImuShort *pImuShort, size_t idx);

/*!
 * Set the delta angle values.
 *
 * \param[in]   pImuShort                   Input IMU short message instance.
 * \param[in]   pArray                      Array of delta angle values, in rad.s^-1.
 * \param[in]   arraySize                   Size of the array (should be 3).
 */
void sbgEComLogImuShortSetDeltaAngle(SbgEComLogImuShort *pImuShort, const float *pArray, size_t arraySize);

/*!
 * Return from an IMU Short log, the X, Y or Z delta velocity value in m.s^-2
 *
 * \param[in]   pImuShort                   Input IMU short message instance.
 * \param[in]   idx                         The component to return from 0 to 2.
 * \return                                  The delta velocity value converted in m.s^-2.
 */
float sbgEComLogImuShortGetDeltaVelocity(const SbgEComLogImuShort *pImuShort, size_t idx);

/*!
 * Set the delta velocity values.
 *
 * \param[in]   pImuShort                   Input IMU short message instance.
 * \param[in]   pArray                      Array of delta velocity values, in m.s^-2.
 * \param[in]   arraySize                   Size of the array (should be 3).
 */
void sbgEComLogImuShortSetDeltaVelocity(SbgEComLogImuShort *pImuShort, const float *pArray, size_t arraySize);

/*!
 * Return from an IMU Short log, the temperature in °C
 *
 * \param[in]   pImuShort                   Input IMU short message instance.
 * \return                                  The converted temperature in °C
 */
float sbgEComLogImuShortGetTemperature(const SbgEComLogImuShort *pImuShort);

/*!
 * Set the temperature.
 *
 * \param[in]   pImuShort                   Input IMU short message instance.
 * \param[in]   temperature                 Temperature value to set, in °C.
 */
void sbgEComLogImuShortSetTemperature(SbgEComLogImuShort *pImuShort, float temperature);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogImuLegacy      SbgLogImuData);
SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogImuShort       SbgLogImuShort);
SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogImuFastLegacy  SbgLogFastImuData);

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
