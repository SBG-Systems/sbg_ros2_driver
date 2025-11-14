
/*!
 * \file            jsonHelper.h
 * \ingroup         main
 * \author          SBG Systems
 * \date            2024-11-14
 *
 * \brief           Provides simple and naive JSON parsing helpers.
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

#ifndef SBG_JSON_HELPER_H
#define SBG_JSON_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

// sbgCommonLib headers
#include <sbgCommon.h>

// Local headers
#include "cJSON.h"

//----------------------------------------------------------------------//
//- Public definition                                                  -//
//----------------------------------------------------------------------//

/*!
 * Store device information as returned by the sbgInsRestApi 'api/v1/info' endpoint.
 */
typedef struct _DeviceInfo
{
    char        productCode[32];
    char        serialNumber[32];
    char        hwRevision[32];
    char        mnfVersion[32];
    char        fmwVersion[32];
    char        btVersion[32];
} DeviceInfo;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Parse a JSON node that contains the device information and return it.
 * 
 * \param[in]   pJsonNode                       The JSON node to parse the device information from.
 * \param[out]  pDeviceInfo                     Parsed device information.
 * \return                                      SBG_NO_ERROR if all fields have been parsed successfully.
 */
SbgErrorCode restApiHelperParseDeviceInfo(const cJSON *pJsonNode, DeviceInfo *pDeviceInfo);

/*!
 * Parse an error JSON payload to extract each field and print information.
 * 
 * \param[in]   pContent                    The JSON payload to parse
 */
SbgErrorCode restApiHelperPrintErrorDetails(const char *pContent);

/*!
 * Parse an error JSON payload to extract each field and print information.
 * 
 * \param[in]   pContent                    The JSON payload to parse
 */
SbgErrorCode restApiHelperParseSuccess(const char *pContent, bool *pNeedReboot);

#ifdef __cplusplus
}
#endif

#endif  // SBG_JSON_HELPER_H
