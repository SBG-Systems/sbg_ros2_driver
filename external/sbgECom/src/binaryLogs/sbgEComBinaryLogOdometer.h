/*!
 *	\file		sbgEComBinaryLogOdometer.h
 *  \author		SBG Systems (Raphael Siryani)
 *	\date		25 February 2013
 *
 *	\brief		This file is used to parse received odometer binary logs.
 *
 *	\section CodeCopyright Copyright Notice 
 *  The MIT license
 *  
 *  Copyright (C) 2007-2020, SBG Systems SAS. All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */
#ifndef __SBG_ECOM_BINARY_LOG_ODOMETER_H__
#define __SBG_ECOM_BINARY_LOG_ODOMETER_H__

#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

//----------------------------------------------------------------------//
//- Log odometer status definitions                                    -//
//----------------------------------------------------------------------//

/*!
 * Odometer / velocity status mask definitions.
 */
#define SBG_ECOM_ODO_REAL_MEAS				(0x0001 << 0)			/*!< Set to 1 if this log comes from a real pulse measurement or from a timeout. */
#define SBG_ECOM_ODO_TIME_SYNC				(0x0001 << 1)			/*!< Set to 1 if the velocity information is correctly time synchronized. */

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * Log structure for odometer data.
 */
typedef struct _SbgLogOdometerData
{
	uint32_t	timeStamp;				/*!< Time in us since the sensor power up. */
	uint16_t	status;					/*!< Odometer velocity status bitmask. */
	float	velocity;				/*!< Velocity in m.s^-1 in the odometer direction. */
} SbgLogOdometerData;

//----------------------------------------------------------------------//
//- Operations                                                         -//
//----------------------------------------------------------------------//

/*!
 * Parse data for the SBG_ECOM_LOG_ODO_VEL message and fill the corresponding structure.
 * \param[in]	pInputStream				Input stream buffer to read the payload from.
 * \param[out]	pOutputData					Pointer on the output structure that stores parsed data.
 * \return									SBG_NO_ERROR if the payload has been parsed.
 */
SbgErrorCode sbgEComBinaryLogParseOdometerData(SbgStreamBuffer *pInputStream, SbgLogOdometerData *pOutputData);

/*!
 * Write data for the SBG_ECOM_LOG_ODO_VEL message to the output stream buffer from the provided structure.
 * \param[out]	pOutputStream				Output stream buffer to write the payload to.
 * \param[in]	pInputData					Pointer on the input structure that stores data to write.
 * \return									SBG_NO_ERROR if the message has been generated in the provided buffer.
 */
SbgErrorCode sbgEComBinaryLogWriteOdometerData(SbgStreamBuffer *pOutputStream, const SbgLogOdometerData *pInputData);

#endif
