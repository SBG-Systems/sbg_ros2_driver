/*!
 * \file            sbgEComLogDiag.h
 * \ingroup         binaryLogs
 * \author          SBG Systems
 * \date            12 June 2019
 *
 * \brief           Parse diagnostic logs emitted by the device.
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

#ifndef SBG_ECOM_LOG_DIAG_H
#define SBG_ECOM_LOG_DIAG_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <protocol/sbgEComProtocol.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Constant definitions                                               -//
//----------------------------------------------------------------------//

/*!
 * Maximum size of the log string, in bytes.
 */
#define SBG_ECOM_LOG_DIAG_MAX_STRING_SIZE                   (SBG_ECOM_MAX_PAYLOAD_SIZE - 6)

//----------------------------------------------------------------------//
//- Structure definitions                                              -//
//----------------------------------------------------------------------//

/*!
 * Diagnostic log structure.
 */
typedef struct _SbgEComLogDiagData
{
    uint32_t                             timestamp;                                 /*!< Timestamp, in microseconds. */
    SbgDebugLogType                      type;                                      /*!< Log type. */
    SbgErrorCode                         errorCode;                                 /*!< Error code. */
    char                                 string[SBG_ECOM_LOG_DIAG_MAX_STRING_SIZE]; /*!< Log string, null-terminated. */
} SbgEComLogDiagData;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Parse data for the SBG_ECOM_LOG_DIAG message and fill the corresponding structure.
 * 
 * \param[out]  pLogData                    Log structure instance to fill.
 * \param[in]   pStreamBuffer               Input stream buffer to read the log from.
 * \return                                  SBG_NO_ERROR if a valid log has been read from the stream buffer.
 */
SbgErrorCode sbgEComLogDiagReadFromStream(SbgEComLogDiagData *pLogData, SbgStreamBuffer *pStreamBuffer);

/*!
 * Write data for the SBG_ECOM_LOG_DIAG message to the output stream buffer from the provided structure.
 *
 * \param[in]   pLogData                    Log structure instance to write.
 * \param[out]  pStreamBuffer               Output stream buffer to write the log to.
 * \return                                  SBG_NO_ERROR if the log has been written to the stream buffer.
 */
SbgErrorCode sbgEComLogDiagWriteToStream(const SbgEComLogDiagData *pLogData, SbgStreamBuffer *pStreamBuffer);

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SBG_DEPRECATED_TYPEDEF(typedef struct _SbgEComLogDiagData SbgLogDiagData);

SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogParseDiagData(SbgStreamBuffer *pStreamBuffer, SbgEComLogDiagData *pLogData));
SBG_DEPRECATED(SbgErrorCode sbgEComBinaryLogWriteDiagData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogDiagData *pLogData));

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_LOG_DIAG_H
