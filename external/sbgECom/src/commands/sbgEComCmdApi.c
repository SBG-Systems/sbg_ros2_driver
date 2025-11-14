// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <sbgECom.h>

// Local headers
#include "sbgEComCmdApi.h"
#include "sbgEComCmdCommon.h"

//----------------------------------------------------------------------//
//- Constant definitions                                               -//
//----------------------------------------------------------------------//

/*!
 * Status code indicating success.
 */
#define SBG_ECOM_CMD_API_STATUS_CODE_OK                     (200)

/*!
 * Status code indicating an internal server error.
 */
#define SBG_ECOM_CMD_API_STATUS_CODE_INTERNAL_SERVER_ERROR  (500)

//----------------------------------------------------------------------//
//- Private methods                                                    -//
//----------------------------------------------------------------------//

/*!
 * Parse the payload of a REST API reply.
 *
 * \param[in]   pReply                      REST API reply.
 * \return                                  SBG_NO_ERROR if successful.
 */
static SbgErrorCode sbgEComCmdApiReplyParsePayload(SbgEComCmdApiReply *pReply)
{
    SbgErrorCode                         errorCode;
    SbgStreamBuffer                      streamBuffer;
    uint16_t                             statusCode;

    assert(pReply);

    sbgStreamBufferInitForRead(&streamBuffer, sbgEComProtocolPayloadGetBuffer(&pReply->payload), sbgEComProtocolPayloadGetSize(&pReply->payload));

    statusCode = sbgStreamBufferReadUint16LE(&streamBuffer);

    errorCode = sbgStreamBufferGetLastError(&streamBuffer);

    if (errorCode == SBG_NO_ERROR)
    {
        const char                      *pContent;
        size_t                           size;

        pContent    = sbgStreamBufferGetCursor(&streamBuffer);
        size        = sbgStreamBufferGetSpace(&streamBuffer);

        if (size != 0)
        {
            if (pContent[size - 1] == '\0')
            {
                pReply->statusCode  = statusCode;
                pReply->pContent    = pContent;

                errorCode = SBG_NO_ERROR;
            }
            else
            {
                errorCode = SBG_INVALID_FRAME;
                SBG_LOG_ERROR(errorCode, "invalid content format");
            }
        }
        else
        {
            errorCode = SBG_INVALID_FRAME;
            SBG_LOG_ERROR(errorCode, "invalid content size");
        }
    }
    else
    {
        SBG_LOG_ERROR(errorCode, "unable to read status code");
    }

    return errorCode;
}

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

void sbgEComCmdApiReplyConstruct(SbgEComCmdApiReply *pReply)
{
    assert(pReply);

    sbgEComProtocolPayloadConstruct(&pReply->payload);

    pReply->statusCode  = SBG_ECOM_CMD_API_STATUS_CODE_INTERNAL_SERVER_ERROR;
    pReply->pContent    = NULL;
}

void sbgEComCmdApiReplyDestroy(SbgEComCmdApiReply *pReply)
{
    assert(pReply);

    sbgEComProtocolPayloadDestroy(&pReply->payload);
}

bool sbgEComCmdApiReplySuccessful(const SbgEComCmdApiReply *pReply)
{
    assert(pReply);

    return pReply->statusCode == SBG_ECOM_CMD_API_STATUS_CODE_OK;
}

SbgErrorCode sbgEComCmdApiGet(SbgEComHandle *pHandle, const char *pPath, const char *pQuery, SbgEComCmdApiReply *pReply)
{
    SbgErrorCode                         errorCode;
    uint8_t                             *pSendBuffer;
    SbgStreamBuffer                      streamBuffer;
    size_t                               pathSize;
    size_t                               querySize;
    size_t                               size;

    assert(pHandle);
    assert(pPath);
    assert(pReply);

    if (!pQuery)
    {
        pQuery = "";
    }

    pathSize    = strlen(pPath) + 1;
    querySize   = strlen(pQuery) + 1;
    size        = pathSize + querySize;

    pSendBuffer = malloc(size);

    if (pSendBuffer)
    {
        sbgStreamBufferInitForWrite(&streamBuffer, pSendBuffer, size);

        sbgStreamBufferWriteBuffer(&streamBuffer, pPath, pathSize);
        errorCode = sbgStreamBufferWriteBuffer(&streamBuffer, pQuery, querySize);
        assert(errorCode == SBG_NO_ERROR);

        for (uint32_t i = 0; i < pHandle->numTrials; i++)
        {
            errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_API_GET, sbgStreamBufferGetLinkedBuffer(&streamBuffer), sbgStreamBufferGetLength(&streamBuffer));

            if (errorCode == SBG_NO_ERROR)
            {
                errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_API_GET, &pReply->payload, pHandle->cmdDefaultTimeOut);

                if (errorCode == SBG_NO_ERROR)
                {
                    errorCode = sbgEComCmdApiReplyParsePayload(pReply);
                    break;
                }
            }
            else
            {
                break;
            }
        }

        free(pSendBuffer);
    }
    else
    {
        errorCode = SBG_MALLOC_FAILED;
        SBG_LOG_ERROR(errorCode, "unable allocate buffer");
    }

    return errorCode;
}

SbgErrorCode sbgEComCmdApiPost(SbgEComHandle *pHandle, const char *pPath, const char *pQuery, const char *pBody, SbgEComCmdApiReply *pReply)
{
    SbgErrorCode                         errorCode;
    uint8_t                             *pSendBuffer;
    SbgStreamBuffer                      streamBuffer;
    size_t                               pathSize;
    size_t                               querySize;
    size_t                               bodySize;
    size_t                               size;

    assert(pHandle);
    assert(pPath);
    assert(pReply);

    if (!pQuery)
    {
        pQuery = "";
    }

    if (!pBody)
    {
        pBody = "";
    }

    pathSize    = strlen(pPath) + 1;
    querySize   = strlen(pQuery) + 1;
    bodySize    = strlen(pBody) + 1;
    size        = pathSize + querySize + bodySize;

    pSendBuffer = malloc(size);

    if (pSendBuffer)
    {
        sbgStreamBufferInitForWrite(&streamBuffer, pSendBuffer, size);

        sbgStreamBufferWriteBuffer(&streamBuffer, pPath, pathSize);
        sbgStreamBufferWriteBuffer(&streamBuffer, pQuery, querySize);
        errorCode = sbgStreamBufferWriteBuffer(&streamBuffer, pBody, bodySize);
        assert(errorCode == SBG_NO_ERROR);

        for (uint32_t i = 0; i < pHandle->numTrials; i++)
        {
            errorCode = sbgEComProtocolSend(&pHandle->protocolHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_API_POST, sbgStreamBufferGetLinkedBuffer(&streamBuffer), sbgStreamBufferGetLength(&streamBuffer));

            if (errorCode == SBG_NO_ERROR)
            {
                errorCode = sbgEComReceiveCmd2(pHandle, SBG_ECOM_CLASS_LOG_CMD_0, SBG_ECOM_CMD_API_POST, &pReply->payload, pHandle->cmdDefaultTimeOut);

                if (errorCode == SBG_NO_ERROR)
                {
                    errorCode = sbgEComCmdApiReplyParsePayload(pReply);
                    break;
                }
            }
            else
            {
                break;
            }
        }

        free(pSendBuffer);
    }
    else
    {
        errorCode = SBG_MALLOC_FAILED;
        SBG_LOG_ERROR(errorCode, "unable allocate buffer");
    }

    return errorCode;
}
