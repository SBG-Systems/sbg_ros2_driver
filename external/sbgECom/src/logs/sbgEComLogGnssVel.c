// sbgCommonLib headers
#include <sbgCommon.h>
#include <streamBuffer/sbgStreamBuffer.h>

// Local headers
#include "sbgEComLogGnssVel.h"

//----------------------------------------------------------------------//
//- Private definitions                                                -//
//----------------------------------------------------------------------//

#define SBG_ECOM_LOG_GNSS_VEL_STATUS_SHIFT      (0u)                    /*!< Shift used to extract the GPS velocity status part. */
#define SBG_ECOM_LOG_GNSS_VEL_STATUS_MASK       (0x0000003Fu)           /*!< Mask used to keep only the GPS velocity status part. */

#define SBG_ECOM_LOG_GNSS_VEL_TYPE_SHIFT        (6u)                    /*!< Shift used to extract the GPS velocity type part. */
#define SBG_ECOM_LOG_GNSS_VEL_TYPE_MASK         (0x0000003Fu)           /*!< Mask used to keep only the GPS velocity type part. */

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

void sbgEComLogGnssVelZeroInit(SbgEComLogGnssVel *pLogData)
{
    assert(pLogData);

    memset(pLogData, 0x00, sizeof(*pLogData));

    pLogData->velocityAcc[0]    = 9999.0f;
    pLogData->velocityAcc[1]    = 9999.0f;
    pLogData->velocityAcc[2]    = 9999.0f;

    pLogData->courseAcc         = 180.0f;
    
    sbgEComLogGnssVelSetStatus(pLogData,        SBG_ECOM_GNSS_VEL_STATUS_INSUFFICIENT_OBS);
    sbgEComLogGnssVelSetType(pLogData,          SBG_ECOM_GNSS_VEL_TYPE_NO_SOLUTION);
}

SbgErrorCode sbgEComLogGnssVelReadFromStream(SbgEComLogGnssVel *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    pLogData->timeStamp         = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status            = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->timeOfWeek        = sbgStreamBufferReadUint32LE(pStreamBuffer);
    
    pLogData->velocity[0]       = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocity[1]       = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocity[2]       = sbgStreamBufferReadFloatLE(pStreamBuffer);
    
    pLogData->velocityAcc[0]    = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocityAcc[1]    = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->velocityAcc[2]    = sbgStreamBufferReadFloatLE(pStreamBuffer);
    
    pLogData->course            = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->courseAcc         = sbgStreamBufferReadFloatLE(pStreamBuffer);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogGnssVelWriteToStream(const SbgEComLogGnssVel *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pLogData);
    assert(pStreamBuffer);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->status);
    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeOfWeek);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocity[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocity[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocity[2]);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocityAcc[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocityAcc[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->velocityAcc[2]);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->course);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->courseAcc);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

void sbgEComLogGnssVelSetStatus(SbgEComLogGnssVel *pLogData, SbgEComGnssVelStatus status)
{
    assert(pLogData);
    assert(status <= SBG_ECOM_LOG_GNSS_VEL_STATUS_MASK);

    pLogData->status &= ~(SBG_ECOM_LOG_GNSS_VEL_STATUS_MASK << SBG_ECOM_LOG_GNSS_VEL_STATUS_SHIFT);
    pLogData->status |= ((uint32_t)status&SBG_ECOM_LOG_GNSS_VEL_STATUS_MASK) << SBG_ECOM_LOG_GNSS_VEL_STATUS_SHIFT;
}

SbgEComGnssVelStatus sbgEComLogGnssVelGetStatus(const SbgEComLogGnssVel *pLogData)
{
    assert(pLogData);

    return (SbgEComGnssVelStatus)((pLogData->status >> SBG_ECOM_LOG_GNSS_VEL_STATUS_SHIFT)&SBG_ECOM_LOG_GNSS_VEL_STATUS_MASK);
}

void sbgEComLogGnssVelSetType(SbgEComLogGnssVel *pLogData, SbgEComGnssVelType posType)
{
    assert(pLogData);
    assert(posType <= SBG_ECOM_LOG_GNSS_VEL_TYPE_MASK);

    pLogData->status &= ~(SBG_ECOM_LOG_GNSS_VEL_TYPE_MASK << SBG_ECOM_LOG_GNSS_VEL_TYPE_SHIFT);
    pLogData->status |= ((uint32_t)posType&SBG_ECOM_LOG_GNSS_VEL_TYPE_MASK) << SBG_ECOM_LOG_GNSS_VEL_TYPE_SHIFT;
}

SbgEComGnssVelType sbgEComLogGnssVelGetType(const SbgEComLogGnssVel *pLogData)
{
    assert(pLogData);

    return (SbgEComGnssVelType)((pLogData->status >> SBG_ECOM_LOG_GNSS_VEL_TYPE_SHIFT)&SBG_ECOM_LOG_GNSS_VEL_TYPE_MASK);
}

bool sbgEComLogGnssVelDownVelocityIsValid(const SbgEComLogGnssVel *pLogData)
{
    assert(pLogData);
    
    if (pLogData->velocityAcc[2] < (9999.0f - FLT_EPSILON))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

uint32_t sbgEComLogGpsVelBuildStatus(SbgEComGnssVelStatus status, SbgEComGnssVelType type)
{
    return  ((((uint32_t)status)&SBG_ECOM_LOG_GNSS_VEL_STATUS_MASK) << SBG_ECOM_LOG_GNSS_VEL_STATUS_SHIFT) |
            ((((uint32_t)type)&SBG_ECOM_LOG_GNSS_VEL_TYPE_MASK) << SBG_ECOM_LOG_GNSS_VEL_TYPE_SHIFT);
}

SbgEComGnssVelStatus sbgEComLogGpsVelGetStatus(uint32_t status)
{
    return (SbgEComGnssVelStatus)((status >> SBG_ECOM_LOG_GNSS_VEL_STATUS_SHIFT) & SBG_ECOM_LOG_GNSS_VEL_STATUS_MASK);
}

SbgEComGnssVelType sbgEComLogGpsVelGetType(uint32_t status)
{
    return (SbgEComGnssVelType)((status >> SBG_ECOM_LOG_GNSS_VEL_TYPE_SHIFT) & SBG_ECOM_LOG_GNSS_VEL_TYPE_MASK);
}

SbgErrorCode sbgEComBinaryLogParseGpsVelData(SbgStreamBuffer *pStreamBuffer, SbgEComLogGnssVel *pLogData)
{
    return sbgEComLogGnssVelReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteGpsVelData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogGnssVel *pLogData)
{
    return sbgEComLogGnssVelWriteToStream(pLogData, pStreamBuffer);
}
