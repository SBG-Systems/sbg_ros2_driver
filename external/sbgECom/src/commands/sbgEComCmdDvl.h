/*!
 * \file            sbgEComCmdDvl.h
 * \ingroup         commands
 * \author          SBG Systems
 * \date            13 December 2018
 *
 * \brief           DVL (Doppler Velocity Logger) aiding module configuration commands.
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

#ifndef SBG_ECOM_CMD_DVL_H
#define SBG_ECOM_CMD_DVL_H

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
 * This enum defines the different DVL model IDs available in standard
 */
typedef enum _SbgEComDvlModelsIds
{
    SBG_ECOM_DVL_MODEL_GENERIC_PD6  = 202,          /*!< Generic DVL using PD6 protocol format. */
    SBG_ECOM_DVL_MODEL_WAYFINDER    = 203,          /*!< Teledyne Wayfinder DVL using proprietary protocol. */
    SBG_ECOM_DVL_MODEL_NORTEK       = 204,          /*!< Nortek DVL using proprietary protocol. */
} SbgEComDvlModelsIds;

/*!
 * DVL (Doppler Velocity Log) mechanical installation parameters, including lever arm and alignment.
 * 
 * \note In ELLIPSE firmware 3.x and above, lever arms are always treated as precise, with no online re-estimation performed.  
 * The `preciseInstallation` field is deprecated and implicitly considered true.
 */
typedef struct _SbgEComDvlInstallation
{
    float                   leverArm[3];            /*!< DVL lever arm (X, Y, Z) in meters, expressed from the DVL to the IMU. */
    float                   alignment[3];           /*!< DVL alignment (roll, pitch, yaw) in radians. */
    bool                    preciseInstallation;    /*!< [DEPRECATED] Indicates whether both the DVL lever arm and alignment are precise and do not require in-run estimation. */
} SbgEComDvlInstallation;

/*!
 * Contains configuration settings for DVL module data rejection.
 */
typedef struct _SbgEComDvlRejectionConf
{
    SbgEComRejectionMode    bottomLayer;            /*!< Rejection mode for the bottom tracking (ie when the velocity measurement is in respect to the seabed). */
    SbgEComRejectionMode    waterLayer;             /*!< Rejection mode for the water tracking (ie when the velocity measurement is relative to a water layer). */
} SbgEComDvlRejectionConf;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Set the DVL model to use that both defines the protocol as well as the associated error model.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   modelId                     DVL model ID to set
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdDvlSetModelId(SbgEComHandle *pHandle, SbgEComDvlModelsIds modelId);

/*!
 * Retrieve the DVL model id currently in use by the device.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pModelId                    Returns the DVL model ID currently in use by the device.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdDvlGetModelId(SbgEComHandle *pHandle, SbgEComDvlModelsIds *pModelId);

/*!
 * Set the lever arm and alignment configuration of the DVL module.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pDvlInstallation            The DVL lever arm and alignment configuration to apply.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdDvlInstallationSet(SbgEComHandle *pHandle, const SbgEComDvlInstallation *pDvlInstallation);

/*!
 * Retrieve the lever arm and alignment configuration of the DVL module.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pDvlInstallation            Returns the DVL lever arm and alignment configuration currently in use.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdDvlInstallationGet(SbgEComHandle *pHandle, SbgEComDvlInstallation *pDvlInstallation);

/*!
 * Set the rejection configuration of the DVL module (this command doesn't need a reboot to be applied)
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pRejectConf                 The new DVL rejection configuration to set.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdDvlSetRejection(SbgEComHandle *pHandle, const SbgEComDvlRejectionConf *pRejectConf);

/*!
 * Retrieve the current rejection configuration of the DVL module.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pRejectConf                 Return the DVL rejection configuration currently in use.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdDvlGetRejection(SbgEComHandle *pHandle, SbgEComDvlRejectionConf *pRejectConf);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_CMD_DVL_H
