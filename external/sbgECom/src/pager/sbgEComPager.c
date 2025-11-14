// sbgCommonLib headers
#include <sbgCommon.h>

// Local headers
#include "sbgEComPager.h"

//----------------------------------------------------------------------//
//- Private functions                                                  -//
//----------------------------------------------------------------------//

/*!
 * Reset a pager.
 *
 * \param[in]   pPager                      Pager.
 */
static void sbgEComPagerReset(SbgEComPager *pPager)
{
    assert(pPager);

    pPager->size        = 0;
    pPager->pageIndex   = 0;
    pPager->nrPages     = 0;
}

//----------------------------------------------------------------------//
//- Public functions                                                   -//
//----------------------------------------------------------------------//


SbgErrorCode sbgEComPagerInitForJoin(SbgEComPager *pPager, void *pBuffer, size_t bufferSize)
{
    assert(pPager);
    assert(pBuffer || (bufferSize == 0));

    pPager->pageIndex   = 0;
    pPager->size        = 0;
    pPager->nrPages     = 0;
    pPager->pBuffer     = pBuffer;
    pPager->bufferSize  = bufferSize;

    return SBG_NO_ERROR;
}


SbgErrorCode sbgEComPagerJoin(SbgEComPager *pPager, uint16_t pageIndex, uint16_t nrPages, const void *pPage, size_t pageSize)
{
    SbgErrorCode                         errorCode = SBG_NOT_READY;

    assert(pPager);
    assert(pageIndex < nrPages);
    assert(pPage || (pageSize == 0));

    if (pPager->pageIndex != pageIndex)
    {
        if ((pageIndex != 0) || (pPager->pageIndex != pPager->nrPages))
        {
            SBG_LOG_WARNING(SBG_ERROR, "unexpected page index, pager reset");
        }

        sbgEComPagerReset(pPager);
    }

    if (pageIndex == 0)
    {
        pPager->nrPages = nrPages;
    }

    if (pPager->pageIndex == pageIndex)
    {
        size_t                               newSize;

        newSize = pPager->size + pageSize;

        if (newSize <= pPager->bufferSize)
        {
            memcpy(&pPager->pBuffer[pPager->size], pPage, pageSize);

            pPager->size = newSize;
            pPager->pageIndex++;

            if (pPager->pageIndex == pPager->nrPages)
            {
                errorCode = SBG_NO_ERROR;
            }
        }
        else
        {
            SBG_LOG_ERROR(SBG_BUFFER_OVERFLOW, "data too large");

            sbgEComPagerReset(pPager);
        }
    }

    return errorCode;
}

bool sbgEComPagerJoinIsComplete(const SbgEComPager *pPager)
{
    assert(pPager);

    return (pPager->nrPages > 0) && (pPager->pageIndex == pPager->nrPages);
}

size_t sbgEComPagerGetBufferSize(const SbgEComPager *pPager)
{
    assert(pPager);

    return pPager->size;
}
