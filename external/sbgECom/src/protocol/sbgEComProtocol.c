// sbgCommonLib headers
#include <sbgCommon.h>
#include <crc/sbgCrc.h>
#include <interfaces/sbgInterface.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Local headers
#include "sbgEComProtocol.h"

//----------------------------------------------------------------------//
//- Constant definitions                                               -//
//----------------------------------------------------------------------//

/*!
 * Delay before another send attempt when sending a large payload, in milliseconds.
 */
#define SBG_ECOM_PROTOCOL_EXT_SEND_DELAY                    (50)

//----------------------------------------------------------------------//
//- Private functions                                                  -//
//----------------------------------------------------------------------//

/*!
 * Clear the content of a payload.
 *
 * Any allocated resource is released, and the payload returns to its constructed state.
 *
 * \param[in]   pPayload                    Payload.
 */
static void sbgEComProtocolPayloadClear(SbgEComProtocolPayload *pPayload)
{
    assert(pPayload);

    if (pPayload->allocated)
    {
        free(pPayload->pBuffer);

        pPayload->allocated = false;
    }

    pPayload->pBuffer   = NULL;
    pPayload->size      = 0;
}

/*!
 * Set the properties of a payload.
 *
 * \param[in]   pPayload                    Payload.
 * \param[in]   allocated                   True if the given buffer is allocated with malloc().
 * \param[in]   pBuffer                     Buffer.
 * \param[in]   size                        Buffer size, in bytes.
 */
static void sbgEComProtocolPayloadSet(SbgEComProtocolPayload *pPayload, bool allocated, void *pBuffer, size_t size)
{
    assert(pPayload);
    assert(pBuffer);

    pPayload->allocated = allocated;
    pPayload->pBuffer   = pBuffer;
    pPayload->size      = size;
}

/*!
 * Discard unused bytes from the work buffer of a protocol.
 *
 * \param[in]   pProtocol                   Protocol.
 */
static void sbgEComProtocolDiscardUnusedBytes(SbgEComProtocol *pProtocol)
{
    assert(pProtocol);

    if (pProtocol->discardSize != 0)
    {
        assert(pProtocol->discardSize <= pProtocol->rxBufferSize);

        memmove(pProtocol->rxBuffer, &pProtocol->rxBuffer[pProtocol->discardSize], pProtocol->rxBufferSize - pProtocol->discardSize);

        pProtocol->rxBufferSize -= pProtocol->discardSize;
        pProtocol->discardSize  = 0;
    }
}

/*!
 * Read data from the underlying interface into the work buffer of a protocol.
 *
 * \param[in]   pProtocol                   Protocol.
 */
static void sbgEComProtocolRead(SbgEComProtocol *pProtocol)
{
    SbgErrorCode                         errorCode;

    assert(pProtocol);

    if (pProtocol->rxBufferSize < sizeof(pProtocol->rxBuffer))
    {
        size_t                           nrBytesRead;

        errorCode = sbgInterfaceRead(pProtocol->pLinkedInterface, &pProtocol->rxBuffer[pProtocol->rxBufferSize], &nrBytesRead, sizeof(pProtocol->rxBuffer) - pProtocol->rxBufferSize);

        if (errorCode == SBG_NO_ERROR)
        {
            pProtocol->rxBufferSize += nrBytesRead;
        }
    }
}

/*!
 * Find SYNC bytes in the work buffer of a protocol.
 *
 * The output offset is set if either SBG_NO_ERROR or SBG_NOT_CONTINUOUS_FRAME is returned.
 *
 * \param[in]   pProtocol                   Protocol.
 * \param[in]   startOffset                 Start offset, in bytes.
 * \param[out]  pOffset                     Offset, in bytes.
 * \return                                  SBG_NO_ERROR if successful,
 *                                          SBG_NOT_CONTINUOUS_FRAME if only the first SYNC byte was found,
 *                                          SBG_NOT_READY otherwise.
 */
static SbgErrorCode sbgEComProtocolFindSyncBytes(SbgEComProtocol *pProtocol, size_t startOffset, size_t *pOffset)
{
    SbgErrorCode                         errorCode;

    assert(pProtocol);
    assert(pOffset);
    assert(pProtocol->rxBufferSize > 0);

    errorCode = SBG_NOT_READY;

    for (size_t i = startOffset; i < (pProtocol->rxBufferSize - 1); i++)
    {
        if ((pProtocol->rxBuffer[i] == SBG_ECOM_SYNC_1) && (pProtocol->rxBuffer[i + 1] == SBG_ECOM_SYNC_2))
        {
            *pOffset    = i;
            errorCode   = SBG_NO_ERROR;
            break;
        }
    }

    //
    // The SYNC bytes were not found, but check if the last byte in the work buffer is the first SYNC byte,
    // as it could result from receiving a partial frame.
    //
    if ((errorCode != SBG_NO_ERROR) && (pProtocol->rxBuffer[pProtocol->rxBufferSize - 1] == SBG_ECOM_SYNC_1))
    {
        *pOffset    = pProtocol->rxBufferSize - 1;
        errorCode   = SBG_NOT_CONTINUOUS_FRAME;
    }

    return errorCode;
}

