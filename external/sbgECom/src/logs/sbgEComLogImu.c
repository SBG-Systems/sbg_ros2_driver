#include "sbgEComLogImu.h"

//----------------------------------------------------------------------//
//- Private constant definitions                                       -//
//----------------------------------------------------------------------//

/*!
 * Standard gyroscope scale factor.
 */
#define SBG_ECOM_LOG_IMU_GYRO_SCALE_STD                     (67108864.0f)

/*!
 * High range gyroscope scale factor derived from 10000 degrees per second maximum range.
 *
 * Calculation: (2^31 - 1) / (10000 * π / 180)
 */
#define SBG_ECOM_LOG_IMU_GYRO_SCALE_HIGH                    (12304174.0f)

/*!
 * Maximum value for the standard scale factor, in radians per second.
 *
 * Approximately equivalent to 1833 °/s.
 */
#define SBG_ECOM_LOG_IMU_GYRO_SCALE_STD_MAX_RAD             ((float)INT32_MAX / SBG_ECOM_LOG_IMU_GYRO_SCALE_STD)

/*!
 * Standard accelerometer scale factor.
 */
#define SBG_ECOM_LOG_IMU_ACCEL_SCALE_STD                    (1048576.0f)

/*!
 * Standard temperature scale factor.
 */
#define SBG_ECOM_LOG_IMU_TEMP_SCALE_STD                     (256.0f)

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComLogImuLegacyReadFromStream(SbgEComLogImuLegacy *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->timeStamp         = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status            = sbgStreamBufferReadUint16LE(pStreamBuffer);

    pLogData->accelerometers[0] = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->accelerometers[1] = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->accelerometers[2] = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->gyroscopes[0]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->gyroscopes[1]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->gyroscopes[2]     = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->temperature       = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->deltaVelocity[0]  = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->deltaVelocity[1]  = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->deltaVelocity[2]  = sbgStreamBufferReadFloatLE(pStreamBuffer);

    pLogData->deltaAngle[0]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->deltaAngle[1]     = sbgStreamBufferReadFloatLE(pStreamBuffer);
    pLogData->deltaAngle[2]     = sbgStreamBufferReadFloatLE(pStreamBuffer);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogImuLegacyWriteToStream(const SbgEComLogImuLegacy *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->accelerometers[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->accelerometers[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->accelerometers[2]);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->gyroscopes[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->gyroscopes[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->gyroscopes[2]);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->temperature);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->deltaVelocity[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->deltaVelocity[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->deltaVelocity[2]);

    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->deltaAngle[0]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->deltaAngle[1]);
    sbgStreamBufferWriteFloatLE(pStreamBuffer,  pLogData->deltaAngle[2]);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogImuShortReadFromStream(SbgEComLogImuShort *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->timeStamp         = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status            = sbgStreamBufferReadUint16LE(pStreamBuffer);

    pLogData->deltaVelocity[0]  = sbgStreamBufferReadInt32LE(pStreamBuffer);
    pLogData->deltaVelocity[1]  = sbgStreamBufferReadInt32LE(pStreamBuffer);
    pLogData->deltaVelocity[2]  = sbgStreamBufferReadInt32LE(pStreamBuffer);

    pLogData->deltaAngle[0]     = sbgStreamBufferReadInt32LE(pStreamBuffer);
    pLogData->deltaAngle[1]     = sbgStreamBufferReadInt32LE(pStreamBuffer);
    pLogData->deltaAngle[2]     = sbgStreamBufferReadInt32LE(pStreamBuffer);

    pLogData->temperature       = sbgStreamBufferReadInt16LE(pStreamBuffer);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogImuShortWriteToStream(const SbgEComLogImuShort *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);

    sbgStreamBufferWriteInt32LE(pStreamBuffer,  pLogData->deltaVelocity[0]);
    sbgStreamBufferWriteInt32LE(pStreamBuffer,  pLogData->deltaVelocity[1]);
    sbgStreamBufferWriteInt32LE(pStreamBuffer,  pLogData->deltaVelocity[2]);

    sbgStreamBufferWriteInt32LE(pStreamBuffer,  pLogData->deltaAngle[0]);
    sbgStreamBufferWriteInt32LE(pStreamBuffer,  pLogData->deltaAngle[1]);
    sbgStreamBufferWriteInt32LE(pStreamBuffer,  pLogData->deltaAngle[2]);

    sbgStreamBufferWriteInt16LE(pStreamBuffer,  pLogData->temperature);

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogImuFastLegacyReadFromStream(SbgEComLogImuFastLegacy *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    pLogData->timeStamp         = sbgStreamBufferReadUint32LE(pStreamBuffer);
    pLogData->status            = sbgStreamBufferReadUint16LE(pStreamBuffer);

    pLogData->accelerometers[0] = (float)sbgStreamBufferReadInt16LE(pStreamBuffer) * 0.01f;
    pLogData->accelerometers[1] = (float)sbgStreamBufferReadInt16LE(pStreamBuffer) * 0.01f;
    pLogData->accelerometers[2] = (float)sbgStreamBufferReadInt16LE(pStreamBuffer) * 0.01f;

    pLogData->gyroscopes[0]     = (float)sbgStreamBufferReadInt16LE(pStreamBuffer) * 0.001f;
    pLogData->gyroscopes[1]     = (float)sbgStreamBufferReadInt16LE(pStreamBuffer) * 0.001f;
    pLogData->gyroscopes[2]     = (float)sbgStreamBufferReadInt16LE(pStreamBuffer) * 0.001f;

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

SbgErrorCode sbgEComLogImuFastLegacyWriteToStream(const SbgEComLogImuFastLegacy *pLogData, SbgStreamBuffer *pStreamBuffer)
{
    assert(pStreamBuffer);
    assert(pLogData);

    sbgStreamBufferWriteUint32LE(pStreamBuffer, pLogData->timeStamp);
    sbgStreamBufferWriteUint16LE(pStreamBuffer, pLogData->status);

    sbgStreamBufferWriteInt16LE(pStreamBuffer,  (int16_t)(pLogData->accelerometers[0] * 100.0f));
    sbgStreamBufferWriteInt16LE(pStreamBuffer,  (int16_t)(pLogData->accelerometers[1] * 100.0f));
    sbgStreamBufferWriteInt16LE(pStreamBuffer,  (int16_t)(pLogData->accelerometers[2] * 100.0f));

    sbgStreamBufferWriteInt16LE(pStreamBuffer,  (int16_t)(pLogData->gyroscopes[0] * 1000.0f));
    sbgStreamBufferWriteInt16LE(pStreamBuffer,  (int16_t)(pLogData->gyroscopes[1] * 1000.0f));
    sbgStreamBufferWriteInt16LE(pStreamBuffer,  (int16_t)(pLogData->gyroscopes[2] * 1000.0f));

    return sbgStreamBufferGetLastError(pStreamBuffer);
}

//----------------------------------------------------------------------//
//- Public setters/getters                                             -//
//----------------------------------------------------------------------//

float sbgEComLogImuShortGetDeltaAngle(const SbgEComLogImuShort *pImuShort, size_t idx)
{
    float                                scaleFactor = SBG_ECOM_LOG_IMU_GYRO_SCALE_STD;

    assert(pImuShort);
    assert(idx < 3);

    if (pImuShort->status & SBG_ECOM_IMU_GYROS_USE_HIGH_SCALE)
    {
        scaleFactor = SBG_ECOM_LOG_IMU_GYRO_SCALE_HIGH;
    }

    return pImuShort->deltaAngle[idx] / scaleFactor;
}

void sbgEComLogImuShortSetDeltaAngle(SbgEComLogImuShort *pImuShort, const float *pArray, size_t arraySize)
{
    float                                scaleFactor = SBG_ECOM_LOG_IMU_GYRO_SCALE_STD;

    assert(pImuShort);
    assert(pArray);
    assert(arraySize == 3);

    pImuShort->status &= ~SBG_ECOM_IMU_GYROS_USE_HIGH_SCALE;

    for (size_t i = 0; i < arraySize; i++)
    {
        if (fabsf(pArray[i]) > SBG_ECOM_LOG_IMU_GYRO_SCALE_STD_MAX_RAD)
        {
            scaleFactor         = SBG_ECOM_LOG_IMU_GYRO_SCALE_HIGH;
            pImuShort->status   |= SBG_ECOM_IMU_GYROS_USE_HIGH_SCALE;
            break;
        }
    }

    for (size_t i = 0; i < arraySize; i++)
    {
        pImuShort->deltaAngle[i] = (int32_t)(pArray[i] * scaleFactor);
    }
}

float sbgEComLogImuShortGetDeltaVelocity(const SbgEComLogImuShort *pImuShort, size_t idx)
{
    assert(pImuShort);
    assert(idx < 3);

    return pImuShort->deltaVelocity[idx] / SBG_ECOM_LOG_IMU_ACCEL_SCALE_STD;
}

void sbgEComLogImuShortSetDeltaVelocity(SbgEComLogImuShort *pImuShort, const float *pArray, size_t arraySize)
{
    assert(pImuShort);
    assert(pArray);
    assert(arraySize == 3);

    for (size_t i = 0; i < arraySize; i++)
    {
        pImuShort->deltaVelocity[i] = (int32_t)(pArray[i] * SBG_ECOM_LOG_IMU_ACCEL_SCALE_STD);
    }
}

float sbgEComLogImuShortGetTemperature(const SbgEComLogImuShort *pImuShort)
{
    assert(pImuShort);

    return pImuShort->temperature / SBG_ECOM_LOG_IMU_TEMP_SCALE_STD;
}

void sbgEComLogImuShortSetTemperature(SbgEComLogImuShort *pImuShort, float temperature)
{
    assert(pImuShort);

    pImuShort->temperature = (int16_t)(temperature * SBG_ECOM_LOG_IMU_TEMP_SCALE_STD);
}

//----------------------------------------------------------------------//
//- DEPRECATED - Used for backward compatibility                       -//
//----------------------------------------------------------------------//

SbgErrorCode sbgEComBinaryLogParseImuData(SbgStreamBuffer *pStreamBuffer, SbgEComLogImuLegacy *pLogData)
{
    return sbgEComLogImuLegacyReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteImuData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogImuLegacy *pLogData)
{
    return sbgEComLogImuLegacyWriteToStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogParseImuShort(SbgStreamBuffer *pStreamBuffer, SbgEComLogImuShort *pLogData)
{
    return sbgEComLogImuShortReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteImuShort(SbgStreamBuffer *pStreamBuffer, const SbgEComLogImuShort *pLogData)
{
    return sbgEComLogImuShortWriteToStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogParseFastImuData(SbgStreamBuffer *pStreamBuffer, SbgEComLogImuFastLegacy *pLogData)
{
    return sbgEComLogImuFastLegacyReadFromStream(pLogData, pStreamBuffer);
}

SbgErrorCode sbgEComBinaryLogWriteFastImuData(SbgStreamBuffer *pStreamBuffer, const SbgEComLogImuFastLegacy *pLogData)
{
    return sbgEComLogImuFastLegacyWriteToStream(pLogData, pStreamBuffer);
}

float sbgLogImuShortGetDeltaAngle(const SbgEComLogImuShort *pImuShort, size_t idx)
{
    return sbgEComLogImuShortGetDeltaAngle(pImuShort, idx);
}

float sbgLogImuShortGetDeltaVelocity(const SbgEComLogImuShort *pImuShort, size_t idx)
{
    return sbgEComLogImuShortGetDeltaVelocity(pImuShort, idx);
}

float sbgLogImuShortGetTemperature(const SbgEComLogImuShort *pImuShort)
{
    return sbgEComLogImuShortGetTemperature(pImuShort);
}
