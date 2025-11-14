#include "sbgEComLogEkf.h"

//----------------------------------------------------------------------//
//- Private definitions                                                -//
//----------------------------------------------------------------------//

/*!
 * Solution status mode definitions.
 */
#define SBG_ECOM_LOG_EKF_SOLUTION_MODE_SHIFT        (0u)                /*!< Shift used to extract the clock status part. */
#define SBG_ECOM_LOG_EKF_SOLUTION_MODE_MASK         (0x0000000Fu)       /*!< Mask used to keep only the clock status part. */

//----------------------------------------------------------------------//
//- Public methods (SbgEComLogEkfEuler)                                -//
//----------------------------------------------------------------------//

float sbgEComLogEkfEulerGetMagneticHeading(const SbgEComLogEkfEuler *pEkfEuler)
{
    float   heading;

    assert(pEkfEuler);

    if (pEkfEuler->magDeclination != NAN)
    {
        heading = pEkfEuler->euler[2] - pEkfEuler->magDeclination;

        if (heading > SBG_PI_F)
        {
            heading = heading - 2.0f * SBG_PI_F;
        }
        else if (heading < -SBG_PI_F)
        {
            heading = heading + 2.0f * SBG_PI_F;
        }
    }
    else
    {
        heading = pEkfEuler->euler[2];
    }

    return heading;
}