/*!
 * Parse a frame in the work buffer of a protocol.
 *
 * A non-zero number of pages indicates the reception of an extended frame.
 *
 * \param[in]   pProtocol                   Protocol.
 * \param[in]   offset                      Frame offset in the protocol work buffer.
 * \param[out]  pEndOffset                  Frame end offset in the protocol work buffer.
 * \param[out]  pMsgClass                   Message class.
 * \param[out]  pMsgId                      Message ID.
 * \param[out]  pTransferId                 Transfer ID.
 * \param[out]  pPageIndex                  Page index.
 * \param[out]  pNrPages                    Number of pages.
 * \param[out]  pBuffer                     Payload buffer.
 * \param[out]  pSize                       Payload buffer size, in bytes.
 * \return                                  SBG_NO_ERROR if successful,
 *                                          SBG_NOT_READY if the frame is incomplete,
 *                                          SBG_INVALID_FRAME if the frame is invalid,
 *                                          SBG_INVALID_CRC if the frame CRC is invalid.
 */
static SbgErrorCode sbgEComProtocolParseFrame(SbgEComProtocol *pProtocol, size_t offset, size_t *pEndOffset, uint8_t *pMsgClass, uint8_t *pMsgId, uint8_t *pTransferId, uint16_t *pPageIndex, uint16_t *pNrPages, void **pBuffer, size_t *pSize)
{
    SbgErrorCode                         errorCode;
    SbgStreamBuffer                      streamBuffer;
    uint8_t                              msgId;
    uint8_t                              msgClass;
    size_t                               standardPayloadSize;

    assert(pProtocol);
    assert(offset < pProtocol->rxBufferSize);
    assert(pEndOffset);
    assert(pMsgClass);
    assert(pMsgId);
    assert(pTransferId);
    assert(pPageIndex);
    assert(pNrPages);
    assert(pBuffer);
    assert(pSize);

    sbgStreamBufferInitForRead(&streamBuffer, &pProtocol->rxBuffer[offset], pProtocol->rxBufferSize - offset);

    //
    // Skip SYNC bytes.
    //
    sbgStreamBufferSeek(&streamBuffer, 2, SB_SEEK_CUR_INC);

    msgId               = sbgStreamBufferReadUint8(&streamBuffer);
    msgClass            = sbgStreamBufferReadUint8(&streamBuffer);
    standardPayloadSize = sbgStreamBufferReadUint16LE(&streamBuffer);

    errorCode = sbgStreamBufferGetLastError(&streamBuffer);

    if (errorCode == SBG_NO_ERROR)
    {
        if (standardPayloadSize <= SBG_ECOM_MAX_PAYLOAD_SIZE)
        {
            if (sbgStreamBufferGetSize(&streamBuffer) >= (standardPayloadSize + 9))
            {
                size_t                   payloadSize;
                uint8_t                  transferId;
                uint16_t                 pageIndex;
                uint16_t                 nrPages;

                if ((msgClass & 0x80) == 0)
                {
                    payloadSize = standardPayloadSize;

                    transferId  = 0;
                    pageIndex   = 0;
                    nrPages     = 0;

                    errorCode = SBG_NO_ERROR;
                }
                else
                {
                    msgClass &= 0x7f;

                    //
                    // In extended frames, the payload size includes the extended headers.
                    //
                    payloadSize = standardPayloadSize - 5;

                    transferId  = sbgStreamBufferReadUint8(&streamBuffer);
                    pageIndex   = sbgStreamBufferReadUint16LE(&streamBuffer);
                    nrPages     = sbgStreamBufferReadUint16LE(&streamBuffer);

                    if ((transferId & 0xf0) != 0)
                    {
                        SBG_LOG_WARNING(SBG_INVALID_FRAME, "reserved bits set in extended headers");
                        transferId &= 0xf;
                    }

                    if (pageIndex < nrPages)
                    {
                        errorCode = SBG_NO_ERROR;
                    }
                    else
                    {
                        errorCode = SBG_INVALID_FRAME;
                        SBG_LOG_ERROR(errorCode, "invalid page information : %" PRIu16 "/%" PRIu16, pageIndex, nrPages);
                    }
                }

                if (errorCode == SBG_NO_ERROR)
                {
                    void                *pPayloadAddr;
                    uint16_t             frameCrc;
                    uint8_t              lastByte;

                    pPayloadAddr = sbgStreamBufferGetCursor(&streamBuffer);

                    sbgStreamBufferSeek(&streamBuffer, payloadSize, SB_SEEK_CUR_INC);

                    frameCrc    = sbgStreamBufferReadUint16LE(&streamBuffer);
                    lastByte    = sbgStreamBufferReadUint8(&streamBuffer);

                    assert(sbgStreamBufferGetLastError(&streamBuffer) == SBG_NO_ERROR);

                    if (lastByte == SBG_ECOM_ETX)
                    {
                        uint16_t         computedCrc;

                        //
                        // The CRC spans from the header (excluding the SYNC bytes) up to the CRC bytes.
                        //
                        sbgStreamBufferSeek(&streamBuffer, 2, SB_SEEK_SET);
                        computedCrc = sbgCrc16Compute(sbgStreamBufferGetCursor(&streamBuffer), standardPayloadSize + 4);

                        if (frameCrc == computedCrc)
                        {
                            *pEndOffset     = offset + standardPayloadSize + 9;
                            *pMsgClass      = msgClass;
                            *pMsgId         = msgId;
                            *pTransferId    = transferId;
                            *pPageIndex     = pageIndex;
                            *pNrPages       = nrPages;
                            *pBuffer        = pPayloadAddr;
                            *pSize          = payloadSize;

                            errorCode = SBG_NO_ERROR;
                        }
                        else
                        {
                            errorCode = SBG_INVALID_CRC;
                            SBG_LOG_ERROR(errorCode, "invalid CRC, frame:%#" PRIx16 " computed:%#" PRIx16, frameCrc, computedCrc);
                        }
                    }
                    else
                    {
                        errorCode = SBG_INVALID_FRAME;
                        SBG_LOG_ERROR(errorCode, "invalid end-of-frame: byte:%#" PRIx8, lastByte);
                    }
                }
            }
            else
            {
                errorCode = SBG_NOT_READY;
            }
        }
        else
        {
            errorCode = SBG_INVALID_FRAME;
            SBG_LOG_ERROR(errorCode, "invalid payload size %zu", standardPayloadSize);
        }
    }
    else
    {
        errorCode = SBG_NOT_READY;
    }

    return errorCode;
}

