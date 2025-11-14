/*!
 * \file            sbgEComLogGnssVel.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            09 May 2023
 *
 * \brief           GNSS velocity logs.
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

#ifndef SBG_ECOM_LOG_GNSS_VEL_H
#define SBG_ECOM_LOG_GNSS_VEL_H

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
 * GNSS velocity status definitions.
 */
typedef enum _SbgEComGnssVelStatus
{
    SBG_ECOM_GNSS_VEL_STATUS_SOL_COMPUTED       = 0,                            /*!< A valid solution has been computed. */
    SBG_ECOM_GNSS_VEL_STATUS_INSUFFICIENT_OBS   = 1,                            /*!< Not enough valid SV to compute a solution. */
    SBG_ECOM_GNSS_VEL_STATUS_INTERNAL_ERROR     = 2,                            /*!< An internal error has occurred. */
    SBG_ECOM_GNSS_VEL_STATUS_LIMIT              = 3                             /*!< Velocity limit exceeded. */
} SbgEComGnssVelStatus;

/*!
 * GNSS velocity types definitions.
 */
typedef enum _SbgEComGnssVelType
{
    SBG_ECOM_GNSS_VEL_TYPE_NO_SOLUTION          = 0,                            /*!< No valid velocity solution available. */
    SBG_ECOM_GNSS_VEL_TYPE_UNKNOWN              = 1,                            /*!< An unknown solution type has been computed. */
    SBG_ECOM_GNSS_VEL_TYPE_DOPPLER              = 2,                            /*!< A Doppler velocity has been computed. */
    SBG_ECOM_GNSS_VEL_TYPE_DIFFERENTIAL         = 3                             /*!< A differential velocity has been computed between two positions. */
} SbgEComGnssVelType;

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * \brief Structure that stores data for the SBG_ECOM_LOG_GPS#_VEL message.
 * 
 * This structure holds information about GPS velocity, including timestamp,
 * status, time of week, velocity components, and their accuracies.
 *
 * \note Some GNSS protocols, such as NMEA, cannot provide a full 3D velocity.
 * An invalid down velocity component should be indicated by using the maximum
 * standard deviation value of 9999 m/s.
 */
typedef struct _SbgEComLogGnssVel
{
    uint32_t        timeStamp;              /*!< Time in microseconds since the sensor power up. */
    uint32_t        status;                 /*!< GPS velocity status, type and bitmask. */
    uint32_t        timeOfWeek;             /*!< GPS time of week in milliseconds. */
    float           velocity[3];            /*!< GPS North, East, Down velocity in m/s. */
    float           velocityAcc[3];         /*!< GPS North, East, Down velocity 1 sigma accuracy in m/s (0 to 9999). */
    float           course;                 /*!< Track ground course in degrees (0 to 360). */
    float           courseAcc;              /*!< Course accuracy in degrees (0 to 180). */
} SbgEComLogGnssVel;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Zero initialize the message struct.
 * 
 * \param[out]  pLogData                    Structure instance to zero init.
 */
void sbgEComLogGnssVelZeroInit(SbgEComLogGnssVel *pLogData);

/*!
 * Parse data for the SBG_ECOM_LOG_GPS#_VEL message and fill the corresponding structure.
 * 
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogGnssVelReadFromStream(SbgEComLogGnssVel *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_GPS#_VEL message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogGnssVelWriteToStream(const SbgEComLogGnssVel *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

/*!
 * Set the GNSS position solution status.
 * 
 * \param[in]   pLogData            Log instance.
 * \param[in]   status              The solution status to set.
 */
void sbgEComLogGnssVelSetStatus(SbgEComLogGnssVel *pLogData, SbgEComGnssVelStatus status);

/*!
 * Returns the GNSS position solution status.
 * 
 * \param[in]   pLogData            Log instance.
 * \return                          The solution status.
 */
SbgEComGnssVelStatus sbgEComLogGnssVelGetStatus(const SbgEComLogGnssVel *pLogData);

/*!
 * Set the GNSS position solution type.
 * 
 * \param[in]   pLogData            Log instance.
 * \param[in]   posType             The solution type to set.
 */
void sbgEComLogGnssVelSetType(SbgEComLogGnssVel *pLogData, SbgEComGnssVelType posType);

/*!
 * Returns the GNSS position solution type.
 * 
 * \param[in]   pLogData            Log instance.
 * \return                          The solution type.
 */
SbgEComGnssVelType sbgEComLogGnssVelGetType(const SbgEComLogGnssVel *pLogData);

/*!
 * Returns true if the velocity down component is available/valid.
 *
 * Some GNSS receivers such as the ones using NMEA protocols can't provide down velocity.
 * This methods checks the velocity standard deviation down component.
 * 
 * \param[in]   pLogData            Log instance.
 * \return                          true if the velocity down component is available/valid.
 */
bool sbgEComLogGnssVelDownVelocityIsValid(const SbgEComLogGnssVel *pLogData);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

#ifdef SBG_ECOM_USE_DEPRECATED_MACROS
    #define SBG_ECOM_VEL_SOL_COMPUTED           (SBG_ECOM_GNSS_VEL_STATUS_SOL_COMPUTED)
    #define SBG_ECOM_VEL_INSUFFICIENT_OBS       (SBG_ECOM_GNSS_VEL_STATUS_INSUFFICIENT_OBS)
    #define SBG_ECOM_VEL_INTERNAL_ERROR         (SBG_ECOM_GNSS_VEL_STATUS_INTERNAL_ERROR)
    #define SBG_ECOM_VEL_LIMIT                  (SBG_ECOM_GNSS_VEL_STATUS_LIMIT)
        
    #define SBG_ECOM_VEL_NO_SOLUTION            (SBG_ECOM_GNSS_VEL_TYPE_NO_SOLUTION)
    #define SBG_ECOM_VEL_UNKNOWN_TYPE           (SBG_ECOM_GNSS_VEL_TYPE_UNKNOWN)
    #define SBG_ECOM_VEL_DOPPLER                (SBG_ECOM_GNSS_VEL_TYPE_DOPPLER)
    #define SBG_ECOM_VEL_DIFFERENTIAL           (SBG_ECOM_GNSS_VEL_TYPE_DIFFERENTIAL)
#endif

SBG_DEPRECATED_TYPEDEF(typedef enum _SbgEComGnssVelStatus   SbgEComGpsVelStatus);
SBG_DEPRECATED_TYPEDEF(typedef enum _SbgEComGnssVelType     SbgEComGpsVelType);
SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogGnssVel    SbgLogGpsVel);

SBG_DEPRECATED(uint32_t sbgEComLogGpsVelBuildStatus(SbgEComGnssVelStatus status, SbgEComGnssVelType type));
SBG_DEPRECATED(SbgEComGnssVelStatus sbgEComLogGpsVelGetStatus(uint32_t status));
SBG_DEPRECATED(SbgEComGnssVelType sbgEComLogGpsVelGetType(uint32_t status));

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParseGpsVelData(SbgStreamBuffer *pStreamBuffer, SbgEComLogGnssVel *pLogData));
SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogWriteGpsVelData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogGnssVel *pLogData));

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_GNSS_VEL_H
