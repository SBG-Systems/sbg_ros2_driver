/*!
 * \file            sbgEComCmdApi.h
 * \ingroup         commands
 * \author          SBG Systems
 * \date            October 14, 2020
 *
 * \brief           REST API related commands.
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

#ifndef SBG_ECOM_CMD_API_H
#define SBG_ECOM_CMD_API_H

// sbgCommonLib headers
#include <sbgCommon.h>

// Project headers
#include <sbgECom.h>
#include <protocol/sbgEComProtocol.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Structure definitions                                              -//
//----------------------------------------------------------------------//

/*!
 * Reply to REST API commands.
 *
 * The reply content is a null-terminated string, normally in JSON format.
 *
 * The content directly refers to data inside the payload.
 */
typedef struct _SbgEComCmdApiReply
{
    SbgEComProtocolPayload               payload;                                   /*!< Payload. */
    uint16_t                             statusCode;                                /*!< Status code. */
    const char                          *pContent;                                  /*!< Content. */
} SbgEComCmdApiReply;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * REST API reply constructor.
 *
 * \param[in]   pReply                      REST API reply.
 */
void sbgEComCmdApiReplyConstruct(SbgEComCmdApiReply *pReply);

/*!
 * REST API reply destructor.
 *
 * \param[in]   pReply                      REST API reply.
 */
void sbgEComCmdApiReplyDestroy(SbgEComCmdApiReply *pReply);

/*!
 * Check if a reply indicates successful command execution.
 *
 * \param[in]   pReply                      REST API reply.
 * \return                                  True if the reply indicates successful command execution.
 */
bool sbgEComCmdApiReplySuccessful(const SbgEComCmdApiReply *pReply);

/*!
 * Send a GET command.
 *
 * The reply must be destroyed before the next attempt to receive data, either logs or command replies.
 *
 * \param[in]   pHandle                     ECom handle.
 * \param[in]   pPath                       URI path component.
 * \param[in]   pQuery                      Query string, may be NULL.
 * \param[out]  pReply                      Reply.
 * \return                                  SBG_NO_ERROR if successful.
 */
SbgErrorCode sbgEComCmdApiGet(SbgEComHandle *pHandle, const char *pPath, const char *pQuery, SbgEComCmdApiReply *pReply);

/*!
 * Send a POST command.
 *
 * The reply must be destroyed before the next attempt to receive data, either logs or command replies.
 *
 * \param[in]   pHandle                     ECom handle.
 * \param[in]   pPath                       URI path component.
 * \param[in]   pQuery                      Query string, may be NULL.
 * \param[in]   pBody                       Body, may be NULL.
 * \param[out]  pReply                      Reply.
 * \return                                  SBG_NO_ERROR if successful.
 */
SbgErrorCode sbgEComCmdApiPost(SbgEComHandle *pHandle, const char *pPath, const char *pQuery, const char *pBody, SbgEComCmdApiReply *pReply);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_CMD_API_H