/*!
 * Find a frame in the work buffer of a protocol.
 *
 * If an extended frame is received, the number of pages is set to a non-zero value.
 *
 * \param[in]   pProtocol                   Protocol.
 * \param[out]  pMsgClass                   Message class.
 * \param[out]  pMsgId                      Message ID.
 * \param[out]  pTransferId                 Transfer ID.
 * \param[out]  pPageIndex                  Page index.
 * \param[out]  pNrPages                    Number of pages.
 * \param[out]  pBuffer                     Payload buffer.
 * \param[out]  pSize                       Payload buffer size, in bytes.
 * \return                                  SBG_NO_ERROR if successful,
 *                                          SBG_NOT_READY if no frame was found.
 */
static SbgErrorCode sbgEComProtocolFindFrame(SbgEComProtocol *pProtocol, uint8_t *pMsgClass, uint8_t *pMsgId, uint8_t *pTransferId, uint16_t *pPageIndex, uint16_t *pNrPages, void **pBuffer, size_t *pSize)
{
    SbgErrorCode                         errorCode;
    size_t                               startOffset;

    assert(pProtocol);

    errorCode   = SBG_NOT_READY;
    startOffset = 0;

    while (startOffset < pProtocol->rxBufferSize)
    {
        size_t                           offset;

        errorCode = sbgEComProtocolFindSyncBytes(pProtocol, startOffset, &offset);

        if (errorCode == SBG_NO_ERROR)
        {
            size_t                       endOffset;

            errorCode = sbgEComProtocolParseFrame(pProtocol, offset, &endOffset, pMsgClass, pMsgId, pTransferId, pPageIndex, pNrPages, pBuffer, pSize);

            if (errorCode == SBG_NO_ERROR)
            {
                //
                // Valid frame found, discard all data up to and including that frame
                // on the next read.
                //
                pProtocol->discardSize = endOffset;
                
                //
                // If installed, call the method used to intercept received sbgECom frames
                //
                if (pProtocol->pReceiveFrameCb)
                {
                    SbgStreamBuffer     fullFrameStream;

                    sbgStreamBufferInitForRead(&fullFrameStream, &pProtocol->rxBuffer[offset], endOffset-offset);
                    pProtocol->pReceiveFrameCb(pProtocol, *pMsgClass, *pMsgId, &fullFrameStream, pProtocol->pUserArg);
                }

                break;
            }
            else if (errorCode == SBG_NOT_READY)
            {
                //
                // There may be a valid frame at the parse offset, but it's not complete.
                // Have all preceding bytes discarded on the next read.
                //
                pProtocol->discardSize = offset;
                break;
            }
            else
            {
                //
                // Not a valid frame, skip SYNC bytes and try again.
                //
                startOffset = offset + 2;
                errorCode = SBG_NOT_READY;
            }
        }
        else if (errorCode == SBG_NOT_CONTINUOUS_FRAME)
        {
            //
            // The first SYNC byte was found, but not the second. It may be a valid
            // frame, so keep the SYNC byte but have all preceding bytes discarded
            // on the next read.
            //
            pProtocol->discardSize = offset;
            errorCode = SBG_NOT_READY;
            break;
        }
        else
        {
            //
            // No SYNC byte found, discard all data.
            //
            pProtocol->rxBufferSize = 0;
            errorCode = SBG_NOT_READY;
            break;
        }
    }

    assert(pProtocol->discardSize <= pProtocol->rxBufferSize);

    return errorCode;
}

/*!
 * Send a standard frame.
 *
 * \param[in]   pProtocol                   Protocol.
 * \param[in]   msgClass                    Message class.
 * \param[in]   msgId                       Message ID.
 * \param[in]   pData                       Data buffer.
 * \param[in]   size                        Data buffer size, in bytes.
 * \return                                  SBG_NO_ERROR if successful.
 */
