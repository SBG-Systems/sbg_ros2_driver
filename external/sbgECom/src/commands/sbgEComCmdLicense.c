// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <sbgECom.h>
#include <transfer/sbgEComTransfer.h>

// Local headers
#include "sbgEComCmdCommon.h"
#include "sbgEComCmdLicense.h"


//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComCmdLicenseApply(SbgEComHandle *pHandle, const void *pBuffer, size_t size)
{
    SbgErrorCode    errorCode;
    uint32_t        currentTimeOut;

    assert(pHandle);
    assert(pBuffer);
    assert(size > 0);

    //
    // Define a time out of 10s to let enough time for the GNSS receiver to apply the license
    //
    currentTimeOut = pHandle->cmdDefaultTimeOut;
    pHandle->cmdDefaultTimeOut = 10000;

    //
    // Call function that handle data transfer
    //
    errorCode = sbgEComTransferSend(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_LICENSE_APPLY, pBuffer, size);

    //
    // Restore the default time out
    //
    pHandle->cmdDefaultTimeOut = currentTimeOut;

    return errorCode;
}
