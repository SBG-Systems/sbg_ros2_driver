// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <sbgECom.h>

// Local headers
#include "sbgEComCmdCommon.h"
#include "sbgEComCmdFeatures.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComCmdGetFeatures(SbgEComHandle *pHandle, SbgEComFeatures *pFeatures)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComProtocolPayload  receivedPayload;
    uint32_t                trial;
    
    assert(pHandle);
    assert(pFeatures);

    sbgEComProtocolPayloadConstruct(&receivedPayload);

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Send the command only since this is a no-payload command
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_FEATURES, NULL, 0);

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_FEATURES, &receivedPayload, pHandle->cmdDefaultTimeOut);

            //
            // Test if we have received a SBG_ECOM_CMD_GPS_FEATURES command
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
                pFeatures->sensorFeaturesMask   =  sbgStreamBufferReadUint32LE(&inputStream);
                pFeatures->gnssType             = (SbgEComGnssType)sbgStreamBufferReadUint8LE(&inputStream);
                pFeatures->gnssUpdateRate       = sbgStreamBufferReadUint8LE(&inputStream);
                pFeatures->gnssSignalsMask      = sbgStreamBufferReadUint32LE(&inputStream);
                pFeatures->gnssFeaturesMask     = sbgStreamBufferReadUint32LE(&inputStream);
                sbgStreamBufferReadBuffer(&inputStream, pFeatures->gnssProductCode, 32*sizeof(char));
                sbgStreamBufferReadBuffer(&inputStream, pFeatures->gnssSerialNumber, 32*sizeof(char));
                
                //
                // Only parse the GNSS firmware version if available
                //
                if (sbgStreamBufferGetSpace(&inputStream) > 0)
                {
                    sbgStreamBufferReadBuffer(&inputStream, pFeatures->gnssFirmwareVersion, 32 * sizeof(char));
                }
                else
                {
                    strcpy(pFeatures->gnssFirmwareVersion, "");
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
