// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <sbgECom.h>

// Local headers
#include "sbgEComCmdCommon.h"
#include "sbgEComCmdMag.h"

//----------------------------------------------------------------------//
//- Magnetometer commands                                              -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComCmdMagSetModelId(SbgEComHandle *pHandle, SbgEComMagModelsStdId modelId)
{
    assert(pHandle);
    
    return sbgEComCmdGenericSetModelId(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_MAGNETOMETER_MODEL_ID, modelId);
}

SbgErrorCode sbgEComCmdMagGetModelId(SbgEComHandle *pHandle, SbgEComMagModelsStdId *pModelId)
{
    SbgErrorCode    errorCode = SBG_NO_ERROR;
    uint32_t        modelIdAsUint;

    assert(pHandle);
    assert(pModelId);

    errorCode = sbgEComCmdGenericGetModelId(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_MAGNETOMETER_MODEL_ID, &modelIdAsUint);

    if (errorCode == SBG_NO_ERROR)
    {
        *pModelId = (SbgEComMagModelsStdId)modelIdAsUint;
    }

    return errorCode;
}

SbgErrorCode sbgEComCmdMagSetCalibData(SbgEComHandle *pHandle, const float *pOffset, const float *pMatrix)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    SbgStreamBuffer     outputStream;
    uint8_t             outputBuffer[12 * sizeof(float)];
    uint32_t            trial;
    size_t              i;
    
    assert(pHandle);
    assert(pOffset);
    assert(pMatrix);

    sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));
    
    sbgStreamBufferWriteFloatLE(&outputStream, pOffset[0]);
    sbgStreamBufferWriteFloatLE(&outputStream, pOffset[1]);
    sbgStreamBufferWriteFloatLE(&outputStream, pOffset[2]);

    for (i = 0; i < 9; i++)
    {
        sbgStreamBufferWriteFloatLE(&outputStream, pMatrix[i]);
    }

    errorCode = sbgStreamBufferGetLastError(&outputStream);

    if (errorCode == SBG_NO_ERROR)
    {
        for (trial = 0; trial < pHandle->numTrials; trial++)
        {
            errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_SET_MAG_CALIB, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

            if (errorCode == SBG_NO_ERROR)
            {
                errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_SET_MAG_CALIB, pHandle->cmdDefaultTimeOut);

                if (errorCode == SBG_NO_ERROR)
                {
                    break;
                }
            }
            else
            {
                //
                // Unable to send the command, stop here
                //
                break;
            }
        }
    }

    return errorCode;
}

SbgErrorCode sbgEComCmdMagSetCalibData2(SbgEComHandle *pHandle, const float *pOffset, const float *pMatrix, SbgEComMagCalibMode mode)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    SbgStreamBuffer     outputStream;
    uint8_t             outputBuffer[12 * sizeof(float) + sizeof(uint8_t)];
    uint32_t            trial;
    size_t              i;

    assert(pHandle);
    assert(pOffset);
    assert(pMatrix);
    
    sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

    sbgStreamBufferWriteFloatLE(&outputStream, pOffset[0]);
    sbgStreamBufferWriteFloatLE(&outputStream, pOffset[1]);
    sbgStreamBufferWriteFloatLE(&outputStream, pOffset[2]);

    for (i = 0; i < 9; i++)
    {
        sbgStreamBufferWriteFloatLE(&outputStream, pMatrix[i]);
    }

    sbgStreamBufferWriteUint8LE(&outputStream,  (uint8_t)mode);

    errorCode = sbgStreamBufferGetLastError(&outputStream);

    if (errorCode == SBG_NO_ERROR)
    {
        for (trial = 0; trial < pHandle->numTrials; trial++)
        {
            errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_SET_MAG_CALIB, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

            if (errorCode == SBG_NO_ERROR)
            {
                errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_SET_MAG_CALIB, pHandle->cmdDefaultTimeOut);

                if (errorCode == SBG_NO_ERROR)
                {
                    break;
                }
            }
            else
            {
                //
                // Unable to send the command, stop here
                //
                break;
            }
        }
    }

    return errorCode;
}

SbgErrorCode sbgEComCmdMagGetRejection(SbgEComHandle *pHandle, SbgEComMagRejectionConf *pRejectConf)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComProtocolPayload  receivedPayload;
    uint32_t                trial;
    
    assert(pHandle);
    assert(pRejectConf);

    sbgEComProtocolPayloadConstruct(&receivedPayload);

    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_MAGNETOMETER_REJECT_MODE, NULL, 0);

        if (errorCode == SBG_NO_ERROR)
        {
            SbgStreamBuffer     inputStream;

            errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_MAGNETOMETER_REJECT_MODE, &receivedPayload, pHandle->cmdDefaultTimeOut);

            if (errorCode == SBG_NO_ERROR)
            {
                sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));

                //
                // Parse received payload
                //
                pRejectConf->magneticField = (SbgEComRejectionMode)sbgStreamBufferReadUint8LE(&inputStream);

                break;
            }
        }
        else
        {
            //
            // Unable to send the command, stop here
            //
            break;
        }
    }

    sbgEComProtocolPayloadDestroy(&receivedPayload);

    return errorCode;
}

