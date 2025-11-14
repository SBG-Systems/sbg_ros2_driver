// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <sbgECom.h>

// Local headers
#include "sbgEComCmdCommon.h"
#include "sbgEComCmdDvl.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComCmdDvlSetModelId(SbgEComHandle *pHandle, SbgEComDvlModelsIds modelId)
{
    assert(pHandle);

    return sbgEComCmdGenericSetModelId(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_DVL_MODEL_ID, modelId);
}

SbgErrorCode sbgEComCmdDvlGetModelId(SbgEComHandle *pHandle, SbgEComDvlModelsIds *pModelId)
{
    SbgErrorCode    errorCode = SBG_NO_ERROR;
    uint32_t        modelIdAsUint;

    assert(pHandle);
    assert(pModelId);

    errorCode =  sbgEComCmdGenericGetModelId(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_DVL_MODEL_ID, &modelIdAsUint);

    if (errorCode == SBG_NO_ERROR)
    {
            *pModelId = (SbgEComDvlModelsIds)modelIdAsUint;
    }
    
    return errorCode;
}

SbgErrorCode sbgEComCmdDvlInstallationSet(SbgEComHandle *pHandle, const SbgEComDvlInstallation *pDvlInstallation)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    uint32_t            trial;
    uint8_t             outputBuffer[64];
    SbgStreamBuffer     outputStream;

    assert(pHandle);
    assert(pDvlInstallation);

    //
    // Create the command payload
    //
    sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

    //
    // Build payload
    //
    sbgStreamBufferWriteFloatLE(&outputStream, pDvlInstallation->leverArm[0]);
    sbgStreamBufferWriteFloatLE(&outputStream, pDvlInstallation->leverArm[1]);
    sbgStreamBufferWriteFloatLE(&outputStream, pDvlInstallation->leverArm[2]);

    sbgStreamBufferWriteFloatLE(&outputStream, pDvlInstallation->alignment[0]);
    sbgStreamBufferWriteFloatLE(&outputStream, pDvlInstallation->alignment[1]);
    sbgStreamBufferWriteFloatLE(&outputStream, pDvlInstallation->alignment[2]);

    sbgStreamBufferWriteBooleanLE(&outputStream, pDvlInstallation->preciseInstallation);
    
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
            errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_DVL_INSTALLATION, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

            //
            // Make sure that the command has been sent
            //
            if (errorCode == SBG_NO_ERROR)
            {
                //
                // Try to read the device answer for 500 ms
                //
                errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_DVL_INSTALLATION, pHandle->cmdDefaultTimeOut);

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

SbgErrorCode sbgEComCmdDvlInstallationGet(SbgEComHandle *pHandle, SbgEComDvlInstallation *pDvlInstallation)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComProtocolPayload  receivedPayload;
    uint32_t                trial;
    
    assert(pHandle);
    assert(pDvlInstallation);

    sbgEComProtocolPayloadConstruct(&receivedPayload);

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Send the command only since this is a no-payload command
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_DVL_INSTALLATION, NULL, 0);

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_DVL_INSTALLATION, &receivedPayload, pHandle->cmdDefaultTimeOut);

            //
            // Test if we have received a SBG_ECOM_CMD_GNSS_1_LEVER_ARM_ALIGNMENT command
            //
            if (errorCode == SBG_NO_ERROR)
            {
                SbgStreamBuffer     inputStream;

                //
                // Initialize stream buffer to parse the payload
                //
                sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));

                pDvlInstallation->leverArm[0]   = sbgStreamBufferReadFloatLE(&inputStream);
                pDvlInstallation->leverArm[1] = sbgStreamBufferReadFloatLE(&inputStream);
                pDvlInstallation->leverArm[2] = sbgStreamBufferReadFloatLE(&inputStream);

                pDvlInstallation->alignment[0] = sbgStreamBufferReadFloatLE(&inputStream);
                pDvlInstallation->alignment[1] = sbgStreamBufferReadFloatLE(&inputStream);
                pDvlInstallation->alignment[2] = sbgStreamBufferReadFloatLE(&inputStream);

                pDvlInstallation->preciseInstallation   = sbgStreamBufferReadBooleanLE(&inputStream);

                //
                // The command has been executed successfully so return if an error has occurred during payload parsing
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

SbgErrorCode sbgEComCmdDvlSetRejection(SbgEComHandle *pHandle, const SbgEComDvlRejectionConf *pRejectConf)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    uint32_t            trial;
    uint8_t             outputBuffer[2*sizeof(uint8_t)];
    SbgStreamBuffer     outputStream;

    assert(pHandle);
    assert(pRejectConf);

    //
    // Create the command payload
    //
    sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

    //
    // Build payload
    //
    sbgStreamBufferWriteUint8LE(&outputStream, pRejectConf->bottomLayer);
    sbgStreamBufferWriteUint8LE(&outputStream, pRejectConf->waterLayer);

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
            errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_DVL_REJECT_MODES, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

            //
            // Make sure that the command has been sent
            //
            if (errorCode == SBG_NO_ERROR)
            {
                //
                // Try to read the device answer for 500 ms
                //
                errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_DVL_REJECT_MODES, pHandle->cmdDefaultTimeOut);

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

SbgErrorCode sbgEComCmdDvlGetRejection(SbgEComHandle *pHandle, SbgEComDvlRejectionConf *pRejectConf)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComProtocolPayload  receivedPayload;
    uint32_t                trial;

    assert(pHandle);
    assert(pRejectConf);

    sbgEComProtocolPayloadConstruct(&receivedPayload);

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Send the command only since this is a no-payload command
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_DVL_REJECT_MODES, NULL, 0);

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_DVL_REJECT_MODES, &receivedPayload, pHandle->cmdDefaultTimeOut);

            //
            // Test if we have received a SBG_ECOM_CMD_GNSS_1_REJECT_MODES command
            //
            if (errorCode == SBG_NO_ERROR)
            {
                SbgStreamBuffer     inputStream;

                //
                // Initialize stream buffer to parse payload
                //
                sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));

                //
                // Parse the payload
                //
                pRejectConf->bottomLayer    = (SbgEComRejectionMode)sbgStreamBufferReadUint8LE(&inputStream);
                pRejectConf->waterLayer     = (SbgEComRejectionMode)sbgStreamBufferReadUint8LE(&inputStream);
                
                //
                // The command has been executed successfully so return if an error has occurred during payload parsing
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
