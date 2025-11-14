/*!
 * \file            sbgEComCmdCommon.h
 * \ingroup         commands
 * \author          SBG Systems
 * \date            11 June 2014
 *
 * \brief           Definitions and methods common to all commands.
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

#ifndef SBG_ECOM_CMD_COMMON_H
#define SBG_ECOM_CMD_COMMON_H

// sbgCommonLib headers
#include <sbgCommon.h>

// Project headers
#include <sbgECom.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Definitions                                                        -//
//----------------------------------------------------------------------//

#define SBG_ECOM_DEFAULT_CMD_TIME_OUT   (500)   /*!< Default time out in ms for commands reception. */

/*!
 * List of all rejection modes for aiding inputs.
 */
typedef enum _SbgEComRejectionMode
{
    SBG_ECOM_NEVER_ACCEPT_MODE      = 0,        /*!< Measurement is not taken into account. */
    SBG_ECOM_AUTOMATIC_MODE         = 1,        /*!< Measurement is accepted and rejected automatically depending on consistency checks */
    SBG_ECOM_ALWAYS_ACCEPT_MODE     = 2         /*!< Measurement is always accepted. Should be used with caution */
} SbgEComRejectionMode;

/*!
 * List of all axis directions for modules/sensor alignment.
 */
typedef enum _SbgEComAxisDirection
{
    SBG_ECOM_ALIGNMENT_FORWARD      = 0,        /*!< IMU/module Axis is turned in vehicle's forward direction. */
    SBG_ECOM_ALIGNMENT_BACKWARD     = 1,        /*!< IMU/module Axis is turned in vehicle's backward direction. */
    SBG_ECOM_ALIGNMENT_LEFT         = 2,        /*!< IMU/module Axis is turned in vehicle's left direction. */
    SBG_ECOM_ALIGNMENT_RIGHT        = 3,        /*!< IMU/module Axis is turned in vehicle's right direction. */
    SBG_ECOM_ALIGNMENT_UP           = 4,        /*!< IMU/module Axis is turned in vehicle's up direction. */
    SBG_ECOM_ALIGNMENT_DOWN         = 5         /*!< IMU/module Axis is turned in vehicle's down direction. */
} SbgEComAxisDirection;

//----------------------------------------------------------------------//
//- Common command reception operations                                -//
//----------------------------------------------------------------------//

/*!
 * Receive a command message.
 *
 * All binary logs received are handled trough the standard callback system.
 *
 * \param[in]   pHandle                 SbgECom handle.
 * \param[out]  pMsgClass               Message class.
 * \param[out]  pMsgId                  Message ID.
 * \param[out]  pData                   Data buffer, may be NULL.
 * \param[out]  pSize                   Number of bytes received, in bytes, may be NULL.
 * \param[in]   maxSize                 Data buffer size, in bytes.
 * \param[in]   timeOut                 Time-out, in ms.
 * \return                              SBG_NO_ERROR if successful,
 *                                      SBG_TIME_OUT if no command message was received within the specified time out (even if timeOut = 0).
 *                                      SBG_NOT_READY to indicate the underlying interface is empty (only applicable when timeOut = 0).
 *                                      SBG_BUFFER_OVERFLOW if the payload of the received frame couldn't fit into the buffer,
 */
SbgErrorCode sbgEComReceiveAnyCmd(SbgEComHandle *pHandle, uint8_t *pMsgClass, uint8_t *pMsgId, void *pData, size_t *pSize, size_t maxSize, uint32_t timeOut);

/*!
 * Receive a command message.
 *
 * All binary logs received are handled trough the standard callback system.
 *
 * This function is equivalent to sbgEComReceiveAnyCmd() with two exceptions :
 *  - the use of a payload object allows handling payloads not limited by the size of a user-provided buffer
 *  - the payload object allows direct access to the protocol work buffer to avoid an extra copy per call
 *
 * Any allocated resource associated with the given payload is released when calling this function.
 *
 * Because the payload buffer may directly refer to the protocol work buffer on return, it is only valid until
 * the next attempt to receive a frame, with any of the receive functions.
 *
 * \param[in]   pHandle                 SbgECom handle.
 * \param[out]  pMsgClass               Message class.
 * \param[out]  pMsgId                  Message ID.
 * \param[out]  pPayload                Payload.
 * \param[in]   timeOut                 Time-out, in ms.
 * \return                              SBG_NO_ERROR if successful.
 *                                      SBG_TIME_OUT if no command message was received within the specified time out (even if timeOut = 0).
 *                                      SBG_NOT_READY to indicate the underlying interface is empty (only applicable when timeOut = 0).
 */