static SbgErrorCode sbgEComProtocolSendStandardFrame(SbgEComProtocol *pProtocol, uint8_t msgClass, uint8_t msgId, const void *pData, size_t size)
{
    uint8_t                              buffer[SBG_ECOM_MAX_BUFFER_SIZE];
    SbgStreamBuffer                      streamBuffer;
    const uint8_t                       *crcDataStart;
    const uint8_t                       *crcDataEnd;
    uint16_t                             crc;

    assert(pProtocol);
    assert((msgClass & 0x80) == 0);
    assert(size <= SBG_ECOM_MAX_PAYLOAD_SIZE);
    assert(pData || (size == 0));

    sbgStreamBufferInitForWrite(&streamBuffer, buffer, sizeof(buffer));

    sbgStreamBufferWriteUint8(&streamBuffer, SBG_ECOM_SYNC_1);
    sbgStreamBufferWriteUint8(&streamBuffer, SBG_ECOM_SYNC_2);

    crcDataStart = sbgStreamBufferGetCursor(&streamBuffer);

    sbgStreamBufferWriteUint8(&streamBuffer, msgId);
    sbgStreamBufferWriteUint8(&streamBuffer, msgClass);

    sbgStreamBufferWriteUint16LE(&streamBuffer, (uint16_t)size);

    sbgStreamBufferWriteBuffer(&streamBuffer, pData, size);

    crcDataEnd = sbgStreamBufferGetCursor(&streamBuffer);

    crc = sbgCrc16Compute(crcDataStart, crcDataEnd - crcDataStart);

    sbgStreamBufferWriteUint16LE(&streamBuffer, crc);

    sbgStreamBufferWriteUint8(&streamBuffer, SBG_ECOM_ETX);

    assert(sbgStreamBufferGetLastError(&streamBuffer) == SBG_NO_ERROR);

    return sbgInterfaceWrite(pProtocol->pLinkedInterface, sbgStreamBufferGetLinkedBuffer(&streamBuffer), sbgStreamBufferGetLength(&streamBuffer));
}

/*!
 * Send an extended frame.
 *
 * \param[in]   pProtocol                   Protocol.
 * \param[in]   msgClass                    Message class.
 * \param[in]   msgId                       Message ID.
 * \param[in]   transferId                  Transfer ID.
 * \param[in]   pageIndex                   Page index (0 to 65534)
 * \param[in]   nrPages                     Total number of pages (1 to 65535)
 * \param[in]   pData                       Data buffer.
 * \param[in]   size                        Data buffer size, in bytes.
 * \return                                  SBG_NO_ERROR if successful.
 */
static SbgErrorCode sbgEComProtocolSendExtendedFrame(SbgEComProtocol *pProtocol, uint8_t msgClass, uint8_t msgId, uint8_t transferId, size_t pageIndex, size_t nrPages, const void *pData, size_t size)
{
    SbgErrorCode                         errorCode;
    uint8_t                              buffer[SBG_ECOM_MAX_BUFFER_SIZE];
    SbgStreamBuffer                      streamBuffer;
    const uint8_t                       *crcDataStart;
    const uint8_t                       *crcDataEnd;
    uint16_t                             crc;

    assert(pProtocol);
    assert((msgClass & 0x80) == 0);
    assert((transferId & 0xf0) == 0);
    assert(pageIndex < UINT16_MAX);
    assert((nrPages > 0) && (nrPages <= UINT16_MAX));
    assert(pageIndex < nrPages);
    assert(size <= SBG_ECOM_MAX_EXTENDED_PAYLOAD_SIZE);
    assert(pData || (size == 0));

    sbgStreamBufferInitForWrite(&streamBuffer, buffer, sizeof(buffer));

    sbgStreamBufferWriteUint8(&streamBuffer, SBG_ECOM_SYNC_1);
    sbgStreamBufferWriteUint8(&streamBuffer, SBG_ECOM_SYNC_2);

    crcDataStart = sbgStreamBufferGetCursor(&streamBuffer);

    sbgStreamBufferWriteUint8(&streamBuffer, msgId);
    sbgStreamBufferWriteUint8(&streamBuffer, 0x80 | msgClass);

    //
    // For compatibility reasons, the size must span over the extended headers.
    //
    sbgStreamBufferWriteUint16LE(&streamBuffer, (uint16_t)size + 5);

    sbgStreamBufferWriteUint8(&streamBuffer, transferId);
    sbgStreamBufferWriteUint16LE(&streamBuffer, (uint16_t)pageIndex);
    sbgStreamBufferWriteUint16LE(&streamBuffer, (uint16_t)nrPages);

    sbgStreamBufferWriteBuffer(&streamBuffer, pData, size);

    crcDataEnd = sbgStreamBufferGetCursor(&streamBuffer);

    crc = sbgCrc16Compute(crcDataStart, crcDataEnd - crcDataStart);

    sbgStreamBufferWriteUint16LE(&streamBuffer, crc);

    sbgStreamBufferWriteUint8(&streamBuffer, SBG_ECOM_ETX);

    assert(sbgStreamBufferGetLastError(&streamBuffer) == SBG_NO_ERROR);

    for (;;)
    {
        errorCode = sbgInterfaceWrite(pProtocol->pLinkedInterface, sbgStreamBufferGetLinkedBuffer(&streamBuffer), sbgStreamBufferGetLength(&streamBuffer));

        if (errorCode != SBG_BUFFER_OVERFLOW)
        {
            break;
        }

        sbgSleep(SBG_ECOM_PROTOCOL_EXT_SEND_DELAY);
    }

    return errorCode;
}

