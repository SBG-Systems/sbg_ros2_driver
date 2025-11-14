/*!
 * \file            sbgEComCmdLicense.h
 * \ingroup         commands
 * \author          SBG Systems
 * \date            25 February 2015
 *
 * \brief           Command used to upload and apply an activation license.
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

#ifndef SBG_ECOM_CMD_LICENSE_H
#define SBG_ECOM_CMD_LICENSE_H

// sbgCommonLib headers
#include <sbgCommon.h>

// Project headers
#include <sbgECom.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Upload and apply a new license to a device.
 * 
 * The device will reboot automatically to use the new license.
 * 
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pBuffer                     Read only buffer containing the license.
 * \param[in]   size                        Size of the buffer.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdLicenseApply(SbgEComHandle *pHandle, const void *pBuffer, size_t size);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_CMD_LICENSE_H
