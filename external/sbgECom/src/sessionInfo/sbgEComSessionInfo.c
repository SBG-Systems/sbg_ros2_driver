// sbgCommonLib headers
#include <sbgCommon.h>

// Project headers
#include <pager/sbgEComPager.h>

// Local headers
#include "sbgEComSessionInfo.h"

//----------------------------------------------------------------------//
//- Private functions                                                  -//
//----------------------------------------------------------------------//

/*!
 * Checks whether a multi-page message has been completely received.
 *
 * \param[in]   pCtx                        Session information context.
 * \return                                  True if the entire message has been received.
 */
static bool sbgEComSessionInfoCtxIsComplete(const SbgEComSessionInfoCtx *pCtx)
{
    assert(pCtx);

    return sbgEComPagerJoinIsComplete(&pCtx->pager);
}

//----------------------------------------------------------------------//
//- Public functions                                                   -//
//----------------------------------------------------------------------//

void sbgEComSessionInfoCtxConstruct(SbgEComSessionInfoCtx *pCtx)
{
    assert(pCtx);

    memset(pCtx->string, 0, sizeof(pCtx->string));

    sbgEComPagerInitForJoin(&pCtx->pager, pCtx->string, sizeof(pCtx->string) - 1);
}

SbgErrorCode sbgEComSessionInfoCtxProcess(SbgEComSessionInfoCtx *pCtx, uint16_t pageIndex, uint16_t nrPages, const void *pBuffer, size_t size)
{
    SbgErrorCode                         errorCode;
    size_t                               stringSize;

    assert(pCtx);

    errorCode   = sbgEComPagerJoin(&pCtx->pager, pageIndex, nrPages, pBuffer, size);
    stringSize  = sbgEComPagerGetBufferSize(&pCtx->pager);

    pCtx->string[stringSize] = '\0';

    return errorCode;
}

const char *sbgEComSessionInfoCtxGetString(const SbgEComSessionInfoCtx *pCtx)
{
    const char                          *pString = NULL;

    assert(pCtx);

    if (sbgEComSessionInfoCtxIsComplete(pCtx))
    {
        pString = pCtx->string;
    }

    return pString;
}
