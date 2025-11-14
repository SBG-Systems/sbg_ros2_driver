/*!
 * \file            sbgEComLogEkfRotAccel.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            12 June 2023
 *
 * \brief           Write and parse SBG_ECOM_LOG_EKF_ROT_ACCEL_XXXX messages.
 * 
 * The SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY and SBG_ECOM_LOG_EKF_ROT_ACCEL_NED logs
 * returns compensated INS rotation rate and lateral accelerations.
 * 
 * The measurements are either expressed in body or NED frames. Please double 
 * check if you need body or NED outputs are they are very different.
 *
 * For example, a rotation along the DOWN axis represents how fast the INS heading
 * is changing whatever the roll/pitch is.
 * 
 * A rotation along the Z axis is simply the gyroscope Z reading corrected for
 * bias, scale factor errors and earth rotation.
 * 
 * The rate are corrected for sensor bias, scale factor error and earth rotation
 * has also been removed. In short, it should returns zero if the INS is
 * perfectly still.
 * 
 * The lateral accelerations are corrected for sensor bias, scale factor and the
 * local earth gravity has been removed. It should returns zero if the INS is not
 * moving at all.
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

#ifndef SBG_ECOM_LOG_EKF_ROT_ACCEL_H
#define SBG_ECOM_LOG_EKF_ROT_ACCEL_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Public definitions                                                 -//
//----------------------------------------------------------------------//

/*!
 * INS compensated body or North, East, Down rotation rates and linear accelerations.
 */
typedef struct _SbgEComLogEkfRotAccel
{
    uint32_t    timeStamp;              /*!< Time in us since the sensor power up. */
    uint32_t    status;                 /*!< Status - reserved for future use. */
    float       rate[3];                /*!< X,Y,Z or North/East/Down INS rotation rate free from earth rotation and sensor bias/scale errors (rad.s^-1). */
    float       acceleration[3];        /*!< X,Y,Z or North/East/Down INS accelerations and free from gravity and sensor bias/scale errors (m.s^-2). */
} SbgEComLogEkfRotAccel;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Construct an empty / zero initialized instance.
 *
 * \param[in]   pLogData                    Log instance.
 */
void sbgEComLogEkfRotAccelConstruct(SbgEComLogEkfRotAccel *pLogData);
 
/*!
 * Read SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY or SBG_ECOM_LOG_EKF_ROT_ACCEL_NED messages from a stream buffer.
 * 
 * \param[out]  pLogData                    Log instance.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogEkfRotAccelReadFromStream(SbgEComLogEkfRotAccel *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY or SBG_ECOM_LOG_EKF_ROT_ACCEL_NED messages to a stream buffer.
 *
 * \param[in]   pLogData                    Log instance.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogEkfRotAccelWriteToStream(const SbgEComLogEkfRotAccel *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_EKF_ROT_ACCEL_H
