// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <sbgECom.h>

// Local headers
#include "sbgEComCmdCommon.h"
#include "sbgEComCmdEvent.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComCmdSyncInGetConf(SbgEComHandle *pHandle, SbgEComSyncInId syncInId, SbgEComSyncInConf *pConf)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComProtocolPayload  receivedPayload;
    uint32_t                trial;

    assert(pHandle);
    assert(pConf);

    sbgEComProtocolPayloadConstruct(&receivedPayload);

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        uint8_t syncInIdParam = syncInId;

        //
        // Send the command with syncInId as a 1-byte payload
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_SYNC_IN_CONF, &syncInIdParam, sizeof(syncInIdParam));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_SYNC_IN_CONF, &receivedPayload, pHandle->cmdDefaultTimeOut);

            //
            // Test if we have received a SBG_ECOM_CMD_SYNC_IN_CONF command
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
                // First is returned the id of the sync, then the sensitivity and the delay at last.
                //
                syncInId            = (SbgEComSyncInId)sbgStreamBufferReadUint8LE(&inputStream);                    
                pConf->sensitivity  = (SbgEComSyncInSensitivity)sbgStreamBufferReadUint8LE(&inputStream);
                pConf->delay        = sbgStreamBufferReadInt32LE(&inputStream);

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

SbgErrorCode sbgEComCmdSyncInSetConf(SbgEComHandle *pHandle, SbgEComSyncInId syncInId, const SbgEComSyncInConf *pConf)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    uint32_t            trial;
    uint8_t             outputBuffer[8];
    SbgStreamBuffer     outputStream;

    assert(pHandle);
    assert(pConf);

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Init stream buffer for output
        //
        sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

        //
        // Build payload
        //
        sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)syncInId);          
        sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pConf->sensitivity);
        sbgStreamBufferWriteInt32LE(&outputStream, pConf->delay);

        //
        // Send the message over ECom
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_SYNC_IN_CONF, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_SYNC_IN_CONF, pHandle->cmdDefaultTimeOut);

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

SbgErrorCode sbgEComCmdSyncOutGetConf(SbgEComHandle *pHandle, SbgEComSyncOutId syncOutId, SbgEComSyncOutConf *pConf)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComProtocolPayload  receivedPayload;
    uint32_t                trial;
    
    assert(pHandle);
    assert(pConf);

    sbgEComProtocolPayloadConstruct(&receivedPayload);

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        uint8_t syncOutIdParam = syncOutId;

        //
        // Send the command with syncOutId as a 1-byte payload
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_SYNC_OUT_CONF, &syncOutIdParam, sizeof(syncOutIdParam));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_SYNC_OUT_CONF, &receivedPayload, pHandle->cmdDefaultTimeOut);

            //
            // Test if we have received a SBG_ECOM_CMD_SYNC_OUT_CONF command
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
                // First is returned the id of the sync, then a reserved field, the output function, polarity and the duration at last.
                //
                syncOutId = (SbgEComSyncOutId)sbgStreamBufferReadUint8LE(&inputStream);
                sbgStreamBufferReadUint8LE(&inputStream);
                pConf->outputFunction = (SbgEComSyncOutFunction)sbgStreamBufferReadUint16LE(&inputStream);
                pConf->polarity = (SbgEComSyncOutPolarity)sbgStreamBufferReadUint8LE(&inputStream);
                pConf->duration = sbgStreamBufferReadUint32LE(&inputStream);

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

SbgErrorCode sbgEComCmdSyncOutSetConf(SbgEComHandle *pHandle, SbgEComSyncOutId syncOutId, const SbgEComSyncOutConf *pConf)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    uint32_t            trial;
    uint8_t             outputBuffer[16];
    SbgStreamBuffer     outputStream;

    assert(pHandle);
    assert(pConf);
    
    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Init stream buffer for output
        //
        sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

        //
        // Build payload
        //
        sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)syncOutId);
        sbgStreamBufferWriteUint8LE(&outputStream, 0);
        sbgStreamBufferWriteUint16LE(&outputStream, (uint16_t)pConf->outputFunction);
        sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pConf->polarity);
        sbgStreamBufferWriteUint32LE(&outputStream, pConf->duration);

        //
        // Send the payload over ECom
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_SYNC_OUT_CONF, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_SYNC_OUT_CONF, pHandle->cmdDefaultTimeOut);

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
