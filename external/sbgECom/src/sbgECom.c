// sbgCommonLib headers
#include <sbgCommon.h>
#include <interfaces/sbgInterface.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Local headers
#include "commands/sbgEComCmdCommon.h"
#include "sbgECom.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComInit(SbgEComHandle *pHandle, SbgInterface *pInterface)
{
    SbgErrorCode errorCode = SBG_NO_ERROR;
    
    assert(pHandle);
    assert(pInterface);
    
    //
    // Initialize the sbgECom handle
    //
    pHandle->pReceiveLogCallback    = NULL;
    pHandle->pUserArg               = NULL;

    //
    // Initialize the default number of trials and time out
    //
    pHandle->numTrials          = 3;
    pHandle->cmdDefaultTimeOut  = SBG_ECOM_DEFAULT_CMD_TIME_OUT;

    //
    // Initialize the protocol 
    //
    errorCode = sbgEComProtocolInit(&pHandle->protocolHandle, pInterface);
    
    return errorCode;
}

SbgErrorCode sbgEComClose(SbgEComHandle *pHandle)
{
    SbgErrorCode errorCode = SBG_NO_ERROR;

    assert(pHandle);

    //
    // Close the protocol
    //
    errorCode = sbgEComProtocolClose(&pHandle->protocolHandle);
    
    return errorCode;
}

SbgErrorCode sbgEComHandleOneLog(SbgEComHandle *pHandle)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    SbgEComLogUnion     logData;
    uint8_t             receivedMsg;
    uint8_t             receivedMsgClass;
    size_t              payloadSize;
    uint8_t             payloadData[SBG_ECOM_MAX_PAYLOAD_SIZE];

    assert(pHandle);

    //
    // Try to read a received frame
    //
    errorCode = sbgEComProtocolReceive(&pHandle->protocolHandle, &receivedMsgClass, &receivedMsg, payloadData, &payloadSize, sizeof(payloadData));

    //
    // Test if we have received a valid frame
    //
    if (errorCode == SBG_NO_ERROR)
    {
        //
        // Test if the received frame is a binary log
        //
        if (sbgEComMsgClassIsALog((SbgEComClass)receivedMsgClass))
        {
            //
            // The received frame is a binary log one
            //
            errorCode = sbgEComLogParse((SbgEComClass)receivedMsgClass, (SbgEComMsgId)receivedMsg, payloadData, payloadSize, &logData);

            //
            // Test if the incoming log has been parsed successfully
            //
            if (errorCode == SBG_NO_ERROR)
            {
                //
                // Test if we have a valid callback to handle received logs
                //
                if (pHandle->pReceiveLogCallback)
                {
                    //
                    // Call the binary log callback using the new method
                    //
                    errorCode = pHandle->pReceiveLogCallback(pHandle, (SbgEComClass)receivedMsgClass, receivedMsg, &logData, pHandle->pUserArg);
                }

                //
                // Clean up resources allocated during parsing, if any.
                //
                sbgEComLogCleanup(&logData, (SbgEComClass)receivedMsgClass, (SbgEComMsgId)receivedMsg);
            }
            else
            {
                //
                // Call the on error callback
                //
            }
        }
        else
        {
            //
            // We have received a command, it shouldn't happen
            //
        }
    }
    else if (errorCode != SBG_NOT_READY)
    {
        //
        // We have received an invalid frame
        //
        SBG_LOG_WARNING(errorCode, "Invalid frame received");
    }
    
    return errorCode;
}

SbgErrorCode sbgEComHandle(SbgEComHandle *pHandle)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;

    assert(pHandle);

    //
    // Try to read all received frames, we thus loop until we get an SBG_NOT_READY error
    //
    do
    {
        //
        // Try to read and parse one frame
        //
        errorCode = sbgEComHandleOneLog(pHandle);
    } while (errorCode != SBG_NOT_READY);
    
    return errorCode;
}

SbgErrorCode sbgEComPurgeIncoming(SbgEComHandle *pHandle)
{
    SbgErrorCode    errorCode = SBG_NO_ERROR;

    assert(pHandle);

    errorCode = sbgEComProtocolPurgeIncoming(&pHandle->protocolHandle);

    return errorCode;
}

void sbgEComSetReceiveLogCallback(SbgEComHandle *pHandle, SbgEComReceiveLogFunc pReceiveLogCallback, void *pUserArg)
{
    assert(pHandle);

    //
    // Define the callback and the user argument
    //
    pHandle->pReceiveLogCallback    = pReceiveLogCallback;
    pHandle->pUserArg               = pUserArg;
}

