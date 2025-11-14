// STL headers
#include <array>

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgEComLib.h>

// Local headers
#include "imuDataMean.h"

namespace sbg
{

//----------------------------------------------------------------------//
//- CImuDataMean                                                       -//
//----------------------------------------------------------------------//

CImuDataMean::CImuDataMean()
{
	reset();
}

int32_t CImuDataMean::add(const SbgEComLogImuLegacy &imuData)
{
	m_timeStamp			 = imuData.timeStamp;
	m_status			|= (uint32_t)imuData.status;

	m_accelerometer[0]	+= imuData.deltaVelocity[0];
	m_accelerometer[1]	+= imuData.deltaVelocity[1];
	m_accelerometer[2]	+= imuData.deltaVelocity[2];

	m_gyroscope[0]		+= imuData.deltaAngle[0];
	m_gyroscope[1]		+= imuData.deltaAngle[1];
	m_gyroscope[2]		+= imuData.deltaAngle[2];
	
	m_temperature		+= imuData.temperature;
	m_numAccValues		+= 1;

	return m_numAccValues;
}

int32_t CImuDataMean::add(const SbgEComLogImuShort &imuData)
{
	m_timeStamp			 = imuData.timeStamp;
	m_status			|= (uint32_t)imuData.status;

	m_accelerometer[0]	+= sbgEComLogImuShortGetDeltaVelocity(&imuData, 0);
	m_accelerometer[1]	+= sbgEComLogImuShortGetDeltaVelocity(&imuData, 1);
	m_accelerometer[2]	+= sbgEComLogImuShortGetDeltaVelocity(&imuData, 2);

	m_gyroscope[0]		+= sbgEComLogImuShortGetDeltaAngle(&imuData, 0);
	m_gyroscope[1]		+= sbgEComLogImuShortGetDeltaAngle(&imuData, 1);
	m_gyroscope[2]		+= sbgEComLogImuShortGetDeltaAngle(&imuData, 2);
	
	m_temperature		+= sbgEComLogImuShortGetTemperature(&imuData);
	m_numAccValues		+= 1;

	return m_numAccValues;
}

int32_t CImuDataMean::add(const SbgEComLogImuFastLegacy &imuData)
{
	m_timeStamp			 = imuData.timeStamp;
	m_status			|= (uint32_t)imuData.status;

	m_accelerometer[0]	+= imuData.accelerometers[0];
	m_accelerometer[1]	+= imuData.accelerometers[1];
	m_accelerometer[2]	+= imuData.accelerometers[2];

	m_gyroscope[0]		+= imuData.gyroscopes[0];
	m_gyroscope[1]		+= imuData.gyroscopes[1];
	m_gyroscope[2]		+= imuData.gyroscopes[2];

	m_numAccValues		+= 1;

	return m_numAccValues;
}

void CImuDataMean::reset()
{
	m_timeStamp			= 0;
	m_status			= 0;

	m_accelerometer[0]	= 0;
	m_accelerometer[1]	= 0;
	m_accelerometer[2]	= 0;

	m_gyroscope[0]		= 0;
	m_gyroscope[1]		= 0;
	m_gyroscope[2]		= 0;

	m_temperature		= 0;
	m_numAccValues		= 0;
}

uint32_t CImuDataMean::getTimeStamp() const
{
	return m_timeStamp;
}

uint32_t CImuDataMean::getStatus() const
{
	return m_status;
}

double CImuDataMean::getAccelerometer(size_t idx) const
{
	assert(idx < m_accelerometer.size());
	assert(m_numAccValues > 0);

	return m_accelerometer[idx] / m_numAccValues;
}

double CImuDataMean::getGyroscope(size_t idx) const
{
	assert(idx < m_gyroscope.size());
	assert(m_numAccValues > 0);

	return m_gyroscope[idx] / m_numAccValues;
}

double CImuDataMean::getGyroscopeDeg(size_t idx) const
{
	return sbgRadToDegd(getGyroscope(idx));
}

double CImuDataMean::getTemperature() const
{
	assert(m_numAccValues > 0);

	return m_temperature / m_numAccValues;
}
}; // Namespace sbg
