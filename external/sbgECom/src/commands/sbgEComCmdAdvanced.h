/*!
 * \file            sbgEComCmdAdvanced.h
 * \ingroup         commands
 * \author          SBG Systems
 * \date            11 June 2014
 *
 * \brief           Advanced settings related commands.
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

#ifndef SBG_ECOM_CMD_ADVANCED_H
#define SBG_ECOM_CMD_ADVANCED_H

// sbgCommonLib headers
#include <sbgCommon.h>

// Project headers
#include <sbgECom.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Advanced definitions                                               -//
//----------------------------------------------------------------------//

/*!
 * List of available time reference source.
 */
typedef enum _SbgEComTimeReferenceSrc
{
    SBG_ECOM_TIME_REF_DISABLED = 0,             /*!< The device is running it's internal clock without any time reference. */
    SBG_ECOM_TIME_REF_SYNC_IN_A,                /*!< The main port sync in A is used as a time reference. */
    SBG_ECOM_TIME_REF_UTC_GPS_1                 /*!< The GPS 1 module is used to provide both time reference and UTC data. */
} SbgEComTimeReferenceSrc;

/*!
 * List all options for the GNSS bitmask options
 */
#define SBG_ECOM_GNSS_OPT_01            (uint32_t)(0x00000001 << 0)     /*!< Reserved advanced GNSS option for ELLIPSE-N/D v3 */

/*!
 * List all options for the NMEA bitmask options
 */
#define SBG_ECOM_NMEA_OPT_MODE_STD      (uint32_t)(0x00000001 << 0)     /*!< Output NMEA messages that complies with 82 chars limit */
#define SBG_ECOM_NMEA_OPT_FORCE_UTC     (uint32_t)(0x00000001 << 4)     /*!< Always output time in NMEA messages even if invalid */

//----------------------------------------------------------------------//
//- Advanced configurations                                            -//
//----------------------------------------------------------------------//

/*!
 * Structure containing all the info for advanced configuration.
 */
typedef struct _SbgEComAdvancedConf
{
    SbgEComTimeReferenceSrc timeReference;      /*!< Time reference source for clock alignment. */
    uint32_t                gnssOptions;        /*!< Advanced GNSS options - contact SBG Systems. */
    uint32_t                nmeaOptions;        /*!< Advanced NMEA output options. */
} SbgEComAdvancedConf;

/*!
 * Structure containing all validity thresholds (status outputs)
 * Setting these thresholds to 0.0 will keep default configuration
 */
typedef struct _SbgEComValidityThresholds
{
    float   positionThreshold;                  /*!< Norm of the position standard deviation threshold to raise position valid flag (m)*/
    float   velocityThreshold;                  /*!< Norm of the velocity standard deviation threshold to raise velocity valid flag  (m/s)*/
    float   attitudeThreshold;                  /*!< Max of the roll/pitch standard deviations threshold to raise attitude valid flag (rad) */
    float   headingThreshold;                   /*!< Heading standard deviations threshold to raise heading valid flag (rad) */
} SbgEComValidityThresholds;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Retrieve the advanced configurations.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pConf                       Pointer to a SbgEComAdvancedConf to contain the current configuration.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdAdvancedGetConf(SbgEComHandle *pHandle, SbgEComAdvancedConf *pConf);

/*!
 * Set the advanced configurations.
 * 
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pConf                       Pointer to a SbgEComAdvancedConf that contains the new configuration.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdAdvancedSetConf(SbgEComHandle *pHandle, const SbgEComAdvancedConf *pConf);

/*!
 * Retrieve the current validity thresholds
 * 
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pConf                       Pointer to a SbgEComValidityThresholds to contain the current configuration.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */

SbgErrorCode sbgEComCmdAdvancedGetThresholds(SbgEComHandle *pHandle, SbgEComValidityThresholds *pConf);

/*!
 * Set the validity thresholds
 * 
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pConf                       Pointer to a SbgEComValidityThresholds that contains the new configuration.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdAdvancedSetThresholds(SbgEComHandle *pHandle, const SbgEComValidityThresholds *pConf);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_CMD_ADVANCED_H