void sbgEComSetCmdTrialsAndTimeOut(SbgEComHandle *pHandle, uint32_t numTrials, uint32_t cmdDefaultTimeOut)
{
    assert(pHandle);
    assert(numTrials > 0);
    assert(cmdDefaultTimeOut > 0);

    //
    // Define the new settings
    //
    pHandle->numTrials          = numTrials;
    pHandle->cmdDefaultTimeOut  = cmdDefaultTimeOut;
}

void sbgEComErrorToString(SbgErrorCode errorCode, char errorMsg[256])
{
    if (errorMsg)
    {
        //
        // For each error code, copy the error msg
        //
        switch (errorCode)
        {
        case SBG_NO_ERROR:
            strcpy(errorMsg, "SBG_NO_ERROR: No error."); 
            break;
        case SBG_ERROR:
            strcpy(errorMsg, "SBG_ERROR: Generic error."); 
            break;
        case SBG_NULL_POINTER:
            strcpy(errorMsg, "SBG_NULL_POINTER: A pointer is null."); 
            break;
        case SBG_INVALID_CRC:
            strcpy(errorMsg, "SBG_INVALID_CRC: The received frame has an invalid CRC.");
            break;
        case SBG_INVALID_FRAME:
            strcpy(errorMsg, "SBG_INVALID_FRAME: The received frame is invalid.");
            break;
        case SBG_TIME_OUT:
            strcpy(errorMsg, "SBG_TIME_OUT: We have a time out during frame reception.");
            break;
        case SBG_WRITE_ERROR:
            strcpy(errorMsg, "SBG_WRITE_ERROR: All bytes hasn't been written.");
            break;
        case SBG_READ_ERROR:
            strcpy(errorMsg, "SBG_READ_ERROR: All bytes hasn't been read.");
            break;
        case SBG_BUFFER_OVERFLOW:
            strcpy(errorMsg, "SBG_BUFFER_OVERFLOW: A buffer is too small to contain so much data.");
            break;
        case SBG_INVALID_PARAMETER:
            strcpy(errorMsg, "SBG_INVALID_PARAMETER: An invalid parameter has been founded.");
            break;
        case SBG_NOT_READY:
            strcpy(errorMsg, "SBG_NOT_READY: A device isn't ready (Rx isn't ready for example).");
            break;
        case SBG_MALLOC_FAILED:
            strcpy(errorMsg, "SBG_MALLOC_FAILED: Failed to allocate a buffer.");
            break;
        case SBG_CALIB_MAG_NOT_ENOUGH_POINTS:
            strcpy(errorMsg, "SBG_CALIB_MAG_NOT_ENOUGH_POINTS: Not enough points were available to perform magnetometers calibration.");
            break;
        case SBG_CALIB_MAG_INVALID_TAKE:
            strcpy(errorMsg, "SBG_CALIB_MAG_INVALID_TAKE: The calibration procedure could not be properly executed due to insufficient precision.");
            break;
        case SBG_CALIB_MAG_SATURATION:
            strcpy(errorMsg, "SBG_CALIB_MAG_SATURATION: Saturation were detected when attempt to calibrate magnetos.");
            break;
        case SBG_CALIB_MAG_POINTS_NOT_IN_A_PLANE:
            strcpy(errorMsg, "SBG_CALIB_MAG_POINTS_NOT_IN_A_PLANE: 2D calibration procedure could not be performed.");
            break;
        case SBG_DEVICE_NOT_FOUND:
            strcpy(errorMsg, "SBG_DEVICE_NOT_FOUND: A device couldn't be founded or opened.");
            break;
        case SBG_OPERATION_CANCELLED:
            strcpy(errorMsg, "SBG_OPERATION_CANCELLED: An operation has been canceled by a user.");
            break;
        case SBG_NOT_CONTINUOUS_FRAME:
            strcpy(errorMsg, "SBG_NOT_CONTINUOUS_FRAME: We have received a frame that isn't a continuous one.");
            break;
        case SBG_INCOMPATIBLE_HARDWARE:
            strcpy(errorMsg, "SBG_INCOMPATIBLE_HARDWARE: Hence valid, the configuration cannot be executed because of incompatible hardware.");
            break;
        default:
            sprintf(errorMsg, "Undefined error code: %u", errorCode);
            break;
        }
    }
}
