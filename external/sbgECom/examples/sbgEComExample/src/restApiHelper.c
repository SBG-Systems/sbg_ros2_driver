// sbgCommonLib headers
#include <sbgCommon.h>

// Local headers
#include "cJSON.h"
#include "restApiHelper.h"

//----------------------------------------------------------------------//
//- Public definitions                                                 -//
//----------------------------------------------------------------------//

SbgErrorCode restApiHelperParseDeviceInfo(const cJSON *pJsonNode, DeviceInfo *pDeviceInfo)
{
    SbgErrorCode        errorCode = SBG_NO_ERROR;

    assert(pJsonNode);
    assert(pDeviceInfo);

    const cJSON *pProductCode   = cJSON_GetObjectItemCaseSensitive(pJsonNode, "productCode");
    const cJSON *pSerialNumber  = cJSON_GetObjectItemCaseSensitive(pJsonNode, "serialNumber");
    const cJSON *pHwRevision    = cJSON_GetObjectItemCaseSensitive(pJsonNode, "hwRevision");
    const cJSON *pMnfVersion    = cJSON_GetObjectItemCaseSensitive(pJsonNode, "mnfVersion");
    const cJSON *pCalibVersion  = cJSON_GetObjectItemCaseSensitive(pJsonNode, "calibVersion");
    const cJSON *pFmwVersion    = cJSON_GetObjectItemCaseSensitive(pJsonNode, "fmwVersion");
    const cJSON *pBtVersion     = cJSON_GetObjectItemCaseSensitive(pJsonNode, "btVersion");

    if (cJSON_IsString(pProductCode) && (pProductCode->valuestring) )
    {
        strncpy(pDeviceInfo->productCode, pProductCode->valuestring, sizeof(pDeviceInfo->productCode));
        pDeviceInfo->productCode[sizeof(pDeviceInfo->productCode)-1] = '\0';
    }
    else
    {
        errorCode = SBG_INVALID_PARAMETER;
        SBG_LOG_WARNING(errorCode, "'productCode' is malformatted");
    }

    if (cJSON_IsString(pSerialNumber) && (pSerialNumber->valuestring) )
    {
        strncpy(pDeviceInfo->serialNumber, pSerialNumber->valuestring, sizeof(pDeviceInfo->serialNumber));
        pDeviceInfo->serialNumber[sizeof(pDeviceInfo->serialNumber)-1] = '\0';
    }
    else
    {
        errorCode = SBG_INVALID_PARAMETER;
        SBG_LOG_WARNING(errorCode, "'serialNumber' is malformatted");
    }

    if (cJSON_IsString(pHwRevision) && (pHwRevision->valuestring) )
    {
        strncpy(pDeviceInfo->hwRevision, pHwRevision->valuestring, sizeof(pDeviceInfo->hwRevision));
        pDeviceInfo->hwRevision[sizeof(pDeviceInfo->hwRevision)-1] = '\0';
    }
    else
    {
        errorCode = SBG_INVALID_PARAMETER;
        SBG_LOG_WARNING(errorCode, "'hwRevision' is malformatted");
    }

    //
    // In initial sbgInsRestApi implementation, the field "mnfVersion" was called "calibVersion"
    // Handle this specific case to support both implementations
    // 
    // It will be removed in future release
    //
    if (cJSON_IsString(pMnfVersion) && (pMnfVersion->valuestring) )
    {
        strncpy(pDeviceInfo->mnfVersion, pMnfVersion->valuestring, sizeof(pDeviceInfo->mnfVersion));
        pDeviceInfo->mnfVersion[sizeof(pDeviceInfo->mnfVersion)-1] = '\0';
    }
    else if (cJSON_IsString(pCalibVersion) && (pCalibVersion->valuestring))
    {
        strncpy(pDeviceInfo->mnfVersion, pCalibVersion->valuestring, sizeof(pDeviceInfo->mnfVersion));
        pDeviceInfo->mnfVersion[sizeof(pDeviceInfo->mnfVersion)-1] = '\0';
    }
    else
    {
        errorCode = SBG_INVALID_PARAMETER;
        SBG_LOG_WARNING(errorCode, "'mnfVersion' is malformatted");
    }

    if (cJSON_IsString(pFmwVersion) && (pFmwVersion->valuestring) )
    {
        strncpy(pDeviceInfo->fmwVersion, pFmwVersion->valuestring, sizeof(pDeviceInfo->fmwVersion));
        pDeviceInfo->fmwVersion[sizeof(pDeviceInfo->fmwVersion)-1] = '\0';
    }
    else
    {
        errorCode = SBG_INVALID_PARAMETER;
        SBG_LOG_WARNING(errorCode, "'fmwVersion' is malformatted");
    }

    if (cJSON_IsString(pBtVersion) && (pBtVersion->valuestring) )
    {
        strncpy(pDeviceInfo->btVersion, pBtVersion->valuestring, sizeof(pDeviceInfo->btVersion));
        pDeviceInfo->btVersion[sizeof(pDeviceInfo->btVersion)-1] = '\0';
    }
    else
    {
        errorCode = SBG_INVALID_PARAMETER;
        SBG_LOG_WARNING(errorCode, "'btVersion' is malformatted");
    }
    
    return errorCode;
}

