// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <sbgECom.h>

// Local headers
#include "sbgEComCmdCommon.h"
#include "sbgEComCmdGnss.h"

//----------------------------------------------------------------------//
//- Private methods                                                    -//
//----------------------------------------------------------------------//

/*!
 * Set GNSS error model id.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   id                          Model ID to set
 * \param[in]   cmdId                       The command identifier to set parameters for a specific GNSS module.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
static SbgErrorCode sbgEComCmdGnssSetModelId(SbgEComHandle *pHandle, SbgEComGnssModelsStdIds modelId, SbgEComCmd cmdId)
{
    assert(pHandle);

    return sbgEComCmdGenericSetModelId(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, cmdId, modelId);
}

/*!
 * Retrieve GNSS error model id.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pModelId                    Retrieved model id.
 * \param[in]   cmdId                       The command identifier to get parameters for a specific GNSS module.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
static SbgErrorCode sbgEComCmdGnssGetModelId(SbgEComHandle *pHandle, SbgEComGnssModelsStdIds *pModelId, SbgEComCmd cmdId)
{
    SbgErrorCode    errorCode = SBG_NO_ERROR;
    uint32_t        modelIdAsUint;

    assert(pHandle);
    assert(pModelId);

    errorCode = sbgEComCmdGenericGetModelId(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, cmdId, &modelIdAsUint);

    if (errorCode == SBG_NO_ERROR)
    {
        *pModelId = (SbgEComGnssModelsStdIds)modelIdAsUint;
    }

    return errorCode;
}

/*!
 * Retrieve the mechanical installation parameters for the GNSS # module.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pGnssInstallation           Used to store the retrieved the GNSS installation parameters.
 * \param[in]   cmdId                       The command identifier to get parameters for a specific GNSS module.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
static SbgErrorCode sbgEComCmdGnssInstallationGet(SbgEComHandle *pHandle, SbgEComGnssInstallation *pGnssInstallation, SbgEComCmd cmdId)
{
    SbgErrorCode            errorCode = SBG_NO_ERROR;
    SbgEComProtocolPayload  receivedPayload;
    uint32_t                trial;

    assert(pHandle);
    assert(pGnssInstallation);

    sbgEComProtocolPayloadConstruct(&receivedPayload);

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Send the command only since this is a no-payload command
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, cmdId, NULL, 0);

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, cmdId, &receivedPayload, pHandle->cmdDefaultTimeOut);

            //
            // Test if we have received a valid answer
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
                pGnssInstallation->leverArmPrimary[0]       = sbgStreamBufferReadFloatLE(&inputStream);
                pGnssInstallation->leverArmPrimary[1]       = sbgStreamBufferReadFloatLE(&inputStream);
                pGnssInstallation->leverArmPrimary[2]       = sbgStreamBufferReadFloatLE(&inputStream);
                pGnssInstallation->leverArmPrimaryPrecise   = sbgStreamBufferReadBooleanLE(&inputStream);

                pGnssInstallation->leverArmSecondary[0]     = sbgStreamBufferReadFloatLE(&inputStream);
                pGnssInstallation->leverArmSecondary[1]     = sbgStreamBufferReadFloatLE(&inputStream);
                pGnssInstallation->leverArmSecondary[2]     = sbgStreamBufferReadFloatLE(&inputStream);
                pGnssInstallation->leverArmSecondaryMode    = (SbgEComGnssInstallationMode)sbgStreamBufferReadUint8LE(&inputStream);
                
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

/*!
 * Set the mechanical installation parameters for the GNSS # module.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[in]   pGnssInstallation           The GNSS installation parameters to set.
 * \param[in]   cmdId                       The command identifier to set parameters for a specific GNSS module.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
static SbgErrorCode sbgEComCmdGnssInstallationSet(SbgEComHandle *pHandle, const SbgEComGnssInstallation *pGnssInstallation, SbgEComCmd cmdId)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    uint32_t            trial;
    uint8_t             outputBuffer[64];
    SbgStreamBuffer     outputStream;

    assert(pHandle);
    assert(pGnssInstallation);

    //
    // Send the command three times
    //
    for (trial = 0; trial < pHandle->numTrials; trial++)
    {
        //
        // Initialize stream buffer for output
        //
        sbgStreamBufferInitForWrite(&outputStream, outputBuffer, sizeof(outputBuffer));

        //
        // Build payload
        //
        sbgStreamBufferWriteFloatLE(&outputStream, pGnssInstallation->leverArmPrimary[0]);
        sbgStreamBufferWriteFloatLE(&outputStream, pGnssInstallation->leverArmPrimary[1]);
        sbgStreamBufferWriteFloatLE(&outputStream, pGnssInstallation->leverArmPrimary[2]);
        sbgStreamBufferWriteBooleanLE(&outputStream, pGnssInstallation->leverArmPrimaryPrecise);

        sbgStreamBufferWriteFloatLE(&outputStream, pGnssInstallation->leverArmSecondary[0]);
        sbgStreamBufferWriteFloatLE(&outputStream, pGnssInstallation->leverArmSecondary[1]);
        sbgStreamBufferWriteFloatLE(&outputStream, pGnssInstallation->leverArmSecondary[2]);
        sbgStreamBufferWriteUint8LE(&outputStream, pGnssInstallation->leverArmSecondaryMode);

        //
        // Send the payload over ECom
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, cmdId, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, cmdId, pHandle->cmdDefaultTimeOut);

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

/*!
 * Retrieve the rejection configuration of the gnss module.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pAlignConf                  Pointer to a SbgEComGnssRejectionConf struct to hold rejection configuration of the gnss module.
 * \param[in]   cmdId                       The command identifier to get parameters for a specific GNSS module.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
static SbgErrorCode sbgEComCmdGnssGetRejection(SbgEComHandle *pHandle, SbgEComGnssRejectionConf *pRejectConf, SbgEComCmd cmdId)
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
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, cmdId, NULL, 0);

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, cmdId, &receivedPayload, pHandle->cmdDefaultTimeOut);

            //
            // Test if we have received a SBG_ECOM_CMD_GNSS_1_REJECT_MODES command
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
                pRejectConf->position   = (SbgEComRejectionMode)sbgStreamBufferReadUint8LE(&inputStream);
                pRejectConf->velocity   = (SbgEComRejectionMode)sbgStreamBufferReadUint8LE(&inputStream);
                sbgStreamBufferReadUint8LE(&inputStream);                                                   // Skipped for backward compatibility
                pRejectConf->hdt        = (SbgEComRejectionMode)sbgStreamBufferReadUint8LE(&inputStream);

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

/*!
 * Set the rejection configuration of the gnss module.
 *
 * \param[in]   pHandle                     A valid sbgECom handle.
 * \param[out]  pAlignConf                  Pointer to a SbgEComGnssRejectionConf struct holding rejection configuration for the gnss module.
 * \param[in]   cmdId                       The command identifier to set parameters for a specific GNSS module.
 * \return                                  SBG_NO_ERROR if the command has been executed successfully.
 */
