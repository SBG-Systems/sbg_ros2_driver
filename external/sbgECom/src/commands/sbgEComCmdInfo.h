/*!
 * \file            sbgEComCmdInfo.h
 * \ingroup         commands
 * \author          SBG Systems
 * \date            11 June 2014
 *
 * \brief           Commands used to query the device information.
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

#ifndef SBG_ECOM_CMD_INFO_H
#define SBG_ECOM_CMD_INFO_H

// sbgCommonLib headers
#include <sbgCommon.h>

// Project headers
#include <sbgECom.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Device info definitions                                            -//
//----------------------------------------------------------------------//

/* Misc */
#define SBG_ECOM_INFO_PRODUCT_CODE_LENGTH           (32)

//----------------------------------------------------------------------//
//- Device Info structure                                              -//
//----------------------------------------------------------------------//

/*!
 * Helper structure to retrieve device info.
 */
typedef struct _SbgEComDeviceInfo
{
    uint8_t     productCode[SBG_ECOM_INFO_PRODUCT_CODE_LENGTH]; /*!< Human readable Product Code. */
    uint32_t    serialNumber;                                   /*!< Device serial number */
    uint32_t    calibationRev;                                  /*!< Calibration data revision */
    uint16_t    calibrationYear;                                /*!< Device Calibration Year */
    uint8_t     calibrationMonth;                               /*!< Device Calibration Month */
    uint8_t     calibrationDay;                                 /*!< Device Calibration Day */
    uint32_t    hardwareRev;                                    /*!< Device hardware revision */
    uint32_t    firmwareRev;                                    /*!< Firmware revision */
} SbgEComDeviceInfo;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Retrieve the device information.
 * 
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pInfo                       A pointer to a structure to hold device information.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdGetInfo(SbgEComHandle *pHandle, SbgEComDeviceInfo *pInfo);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_CMD_INFO_H
