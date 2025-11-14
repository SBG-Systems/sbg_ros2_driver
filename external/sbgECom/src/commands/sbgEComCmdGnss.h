/*!
 * \file            sbgEComCmdGnss.h
 * \ingroup         commands
 * \author          SBG Systems
 * \date            11 June 2014
 *
 * \brief           GNSS aiding module configuration commands.
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

#ifndef SBG_ECOM_CMD_GNSS_H
#define SBG_ECOM_CMD_GNSS_H

// sbgCommonLib headers
#include <sbgCommon.h>

// Project headers
#include <sbgECom.h>

// Local headers
#include "sbgEComCmdCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Public definitions                                                 -//
//----------------------------------------------------------------------//

/*!
 * This enum defines the different GNSS model IDs available in standard
 */
typedef enum _SbgEComGnssModelsStdIds
{
    SBG_ECOM_GNSS_MODEL_INTERNAL                    = 101,      /*!< Default internal GNSS for ELLIPSE-N and ELLIPSE-D */
    SBG_ECOM_GNSS_MODEL_NMEA                        = 102,      /*!< ELLIPSE-E to accept an external GNSS using NMEA protocol */
    SBG_ECOM_GNSS_MODEL_UBLOX_GPS_BEIDOU            = 103,      /*!< Only for ELLIPSE-N hardware 1 & 2 to select GPS+BEIDOU instead of the default GPS+GLONASS */
    SBG_ECOM_GNSS_MODEL_UBLOX_EXTERNAL              = 104,      /*!< ELLIPSE-E to accept an external Ublox GNSS (receive only - passive) */
    SBG_ECOM_GNSS_MODEL_RESERVED_01                 = 105,      /*!< Reserved, do not use */
    SBG_ECOM_GNSS_MODEL_NOVATEL_EXTERNAL            = 106,      /*!< ELLIPSE-E to accept an external Novatel GNSS (receive only - passive) */
    SBG_ECOM_GNSS_MODEL_RESERVED_02                 = 107,      /*!< Reserved, do not use */
    SBG_ECOM_GNSS_MODEL_RESERVED_03                 = 108,      /*!< Reserved, do not use */
    SBG_ECOM_GNSS_MODEL_SEPTENTRIO_EXTERNAL         = 109,      /*!< ELLIPSE-E to accept an external Septentrio GNSS(receive only - passive) */
    SBG_ECOM_GNSS_MODEL_RESERVED_04                 = 110       /*!< Reserved, do not use */
} SbgEComGnssModelsStdIds;

/*!
 * GNSS mechanical installation modes for dual antenna configurations.
 * 
 * \note In ELLIPSE firmware 3.x and above, lever arms are always treated as precise, with no online re-estimation performed.  
 * The mode `SBG_ECOM_GNSS_INSTALLATION_MODE_DUAL_ROUGH` is internally resolved as `SBG_ECOM_GNSS_INSTALLATION_MODE_DUAL_PRECISE`.
 */
typedef enum _SbgEComGnssInstallationMode
{
    SBG_ECOM_GNSS_INSTALLATION_MODE_SINGLE          = 1,        /*!< GNSS operates in single antenna mode; the secondary lever arm is not used. */
    SBG_ECOM_GNSS_INSTALLATION_MODE_DUAL_AUTO       = 2,        /*!< [Reserved] GNSS uses dual antenna data, but the secondary lever arm is not specified. */
    SBG_ECOM_GNSS_INSTALLATION_MODE_DUAL_ROUGH      = 3,        /*!< [Deprecated] GNSS uses dual antenna data with an approximate secondary lever arm position. */
    SBG_ECOM_GNSS_INSTALLATION_MODE_DUAL_PRECISE    = 4         /*!< GNSS uses dual antenna data with a precisely specified secondary lever arm; no online re-estimation needed. */
} SbgEComGnssInstallationMode;

/*!
 * GNSS mechanical installation parameters for use with the command `SBG_ECOM_CMD_GNSS_#_INSTALLATION`.
 * 
 * \note In ELLIPSE firmware 3.x and above, lever arms are always treated as precise, with no online re-estimation performed.  
 * The `leverArmPrimaryPrecise` field is deprecated and implicitly considered true.
 */
typedef struct _SbgEComGnssInstallation
{
    float                       leverArmPrimary[3];             /*!< Primary GNSS antenna lever arm in IMU X, Y, Z axes (meters). */
    bool                        leverArmPrimaryPrecise;         /*!< [Deprecated] Indicates whether the primary lever arm is accurately specified; online re-estimation is not required. */

    float                       leverArmSecondary[3];           /*!< Secondary GNSS antenna lever arm in IMU X, Y, Z axes (meters). */
    SbgEComGnssInstallationMode leverArmSecondaryMode;          /*!< Defines the operating mode for the secondary (dual) antenna. */
} SbgEComGnssInstallation;

/*!
 * Configuration settings for GNSS module data rejection.
 */
typedef struct _SbgEComGnssRejectionConf
{
    SbgEComRejectionMode        position;                       /*!< Rejection mode for GNSS position data. */
    SbgEComRejectionMode        velocity;                       /*!< Rejection mode for GNSS velocity data. */
    SbgEComRejectionMode        hdt;                            /*!< Rejection mode for true heading (HDT). */
} SbgEComGnssRejectionConf;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Set GNSS error model id.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   modelId                     Model ID to set
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdGnss1SetModelId(SbgEComHandle *pHandle, SbgEComGnssModelsStdIds modelId);

/*!
 * Retrieve GNSS error model id.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pModelId                    Retrieved model id.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdGnss1GetModelId(SbgEComHandle *pHandle, SbgEComGnssModelsStdIds *pModelId);

/*!
 * Retrieve the mechanical installation parameters for the GNSS 1 module.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pGnssInstallation           Used to store the retrieved the GNSS installation parameters.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdGnss1InstallationGet(SbgEComHandle *pHandle, SbgEComGnssInstallation *pGnssInstallation);

/*!
 * Set the mechanical installation parameters for the GNSS 1 module.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pGnssInstallation               The GNSS installation parameters to set.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdGnss1InstallationSet(SbgEComHandle *pHandle, const SbgEComGnssInstallation *pGnssInstallation);

/*!
 * Retrieve the rejection configuration of the gnss module.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pRejectConf                 Pointer to a SbgEComGnssRejectionConf struct to hold rejection configuration of the gnss module.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdGnss1GetRejection(SbgEComHandle *pHandle, SbgEComGnssRejectionConf *pRejectConf);

/*!
 * Set the rejection configuration of the gnss module.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pRejectConf                 Pointer to a SbgEComGnssRejectionConf struct holding rejection configuration for the gnss module.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdGnss1SetRejection(SbgEComHandle *pHandle, const SbgEComGnssRejectionConf *pRejectConf);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_CMD_GNSS_H
