// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <sbgECom.h>

// Local headers
#include "sbgEComCmdCommon.h"

//----------------------------------------------------------------------//
//- Common command reception operations                                -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComReceiveAnyCmd(SbgEComHandle *pHandle, uint8_t *pMsgClass, uint8_t *pMsgId, void *pData, size_t *pSize, size_t maxSize, uint32_t timeOut)
{
    SbgErrorCode                         errorCode;
    SbgEComProtocolPayload               payload;

    assert(pHandle);
    assert(pMsgClass);
    assert(pMsgId);

    sbgEComProtocolPayloadConstruct(&payload);

    errorCode = sbgEComReceiveAnyCmd2(pHandle, pMsgClass, pMsgId, &payload, timeOut);

    if (errorCode == SBG_NO_ERROR)
    {
        size_t                           size;

        size = sbgEComProtocolPayloadGetSize(&payload);

        if (size <= maxSize)
        {
            if (pData)
            {
                const void              *pBuffer;

                pBuffer = sbgEComProtocolPayloadGetBuffer(&payload);

                memcpy(pData, pBuffer, size);
            }

            if (pSize)
            {
                *pSize = size;
            }
        }
        else
        {
            errorCode = SBG_BUFFER_OVERFLOW;
        }
    }

    sbgEComProtocolPayloadDestroy(&payload);

    return errorCode;
}

SbgErrorCode sbgEComReceiveAnyCmd2(SbgEComHandle *pHandle, uint8_t *pMsgClass, uint8_t *pMsgId, SbgEComProtocolPayload *pPayload, uint32_t timeOut)
{
    SbgErrorCode                         errorCode;
    uint32_t                             start;

    assert(pHandle);

    if (timeOut > 0)
    {
        start = sbgGetTime();
    }
    else
    {
        //
        // Avoid compiler warning
        //
        start = 0;
    }

    for (;;)
    {
        uint8_t                          receivedMsgClass;
        uint8_t                          receivedMsgId;
        uint32_t                         now;

        errorCode = sbgEComProtocolReceive2(&pHandle->protocolHandle, &receivedMsgClass, &receivedMsgId, pPayload);

        if (errorCode == SBG_NO_ERROR)
        {
            if (sbgEComMsgClassIsALog((SbgEComClass)receivedMsgClass))
            {
                if (pHandle->pReceiveLogCallback)
                {
                    SbgEComLogUnion          logData;

                    errorCode = sbgEComLogParse((SbgEComClass)receivedMsgClass, receivedMsgId, sbgEComProtocolPayloadGetBuffer(pPayload), sbgEComProtocolPayloadGetSize(pPayload), &logData);

                    if (errorCode == SBG_NO_ERROR)
                    {
                        pHandle->pReceiveLogCallback(pHandle, (SbgEComClass)receivedMsgClass, receivedMsgId, &logData, pHandle->pUserArg);                      

                        sbgEComLogCleanup(&logData, (SbgEComClass)receivedMsgClass, (SbgEComMsgId)receivedMsgId);
                    }
                }
            }
            else
            {
                if (pMsgClass)
                {
                    *pMsgClass = receivedMsgClass;
                }

                if (pMsgId)
                {
                    *pMsgId = receivedMsgId;
                }

                break;
            }
        }
        
        if (timeOut > 0)
        {
            //
            // Only sleep if the Rx buffer is empty, otherwise we should retry ASAP to drain it
            //
            if (errorCode == SBG_NOT_READY)
            {
                sbgSleep(1);
            }

            now = sbgGetTime();

            if ((now - start) >= timeOut)
            {
                errorCode = SBG_TIME_OUT;
                break;
            }
        }
        else
        {
            if (errorCode == SBG_NO_ERROR)
            {
                errorCode = SBG_TIME_OUT;
            }
            else
            {
                errorCode = SBG_NOT_READY;
            }
            
            break;
        }
    }

    return errorCode;
}

