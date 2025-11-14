// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Project headers
#include <pager/sbgEComPager.h>

// Local headers
#include "sbgEComVibMonFftCtx.h"

//----------------------------------------------------------------------//
//- Private functions (SbgEComVibMonFftCtx)                            -//
//----------------------------------------------------------------------//

/*!
 * Checks whether a multi-page message has been completely received.
 *
 * \param[in]   pCtx                        Vibration monitoring FFT context.
 * \return                                  True if the entire message has been received.
 */
static bool sbgEComVibMonFftCtxIsComplete(const SbgEComVibMonFftCtx *pCtx)
{
    assert(pCtx);

    return sbgEComPagerJoinIsComplete(&pCtx->pager);
}

//----------------------------------------------------------------------//
//- Public functions (SbgEComVibMonFftCtx)                             -//
//----------------------------------------------------------------------//

void sbgEComVibMonFftCtxConstruct(SbgEComVibMonFftCtx *pCtx)
{
    assert(pCtx);

    memset(pCtx->buffer, 0, sizeof(pCtx->buffer));

    sbgEComPagerInitForJoin(&pCtx->pager, pCtx->buffer, sizeof(pCtx->buffer));
}

SbgErrorCode sbgEComVibMonFftCtxProcess(SbgEComVibMonFftCtx *pCtx, uint16_t pageIndex, uint16_t nrPages, const void *pBuffer, size_t size)
{
    assert(pCtx);

    return sbgEComPagerJoin(&pCtx->pager, pageIndex, nrPages, pBuffer, size);
}

SbgErrorCode sbgEComVibMonFftCtxGetStreamBuffer(const SbgEComVibMonFftCtx *pCtx, SbgStreamBuffer *pStreamBuffer)
{
    SbgErrorCode                         errorCode = SBG_NOT_READY;

    assert(pCtx);
    assert(pStreamBuffer);

    if (sbgEComVibMonFftCtxIsComplete(pCtx))
    {
        errorCode = sbgStreamBufferInitForRead(pStreamBuffer, pCtx->buffer, sbgEComPagerGetBufferSize(&pCtx->pager));
    }

    return errorCode;
}
