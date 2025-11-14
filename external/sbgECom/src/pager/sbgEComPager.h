/*!
 * \file            sbgEComPager.h
 * \ingroup         pager
 * \author          SBG Systems
 * \date            May 5, 2025
 *
 * \brief           Page management.
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

#ifndef SBG_ECOM_PAGER_H
#define SBG_ECOM_PAGER_H

// sbgCommonLib headers
#include <sbgCommon.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Structure definitions                                              -//
//----------------------------------------------------------------------//

/*!
 * Pager.
 *
 * Operations on a pager are not thread-safe.
 */
typedef struct _SbgEComPager
{
    uint16_t                             nrPages;                                   /*!< Total number of pages. */
    uint16_t                             pageIndex;                                 /*!< Page index. */
    size_t                               size;                                      /*!< Buffer size, in bytes. */
    uint8_t                             *pBuffer;                                   /*!< Pointer to the buffer linked with this pager. */
    size_t                               bufferSize;                                /*!< Size of the linked buffer, in bytes. */
} SbgEComPager;

//----------------------------------------------------------------------//
//- Public functions                                                   -//
//----------------------------------------------------------------------//


/*!
 * Initialize a pager for join operations, and link it to a buffer.
 *
 * The pager will receive page one by one, and join them into a buffer.
 *
 * \param[in]   pPager                      Pager.
 * \param[in]   pBuffer                     Pointer on an allocated buffer to link with this pager.
 * \param[in]   bufferSize                  Buffer size, in bytes.
 * \return                                  SBG_NO_ERROR if successful.
 */
SbgErrorCode sbgEComPagerInitForJoin(SbgEComPager *pPager, void *pBuffer, size_t bufferSize);


/*!
 * Join a page to the pager.
 *
 * The pager's buffer is complete when all pages have been joined together.
 *
 * \param[in]   pPager                      Pager.
 * \param[in]   pageIndex                   Page index.
 * \param[in]   nrPages                     Total number of pages.
 * \param[in]   pPage                       Page buffer.
 * \param[in]   pageSize                    Page size, in bytes.
 * \return                                  SBG_NO_ERROR if successful, i.e. the buffer has been assembled,
 *                                          SBG_NOT_READY if the buffer is not complete.
 */
SbgErrorCode sbgEComPagerJoin(SbgEComPager *pPager, uint16_t pageIndex, uint16_t nrPages, const void *pPage, size_t pageSize);

/*!
 * Checks whether a multi-page message has been completely received.
 *
 * \param[in]   pPager                      Pager.
 * \return                                  True if the entire message has been received.
 */
bool sbgEComPagerJoinIsComplete(const SbgEComPager *pPager);

/*!
 * Get the pager's buffer size, in bytes.
 *
 * \param[in]   pPager                      Pager.
 * \return                                  Buffer size, in bytes.
 */
size_t sbgEComPagerGetBufferSize(const SbgEComPager *pPager);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_PAGER_H
