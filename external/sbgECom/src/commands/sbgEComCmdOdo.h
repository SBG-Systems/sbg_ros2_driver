/*!
 * \file            sbgEComCmdOdo.h
 * \ingroup         commands
 * \author          SBG Systems
 * \date            11 June 2014
 *
 * \brief           Odometer / DMI aiding module configuration commands.
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

#ifndef SBG_ECOM_CMD_ODO_H
#define SBG_ECOM_CMD_ODO_H

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
 * Holds all necessary information for Odometer module parameter configuration.
 */
typedef struct _SbgEComOdoConf
{
    float       gain;                       /*!< Odometer's gain in pulses / meter. */
    uint8_t     gainError;                  /*!< User gain average error in % */
    bool        reverseMode;                /*!< Whether the odometer is in reverse mode or not. */
} SbgEComOdoConf;

/*!
 * Holds all necessary information for Odometer module data rejection.
 */
typedef struct _SbgEComOdoRejectionConf
{
    SbgEComRejectionMode    velocity;       /*!< Rejection mode for velocity. */
} SbgEComOdoRejectionConf;

/*!
 * CAN odometer channels definition
 * A channel is an information that can be decoded / used by the device.
 */
typedef enum _SbgEComCmdOdoCanChannel
{
    SBG_ECOM_CMD_ODO_CAN_CH_VELOCITY    = 0,                            /*!< Channel used to decode the vehicle velocity information */
    SBG_ECOM_CMD_ODO_CAN_CH_REVERSE     = 1                             /*!< Channel used to decode the vehicle velocity reverse info (if available). */
} SbgEComCmdOdoCanChannel;

/*
 * Define CAN odometer options bitmask
 */
#define SBG_ECOM_CMD_ODO_CAN_ENABLE             (uint16_t)(0x0001 << 0) /*!< Set to enable CAN odometer information decoding. */
#define SBG_ECOM_CMD_ODO_CAN_ID_EXTENDED        (uint16_t)(0x0001 << 1) /*!< Set for a 29 bit extended CAN message, otherwise standard 11 bit */
#define SBG_ECOM_CMD_ODO_CAN_BIG_ENDIAN         (uint16_t)(0x0001 << 2) /*!< Set if the velocity is encoded in big endian, otherwise little endian */
#define SBG_ECOM_CMD_ODO_CAN_SIGNED             (uint16_t)(0x0001 << 3) /*!< Set to interpret the parsed value as signed, otherwise unsigned. */

/*!
 * Holds all necessary information for CAN Odometer parameter configuration.
 * This format is very similar to info contained in a DBC file.
 */
typedef struct _SbgEComCmdOdoCanConf
{
    uint16_t    options;                /*!< Set of options bit masks such as CAN extended. */
    uint32_t    canId;                  /*!< CAN message ID from which the odometer velocity will be parsed. */

    size_t      startBit;               /*!< Index of field MSB in big endian or LSB in little endian within the payload (any value from 0 to 63). */
    size_t      dataSize;               /*!< Length in bits of the odometer velocity field (any value from 1 to 64 minus dataOffset). */

    float       scale;                  /*!< Value to multiply the parsed field with to get physical unit^in m.s-1. */
    float       offset;                 /*!< Offset to add on the scaled velocity information in m.s-1 (after applying scale factor). */
    float       minValue;               /*!< The minimum velocity to consider the message valid in m.s-1 */
    float       maxValue;               /*!< The maximum velocity to consider the message valid in m.s-1 */
} SbgEComCmdOdoCanConf;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * For quadrature and/or pulse based odometer, retrieve the configuration.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pOdometerConf               Pointer to a SbgEComOdoConf struct to hold configuration of the odometer module.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdOdoGetConf(SbgEComHandle *pHandle, SbgEComOdoConf *pOdometerConf);

/*!
 * For quadrature and/or pulse base odometer, define the configuration.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pOdometerConf               Pointer to a SbgEComOdoConf struct holding configuration for the odometer module.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdOdoSetConf(SbgEComHandle *pHandle, const SbgEComOdoConf *pOdometerConf);

/*!
 * Retrieve the lever arm applicable for both quadrature or CAN based odometer.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pLeverArm                   Array of three values, one for each axis.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdOdoGetLeverArm(SbgEComHandle *pHandle, float *pLeverArm);

/*!
 * Set the lever arm applicable for both quadrature or CAN based odometer.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pLeverArm                   Array of three values, one for each axis.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdOdoSetLeverArm(SbgEComHandle *pHandle, const float *pLeverArm);

/*!
 * Retrieve the velocity rejection configuration for both quadrature or CAN based odometer.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pRejectConf                 Pointer to a SbgEComOdoRejectionConf struct to hold rejection configuration of the odometer module.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdOdoGetRejection(SbgEComHandle *pHandle, SbgEComOdoRejectionConf *pRejectConf);

/*!
 * Set the velocity rejection configuration for both quadrature or CAN based odometer.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pRejectConf                 Pointer to a SbgEComOdoRejectionConf struct holding rejection configuration for the odometer module.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdOdoSetRejection(SbgEComHandle *pHandle, const SbgEComOdoRejectionConf *pRejectConf);

/*!
 * Retrieve the CAN odometer configuration for a specific CAN information channel
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   canChannel                  The CAN channel to retrieve associated DBC configuration.
 * \param[out]  pOdoCanConf                 Struct to hold configuration of the CAN odometer.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdOdoCanGetConf(SbgEComHandle *pHandle, SbgEComCmdOdoCanChannel canChannel, SbgEComCmdOdoCanConf *pOdoCanConf);

/*!
 * Set the CAN odometer configuration for a specific CAN information channel
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   canChannel                  The CAN channel to define associated DBC configuration.
 * \param[in]   pOdoCanConf                 Struct holding configuration for the CAN odometer.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdOdoCanSetConf(SbgEComHandle *pHandle, SbgEComCmdOdoCanChannel canChannel, const SbgEComCmdOdoCanConf *pOdoCanConf);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_CMD_ODO_H
