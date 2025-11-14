/*!
 * \file            sbgEComLogRawData.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            16 November 2020
 *
 * \brief           Parse logs used to store a binary stream such as RAW GNSS or RTCM stream.
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

#ifndef SBG_ECOM_LOG_RAW_DATA_H
#define SBG_ECOM_LOG_RAW_DATA_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Log raw Data const definitions                                     -//
//----------------------------------------------------------------------//

#define SBG_ECOM_RAW_DATA_MAX_BUFFER_SIZE                       (4086u) /*!< Maximum buffer size in bytes that can be stored in the raw data log. */

//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * Structure that stores raw data message.
 */
typedef struct _SbgEComLogRawData
{
    uint8_t         rawBuffer[SBG_ECOM_RAW_DATA_MAX_BUFFER_SIZE];   /*!< Buffer that contains raw data. */
    size_t          bufferSize;                                     /*!< Raw buffer size in bytes. */
} SbgEComLogRawData;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Parse raw data message and fill the corresponding structure.
 * 
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogRawDataReadFromStream(SbgEComLogRawData *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write raw data message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogRawDataWriteToStream(const SbgEComLogRawData *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogRawData SbgLogRawData);

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParseRawData(SbgStreamBuffer *pStreamBuffer, SbgEComLogRawData *pLogData));
SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogWriteRawData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogRawData *pLogData));

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_RAW_DATA_H