SbgErrorCode sbgEComReceiveCmd(SbgEComHandle *pHandle, uint8_t msgClass, uint8_t msgId, void *pData, size_t *pSize, size_t maxSize, uint32_t timeOut)
{
    SbgErrorCode                         errorCode;
    SbgEComProtocolPayload               payload;

    sbgEComProtocolPayloadConstruct(&payload);

    errorCode = sbgEComReceiveCmd2(pHandle, msgClass, msgId, &payload, timeOut);

    if (errorCode == SBG_NO_ERROR)
    {
        size_t                           size;

        size = sbgEComProtocolPayloadGetSize(&payload);

        if (size <= maxSize)
        {
            if (pData)
            {
                const void              *pBuffer;

                pBuffer = sbgEComProtocolPayloadGetBuffer(&payload);

                memcpy(pData, pBuffer, size);
            }

            if (pSize)
            {
                *pSize = size;
            }
        }
        else
        {
            errorCode = SBG_BUFFER_OVERFLOW;
        }
    }

    sbgEComProtocolPayloadDestroy(&payload);

    return errorCode;
}

SbgErrorCode sbgEComReceiveCmd2(SbgEComHandle *pHandle, uint8_t msgClass, uint8_t msgId, SbgEComProtocolPayload *pPayload, uint32_t timeOut)
{
    SbgErrorCode                         errorCode;
    uint32_t                             start;

    assert(pHandle);

    start = sbgGetTime();

    for (;;)
    {
        uint8_t                          receivedMsgClass;
        uint8_t                          receivedMsgId;
        uint32_t                         now;

        errorCode = sbgEComReceiveAnyCmd2(pHandle, &receivedMsgClass, &receivedMsgId, pPayload, 0);

        if (errorCode == SBG_NO_ERROR)
        {
            if ((receivedMsgClass == msgClass) && (receivedMsgId == msgId))
            {
                break;
            }
            else if ((receivedMsgClass == SBG_ECOM_CLASS_LOG_CMD_0) && (receivedMsgId == SBG_ECOM_CMD_ACK))
            {
                SbgStreamBuffer          streamBuffer;
                uint8_t                  ackMsgClass;
                uint8_t                  ackMsgId;
                SbgErrorCode             ackErrorCode;

                sbgStreamBufferInitForRead(&streamBuffer, sbgEComProtocolPayloadGetBuffer(pPayload), sbgEComProtocolPayloadGetSize(pPayload));

                ackMsgId        = sbgStreamBufferReadUint8(&streamBuffer);
                ackMsgClass     = sbgStreamBufferReadUint8(&streamBuffer);
                ackErrorCode    = (SbgErrorCode)sbgStreamBufferReadUint16LE(&streamBuffer);

                errorCode = sbgStreamBufferGetLastError(&streamBuffer);

                if ((errorCode == SBG_NO_ERROR) && (ackMsgClass == msgClass) && (ackMsgId == msgId))
                {
                    //
                    // If a successful ACK is expected, the caller should instead explicitly wait for it.
                    // Receiving a successful ACK that corresponds to the requested class/message is thus an error!
                    //
                    if (ackErrorCode != SBG_NO_ERROR)
                    {
                        errorCode = ackErrorCode;
                    }
                    else
                    {
                        errorCode = SBG_ERROR;
                    }

                    break;
                }
            }
        }
        else if (errorCode == SBG_NOT_READY)
        {
            sbgSleep(1);
        }
        
        now = sbgGetTime();

        if ((now - start) >= timeOut)
        {
            errorCode = SBG_TIME_OUT;
            break;
        }
    }

    return errorCode;
}

//----------------------------------------------------------------------//
//- ACK related commands  operations                                   -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComWaitForAck(SbgEComHandle *pHandle, uint8_t msgClass, uint8_t msg, uint32_t timeOut)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComProtocolPayload  receivedPayload;    
    uint8_t                 ackClass;
    uint8_t                 ackMsg;

    assert(pHandle);

    sbgEComProtocolPayloadConstruct(&receivedPayload);

    //
    // Try to receive the ACK and discard any other received log
    //
    errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_ACK, &receivedPayload, timeOut);

    if (errorCode == SBG_NO_ERROR)
    {
        //
        // Validate the received ACK frame
        //
        if (sbgEComProtocolPayloadGetSize(&receivedPayload) == 2*sizeof(uint16_t))
        {
            SbgStreamBuffer     inputStream;

            //
            // Initialize a stream buffer to parse the received payload
            //
            sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));

            //
            // The ACK frame contains the ACK message ID and class, and a uint16_t for the return error code
            // We make sure that the ACK is for the correct command
            //
            ackMsg      = sbgStreamBufferReadUint8LE(&inputStream);
            ackClass    = sbgStreamBufferReadUint8LE(&inputStream);

            if ((ackMsg == msg) && (ackClass == msgClass))
            {
                errorCode = (SbgErrorCode)sbgStreamBufferReadUint16LE(&inputStream);
            }
            else
            {
                errorCode = SBG_INVALID_FRAME;
                SBG_LOG_WARNING(errorCode, "received ACK mismatch. expecting %#"PRIx8":%#"PRIx8" got %#"PRIx8":%#"PRIx8, msgClass, msg, ackClass, ackMsg);
            }
        }
        else
        {
            errorCode = SBG_INVALID_FRAME;
            SBG_LOG_WARNING(errorCode, "payload size is invalid for an ACK.");
        }
    }

    sbgEComProtocolPayloadDestroy(&receivedPayload);

    return errorCode;
}

