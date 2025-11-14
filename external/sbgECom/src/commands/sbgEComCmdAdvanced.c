// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <sbgECom.h>

// Local headers
#include "sbgEComCmdAdvanced.h"
#include "sbgEComCmdCommon.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComCmdAdvancedGetConf(SbgEComHandle *pHandle, SbgEComAdvancedConf *pConf)
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
        //
        // Send the command without payload since this is a no-payload command
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_ADVANCED_CONF, NULL, 0);

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_ADVANCED_CONF, &receivedPayload, pHandle->cmdDefaultTimeOut);

            //
            // Test if we have received a SBG_ECOM_CMD_ADVANCED_CONF command
            //
            if (errorCode == SBG_NO_ERROR)
            {
                SbgStreamBuffer         inputStream;

                //
                // Initialize stream buffer to read parameters
                //
                sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));

                //
                // Read the mandatory time reference parameter
                //
                pConf->timeReference    = (SbgEComTimeReferenceSrc)sbgStreamBufferReadUint8LE(&inputStream);
                errorCode               = sbgStreamBufferGetLastError(&inputStream);

                if (errorCode == SBG_NO_ERROR)
                {
                    //
                    // The GNSS options parameter has been introduced in ELLIPSE firmware v2.2
                    // We shouldn't report it as an error for older firmware
                    //
                    pConf->gnssOptions  = sbgStreamBufferReadUint32LE(&inputStream);                    
                    
                    if (sbgStreamBufferGetLastError(&inputStream) == SBG_NO_ERROR)
                    {
                        //
                        // The NMEA options parameter has been introduced in ELLIPSE firmware v2.3
                        // We shouldn't report it as an error for older firmware
                        //
                        pConf->nmeaOptions  = sbgStreamBufferReadUint32LE(&inputStream);
                        
                        if (sbgStreamBufferGetLastError(&inputStream) == SBG_NO_ERROR)
                        {
                            errorCode = SBG_NO_ERROR;
                        }
                        else
                        {
                            pConf->nmeaOptions = 0;
                        }
                    }
                    else
                    {
                        pConf->gnssOptions = 0;
                    }
                }

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

SbgErrorCode sbgEComCmdAdvancedSetConf(SbgEComHandle *pHandle, const SbgEComAdvancedConf *pConf)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    uint32_t            trial;
    uint8_t             outputBuffer[9];
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
        sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pConf->timeReference);
        sbgStreamBufferWriteUint32LE(&outputStream, pConf->gnssOptions);
        sbgStreamBufferWriteUint32LE(&outputStream, pConf->nmeaOptions);

        //
        // Send the payload over ECom
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_ADVANCED_CONF, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_ADVANCED_CONF, pHandle->cmdDefaultTimeOut);

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

SbgErrorCode sbgEComCmdAdvancedGetThresholds(SbgEComHandle *pHandle, SbgEComValidityThresholds *pConf)
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
        //
        // Send the command without payload since this is a no-payload command
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_VALIDITY_THRESHOLDS, NULL, 0);

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_VALIDITY_THRESHOLDS, &receivedPayload, pHandle->cmdDefaultTimeOut);

            //
            // Test if we have received a SBG_ECOM_CMD_VALIDITY_THRESHOLDS command
            //
            if (errorCode == SBG_NO_ERROR)
            {
                SbgStreamBuffer     inputStream;

                //
                // Initialize stream buffer to read parameters
                //
                sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));
                
                //
                // Read parameters and check payload consistency
                //
                pConf->positionThreshold = sbgStreamBufferReadFloatLE(&inputStream);
                pConf->velocityThreshold = sbgStreamBufferReadFloatLE(&inputStream);
                pConf->attitudeThreshold = sbgStreamBufferReadFloatLE(&inputStream);
                pConf->headingThreshold = sbgStreamBufferReadFloatLE(&inputStream);

                errorCode = sbgStreamBufferGetLastError(&inputStream);

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

SbgErrorCode sbgEComCmdAdvancedSetThresholds(SbgEComHandle *pHandle, const SbgEComValidityThresholds *pConf)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    uint32_t            trial;
    uint8_t             outputBuffer[4*sizeof(float)];
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
        sbgStreamBufferWriteFloatLE(&outputStream, pConf->positionThreshold);
        sbgStreamBufferWriteFloatLE(&outputStream, pConf->velocityThreshold);
        sbgStreamBufferWriteFloatLE(&outputStream, pConf->attitudeThreshold);
        sbgStreamBufferWriteFloatLE(&outputStream, pConf->headingThreshold);

        //
        // Send the payload over ECom
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_VALIDITY_THRESHOLDS, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_VALIDITY_THRESHOLDS, pHandle->cmdDefaultTimeOut);

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
