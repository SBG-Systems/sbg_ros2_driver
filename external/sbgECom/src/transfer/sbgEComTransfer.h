/*!
 * \file            sbgEComTransfer.h
 * \ingroup         protocol
 * \author          SBG Systems
 * \date            19 November 2013
 *
 * \brief           Handle large send/receive transfer for specific ECom Protocol commands.
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

#ifndef SBG_ECOM_TRANSFER_H
#define SBG_ECOM_TRANSFER_H

// sbgCommonLib headers
#include <sbgCommon.h>

// Project headers
#include <sbgECom.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Global definitions                                                 -//
//----------------------------------------------------------------------//

#define SBG_ECOM_TRANSFER_MAX_SIZE          (8192u)         /*!< Maximum buffer that can be transmitted using the sbgECom transfer protocol. */
#define SBG_ECOM_TRANSFER_PACKET_SIZE       (512u)          /*!< Max packet size transmitted in a single frame */

//----------------------------------------------------------------------//
//- Communication protocol struct and definitions                      -//
//----------------------------------------------------------------------//

/*!
 * Defines the ECom transfer commands
 */
typedef enum _SbgEComTransferCmd
{
    SBG_ECOM_TRANSFER_START = 0,                            /*!< Command to initiate a transfer. */
    SBG_ECOM_TRANSFER_DATA  = 1,                            /*!< Command to transmit/receive data. */
    SBG_ECOM_TRANSFER_END   = 2                             /*!< Command to end a transfer. */
} SbgEComTransferCmd;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Specific method to handle a large send into multiple frames.
 * 
 * \param[in]   pHandle                 Pointer to a valid SbgEComHandle.
 * \param[in]   msgClass                Original protocol class asking for transfer.
 * \param[in]   msg                     Original protocol message id asking for transfer.
 * \param[in]   pBuffer                 Pointer to the buffer containing the data to send.
 * \param[in]   size                    The size of the buffer.
 * \return                              SBG_NO_ERROR in case of a successful upload.
 */
SbgErrorCode sbgEComTransferSend(SbgEComHandle *pHandle, uint8_t msgClass, uint8_t msg, const void *pBuffer, size_t size);

/*!
 * Specific method to handle a large receive from the device.
 * 
 * \param[in]   pHandle                 Pointer to a valid SbgEComHandle.
 * \param[in]   msgClass                Original protocol class asking for transfer.
 * \param[in]   msg                     Original protocol message id asking for transfer.
 * \param[in]   pBuffer                 Pointer to the buffer where to write data.
 * \param[out]  pActualSize             The final size written into the buffer.
 * \param[in]   bufferSize              The size of the buffer.
 * \return                              SBG_NO_ERROR in case of a successful download.
 */
SbgErrorCode sbgEComTransferReceive(SbgEComHandle *pHandle, uint8_t msgClass, uint8_t msg, void *pBuffer, size_t *pActualSize, size_t bufferSize);

#ifdef __cplusplus
}
#endif

#endif // SBG_ECOM_TRANSFER_H
