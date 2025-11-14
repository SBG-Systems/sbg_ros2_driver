// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <sbgECom.h>

// Local headers
#include "sbgEComCmdCommon.h"
#include "sbgEComCmdInfo.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComCmdGetInfo(SbgEComHandle *pHandle, SbgEComDeviceInfo *pInfo)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComProtocolPayload  receivedPayload;
    uint32_t                trial;
    

    assert(pHandle);
    assert(pInfo);

    sbgEComProtocolPayloadConstruct(&receivedPayload);

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Send the command only since this is a no-payload command
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_INFO, NULL, 0);

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_INFO, &receivedPayload, pHandle->cmdDefaultTimeOut);

            //
            // Test if we have correctly received a message
            //
            if (errorCode == SBG_NO_ERROR)
            {
                //
                // Make sure we have received a payload
                //
                if (sbgEComProtocolPayloadGetSize(&receivedPayload) > 0)
                {
                    SbgStreamBuffer     inputStream;

                    //
                    // Initialize stream buffer to read parameters
                    //
                    sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));

                    //
                    // Read parameters
                    //
                    sbgStreamBufferReadBuffer(&inputStream, pInfo->productCode, SBG_ECOM_INFO_PRODUCT_CODE_LENGTH);
                    pInfo->serialNumber     = sbgStreamBufferReadUint32LE(&inputStream);
                    pInfo->calibationRev    = sbgStreamBufferReadUint32LE(&inputStream);
                    pInfo->calibrationYear  = sbgStreamBufferReadUint16LE(&inputStream);
                    pInfo->calibrationMonth = sbgStreamBufferReadUint8LE(&inputStream);
                    pInfo->calibrationDay   = sbgStreamBufferReadUint8LE(&inputStream);
                    pInfo->hardwareRev      = sbgStreamBufferReadUint32LE(&inputStream);
                    pInfo->firmwareRev      = sbgStreamBufferReadUint32LE(&inputStream);

                    //
                    // We have parsed a message so return immediately but report any error during payload parsing
                    //
                    errorCode = sbgStreamBufferGetLastError(&inputStream);

                    break;
                }
                else
                {
                    //
                    // We should have received a non empty payload so we have received an invalid frame
                    //
                    errorCode = SBG_INVALID_FRAME;
                }
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
