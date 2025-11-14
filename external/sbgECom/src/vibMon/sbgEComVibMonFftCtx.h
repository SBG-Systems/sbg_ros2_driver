/*!
 * \file            sbgEComVibMonFftCtx.h
 * \ingroup         vibMonFft
 * \author          SBG Systems
 * \date            March 20, 2025
 *
 * \brief           Vibration monitoring FFT context management.
 *
 * \copyright       Copyright (C) 2007-2025, SBG Systems SAS. All rights reserved.
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

#ifndef SBG_ECOM_VIB_MON_FFT_CTX_H
#define SBG_ECOM_VIB_MON_FFT_CTX_H

// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <pager/sbgEComPager.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Constant definitions                                               -//
//----------------------------------------------------------------------//

/*!
 * Vibration monitoring FFT buffer size, in bytes.
 *
 * The buffer contains the FFT magnitudes for each bin, as 16-bits unsigned, and some additional data.
 */
#define SBG_ECOM_VIB_MON_FFT_CTX_BUFFER_SIZE                (8192)

//----------------------------------------------------------------------//
//- Structure definitions                                              -//
//----------------------------------------------------------------------//

/*!
 * Vibration monitoring FFT context.
 *
 * This object is used to reconstruct the FFT data from received chunks as the FFT is not transmitted in a single frame.
 */
typedef struct _SbgEComVibMonFftCtx
{
    uint8_t                              buffer[SBG_ECOM_VIB_MON_FFT_CTX_BUFFER_SIZE];  /*!< Buffer. */

    SbgEComPager                         pager;                                         /*!< Pager. */
} SbgEComVibMonFftCtx;

//----------------------------------------------------------------------//
//- Public functions (SbgEComVibMonFftCtx)                             -//
//----------------------------------------------------------------------//

/*!
 * Vibration monitoring FFT constructor.
 *
 * \param[in]   pCtx                        Vibration monitoring FFT context.
 */
void sbgEComVibMonFftCtxConstruct(SbgEComVibMonFftCtx *pCtx);

/*!
 * Process the content of a vibration monitoring FFT log.
 *
 * \param[in]   pCtx                        Vibration monitoring FFT context.
 * \param[in]   pageIndex                   Page index.
 * \param[in]   nrPages                     Total number of pages.
 * \param[in]   pBuffer                     Buffer.
 * \param[in]   size                        Buffer size, in bytes.
 * \return                                  SBG_NO_ERROR if successful, i.e. the buffer has been reassembled,
 *                                          SBG_NOT_READY if the buffer is not complete.
 */
SbgErrorCode sbgEComVibMonFftCtxProcess(SbgEComVibMonFftCtx *pCtx, uint16_t pageIndex, uint16_t nrPages, const void *pBuffer, size_t size);

/*!
 * Get the stream buffer from a vibration monitoring FFT context.
 *
 * If available, the stream buffer remains valid until the next call to the processing function.
 *
 * \param[in]   pCtx                        Vibration monitoring FFT context.
 * \param[out]  pStreamBuffer               Stream buffer.
 * \return                                  SBG_NO_ERROR if the data is parsed and valid.
 *                                          SBG_NOT_READY if the whole reassembled message is not yet fully received.
 */
SbgErrorCode sbgEComVibMonFftCtxGetStreamBuffer(const SbgEComVibMonFftCtx *pCtx, SbgStreamBuffer *pStreamBuffer);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_VIB_MON_FFT_CTX_H
