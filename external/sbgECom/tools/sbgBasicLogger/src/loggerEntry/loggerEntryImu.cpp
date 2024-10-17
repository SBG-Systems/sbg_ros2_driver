// STL headers
#include <iostream>
#include <iomanip>

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgEComLib.h>

// Project headers
#include <loggerManager/loggerEntry.h>
#include <loggerManager/loggerContext.h>

// Local headers
#include "loggerEntryImu.h"
#include "helpers/imuDataMean.h"

namespace sbg
{
//----------------------------------------------------------------------//
//- CLoggerEntryImu                                                    -//
//----------------------------------------------------------------------//

template<typename T>
void CLoggerEntryImu::writeImuDataToFile(const CLoggerContext &context, const T &imuData)
{
	int32_t accValues = m_fileValues.add(imuData);

	if (accValues >= context.getSettings().getFileDecimation())
	{
		m_outFile	<< context.fmtTime(m_fileValues.getTimeStamp())									<< "\t"
					<< context.fmtStatus(m_fileValues.getStatus())									<< "\t"
					<< m_fileValues.getAccelerometer(0)												<< "\t"
					<< m_fileValues.getAccelerometer(1)												<< "\t"
					<< m_fileValues.getAccelerometer(2)												<< "\t"
					<< m_fileValues.getGyroscopeDeg(0)												<< "\t"
					<< m_fileValues.getGyroscopeDeg(1)												<< "\t"
					<< m_fileValues.getGyroscopeDeg(2)												<< "\t"
					<< m_fileValues.getTemperature()												<< "\n";

		m_fileValues.reset();
	}
}

template<typename T>
void CLoggerEntryImu::writeImuDataToConsole(const CLoggerContext &context, const T &imuData)
{
	int32_t accValues = m_consoleValues.add(imuData);

	if (accValues >= context.getSettings().getConsoleDecimation())
	{
		std::cout	<< std::setw(12) << getName()													<< ": "
					<< std::setw(12) << context.fmtStatus(m_consoleValues.getStatus())
					<< std::setw(12) << m_consoleValues.getAccelerometer(0)
					<< std::setw(12) << m_consoleValues.getAccelerometer(1)
					<< std::setw(12) << m_consoleValues.getAccelerometer(2)
					<< std::setw(12) << m_consoleValues.getGyroscopeDeg(0)
					<< std::setw(12) << m_consoleValues.getGyroscopeDeg(1)
					<< std::setw(12) << m_consoleValues.getGyroscopeDeg(2)
					<< std::setw(12) << m_consoleValues.getTemperature()							<< "\n";

		m_consoleValues.reset();
	}
}

void CLoggerEntryImu::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\taccelX\taccelY\taccelZ\tgyroX\tgyroY\tgyroZ\ttemperature\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(m.s^-2)\t(m.s^-2)\t(m.s^-2)\t(deg.s^-1)\t(deg.s^-1)\t(deg.s^-1)\t(degC)\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryImuData                                                -//
//----------------------------------------------------------------------//

std::string CLoggerEntryImuData::getName() const
{
	return "imuData";
}

void CLoggerEntryImuData::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogImuLegacy			&data = logData.imuData;

	writeImuDataToFile(context, data);
}

void CLoggerEntryImuData::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogImuLegacy			&data = logData.imuData;

	writeImuDataToConsole(context, data);
}

//----------------------------------------------------------------------//
//- CLoggerEntryImuShort                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryImuShort::getName() const
{
	return "imuShort";
}

void CLoggerEntryImuShort::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogImuShort			&data = logData.imuShort;

	writeImuDataToFile(context, data);
}

void CLoggerEntryImuShort::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogImuShort			&data = logData.imuShort;

	writeImuDataToConsole(context, data);
}

//----------------------------------------------------------------------//
//- CLoggerEntryImuFast                                                -//
//----------------------------------------------------------------------//

std::string CLoggerEntryImuFast::getName() const
{
	return "imuFast";
}

void CLoggerEntryImuFast::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogImuFastLegacy			&data = logData.fastImuData;

	writeImuDataToFile(context, data);
}

void CLoggerEntryImuFast::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogImuFastLegacy			&data = logData.fastImuData;

	writeImuDataToConsole(context, data);

}
}; // Namespace sbg
