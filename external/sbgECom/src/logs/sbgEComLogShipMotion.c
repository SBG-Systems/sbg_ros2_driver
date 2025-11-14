#include "sbgEComLogShipMotion.h"

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogShipMotionReadFromStream(SbgEComLogShipMotion *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->timeStamp         = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->mainHeavePeriod   = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->shipMotion[0]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->shipMotion[1]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->shipMotion[2]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    
    pLogData->shipAccel[0]      = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->shipAccel[1]      = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->shipAccel[2]      = sbgStreamBufferReadFloatLE(pStreamBuffer);

    //
    // Test if we have a additional information such as ship velocity and status (since version 1.4)
    //
    if (sbgStreamBufferGetSpace(pStreamBuffer) >= 14)
    {
        //
        // Read new outputs
        //
        pLogData->shipVel[0]    = sbgStreamBufferReadFloatLE(pStreamBuffer);
        pLogData->shipVel[1]    = sbgStreamBufferReadFloatLE(pStreamBuffer);
        pLogData->shipVel[2]    = sbgStreamBufferReadFloatLE(pStreamBuffer);

        pLogData->status        = sbgStreamBufferReadUint16LE(pStreamBuffer);
    }
    else
    {
        //
        // Those outputs are not available in previous versions
        //
        pLogData->shipVel[0]    = 0.0f;
        pLogData->shipVel[1]    = 0.0f;
        pLogData->shipVel[2]    = 0.0f;

        pLogData->status        = 0;
    }

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogShipMotionWriteToStream(const SbgEComLogShipMotion *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->mainHeavePeriod);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->shipMotion[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->shipMotion[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->shipMotion[2]);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->shipAccel[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->shipAccel[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->shipAccel[2]);

    //
    // Write additional fields added in version 1.4
    //
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->shipVel[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->shipVel[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->shipVel[2]);

    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComBinaryLogParseShipMotionData(SbgStreamBuffer *pStreamBuffer, SbgEComLogShipMotion *pLogData)
{
    return sbgEComLogShipMotionReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteShipMotionData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogShipMotion *pLogData)
{
    return sbgEComLogShipMotionWriteToStream(pLogData, pStreamBuffer);
}