SbgErrorCode sbgEComReceiveAnyCmd2(SbgEComHandle *pHandle, uint8_t *pMsgClass, uint8_t *pMsgId, SbgEComProtocolPayload *pPayload, uint32_t timeOut);

/*!
 * Receive a specific command message.
 *
 * This function also processes ACK messages for the given class and ID.
 *
 * All binary logs received during this time are handled trough the standard callback system.
 *
 * \param[in]   pHandle                 SbgECom handle.
 * \param[in]   msgClass                Message class.
 * \param[in]   msgId                   Message ID.
 * \param[out]  pData                   Data buffer.
 * \param[out]  pSize                   Number of bytes received, in bytes.
 * \param[in]   maxSize                 Data buffer size, in bytes.
 * \param[in]   timeOut                 Time-out, in ms.
 * \return                              SBG_NO_ERROR if successful,
 *                                      SBG_NOT_READY if no command message has been received,
 *                                      SBG_BUFFER_OVERFLOW if the payload of the received frame couldn't fit into the buffer,
 *                                      SBG_TIME_OUT if no command message was received within the specified time out,
 *                                      any error code reported by an ACK message for the given class and ID.
 */
SbgErrorCode sbgEComReceiveCmd(SbgEComHandle *pHandle, uint8_t msgClass, uint8_t msgId, void *pData, size_t *pSize, size_t maxSize, uint32_t timeOut);

/*!
 * Receive a specific command message.
 *
 * This function also processes ACK messages for the given class and ID.
 *
 * All binary logs received during this time are handled trough the standard callback system.
 *
 * This function is equivalent to sbgEComReceiveCmd() with two exceptions :
 *  - the use of a payload object allows handling payloads not limited by the size of a user-provided buffer
 *  - the payload object allows direct access to the protocol work buffer to avoid an extra copy per call
 *
 * Any allocated resource associated with the given payload is released when calling this function.
 *
 * Because the payload buffer may directly refer to the protocol work buffer on return, it is only valid until
 * the next attempt to receive a frame, with any of the receive functions.
 *
 * \param[in]   pHandle                 SbgECom handle.
 * \param[in]   msgClass                Message class.
 * \param[in]   msgId                   Message ID.
 * \param[out]  pPayload                Payload.
 * \param[in]   timeOut                 Time-out, in ms.
 * \return                              SBG_NO_ERROR if successful,
 *                                      SBG_NOT_READY if no command message has been received 
 *                                      SBG_TIME_OUT if no command message was received within the specified time out,
 *                                      any error code reported by an ACK message for the given class and ID.
 */
SbgErrorCode sbgEComReceiveCmd2(SbgEComHandle *pHandle, uint8_t msgClass, uint8_t msgId, SbgEComProtocolPayload *pPayload, uint32_t timeOut);

//----------------------------------------------------------------------//
//- ACK related commands  operations                                   -//
//----------------------------------------------------------------------//

/*!
 * Wait for an ACK for a specified amount of time.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   msgClass                    The message class that we want to check
 * \param[in]   msg                         The message ID that we want to check
 * \param[in]   timeOut                     Time out in ms during which we can receive the ACK.
 * \return                                  SBG_NO_ERROR if the ACK has been received.
 */
SbgErrorCode sbgEComWaitForAck(SbgEComHandle *pHandle, uint8_t msgClass, uint8_t msg, uint32_t timeOut);

/*!
 * Send an ACK for a specific command with an associated error code.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   msgClass                    The message class that we want to send
 * \param[in]   msg                         The message ID that we want to send.
 * \param[in]   cmdError                    The associated error code.
 * \return                                  SBG_NO_ERROR if the ACK has been sent.
 */
SbgErrorCode sbgEComSendAck(SbgEComHandle *pHandle, uint8_t msgClass, uint8_t msg, SbgErrorCode cmdError);

//----------------------------------------------------------------------//
//- Generic command definitions                                        -//
//----------------------------------------------------------------------//

/*!
 * Generic function to set an error model ID
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   msgClass                    Original message class
 * \param[in]   msg                         Original message ID
 * \param[in]   modelId                     Model ID to set
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdGenericSetModelId(SbgEComHandle *pHandle, uint8_t msgClass, uint8_t msg, uint32_t modelId);

/*!
 * Generic function to get an error model ID
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   msgClass                    Original message class
 * \param[in]   msg                         Original message ID
 * \param[out]  pModelId                    Returns the currently used model ID.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdGenericGetModelId(SbgEComHandle *pHandle, uint8_t msgClass, uint8_t msg, uint32_t *pModelId);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_CMD_COMMON_H
