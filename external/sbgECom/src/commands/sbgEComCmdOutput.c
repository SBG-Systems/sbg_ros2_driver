// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <sbgECom.h>

// Local headers
#include "sbgEComCmdCommon.h"
#include "sbgEComCmdOutput.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComCmdOutputGetConf(SbgEComHandle *pHandle, SbgEComOutputPort outputPort, SbgEComClass classId, SbgEComMsgId msgId, SbgEComOutputMode *pConf)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComProtocolPayload  receivedPayload;
    uint32_t                trial;
    uint8_t                 outputBuffer[5];
    SbgStreamBuffer         outputStream;

    assert(pHandle);
    assert(pConf);

    sbgEComProtocolPayloadConstruct(&receivedPayload);

    //
    // Build payload to send
    //
    sbgStreamBufferInitForWrite(&outputStream, &outputBuffer, sizeof(outputBuffer));

    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)outputPort);
    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)msgId);
    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)classId);

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Send the command and the prepared payload
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_OUTPUT_CONF, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_OUTPUT_CONF, &receivedPayload, pHandle->cmdDefaultTimeOut);

            //
            // Test if we have received a SBG_ECOM_CMD_OUTPUT_CONF command
            //
            if (errorCode == SBG_NO_ERROR)
            {
                SbgStreamBuffer     inputStream;

                //
                // Initialize stream buffer to read parameters
                //
                sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));

                //
                // Parse the received payload
                //
                outputPort  = (SbgEComOutputPort)sbgStreamBufferReadUint8LE(&inputStream);
                msgId       = sbgStreamBufferReadUint8LE(&inputStream);
                classId     = (SbgEComClass)sbgStreamBufferReadUint8LE(&inputStream);
                *pConf      = (SbgEComOutputMode)sbgStreamBufferReadUint16LE(&inputStream);

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
    
    sbgEComProtocolPayloadDestroy(&receivedPayload);

    return errorCode;
}

SbgErrorCode sbgEComCmdOutputSetConf(SbgEComHandle *pHandle, SbgEComOutputPort outputPort, SbgEComClass classId, SbgEComMsgId msgId, SbgEComOutputMode conf)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    uint32_t            trial;
    uint8_t             outputBuffer[5];
    SbgStreamBuffer     outputStream;

    assert(pHandle);

    //
    // Build the payload to send
    //
    sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)outputPort);
    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)msgId);
    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)classId);
    sbgStreamBufferWriteUint16LE(&outputStream, (uint16_t)conf);

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Send the payload over ECom
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_OUTPUT_CONF, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_OUTPUT_CONF, pHandle->cmdDefaultTimeOut);

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
    
    return errorCode;
}

SbgErrorCode sbgEComCmdOutputClassGetEnable(SbgEComHandle *pHandle, SbgEComOutputPort outputPort, SbgEComClass classId, bool *pEnable)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComProtocolPayload  receivedPayload;
    uint32_t                trial;
    uint8_t                 outputBuffer[3];
    SbgStreamBuffer         outputStream;

    assert(pHandle);
    assert(pEnable);

    sbgEComProtocolPayloadConstruct(&receivedPayload);

    //
    // Build payload to send
    //
    sbgStreamBufferInitForWrite(&outputStream, &outputBuffer, sizeof(outputBuffer));

    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)outputPort);
    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)classId);

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Send the command and the prepared payload
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_OUTPUT_CLASS_ENABLE, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_OUTPUT_CLASS_ENABLE, &receivedPayload, pHandle->cmdDefaultTimeOut);

            //
            // Test if we have received a correct answer
            //
            if (errorCode == SBG_NO_ERROR)
            {
                SbgStreamBuffer     inputStream;

                //
                // Initialize stream buffer to read parameters
                //
                sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));

                //
                // Parse the received payload
                //
                outputPort  = (SbgEComOutputPort)sbgStreamBufferReadUint8LE(&inputStream);
                classId     = (SbgEComClass)sbgStreamBufferReadUint8LE(&inputStream);
                *pEnable    = (bool)sbgStreamBufferReadUint8LE(&inputStream);

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

    sbgEComProtocolPayloadDestroy(&receivedPayload);

    return errorCode;
}

SbgErrorCode sbgEComCmdOutputClassSetEnable(SbgEComHandle *pHandle, SbgEComOutputPort outputPort, SbgEComClass classId, bool enable)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    uint32_t            trial;
    uint8_t             outputBuffer[3];
    SbgStreamBuffer     outputStream;

    assert(pHandle);

    //
    // Build payload to send
    //
    sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)outputPort);
    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)classId);
    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)enable);

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Send the payload over ECom
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_OUTPUT_CLASS_ENABLE, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_OUTPUT_CLASS_ENABLE, pHandle->cmdDefaultTimeOut);

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
    
    return errorCode;
}