SbgErrorCode sbgEComSendAck(SbgEComHandle *pHandle, uint8_t msgClass, uint8_t msg, SbgErrorCode cmdError)
{
    SbgStreamBuffer         outputStream;
    uint8_t                 outputBuffer[2*sizeof(uint8_t)+sizeof(uint16_t)];
    
    assert(pHandle);

    //
    // Initialize a stream buffer to write the command payload
    //
    sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

    //
    // Write the message ID and class and then the error code
    //
    sbgStreamBufferWriteUint8LE(&outputStream, msg);
    sbgStreamBufferWriteUint8LE(&outputStream, msgClass);
    sbgStreamBufferWriteUint16LE(&outputStream, (uint16_t)cmdError);

    //
    // Send the ACK command
    //
    return sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_ACK, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));
}

//----------------------------------------------------------------------//
//- Generic command definitions                                        -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComCmdGenericSetModelId(SbgEComHandle *pHandle, uint8_t msgClass, uint8_t msg, uint32_t modelId)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    uint32_t            trial;
    uint8_t             outputBuffer[sizeof(uint32_t)];
    SbgStreamBuffer     outputStream;
    
    assert(pHandle);
    
    //
    // Init stream buffer for output and Build payload
    //
    sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));
    sbgStreamBufferWriteUint32LE(&outputStream, modelId);

    //
    // Make sure the payload has been build correctly
    //
    errorCode = sbgStreamBufferGetLastError(&outputStream);

    if (errorCode == SBG_NO_ERROR)
    {
        //
        // Send the command three times
        //
        for (trial = 0; trial < pHandle->numTrials; trial++)
        {
            //
            // Send the payload over ECom
            //
            errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, msgClass, msg, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

            //
            // Make sure that the command has been sent
            //
            if (errorCode == SBG_NO_ERROR)
            {
                //
                // Try to read the device answer for 500 ms
                //
                errorCode = sbgEComWaitForAck(pHandle, msgClass, msg, pHandle->cmdDefaultTimeOut);

                //
                // Test if we have received a valid ACK
                //
                if (errorCode == SBG_NO_ERROR)
                {
                    //
                    // The command has been executed successfully so return
                    //
                    break;
                }
            }
            else
            {
                //
                // We have a write error so exit the try loop
                //
                break;
            }
        }
    }
    
    return errorCode;
}

SbgErrorCode sbgEComCmdGenericGetModelId(SbgEComHandle *pHandle, uint8_t msgClass, uint8_t msg, uint32_t *pModelId)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComProtocolPayload  receivedPayload;
    uint32_t                trial;
    
    assert(pHandle);
    assert(pModelId);

    sbgEComProtocolPayloadConstruct(&receivedPayload);

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Send the command only since this is a no payload command
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, msgClass, msg, NULL, 0);

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComReceiveCmd2(pHandle, msgClass, msg, &receivedPayload, pHandle->cmdDefaultTimeOut);

            //
            // Test if we have received a the specified command
            //
            if (errorCode == SBG_NO_ERROR)
            {
                SbgStreamBuffer     inputStream;

                //
                // Initialize stream buffer to read parameters
                //
                sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));

                //
                // Read parameters
                //
                *pModelId = sbgStreamBufferReadUint32LE(&inputStream);

                //
                // The command has been executed successfully so return
                // We return the stream buffer error code to catch any overflow error on the payload
                //
                errorCode = sbgStreamBufferGetLastError(&inputStream);
                break;
            }
        }
        else
        {
            //
            // We have a write error so exit the try loop
            //
            break;
        }
    }

    sbgEComProtocolPayloadDestroy(&receivedPayload);

    return errorCode;
}
