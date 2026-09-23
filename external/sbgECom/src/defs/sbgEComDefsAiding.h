/*!
 * \file            sbgEComDefsAiding.h
 * \ingroup         main
 * \author          SBG Systems
 * \date            October 6, 2025
 *
 * \brief           Common enumeration and definitions for aiding data
 *
 * \copyright       Copyright (C) 2007-2026, SBG Systems SAS. All rights reserved.
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

#ifndef SBG_ECOM_DEFS_AIDING_H
#define SBG_ECOM_DEFS_AIDING_H

// sbgCommonLib headers
#include <sbgCommon.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Constant definitions                                               -//
//----------------------------------------------------------------------//

#define SBG_ECOM_AIDING_TIME_TYPE_MASK              (0x0007u)                   /*!< Mask used to keep only the aiding time type part. */

//----------------------------------------------------------------------//
//- Enumeration definitions                                            -//
//----------------------------------------------------------------------//

/*!
 * Time types definitions.
 */
typedef enum _SbgEComAidingTimeType
{
    SBG_ECOM_AIDING_TIME_TYPE_TIMESTAMP         = 0,                            /*!< The timeStamp field represents the time since the INS power up, in us. */
    SBG_ECOM_AIDING_TIME_TYPE_DELAY             = 1,                            /*!< The timeStamp field represents a delay, in us. */
    SBG_ECOM_AIDING_TIME_TYPE_TIME_OF_WEEK      = 2                             /*!< The timeStamp field represents a GPS time of the week, in ms. */
} SbgEComAidingTimeType;

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_DEFS_AIDING_H