static SbgErrorCode sbgEComCmdGnssSetRejection(SbgEComHandle *pHandle, const SbgEComGnssRejectionConf *pRejectConf, SbgEComCmd cmdId)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;
    uint32_t            trial;
    uint8_t             outputBuffer[64];
    SbgStreamBuffer     outputStream;

    assert(pHandle);
    assert(pRejectConf);

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
        sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pRejectConf->position);
        sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pRejectConf->velocity);
        sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)SBG_ECOM_NEVER_ACCEPT_MODE);        // Reserved parameter
        sbgStreamBufferWriteUint8LE(&outputStream, (uint8_t)pRejectConf->hdt);

        //
        // Send the payload over ECom
        //
        errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, cmdId, sbgStreamBufferGetLinkedBuffer(&outputStream), sbgStreamBufferGetLength(&outputStream));

        //
        // Make sure that the command has been sent
        //
        if (errorCode == SBG_NO_ERROR)
        {
            //
            // Try to read the device answer for 500 ms
            //
            errorCode = sbgEComWaitForAck(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, cmdId, pHandle->cmdDefaultTimeOut);

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

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComCmdGnss1SetModelId(SbgEComHandle *pHandle, SbgEComGnssModelsStdIds modelId)
{
    assert(pHandle);

    return sbgEComCmdGnssSetModelId(pHandle, modelId, SBG_ECOM_CMD_GNSS_1_MODEL_ID);
}

SbgErrorCode sbgEComCmdGnss1GetModelId(SbgEComHandle *pHandle, SbgEComGnssModelsStdIds *pModelId)
{
    assert(pHandle);
    assert(pModelId);

    return sbgEComCmdGnssGetModelId(pHandle, pModelId, SBG_ECOM_CMD_GNSS_1_MODEL_ID);
}

SbgErrorCode sbgEComCmdGnss1InstallationGet(SbgEComHandle *pHandle, SbgEComGnssInstallation *pGnssInstallation)
{
    assert(pHandle);
    assert(pGnssInstallation);

    return sbgEComCmdGnssInstallationGet(pHandle, pGnssInstallation, SBG_ECOM_CMD_GNSS_1_INSTALLATION);
}

SbgErrorCode sbgEComCmdGnss1InstallationSet(SbgEComHandle *pHandle, const SbgEComGnssInstallation *pGnssInstallation)
{
    assert(pHandle);
    assert(pGnssInstallation);

    return sbgEComCmdGnssInstallationSet(pHandle, pGnssInstallation, SBG_ECOM_CMD_GNSS_1_INSTALLATION);
}

SbgErrorCode sbgEComCmdGnss1GetRejection(SbgEComHandle *pHandle, SbgEComGnssRejectionConf *pRejectConf)
{
    assert(pHandle);
    assert(pRejectConf);

    return sbgEComCmdGnssGetRejection(pHandle, pRejectConf, SBG_ECOM_CMD_GNSS_1_REJECT_MODES);
}

SbgErrorCode sbgEComCmdGnss1SetRejection(SbgEComHandle *pHandle, const SbgEComGnssRejectionConf *pRejectConf)
{
    assert(pHandle);
    assert(pRejectConf);

    return sbgEComCmdGnssSetRejection(pHandle, pRejectConf, SBG_ECOM_CMD_GNSS_1_REJECT_MODES);
}
