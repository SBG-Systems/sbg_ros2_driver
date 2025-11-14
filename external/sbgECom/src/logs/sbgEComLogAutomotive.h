/*!
 * \file            sbgEComLogAutomotive.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            22 April 2020
 *
 * \brief           Parse dedicated automotive measurements logs.
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

#ifndef SBG_ECOM_LOG_AUTOMOTIVE_H
#define SBG_ECOM_LOG_AUTOMOTIVE_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Constant definitions                                               -//
//----------------------------------------------------------------------//

/*!
 * Automotive data status mask definitions
 */
#define SBG_ECOM_AUTO_DATA_TRACK_VALID                  (0x1u << 0)     /*!< Set to 1 if the track angle is valid. */
#define SBG_ECOM_AUTO_DATA_SLIP_VALID                   (0x1u << 1)     /*!< Set to 1 if the slip angle is valid. */
#define SBG_ECOM_AUTO_DATA_CURVATURE_VALID              (0x1u << 2)     /*!< Set to 1 if the curvature radius is valid. */

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * Log structure for automotive data.
 */
typedef struct _SbgEComLogAutomotive
{
    uint8_t              status;                    /*!< Status bit mask. */
    float                trackAngle;                /*!< Track angle, in rad. */
    float                slipAngle;                 /*!< Slip angle, in rad. */
    float                curvatureRadius;           /*!< Curvature radius, in m, always positive. */
} SbgEComLogAutomotive;

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogAutomotive SbgLogAutoData);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_AUTOMOTIVE_H