/*!
 * Get a transfer ID for the next large send.
 *
 * This function returns a different ID every time it's called.
 *
 * \param[in]   pProtocol                   Protocol.
 * \return                                  Transfer ID of the next large send.
 */
static SbgErrorCode sbgEComProtocolGetTxId(SbgEComProtocol *pProtocol)
{
    uint8_t                              transferId;

    assert(pProtocol);
    assert((pProtocol->nextLargeTxId & 0xf0) == 0);

    transferId = pProtocol->nextLargeTxId;
    pProtocol->nextLargeTxId = (pProtocol->nextLargeTxId + 1) & 0xf0;

    return transferId;
}

/*!
 * Check if a large transfer is in progress.
 *
 * \param[in]   pProtocol                   Protocol.
 * \return                                  True if a large transfer is in progress.
 */
static bool sbgEComProtocolLargeTransferInProgress(const SbgEComProtocol *pProtocol)
{
    assert(pProtocol);

    return pProtocol->pLargeBuffer;
}

/*!
 * Reset The large transfer member variables of a protocol.
 *
 * \param[in]   pProtocol                   Protocol.
 */
static void sbgEComProtocolResetLargeTransfer(SbgEComProtocol *pProtocol)
{
    assert(pProtocol);

    pProtocol->pLargeBuffer     = NULL;
    pProtocol->largeBufferSize  = 0;
    pProtocol->transferId       = 0;
    pProtocol->pageIndex        = 0;
    pProtocol->nrPages          = 0;
}

/*!
 * Clear any large transfer in progress.
 *
 * \param[in]   pProtocol                   Protocol.
 */
static void sbgEComProtocolClearLargeTransfer(SbgEComProtocol *pProtocol)
{
    assert(pProtocol);

    free(pProtocol->pLargeBuffer);

    sbgEComProtocolResetLargeTransfer(pProtocol);
}

/*!
 * Process an extended frame.
 *
 * \param[in]   pProtocol                   Protocol.
 * \param[in]   msgClass                    Message class.
 * \param[in]   msgId                       Message ID.
 * \param[in]   transferId                  Transfer ID.
 * \param[in]   pageIndex                   Page index.
 * \param[in]   nrPages                     Number of pages.
 * \param[in]   pBuffer                     Buffer.
 * \param[in]   size                        Buffer size, in bytes.
 * \return                                  SBG_NO_ERROR if a large transfer is complete,
 *                                          SBG_NOT_READY otherwise.
 */
