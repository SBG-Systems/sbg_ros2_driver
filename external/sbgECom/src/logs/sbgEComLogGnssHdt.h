/*!
 * \file            sbgEComLogGnssHdt.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            05 May 2023
 *
 * \brief           GNSS True Heading related logs.
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

#ifndef SBG_ECOM_LOG_GNSS_HDT_H
#define SBG_ECOM_LOG_GNSS_HDT_H

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
 * GNSS HDT status definitions.
 */
typedef enum _SbgEComGnssHdtStatus
{
    SBG_ECOM_GNSS_HDT_STATUS_SOL_COMPUTED       = 0,                            /*!< A valid solution has been computed. */
    SBG_ECOM_GNSS_HDT_STATUS_INSUFFICIENT_OBS   = 1,                            /*!< Not enough valid SV to compute a solution. */
    SBG_ECOM_GNSS_HDT_STATUS_INTERNAL_ERROR     = 2,                            /*!< An internal error has occurred. */
    SBG_ECOM_GNSS_HDT_STATUS_HEIGHT_LIMIT       = 3                             /*!< The height limit has been exceeded. */
} SbgEComGnssHdtStatus;

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * Structure that stores data for the SBG_ECOM_LOG_GPS#_HDT message.
 */
typedef struct _SbgEComLogGnssHdt
{
    uint32_t        timeStamp;              /*!< Time in us since the sensor power up. */
    uint16_t        status;                 /*!< GPS HDT status, type and bitmask. */
    uint32_t        timeOfWeek;             /*!< GPS time of week in ms. */
    float           heading;                /*!< GPS true heading in degrees (0 to 360). */
    float           headingAccuracy;        /*!< 1 sigma GPS true heading accuracy in degrees (0 to 180). */
    float           pitch;                  /*!< GPS pitch angle measured from the master to the rover in degrees (-90 to +90). */
    float           pitchAccuracy;          /*!< 1 sigma GPS pitch angle accuracy in degrees (0 to 90). */
    float           baseline;               /*!< The distance between the main and aux antenna in meters (added in 2.0) */
    uint8_t         numSvTracked;           /*!< Number of space vehicles tracked by the secondary GNSS antenna - set to 0xFF if not available. (added in 4.0) */
    uint8_t         numSvUsed;              /*!< Number of space vehicles used to compute the dual antenna solution - set to 0xFF if not available. (added in 4.0) */
} SbgEComLogGnssHdt;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Zero initialize the message struct.
 * 
 * \param[out]  pLogData                    Structure instance to zero init.
 */
void sbgEComLogGnssHdtZeroInit(SbgEComLogGnssHdt *pLogData);

/*!
 * Parse data for the SBG_ECOM_LOG_GPS#_HDT message and fill the corresponding structure.
 * 
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogGnssHdtReadFromStream(SbgEComLogGnssHdt *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_GPS#_HDT message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogGnssHdtWriteToStream(const SbgEComLogGnssHdt *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

/*!
 * Set the true heading solution status.
 * 
 * \param[in]   pLogData            Log instance.
 * \param[in]   status              The solution status to set.
 */
void sbgEComLogGnssHdtSetStatus(SbgEComLogGnssHdt *pLogData, SbgEComGnssHdtStatus status);

/*!
 * Returns the true heading solution status.
 * 
 * \param[in]   pLogData            Log instance.
 * \return                          The true heading solution status.
 */
SbgEComGnssHdtStatus sbgEComLogGnssHdtGetStatus(const SbgEComLogGnssHdt *pLogData);

/*!
 * Returns true if the heading information is valid.
 *
 * \param[in]   pLogData            Log instance.
 * \return                          true if the heading information is valid or false otherwise.
 */
bool sbgEComLogGnssHdtHeadingIsValid(const SbgEComLogGnssHdt *pLogData);

/*!
 * Returns true if the pitch information is valid.
 * 
 * Some GNSS receivers don't provide pitch information between antenna 1 and 2.
 * This methods checks if a valid pitch measurement is available with
 * an associated standard deviation.
 *
 * \param[in]   pLogData            Log instance.
 * \return                          true if the pitch information is valid or false otherwise.
 */
bool sbgEComLogGnssHdtPitchIsValid(const SbgEComLogGnssHdt *pLogData);

/*!
 * Set if the baseline field is provided with a valid value.
 * 
 * Some GNSS receivers don't provide the baseline information between antenna 1 and 2.
 * Set this flag to false to report this.
 * 
 * \param[in]   pLogData            Log instance.
 * \param[in]   isValid             true if the baseline is valid.
 */
void sbgEComLogGnssHdtSetBaseLineValid(SbgEComLogGnssHdt *pLogData, bool isValid);

/*!
 * Returns true if the baseline field is valid. 
 *
 * \param[in]   pLogData            Log instance.
 * \return                          true if the baseline field is valid or false otherwise.
 */
bool sbgEComLogGnssHdtBaseLineIsValid(const SbgEComLogGnssHdt *pLogData);

/*!
 * Returns true if the numSvTracked field is valid.
 *
 * \param[in]   pLogData            Log instance.
 * \return                          true if the numSvTracked field is valid or false otherwise.
 */
bool sbgEComLogGnssHdtNumSvTrackedIsValid(const SbgEComLogGnssHdt *pLogData);

/*!
 * Returns true if the numSvUsed field is valid.
 *
 * \param[in]   pLogData            Log instance.
 * \return                          true if the numSvUsed field is valid or false otherwise.
 */
bool sbgEComLogGnssHdtNumSvUsedIsValid(const SbgEComLogGnssHdt *pLogData);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

#ifdef SBG_ECOM_USE_DEPRECATED_MACROS
    #define SBG_ECOM_GPS_HDT_BASELINE_VALID (0x0001 << 6)

    #define SBG_ECOM_HDT_SOL_COMPUTED           (SBG_ECOM_GNSS_HDT_STATUS_SOL_COMPUTED)
    #define SBG_ECOM_HDT_INSUFFICIENT_OBS       (SBG_ECOM_GNSS_HDT_STATUS_INSUFFICIENT_OBS)
    #define SBG_ECOM_HDT_INTERNAL_ERROR         (SBG_ECOM_GNSS_HDT_STATUS_INTERNAL_ERROR)
    #define SBG_ECOM_HDT_HEIGHT_LIMIT           (SBG_ECOM_GNSS_HDT_STATUS_HEIGHT_LIMIT)
#endif

SBG_DEPRECATED_TYPEDEF(typedef enum _SbgEComGnssHdtStatus   SbgEComGpsHdtStatus);
SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogGnssHdt    SbgLogGpsHdt);

SBG_DEPRECATED(uint32_t sbgEComLogGpsHdtBuildStatus(SbgEComGnssHdtStatus status, uint32_t masks));
SBG_DEPRECATED(SbgEComGnssHdtStatus sbgEComLogGpsHdtGetStatus(uint32_t status));

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParseGpsHdtData(SbgStreamBuffer *pStreamBuffer, SbgEComLogGnssHdt *pLogData));
SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogWriteGpsHdtData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogGnssHdt *pLogData));

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_GNSS_HDT_H