SbgErrorCode restApiHelperPrintErrorDetails(const char *pContent)
{
    SbgErrorCode     errorCode = SBG_NO_ERROR;
    cJSON           *pJsonNode;

    assert(pContent);

    pJsonNode = cJSON_Parse(pContent);

    if (pJsonNode)
    {
        const cJSON *pStatus    = cJSON_GetObjectItemCaseSensitive(pJsonNode, "status");
        const cJSON *pTitle     = cJSON_GetObjectItemCaseSensitive(pJsonNode, "title");
        const cJSON *pDetail    = cJSON_GetObjectItemCaseSensitive(pJsonNode, "detail");

        if (cJSON_IsNumber(pStatus) && cJSON_IsString(pTitle) && cJSON_IsString(pDetail) )
        {
            printf("     status: %"PRIu32"\n",  pStatus->valueint);
            printf("      title: %s\n",         pTitle->valuestring);
            printf("    details: %s\n",         pDetail->valuestring);
            printf("\n");
        }
        else
        {
            errorCode = SBG_INVALID_PARAMETER;
            SBG_LOG_WARNING(errorCode, "JSON is malformed");
        }
        
        cJSON_Delete(pJsonNode);
    }
    else
    {
        errorCode = SBG_ERROR;
        SBG_LOG_ERROR(errorCode, "Unable to parse the JSON content");
    }
    
    return errorCode;
}

SbgErrorCode restApiHelperParseSuccess(const char *pContent, bool *pNeedReboot)
{
    SbgErrorCode     errorCode = SBG_NO_ERROR;
    cJSON           *pJsonNode;

    assert(pContent);
    assert(pNeedReboot);

    pJsonNode = cJSON_Parse(pContent);

    if (pJsonNode)
    {
        const cJSON *pJsonNeedReboot    = cJSON_GetObjectItemCaseSensitive(pJsonNode, "needReboot");

        if (cJSON_IsTrue(pJsonNeedReboot))
        {
            *pNeedReboot = true;
        }
        else if (cJSON_IsFalse(pJsonNeedReboot))
        {
            *pNeedReboot = false;
        }
        else
        {
            errorCode = SBG_INVALID_PARAMETER;
            SBG_LOG_WARNING(errorCode, "JSON is malformed");
        }
        
        cJSON_Delete(pJsonNode);
    }
    else
    {
        errorCode = SBG_ERROR;
        SBG_LOG_ERROR(errorCode, "Unable to parse the JSON content");
    }
    
    return errorCode;
}