static SbgErrorCode sbgEComProtocolProcessExtendedFrame(SbgEComProtocol *pProtocol, uint8_t msgClass, uint8_t msgId, uint8_t transferId, uint16_t pageIndex, uint16_t nrPages, const void *pBuffer, size_t size)
{
    SbgErrorCode                         errorCode;

    assert(pProtocol);
    assert((transferId & 0xf0) == 0);
    assert(pageIndex < nrPages);
    assert(pBuffer || (size == 0));
    assert(size <= SBG_ECOM_MAX_EXTENDED_PAYLOAD_SIZE);

    if (pageIndex == 0)
    {
        size_t                           capacity;

        if (sbgEComProtocolLargeTransferInProgress(pProtocol))
        {
            SBG_LOG_ERROR(SBG_ERROR, "large transfer started while a large transfer is in progress");
            SBG_LOG_ERROR(SBG_ERROR, "terminating large transfer");

            sbgEComProtocolClearLargeTransfer(pProtocol);
        }

        capacity = nrPages * SBG_ECOM_MAX_EXTENDED_PAYLOAD_SIZE;

        pProtocol->pLargeBuffer = malloc(capacity);

        if (pProtocol->pLargeBuffer)
        {
            pProtocol->largeBufferSize  = 0;
            pProtocol->msgClass         = msgClass;
            pProtocol->msgId            = msgId;
            pProtocol->transferId       = transferId;
            pProtocol->pageIndex        = 0;
            pProtocol->nrPages          = nrPages;

            errorCode = SBG_NO_ERROR;
        }
        else
        {
            SBG_LOG_ERROR(SBG_MALLOC_FAILED, "unable to allocate buffer");

            sbgEComProtocolResetLargeTransfer(pProtocol);

            errorCode = SBG_NOT_READY;
        }
    }
    else
    {
        if (sbgEComProtocolLargeTransferInProgress(pProtocol))
        {
            errorCode = SBG_NO_ERROR;
        }
        else
        {
            SBG_LOG_ERROR(SBG_ERROR, "extended frame received while no large transfer in progress");

            errorCode = SBG_NOT_READY;
        }
    }

    if (errorCode == SBG_NO_ERROR)
    {
        if (msgClass == pProtocol->msgClass)
        {
            errorCode = SBG_NO_ERROR;
        }
        else
        {
            SBG_LOG_ERROR(SBG_ERROR, "message class mismatch in extended frame");
            SBG_LOG_ERROR(SBG_ERROR, "terminating large transfer");

            sbgEComProtocolClearLargeTransfer(pProtocol);

            errorCode = SBG_NOT_READY;
        }

        if (msgId == pProtocol->msgId)
        {
            errorCode = SBG_NO_ERROR;
        }
        else
        {
            SBG_LOG_ERROR(SBG_ERROR, "message ID mismatch in extended frame");
            SBG_LOG_ERROR(SBG_ERROR, "terminating large transfer");

            sbgEComProtocolClearLargeTransfer(pProtocol);

            errorCode = SBG_NOT_READY;
        }

        if (transferId == pProtocol->transferId)
        {
            errorCode = SBG_NO_ERROR;
        }
        else
        {
            SBG_LOG_ERROR(SBG_ERROR, "transfer ID mismatch in extended frame");
            SBG_LOG_ERROR(SBG_ERROR, "terminating large transfer");

            sbgEComProtocolClearLargeTransfer(pProtocol);

            errorCode = SBG_NOT_READY;
        }

        if (errorCode == SBG_NO_ERROR)
        {
            if (nrPages == pProtocol->nrPages)
            {
                if (pageIndex == pProtocol->pageIndex)
                {
                    size_t               offset;

                    offset = pageIndex * SBG_ECOM_MAX_EXTENDED_PAYLOAD_SIZE;
                    memcpy(&pProtocol->pLargeBuffer[offset], pBuffer, size);

                    pProtocol->largeBufferSize += size;
                    pProtocol->pageIndex++;

                    if (pProtocol->pageIndex != pProtocol->nrPages)
                    {
                        errorCode = SBG_NOT_READY;
                    }
                }
                else
                {
                    SBG_LOG_ERROR(SBG_ERROR, "extended frame received out of sequence");
                    SBG_LOG_ERROR(SBG_ERROR, "terminating large transfer");

                    sbgEComProtocolClearLargeTransfer(pProtocol);

                    errorCode = SBG_NOT_READY;
                }
            }
            else
            {
                SBG_LOG_ERROR(SBG_ERROR, "page count mismatch in extended frame");
                SBG_LOG_ERROR(SBG_ERROR, "terminating large transfer");

                sbgEComProtocolClearLargeTransfer(pProtocol);

                errorCode = SBG_NOT_READY;
            }
        }
    }

    return errorCode;
}

//----------------------------------------------------------------------//
//- Public methods (SbgEComProtocolPayload)                            -//
//----------------------------------------------------------------------//

void sbgEComProtocolPayloadConstruct(SbgEComProtocolPayload *pPayload)
{
    assert(pPayload);

    pPayload->allocated = false;
    pPayload->pBuffer   = NULL;
    pPayload->size      = 0;
}

void sbgEComProtocolPayloadDestroy(SbgEComProtocolPayload *pPayload)
{
    assert(pPayload);

    if (pPayload->allocated)
    {
        free(pPayload->pBuffer);
    }
}

const void *sbgEComProtocolPayloadGetBuffer(const SbgEComProtocolPayload *pPayload)
{
    assert(pPayload);

    return pPayload->pBuffer;
}

size_t sbgEComProtocolPayloadGetSize(const SbgEComProtocolPayload *pPayload)
{
    assert(pPayload);

    return pPayload->size;
}

void *sbgEComProtocolPayloadMoveBuffer(SbgEComProtocolPayload *pPayload)
{
    void                                *pBuffer;

    assert(pPayload);

    if (pPayload->pBuffer)
    {
        if (pPayload->allocated)
        {
            pBuffer = pPayload->pBuffer;

            sbgEComProtocolPayloadConstruct(pPayload);
        }
        else
        {
            pBuffer = malloc(pPayload->size);

            if (pBuffer)
            {
                memcpy(pBuffer, pPayload->pBuffer, pPayload->size);

                sbgEComProtocolPayloadConstruct(pPayload);
            }
            else
            {
                SBG_LOG_ERROR(SBG_MALLOC_FAILED, "unable to allocate buffer");
            }
        }
    }
    else
    {
        pBuffer = NULL;
    }

    return pBuffer;
}

//----------------------------------------------------------------------//
//- Public methods (SbgEComProtocol)                                   -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComProtocolInit(SbgEComProtocol *pProtocol, SbgInterface *pInterface)
{
    assert(pProtocol);
    assert(pInterface);

    memset(pProtocol, 0x00, sizeof(*pProtocol));

    pProtocol->pLinkedInterface = pInterface;

    sbgEComProtocolResetLargeTransfer(pProtocol);

    return SBG_NO_ERROR;
}

SbgErrorCode sbgEComProtocolClose(SbgEComProtocol *pProtocol)
{
    assert(pProtocol);

    pProtocol->pLinkedInterface = NULL;
    pProtocol->rxBufferSize     = 0;
    pProtocol->discardSize      = 0;
    pProtocol->nextLargeTxId    = 0;

    sbgEComProtocolClearLargeTransfer(pProtocol);

    return SBG_NO_ERROR;
}

