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
#include "loggerEntryAidings.h"

namespace sbg
{

//----------------------------------------------------------------------//
//- CLoggerEntryOdometer                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryOdometer::getName() const
{
	return "odometer";
}

void CLoggerEntryOdometer::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\tvelocity\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(m.s^-1)\n";
}

void CLoggerEntryOdometer::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogOdometer			&data = logData.odometerData;

	m_outFile	<< context.fmtTime(data.timeStamp)									<< "\t"
				<< context.fmtStatus(data.status)									<< "\t"
				<< data.velocity													<< "\n";
}

void CLoggerEntryOdometer::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogOdometer			&data = logData.odometerData;

	std::cout	<< std::setw(12) << getName()										<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << data.velocity									<< "\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryAirData                                                -//
//----------------------------------------------------------------------//

std::string CLoggerEntryAirData::getName() const
{
	return "airData";
}

void CLoggerEntryAirData::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\tpressureAbs\taltitude\tpressureDiff\ttrueAirspeed\tairTemperature\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(Pa)\t(m)\t(Pa)\t(ms.^-1)\t(degC)\n";
}

void CLoggerEntryAirData::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogAirData				&data = logData.airData;

	m_outFile	<< context.fmtTime(data.timeStamp)									<< "\t"
				<< context.fmtStatus(data.status)									<< "\t"
				<< data.pressureAbs													<< "\t"
				<< data.altitude													<< "\t"
				<< data.pressureDiff												<< "\t"
				<< data.trueAirspeed												<< "\t"
				<< data.airTemperature												<< "\n";
}

void CLoggerEntryAirData::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogAirData				&data = logData.airData;

	std::cout	<< std::setw(12) << getName()										<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << data.pressureAbs
				<< std::setw(12) << data.altitude
				<< std::setw(12) << data.pressureDiff
				<< std::setw(12) << data.trueAirspeed
				<< std::setw(12) << data.airTemperature								<< "\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryDepth                                                  -//
//----------------------------------------------------------------------//
std::string CLoggerEntryDepth::getName() const
{
	return "depth";
}

void CLoggerEntryDepth::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\tpressure\tdepth\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(Pa)\t(m)\n";
}

void CLoggerEntryDepth::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogDepth				&data = logData.depthData;

	m_outFile	<< context.fmtTime(data.timeStamp)									<< "\t"
				<< context.fmtStatus(data.status)									<< "\t"
				<< data.pressure													<< "\t"
				<< data.depth														<< "\n";
}

void CLoggerEntryDepth::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogDepth				&data = logData.depthData;

	std::cout	<< std::setw(12) << getName()										<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << data.pressure
				<< std::setw(12) << data.depth										<< "\n";
}


//----------------------------------------------------------------------//
//- CLoggerEntryUsbl                                                   -//
//----------------------------------------------------------------------//
std::string CLoggerEntryUsbl::getName() const
{
	return "usbl";
}

void CLoggerEntryUsbl::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\tlatitude\tlongitude\tdepth\tlatitudeStd\tlongitudeStd\tdepthStd\tinformationAge\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(deg)\t(deg)\t(m)\t(m)\t(m)\t(m)\t(us)\n";
}

void CLoggerEntryUsbl::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogUsbl				&data = logData.usblData;

	m_outFile	<< context.fmtTime(data.timeStamp)									<< "\t"
				<< context.fmtStatus(data.status)									<< "\t"
				<< data.latitude													<< "\t"
				<< data.longitude													<< "\t"
				<< data.depth														<< "\t"
				<< data.latitudeAccuracy											<< "\t"
				<< data.longitudeAccuracy											<< "\t"
				<< data.depthAccuracy												<< "\t"
				<< data.informationAge												<< "\n";
}

void CLoggerEntryUsbl::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogUsbl				&data = logData.usblData;

	std::cout	<< std::setw(12) << getName()										<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << data.latitude
				<< std::setw(12) << data.longitude
				<< std::setw(12) << data.depth
				<< std::setw(12) << data.latitudeAccuracy
				<< std::setw(12) << data.longitudeAccuracy
				<< std::setw(12) << data.depthAccuracy
				<< std::setw(12) << data.informationAge								<< "\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryVelocity                                               -//
//----------------------------------------------------------------------//
void CLoggerEntryVelocity::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\tvel0\tvel1\tvel2\tvelStd0\tvelStd1\tvelStd2\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\n";
}

void CLoggerEntryVelocity::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogVelocity			&data = logData.velocityData;

	m_outFile	<< context.fmtTime(data.timeStamp)									<< "\t"
				<< context.fmtStatus(data.status)									<< "\t"
				<< data.velocity[0]													<< "\t"
				<< data.velocity[1]													<< "\t"
				<< data.velocity[2]													<< "\t"
				<< data.velocityStd[0]												<< "\t"
				<< data.velocityStd[1]												<< "\t"
				<< data.velocityStd[2]												<< "\n";
}

void CLoggerEntryVelocity::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogVelocity			&data = logData.velocityData;

	std::cout	<< std::setw(12) << getName()										<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << data.velocity[0]
				<< std::setw(12) << data.velocity[1]
				<< std::setw(12) << data.velocity[2]
				<< std::setw(12) << data.velocityStd[0]
				<< std::setw(12) << data.velocityStd[1]
				<< std::setw(12) << data.velocityStd[2]								<< "\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryVelocity                                               -//
//----------------------------------------------------------------------//
std::string CLoggerEntryVelocity1::getName() const
{
	return "velocity1";
}

//----------------------------------------------------------------------//
//- CLoggerEntryPosition                                               -//
//----------------------------------------------------------------------//
void CLoggerEntryPosition::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\tlatitude\tlongitude\theight\tcovLatLat\tcovLonLon\tcovHgtHgt\tcovLatLon\tcovLatHgt\tcovLonHgt\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(deg)\t(deg)\t(m)\t(m^2)\t(m^2)\t(m^2)\t(m^2)\t(m^2)\t(m^2)\n";
}

void CLoggerEntryPosition::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogPosition			&data = logData.positionData;

	m_outFile	<< context.fmtTime(data.timeStamp)									<< "\t"
				<< context.fmtStatus(data.status)									<< "\t"
				<< data.position[0]													<< "\t"
				<< data.position[1]													<< "\t"
				<< data.position[2]													<< "\t"
				<< data.covLatLat													<< "\t"
				<< data.covLonLon													<< "\t"
				<< data.covHgtHgt													<< "\t"
				<< data.covLatLon													<< "\t"
				<< data.covLatHgt													<< "\t"
				<< data.covLonHgt													<< "\n";
}

void CLoggerEntryPosition::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogPosition			&data = logData.positionData;

	std::cout	<< std::setw(12) << getName()										<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << data.position[0]
				<< std::setw(12) << data.position[1]
				<< std::setw(12) << data.position[2]
				<< std::setw(12) << data.covLatLat
				<< std::setw(12) << data.covLonLon
				<< std::setw(12) << data.covHgtHgt
				<< std::setw(12) << data.covLatLon
				<< std::setw(12) << data.covLatHgt
				<< std::setw(12) << data.covLonHgt									<< "\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryPosition                                               -//
//----------------------------------------------------------------------//
std::string CLoggerEntryPosition1::getName() const
{
	return "position1";
}

}; // Namespace sbg