SbgErrorCode sbgEComCmdMagSetRejection(SbgEComHandle *pHandle, const SbgEComMagRejectionConf *pRejectConf)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    uint32_t            trial;
    uint8_t             outputBuffer[64];
    SbgStreamBuffer     outputStream;

    assert(pHandle);
    assert(pRejectConf);

    sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pRejectConf->magneticField);

    errorCode = sbgStreamBufferGetLastError(&outputStream);

    if (errorCode == SBG_NO_ERROR)
    {
        for (trial = 0; trial < pHandle->numTrials; trial++)
        {
            errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_MAGNETOMETER_REJECT_MODE, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

            if (errorCode == SBG_NO_ERROR)
            {
                errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_MAGNETOMETER_REJECT_MODE, pHandle->cmdDefaultTimeOut);

                if (errorCode == SBG_NO_ERROR)
                {
                    break;
                }
            }
            else
            {
                //
                // Unable to send the command, stop here
                //
                break;
            }
        }
    }

    return errorCode;
}

//----------------------------------------------------------------------//
//- Magnetometer on-board calibration commands                         -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComCmdMagStartCalib(SbgEComHandle *pHandle, SbgEComMagCalibMode mode, SbgEComMagCalibBandwidth bandwidth)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    SbgStreamBuffer     outputStream;
    uint8_t             outputBuffer[2];
    uint32_t            trial;

    assert(pHandle);
        
    sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)mode);
    sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)bandwidth);

    errorCode = sbgStreamBufferGetLastError(&outputStream);
    
    if (errorCode == SBG_NO_ERROR)
    {
        for (trial = 0; trial < pHandle->numTrials; trial++)
        {
            errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_START_MAG_CALIB, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

            if (errorCode == SBG_NO_ERROR)
            {
                errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_START_MAG_CALIB, pHandle->cmdDefaultTimeOut);

                if (errorCode == SBG_NO_ERROR)
                {
                    break;
                }
            }
            else
            {
                //
                // Unable to send the command, stop here
                //
                break;
            }
        }
    }
    
    return errorCode;
}

SbgErrorCode sbgEComCmdMagComputeCalib(SbgEComHandle *pHandle, SbgEComMagCalibResults *pCalibResults)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComProtocolPayload  receivedPayload;
    uint32_t                trial;
    
    assert(pHandle);
    assert(pCalibResults);

    sbgEComProtocolPayloadConstruct(&receivedPayload);

    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_COMPUTE_MAG_CALIB, NULL, 0);

        if (errorCode == SBG_NO_ERROR)
        {
            errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_COMPUTE_MAG_CALIB, &receivedPayload, 5000);

            if (errorCode == SBG_NO_ERROR)
            {
                SbgStreamBuffer     inputStream;
                size_t              i;

                sbgStreamBufferInitForRead(&inputStream, sbgEComProtocolPayloadGetBuffer(&receivedPayload), sbgEComProtocolPayloadGetSize(&receivedPayload));

                //
                // Read quality and status parameters
                //
                pCalibResults->quality          = (SbgEComMagCalibQuality)sbgStreamBufferReadUint8LE(&inputStream);
                pCalibResults->confidence       = (SbgEComMagCalibConfidence)sbgStreamBufferReadUint8LE(&inputStream);
                pCalibResults->advancedStatus   = sbgStreamBufferReadUint16LE(&inputStream);

                pCalibResults->beforeMeanError  = sbgStreamBufferReadFloatLE(&inputStream);
                pCalibResults->beforeStdError   = sbgStreamBufferReadFloatLE(&inputStream);
                pCalibResults->beforeMaxError   = sbgStreamBufferReadFloatLE(&inputStream);

                pCalibResults->afterMeanError   = sbgStreamBufferReadFloatLE(&inputStream);
                pCalibResults->afterStdError    = sbgStreamBufferReadFloatLE(&inputStream);
                pCalibResults->afterMaxError    = sbgStreamBufferReadFloatLE(&inputStream);

                pCalibResults->meanAccuracy     = sbgStreamBufferReadFloatLE(&inputStream);
                pCalibResults->stdAccuracy      = sbgStreamBufferReadFloatLE(&inputStream);
                pCalibResults->maxAccuracy      = sbgStreamBufferReadFloatLE(&inputStream);

                pCalibResults->numPoints        = sbgStreamBufferReadUint16LE(&inputStream);
                pCalibResults->maxNumPoints     = sbgStreamBufferReadUint16LE(&inputStream);

                //
                // Read the computed hard iron offset vector
                //
                pCalibResults->offset[0]        = sbgStreamBufferReadFloatLE(&inputStream);
                pCalibResults->offset[1]        = sbgStreamBufferReadFloatLE(&inputStream);
                pCalibResults->offset[2]        = sbgStreamBufferReadFloatLE(&inputStream);

                //
                // Read the computed soft iron matrix
                //
                for (i = 0; i < 9; i++)
                {
                    pCalibResults->matrix[i]    = sbgStreamBufferReadFloatLE(&inputStream);
                }

                break;
            }
        }
        else
        {
            //
            // Unable to send the command, stop here
            //
            break;
        }
    }

    sbgEComProtocolPayloadDestroy(&receivedPayload);

    return errorCode;
}

//----------------------------------------------------------------------//
//- Private SBG Systems only definitions                               -//
//----------------------------------------------------------------------//