SbgErrorCode sbgEComProtocolPurgeIncoming(SbgEComProtocol *pProtocol)
{
    SbgErrorCode    errorCode = SBG_NO_ERROR;
    size_t          numBytesRead;
    uint32_t        timeStamp;

    //
    // Reset the work buffer
    //
    pProtocol->rxBufferSize     = 0;
    pProtocol->discardSize      = 0;
    pProtocol->nextLargeTxId    = 0;

    sbgEComProtocolClearLargeTransfer(pProtocol);

    //
    // Try to read all incoming data for at least 100 ms and trash them
    ///
    timeStamp = sbgGetTime();

    do
    {
        errorCode = sbgInterfaceRead(pProtocol->pLinkedInterface, pProtocol->rxBuffer, &numBytesRead, sizeof(pProtocol->rxBuffer));

        if (errorCode != SBG_NO_ERROR)
        {
            SBG_LOG_ERROR(errorCode, "Unable to read data from interface");
            break;
        }
    } while ((sbgGetTime() - timeStamp) < 100);

    //
    // If we still have read some bytes it means we were not able to purge successfully the rx buffer
    //
    if ( (errorCode == SBG_NO_ERROR) && (numBytesRead > 0) )
    {
        errorCode = SBG_ERROR;
        SBG_LOG_ERROR(errorCode, "Unable to purge the rx buffer,  %zu bytes remaining", numBytesRead);
    }

    return errorCode;
}

SbgErrorCode sbgEComProtocolSend(SbgEComProtocol *pProtocol, uint8_t msgClass, uint8_t msgId, const void *pData, size_t size)
{
    SbgErrorCode                         errorCode;

    if (size <= SBG_ECOM_MAX_PAYLOAD_SIZE)
    {
        errorCode = sbgEComProtocolSendStandardFrame(pProtocol, msgClass, msgId, pData, size);
    }
    else
    {
        size_t                           nrPages;

        nrPages = sbgDivCeil(size, SBG_ECOM_MAX_EXTENDED_PAYLOAD_SIZE);

        if (nrPages <= UINT16_MAX)
        {
            const uint8_t               *pBuffer;
            size_t                       offset;
            uint8_t                      transferId;

            pBuffer     = pData;
            offset      = 0;
            transferId  = sbgEComProtocolGetTxId(pProtocol);
            errorCode   = SBG_INVALID_PARAMETER;

            for (uint16_t pageIndex = 0; pageIndex < nrPages; pageIndex++)
            {
                size_t                   transferSize;

                if ((size - offset) < SBG_ECOM_MAX_EXTENDED_PAYLOAD_SIZE)
                {
                    transferSize = size - offset;
                }
                else
                {
                    transferSize = SBG_ECOM_MAX_EXTENDED_PAYLOAD_SIZE;
                }

                errorCode = sbgEComProtocolSendExtendedFrame(pProtocol, msgClass, msgId, transferId, pageIndex, nrPages, &pBuffer[offset], transferSize);

                if (errorCode != SBG_NO_ERROR)
                {
                    break;
                }

                offset += transferSize;
            }
        }
        else
        {
            errorCode = SBG_INVALID_PARAMETER;
            SBG_LOG_ERROR(errorCode, "payload size too large: %zu", size);
        }
    }

    return errorCode;
}

SbgErrorCode sbgEComProtocolReceive(SbgEComProtocol *pProtocol, uint8_t *pMsgClass, uint8_t *pMsgId, void *pData, size_t *pSize, size_t maxSize)
{
    SbgErrorCode                         errorCode;
    SbgEComProtocolPayload               payload;

    sbgEComProtocolPayloadConstruct(&payload);

    errorCode = sbgEComProtocolReceive2(pProtocol, pMsgClass, pMsgId, &payload);

    if (errorCode == SBG_NO_ERROR)
    {
        size_t                           size;

        size = sbgEComProtocolPayloadGetSize(&payload);

        if (size <= maxSize)
        {
            if (pData)
            {
                const void              *pBuffer;

                pBuffer = sbgEComProtocolPayloadGetBuffer(&payload);

                memcpy(pData, pBuffer, size);
            }

            if (pSize)
            {
                *pSize = size;
            }
        }
        else
        {
            errorCode = SBG_BUFFER_OVERFLOW;
        }
    }

    sbgEComProtocolPayloadDestroy(&payload);

    return errorCode;
}

