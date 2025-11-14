/*!
 * \file            sbgEComLogUsbl.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            02 June 2014
 *
 * \brief           Parse received USBL position mesurements logs.
 *
 * USBL binary logs contains underwater positioning data of a USBL beacon.
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

#ifndef SBG_ECOM_LOG_USBL_H
#define SBG_ECOM_LOG_USBL_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Log USBL status definitions                                        -//
//----------------------------------------------------------------------//

/*!
 * USBL sensor status mask definitions
 */
#define SBG_ECOM_USBL_TIME_SYNC             (0x0001u << 0)          /*!< Set to 1 if the USBL sensor data is correctly time synchronized. */
#define SBG_ECOM_USBL_POSITION_VALID        (0x0001u << 1)          /*!< Set to 1 if the USBL data represents a valid 2D position. */
#define SBG_ECOM_USBL_DEPTH_VALID           (0x0001u << 2)          /*!< Set to 1 if the USBL data has a valid depth information. */

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * Log structure for USBL data.
 */
typedef struct _SbgEComLogUsbl
{
    uint32_t    timeStamp;              /*!< Time in us since the sensor power up. */
    uint16_t    status;                 /*!< USBL system status bitmask. */

    double      latitude;               /*!< Latitude in degrees, positive north. */
    double      longitude;              /*!< Longitude in degrees, positive east. */

    float       depth;                  /*!< Depth in meters below mean sea level (positive down). */

    float       latitudeAccuracy;       /*!< 1 sigma latitude accuracy in meters. */
    float       longitudeAccuracy;      /*!< 1 sigma longitude accuracy in meters. */
    float       depthAccuracy;          /*!< 1 sigma depth accuracy in meters. */
} SbgEComLogUsbl;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Parse data for the SBG_ECOM_LOG_USBL message and fill the corresponding structure.
 * 
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogUsblReadFromStream(SbgEComLogUsbl *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_USBL message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogUsblWriteToStream(const SbgEComLogUsbl *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogUsbl SbgLogUsblData);

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParseUsblData(SbgStreamBuffer *pStreamBuffer, SbgEComLogUsbl *pLogData));
SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogWriteUsblData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogUsbl *pLogData));

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_USBL_H
