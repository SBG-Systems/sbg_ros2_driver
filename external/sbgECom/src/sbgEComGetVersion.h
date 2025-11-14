/*!
 * \file            sbgEComGetVersion.h
 * \ingroup         main
 * \author          SBG Systems
 * \date            05 February 2013
 * \brief           Version information.
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

#ifndef SBG_ECOM_GET_VERSION_H
#define SBG_ECOM_GET_VERSION_H

// sbgCommonLib headers
#include <sbgCommon.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Returns an integer representing the version of the sbgECom library.
 *
 * \return                                      An integer representing the version of the sbgECom library.
 */
uint32_t sbgEComGetVersion(void);

/*!
 * Retrieve the sbgECom library version as a string (1.0.443-stable).
 *
 * \return                                      Null terminated string that contains the sbgECom library version.
 */
const char *sbgEComGetVersionAsString(void);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_GET_VERSION_H