SbgErrorCode sbgEComProtocolReceive2(SbgEComProtocol *pProtocol, uint8_t *pMsgClass, uint8_t *pMsgId, SbgEComProtocolPayload *pPayload)
{
    SbgErrorCode                         errorCode;
    uint8_t                              msgClass;
    uint8_t                              msgId;
    uint8_t                              transferId;
    uint16_t                             pageIndex;
    uint16_t                             nrPages;
    void                                *pBuffer;
    size_t                               size;

    assert(pProtocol);
    assert(pProtocol->discardSize <= pProtocol->rxBufferSize);

    sbgEComProtocolPayloadClear(pPayload);

    sbgEComProtocolDiscardUnusedBytes(pProtocol);

    sbgEComProtocolRead(pProtocol);

    errorCode = sbgEComProtocolFindFrame(pProtocol, &msgClass, &msgId, &transferId, &pageIndex, &nrPages, &pBuffer, &size);

    if (errorCode == SBG_NO_ERROR)
    {
        if (nrPages == 0)
        {
            if (sbgEComProtocolLargeTransferInProgress(pProtocol))
            {
                SBG_LOG_ERROR(SBG_ERROR, "standard frame received while a large transfer is in progress");
                SBG_LOG_ERROR(SBG_ERROR, "terminating large transfer");

                sbgEComProtocolClearLargeTransfer(pProtocol);
            }

            if (pMsgClass)
            {
                *pMsgClass = msgClass;
            }

            if (pMsgId)
            {
                *pMsgId = msgId;
            }

            sbgEComProtocolPayloadSet(pPayload, false, pBuffer, size);
        }
        else
        {
            errorCode = sbgEComProtocolProcessExtendedFrame(pProtocol, msgClass, msgId, transferId, pageIndex, nrPages, pBuffer, size);

            if (errorCode == SBG_NO_ERROR)
            {
                if (pMsgClass)
                {
                    *pMsgClass = msgClass;
                }

                if (pMsgId)
                {
                    *pMsgId = msgId;
                }

                sbgEComProtocolPayloadSet(pPayload, true, pProtocol->pLargeBuffer, pProtocol->largeBufferSize);
                sbgEComProtocolResetLargeTransfer(pProtocol);
            }
        }
    }

    return errorCode;
}

void sbgEComProtocolSetOnFrameReceivedCb(SbgEComProtocol *pProtocol, SbgEComProtocolFrameCb pOnFrameReceivedCb, void *pUserArg)
{
    assert(pProtocol);
    
    pProtocol->pReceiveFrameCb  = pOnFrameReceivedCb;
    pProtocol->pUserArg         = pUserArg;
}

SbgErrorCode sbgEComStartFrameGeneration(SbgStreamBuffer *pOutputStream, uint8_t msgClass, uint8_t msg, size_t *pStreamCursor)
{
    assert(pOutputStream);
    assert((msgClass & 0x80) == 0);
    assert(pStreamCursor);

    //
    // Backup the current position in the stream buffer
    //
    *pStreamCursor = sbgStreamBufferTell(pOutputStream);

    //
    // Write the header
    //
    sbgStreamBufferWriteUint8LE(pOutputStream, SBG_ECOM_SYNC_1);
    sbgStreamBufferWriteUint8LE(pOutputStream, SBG_ECOM_SYNC_2);

    //
    // Write the message ID and class
    //
    sbgStreamBufferWriteUint8LE(pOutputStream, msg);
    sbgStreamBufferWriteUint8LE(pOutputStream, msgClass);
    
    //
    // For now, we don't know the payload size so skip it
    //
    return sbgStreamBufferSeek(pOutputStream, sizeof(uint16_t), SB_SEEK_CUR_INC);
}

SbgErrorCode sbgEComFinalizeFrameGeneration(SbgStreamBuffer *pOutputStream, size_t streamCursor)
{
    SbgErrorCode    errorCode;
    size_t          payloadSize;    
    size_t          currentPos;
    uint16_t            frameCrc;

    assert(pOutputStream);

    //
    // Test if any error has occurred on the stream first
    //
    errorCode = sbgStreamBufferGetLastError(pOutputStream);

    //
    // Is the stream buffer error free ?
    //
    if (errorCode == SBG_NO_ERROR)
    {
        //
        // Compute the payload size (written data minus the header)
        //
        payloadSize = sbgStreamBufferGetLength(pOutputStream) - streamCursor - 6;

        //
        // Test that the payload size is valid
        //
        if (payloadSize <= SBG_ECOM_MAX_PAYLOAD_SIZE)
        {
            //
            // Backup the current cursor position
            //
            currentPos = sbgStreamBufferTell(pOutputStream);

            //
            // Goto the payload size field (4th byte in the frame)
            //
            sbgStreamBufferSeek(pOutputStream, streamCursor+4, SB_SEEK_SET);

            //
            // Write the payload size
            //
            sbgStreamBufferWriteUint16LE(pOutputStream, (uint16_t)payloadSize);

            //
            // Go back to the previous position
            //
            sbgStreamBufferSeek(pOutputStream, currentPos, SB_SEEK_SET);

            //
            // Compute the 16 bits CRC on the whole frame except Sync 1 and Sync 2
            //
            frameCrc = sbgCrc16Compute((uint8_t*)sbgStreamBufferGetLinkedBuffer(pOutputStream) + streamCursor + 2, payloadSize + 4);

            //
            // Append the CRC
            //
            sbgStreamBufferWriteUint16LE(pOutputStream, frameCrc);

            //
            // Append the ETX
            //
            errorCode = sbgStreamBufferWriteUint8LE(pOutputStream, SBG_ECOM_ETX);
        }
        else
        {
            //
            // Invalid payload size
            //
            errorCode = SBG_BUFFER_OVERFLOW;
            SBG_LOG_ERROR(errorCode, "Payload of %zu bytes is too big for a valid sbgECom log", payloadSize);
        }
    }
    else
    {
        //
        // Notify error
        //
        SBG_LOG_ERROR(errorCode, "Unable to finalize frame because of an error on Stream Buffer");
    }

    return errorCode;
}
