/*!
 * \file            sbgEComCmdAirData.h
 * \ingroup         commands
 * \author          SBG Systems
 * \date            18 February 2019
 *
 * \brief           AirData aiding module configuration commands.
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

#ifndef SBG_ECOM_CMD_AIR_DATA_H
#define SBG_ECOM_CMD_AIR_DATA_H

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
 * This enum defines the different AirData model IDs available in standard
 */
typedef enum _SbgEComAirDataModelsIds
{
    SBG_ECOM_AIR_DATA_MODEL_INTERNAL        = 1,        /*!< Use the internal barometer sensor if available. */
    SBG_ECOM_AIR_DATA_MODEL_GENERIC_ECOM    = 2,        /*!< Generic AirData model using sbgECom input protocol format. */
    SBG_ECOM_AIR_DATA_MODEL_AHRS_500        = 3,        /*!< Crossbow AHRS-500 compatible input for barometric altitude and airspeed. */
} SbgEComAirDataModelsIds;

/*!
 * Holds all necessary information for AirData module data rejection.
 */
typedef struct _SbgEComAirDataRejectionConf
{
    SbgEComRejectionMode    airspeed;                   /*!< Rejection mode for the true air speed measurement. */
    SbgEComRejectionMode    altitude;                   /*!< Rejection mode for the barometric altitude measurement. */
} SbgEComAirDataRejectionConf;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Set the AirData model to use that both defines the protocol as well as the associated error model.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   modelId                     AirData model ID to set
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdAirDataSetModelId(SbgEComHandle *pHandle, SbgEComAirDataModelsIds modelId);

/*!
 * Retrieve the AirData model id currently in use by the device.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pModelId                    Returns the AirData model ID currently in use by the device.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdAirDataGetModelId(SbgEComHandle *pHandle, SbgEComAirDataModelsIds *pModelId);

/*!
 * Set the lever arm configuration of the AirData module.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pLeverArm                   The X, Y, Z airspeed sensor lever arm in meters from the pitot sensor to the IMU.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdAirDataSetLeverArm(SbgEComHandle *pHandle, const float *pLeverArm);

/*!
 * Retrieve the lever arm configuration of the AirData module.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pLeverArm                   Returns the airspeed sensor X,Y,Z lever arm in meters from the pitot sensor to the IMU.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdAirDataGetLeverArm(SbgEComHandle *pHandle, float *pLeverArm);

/*!
 * Set the rejection configuration of the AirData module (this command doesn't need a reboot to be applied)
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pRejectConf                 The new rejection configuration to set.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdAirDataSetRejection(SbgEComHandle *pHandle, const SbgEComAirDataRejectionConf *pRejectConf);

/*!
 * Retrieve the current rejection configuration of the AirData module.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pRejectConf                 Return the rejection configuration currently in use.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdAirDataGetRejection(SbgEComHandle *pHandle, SbgEComAirDataRejectionConf *pRejectConf);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_CMD_AIR_DATA_H
