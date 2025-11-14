// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Local headers
#include "sbgEComVibMonFft.h"

//----------------------------------------------------------------------//
//- Public functions (SbgEComVibMonFft)                                -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComVibMonFftReadFromStream(SbgEComVibMonFft *pVibMonFft, SbgStreamBuffer *pStreamBuffer)
{
    assert(pVibMonFft);
    assert(pStreamBuffer);

    pVibMonFft->timestampUs = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pVibMonFft->status      = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pVibMonFft->fSampling   = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pVibMonFft->binCount    = sbgStreamBufferReadUint16LE(pStreamBuffer);
    pVibMonFft->scaleFactor = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pVibMonFft->offset      = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pVibMonFft->acf         = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pVibMonFft->ecf         = sbgStreamBufferReadFloatLE(pStreamBuffer);

    for (size_t i = 0; i < pVibMonFft->binCount; i++)
    {
        pVibMonFft->buffer[i] = sbgStreamBufferReadUint16LE(pStreamBuffer);
    }

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComVibMonFftWriteToStream(const SbgEComVibMonFft *pVibMonFft, SbgStreamBuffer *pStreamBuffer)
{
    assert(pVibMonFft);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pVibMonFft->timestampUs);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pVibMonFft->status);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pVibMonFft->fSampling);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pVibMonFft->binCount);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pVibMonFft->scaleFactor);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pVibMonFft->offset);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pVibMonFft->acf);
    sbgStreamBufferWriteFloatLE(pStreamBuffer, pVibMonFft->ecf);

    for (size_t i = 0; i < pVibMonFft->binCount; i++)
    {
        sbgStreamBufferWriteUint16LE(pStreamBuffer, pVibMonFft->buffer[i]);
    }

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

SbgEComVibMonAxis sbgEComVibMonFftGetAxis(const SbgEComVibMonFft *pLogData)
{
    assert(pLogData);

    return sbgEComLogVibMonStatusGetAxis(pLogData->status);
}

SbgEComVibMonWindow sbgEComVibMonFftGetWindow(const SbgEComVibMonFft *pLogData)
{
    assert(pLogData);

    return sbgEComLogVibMonStatusGetWindow(pLogData->status);
}

float sbgEComVibMonFftGetFrequencyFromBinIndex(const SbgEComVibMonFft *pVibMonFft, size_t binIndex)
{
    assert(pVibMonFft);
    assert(binIndex < SBG_ARRAY_SIZE(pVibMonFft->buffer));

    return binIndex * ((float)pVibMonFft->fSampling / (pVibMonFft->binCount * 2));
}

size_t sbgEComVibMonFftGetBinIndexFromFrequency(const SbgEComVibMonFft *pVibMonFft, float frequency)
{
    assert(pVibMonFft);
    assert((frequency >= 0.0f) && (frequency * 2.0f < pVibMonFft->fSampling));

    return (size_t)((frequency * pVibMonFft->binCount * 2) / pVibMonFft->fSampling);
}

float sbgEComVibMonFftGetMagnitudeFromBinIndex(const SbgEComVibMonFft *pVibMonFft, size_t binIndex)
{
    assert(pVibMonFft);
    assert(binIndex < SBG_ARRAY_SIZE(pVibMonFft->buffer));

    return pVibMonFft->buffer[binIndex] * pVibMonFft->scaleFactor + pVibMonFft->offset;
}

float sbgEComVibMonFftGetMagnitudeFromBinIndexAcfCorrected(const SbgEComVibMonFft *pVibMonFft, size_t binIndex)
{
    assert(pVibMonFft);

    return pVibMonFft->acf * sbgEComVibMonFftGetMagnitudeFromBinIndex(pVibMonFft, binIndex);
}

float sbgEComVibMonFftGetMagnitudeFromBinIndexEcfCorrected(const SbgEComVibMonFft *pVibMonFft, size_t binIndex)
{
    assert(pVibMonFft);

    return pVibMonFft->ecf * sbgEComVibMonFftGetMagnitudeFromBinIndex(pVibMonFft, binIndex);
}

float sbgEComVibMonFftGetMagnitudeFromFrequency(const SbgEComVibMonFft *pVibMonFft, float frequency)
{
    size_t                               binIndex;

    assert(pVibMonFft);

    binIndex = sbgEComVibMonFftGetBinIndexFromFrequency(pVibMonFft, frequency);

    return sbgEComVibMonFftGetMagnitudeFromBinIndex(pVibMonFft, binIndex);
}

float sbgEComVibMonFftGetMagnitudeFromFrequencyAcfCorrected(const SbgEComVibMonFft *pVibMonFft, float frequency)
{
    size_t                               binIndex;

    assert(pVibMonFft);

    binIndex = sbgEComVibMonFftGetBinIndexFromFrequency(pVibMonFft, frequency);

    return sbgEComVibMonFftGetMagnitudeFromBinIndexAcfCorrected(pVibMonFft, binIndex);
}

float sbgEComVibMonFftGetMagnitudeFromFrequencyEcfCorrected(const SbgEComVibMonFft *pVibMonFft, float frequency)
{
    size_t                               binIndex;

    assert(pVibMonFft);

    binIndex = sbgEComVibMonFftGetBinIndexFromFrequency(pVibMonFft, frequency);

    return sbgEComVibMonFftGetMagnitudeFromBinIndexEcfCorrected(pVibMonFft, binIndex);
}
