// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <sbgECom.h>
#include <transfer/sbgEComTransfer.h>

// Local headers
#include "sbgEComCmdCommon.h"
#include "sbgEComCmdSettings.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComCmdSettingsAction(SbgEComHandle *pHandle, SbgEComSettingsAction action)
{
    SbgErrorCode    errorCode = SBG_NO_ERROR;
    uint32_t        trial;
    uint8_t         outputBuffer[1];
    SbgStreamBuffer outputStream;

    assert(pHandle);

    //
    // Build the payload to send
    //
    sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));
    sbgStreamBufferWriteUint8(&outputStream, action);
    
    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {   
        //
        // Send the command and the action
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_SETTINGS_ACTION, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_SETTINGS_ACTION, pHandle->cmdDefaultTimeOut);

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

SbgErrorCode sbgEComCmdImportSettings(SbgEComHandle *pHandle, const void *pBuffer, size_t size)
{
    //
    // Call function that handle data transfer
    //
    return sbgEComTransferSend(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_IMPORT_SETTINGS, pBuffer, size);
}

SbgErrorCode sbgEComCmdExportSettings(SbgEComHandle *pHandle, void *pBuffer, size_t *pSize, size_t maxSize)
{
    //
    // Call function that handle data transfer
    //
    return sbgEComTransferReceive(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_EXPORT_SETTINGS, pBuffer, pSize, maxSize);
}
