/*!
 * \file            sbgEComSessionInfo.h
 * \ingroup         sessionInfo
 * \author          SBG Systems
 * \date            December 20, 2023
 *
 * \brief           Session information management.
 *
 * Session information, including device information and current settings, may
 * be sent regularly by a device. This module handles the reassembly of that
 * information for the convenience of the user.
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

/*!
 * \defgroup    sessionInfo Session information management
 * \brief       Provides a convenient way to reassembly and work with session information.
 */

#ifndef SBG_ECOM_SESSION_INFO_H
#define SBG_ECOM_SESSION_INFO_H

// sbgCommonLib headers
#include <sbgCommon.h>

// Project headers
#include <pager/sbgEComPager.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Constant definitions                                               -//
//----------------------------------------------------------------------//

/*!
 * Session information buffer size, in bytes.
 */
#define SBG_ECOM_SESSION_INFO_BUFFER_SIZE                   (32768)

//----------------------------------------------------------------------//
//- Structure definitions                                              -//
//----------------------------------------------------------------------//

/*!
 * Session information context.
 *
 * String lengths do not include the null-terminating byte.
 */
typedef struct _SbgEComSessionInfoCtx
{
    char                                 string[SBG_ECOM_SESSION_INFO_BUFFER_SIZE]; /*!< Session information string. */

    SbgEComPager                         pager;                                     /*!< Pager. */
} SbgEComSessionInfoCtx;

//----------------------------------------------------------------------//
//- Public functions                                                   -//
//----------------------------------------------------------------------//

/*!
 * Session information context constructor.
 *
 * On return, the session information string of the context is empty.
 *
 * \param[in]   pCtx                        Session information context.
 */
void sbgEComSessionInfoCtxConstruct(SbgEComSessionInfoCtx *pCtx);

/*!
 * Process the content of a session information log.
 *
 * If successful, the session information string remains valid until the next process operation.
 *
 * \param[in]   pCtx                        Session information context.
 * \param[in]   pageIndex                   Page index.
 * \param[in]   nrPages                     Total number of pages.
 * \param[in]   pBuffer                     Buffer.
 * \param[in]   size                        Buffer size, in bytes.
 * \return                                  SBG_NO_ERROR if successful, i.e. the session information string has been reassembled,
 *                                          SBG_NOT_READY if the session information string is not complete.
 */
SbgErrorCode sbgEComSessionInfoCtxProcess(SbgEComSessionInfoCtx *pCtx, uint16_t pageIndex, uint16_t nrPages, const void *pBuffer, size_t size);

/*!
 * Get the string of a session information context.
 *
 * If valid, the session information string remains so until the next process operation.
 *
 * \param[in]   pCtx                        Session information context.
 * \return                                  Session information string, NULL if not complete.
 */
const char *sbgEComSessionInfoCtxGetString(const SbgEComSessionInfoCtx *pCtx);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_SESSION_INFO_H