SbgErrorCode sbgEComCmdCanOutputGetConf(SbgEComHandle *pHandle, SbgECanMessageId internalId, SbgEComOutputMode *pMode, uint32_t *pUserId, bool *pExtended)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComProtocolPayload  receivedPayload;
    uint32_t                trial;
    uint8_t                 outputBuffer[2];
    SbgStreamBuffer         outputStream;
    
    
    assert(pHandle);
    assert(pMode);
    assert(pUserId);
    assert(pExtended);

    sbgEComProtocolPayloadConstruct(&receivedPayload);

    //
    // Build the payload to send
    //
    sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));
    sbgStreamBufferWriteUint16LE(&outputStream, internalId);

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Send the payload over ECom
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_CAN_OUTPUT_CONF, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_CAN_OUTPUT_CONF, &receivedPayload, pHandle->cmdDefaultTimeOut);

            //
            // Test if we have received a correct answer
            //
            if (errorCode == SBG_NO_ERROR)
            {
                SbgStreamBuffer     inputStream;

                //
                // Initialize stream buffer to read parameters
                //
                sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));

                //
                // Parse the payload
                //
                internalId      = (SbgECanMessageId)sbgStreamBufferReadUint16LE(&inputStream);
                *pMode          = (SbgEComOutputMode)sbgStreamBufferReadUint16LE(&inputStream);
                *pUserId        = sbgStreamBufferReadUint32LE(&inputStream);
                *pExtended      = (bool)sbgStreamBufferReadUint8LE(&inputStream);
                    
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
    
    sbgEComProtocolPayloadDestroy(&receivedPayload);

    return errorCode;
}

SbgErrorCode sbgEComCmdCanOutputSetConf(SbgEComHandle *pHandle, SbgECanMessageId internalId, SbgEComOutputMode mode, uint32_t userId, bool extended)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    uint32_t            trial;
    uint8_t             outputBuffer[9];
    SbgStreamBuffer     outputStream;

    assert(pHandle);

    //
    // Build the payload to send
    //
    sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

    sbgStreamBufferWriteUint16LE(&outputStream, (uint16_t)internalId);
    sbgStreamBufferWriteUint16LE(&outputStream, (uint16_t)mode);
    sbgStreamBufferWriteUint32LE(&outputStream, userId);
    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)extended);

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Send the payload over ECom
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_CAN_OUTPUT_CONF, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_CAN_OUTPUT_CONF, pHandle->cmdDefaultTimeOut);

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

    return errorCode;
}

SbgErrorCode sbgEComCmdOutputGetNmeaTalkerId(SbgEComHandle *pHandle, SbgEComOutputPort outputPort, char *pNmeaTalkerId)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComProtocolPayload  receivedPayload;
    uint32_t                trial;  
    uint8_t                 outputBuffer[1];
    SbgStreamBuffer         outputStream;

    assert(pHandle);
    assert(pNmeaTalkerId);

    sbgEComProtocolPayloadConstruct(&receivedPayload);

    //
    // Build the payload to send
    //
    sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));
    sbgStreamBufferWriteUint8(&outputStream, outputPort);
    
    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Send the command with the output port as a 1-byte payload
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_NMEA_TALKER_ID, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_NMEA_TALKER_ID, &receivedPayload, pHandle->cmdDefaultTimeOut);

            //
            // Test if we have received a SBG_ECOM_CMD_NMEA_TALKER_ID command
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
                outputPort      = (SbgEComOutputPort)sbgStreamBufferReadUint8LE(&inputStream);
                pNmeaTalkerId[0]    = (char)sbgStreamBufferReadUint8LE(&inputStream);
                pNmeaTalkerId[1]    = (char)sbgStreamBufferReadUint8LE(&inputStream);

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
    
    sbgEComProtocolPayloadDestroy(&receivedPayload);

    return errorCode;
}

SbgErrorCode sbgEComCmdOutputSetNmeaTalkerId(SbgEComHandle *pHandle, SbgEComOutputPort outputPort, const char *pNmeaTalkerId)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    uint32_t            trial;
    uint8_t             outputBuffer[3];
    SbgStreamBuffer     outputStream;

    assert(pHandle);
    assert(pNmeaTalkerId);

    //
    // Build the payload to send
    //
    sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)outputPort);
    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)(pNmeaTalkerId[0]));
    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)(pNmeaTalkerId[1]));

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Send the payload over ECom
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_NMEA_TALKER_ID, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_NMEA_TALKER_ID, pHandle->cmdDefaultTimeOut);

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
    
    return errorCode;
}