SbgErrorCode sbgEComLogEkfEulerReadFromStream(SbgEComLogEkfEuler *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->timeStamp         = sbgStreamBufferReadUint32LE(pStreamBuffer);

    pLogData->euler[0]          = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->euler[1]          = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->euler[2]          = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->eulerStdDev[0]    = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->eulerStdDev[1]    = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->eulerStdDev[2]    = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->status            = sbgStreamBufferReadUint32LE(pStreamBuffer);

    //
    // Added in sbgECom 5.0
    //
    if (sbgStreamBufferGetSpace(pStreamBuffer) >= 2*sizeof(float))
    {
        pLogData->magDeclination    = sbgStreamBufferReadFloatLE(pStreamBuffer);
        pLogData->magInclination    = sbgStreamBufferReadFloatLE(pStreamBuffer);
    }
    else
    {
        pLogData->magDeclination        = NAN;
        pLogData->magInclination        = NAN;
    }

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogEkfEulerWriteToStream(const SbgEComLogEkfEuler *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->euler[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->euler[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->euler[2]);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->eulerStdDev[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->eulerStdDev[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->eulerStdDev[2]);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->status);

    //
    // Added in sbgECom 5.0
    //
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->magDeclination);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->magInclination);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public methods (SbgEComLogEkfQuat)                                 -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogEkfQuatReadFromStream(SbgEComLogEkfQuat *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->timeStamp         = sbgStreamBufferReadUint32LE(pStreamBuffer);

    pLogData->quaternion[0]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->quaternion[1]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->quaternion[2]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->quaternion[3]     = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->eulerStdDev[0]    = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->eulerStdDev[1]    = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->eulerStdDev[2]    = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->status            = sbgStreamBufferReadUint32LE(pStreamBuffer);

    //
    // Added in sbgECom 5.0
    //
    if (sbgStreamBufferGetSpace(pStreamBuffer) >= 2*sizeof(float))
    {
        pLogData->magDeclination    = sbgStreamBufferReadFloatLE(pStreamBuffer);
        pLogData->magInclination    = sbgStreamBufferReadFloatLE(pStreamBuffer);
    }
    else
    {
        pLogData->magDeclination        = NAN;
        pLogData->magInclination        = NAN;
    }

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogEkfQuatWriteToStream(const SbgEComLogEkfQuat *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->quaternion[0]); 
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->quaternion[1]); 
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->quaternion[2]); 
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->quaternion[3]); 

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->eulerStdDev[0]); 
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->eulerStdDev[1]); 
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->eulerStdDev[2]);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->status);

    //
    // Added in sbgECom 5.0
    //
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->magDeclination);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->magInclination);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public methods (SbgEComLogEkfNav)                                  -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogEkfNavReadFromStream(SbgEComLogEkfNav *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->timeStamp             = sbgStreamBufferReadUint32LE(pStreamBuffer);

    pLogData->velocity[0]           = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocity[1]           = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocity[2]           = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->velocityStdDev[0]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocityStdDev[1]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocityStdDev[2]     = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->position[0]           = sbgStreamBufferReadDoubleLE(pStreamBuffer);
    pLogData->position[1]           = sbgStreamBufferReadDoubleLE(pStreamBuffer);
    pLogData->position[2]           = sbgStreamBufferReadDoubleLE(pStreamBuffer);

    pLogData->undulation            = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->positionStdDev[0]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->positionStdDev[1]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->positionStdDev[2]     = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->status                = sbgStreamBufferReadUint32LE(pStreamBuffer);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogEkfNavWriteToStream(const SbgEComLogEkfNav *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocity[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocity[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocity[2]);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocityStdDev[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocityStdDev[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocityStdDev[2]);

    sbgStreamBufferWriteDoubleLE(pStreamBuffer, pLogData->position[0]);
    sbgStreamBufferWriteDoubleLE(pStreamBuffer, pLogData->position[1]);
    sbgStreamBufferWriteDoubleLE(pStreamBuffer, pLogData->position[2]);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->undulation);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->positionStdDev[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->positionStdDev[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->positionStdDev[2]);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->status);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public methods (SbgEComLogEkfVelBody)                              -//
//----------------------------------------------------------------------//

void sbgEComLogEkfVelBodyConstruct(SbgEComLogEkfVelBody *pLogData)
{
    assert(pLogData);

    memset(pLogData, 0x00, sizeof(*pLogData));
}

SbgErrorCode sbgEComLogEkfVelBodyReadFromStream(SbgEComLogEkfVelBody *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->timeStamp             = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status                = sbgStreamBufferReadUint32LE(pStreamBuffer);

    pLogData->velocity[0]           = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocity[1]           = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocity[2]           = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->velocityStdDev[0]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocityStdDev[1]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocityStdDev[2]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    
    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogEkfVelBodyWriteToStream(const SbgEComLogEkfVelBody *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->status);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocity[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocity[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocity[2]);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocityStdDev[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocityStdDev[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocityStdDev[2]);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

SbgEComSolutionMode sbgEComLogEkfGetSolutionMode(uint32_t status)
{
    return (SbgEComSolutionMode)((status >> SBG_ECOM_LOG_EKF_SOLUTION_MODE_SHIFT) & SBG_ECOM_LOG_EKF_SOLUTION_MODE_MASK);
}

uint32_t sbgEComLogEkfBuildSolutionStatus(SbgEComSolutionMode solutionMode, uint32_t masks)
{
    return  ((((uint32_t)solutionMode)&SBG_ECOM_LOG_EKF_SOLUTION_MODE_MASK) << SBG_ECOM_LOG_EKF_SOLUTION_MODE_SHIFT) | masks;
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComBinaryLogParseEkfEulerData(SbgStreamBuffer *pStreamBuffer, SbgEComLogEkfEuler *pLogData)
{
    return sbgEComLogEkfEulerReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteEkfEulerData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogEkfEuler *pLogData)
{
    return sbgEComLogEkfEulerWriteToStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogParseEkfQuatData(SbgStreamBuffer *pStreamBuffer, SbgEComLogEkfQuat *pLogData)
{
    return sbgEComLogEkfQuatReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteEkfQuatData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogEkfQuat *pLogData)
{
    return sbgEComLogEkfQuatWriteToStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogParseEkfNavData(SbgStreamBuffer *pStreamBuffer, SbgEComLogEkfNav *pLogData)
{
    return sbgEComLogEkfNavReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteEkfNavData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogEkfNav *pLogData)
{
    return sbgEComLogEkfNavWriteToStream(pLogData, pStreamBuffer);
}
