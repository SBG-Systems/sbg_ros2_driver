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
#include "loggerEntryGnss.h"

namespace sbg
{
//----------------------------------------------------------------------//
//- CLoggerEntryGnssVel                                                -//
//----------------------------------------------------------------------//

void CLoggerEntryGnssVel::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\tgpsTow\tvelN\tvelE\tvelD\tvelStdN\tvelStdE\tvelStdD\ttrack\ttrackStd\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(ms)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(deg)\t(deg)\n";
}

void CLoggerEntryGnssVel::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogGnssVel			&data = logData.gpsVelData;

	m_outFile	<< context.fmtTime(data.timeStamp)										<< "\t"
				<< context.fmtStatus(data.status)										<< "\t"
				<< data.timeOfWeek														<< "\t"
				<< data.velocity[0]														<< "\t"
				<< data.velocity[1]														<< "\t"
				<< data.velocity[2]														<< "\t"
				<< data.velocityAcc[0]													<< "\t"
				<< data.velocityAcc[1]													<< "\t"
				<< data.velocityAcc[2]													<< "\t"
				<< data.course															<< "\t"
				<< data.courseAcc														<< "\n";
}

void CLoggerEntryGnssVel::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogGnssVel			&data = logData.gpsVelData;

	std::cout	<< std::setw(12) << getName()											<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << data.timeOfWeek
				<< std::setw(12) << data.velocity[0]
				<< std::setw(12) << data.velocity[1]
				<< std::setw(12) << data.velocity[2]
				<< std::setw(12) << data.velocityAcc[0]
				<< std::setw(12) << data.velocityAcc[1]
				<< std::setw(12) << data.velocityAcc[2]
				<< std::setw(12) << data.course
				<< std::setw(12) << data.courseAcc										<< "\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryGnssPos                                                -//
//----------------------------------------------------------------------//

void CLoggerEntryGnssPos::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\tstatusExt\tgpsTow\tlatitude\tlongitude\taltitude\tundulation\tlatitudeStd\tlongitudeStd\taltitudeStd\tnumSvTracked\tnumSvUsed\tbaseStationId\tdiffAge\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(na)\t(ms)\t(deg)\t(deg)\t(m)\t(m)\t(m)\t(m)\t(m)\t(na)\t(na)\t(na)\t(0.01s)\n";
}

void CLoggerEntryGnssPos::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogGnssPos			&data = logData.gpsPosData;

	m_outFile	<< context.fmtTime(data.timeStamp)										<< "\t"
				<< context.fmtStatus(data.status)										<< "\t"
				<< context.fmtStatus(data.statusExt)									<< "\t"
				<< data.timeOfWeek														<< "\t"
				<< data.latitude														<< "\t"
				<< data.longitude														<< "\t"
				<< data.altitude														<< "\t"
				<< data.undulation														<< "\t"
				<< data.latitudeAccuracy												<< "\t"
				<< data.longitudeAccuracy												<< "\t"
				<< data.altitudeAccuracy												<< "\t"
				<< (uint32_t)data.numSvTracked											<< "\t"
				<< (uint32_t)data.numSvUsed												<< "\t"
				<< (uint32_t)data.baseStationId											<< "\t"
				<< (uint32_t)data.differentialAge										<< "\n";
}

void CLoggerEntryGnssPos::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogGnssPos			&data = logData.gpsPosData;

	std::cout	<< std::setw(12) << getName()											<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << context.fmtStatus(data.statusExt)
				<< std::setw(12) << data.timeOfWeek
				<< std::setw(12) << data.latitude
				<< std::setw(12) << data.longitude
				<< std::setw(12) << data.altitude
				<< std::setw(12) << data.undulation
				<< std::setw(12) << data.latitudeAccuracy
				<< std::setw(12) << data.longitudeAccuracy
				<< std::setw(12) << data.altitudeAccuracy
				<< std::setw(12) << (uint32_t)data.numSvTracked
				<< std::setw(12) << (uint32_t)data.numSvUsed
				<< std::setw(12) << (uint32_t)data.baseStationId
				<< std::setw(12) << (uint32_t)data.differentialAge						<< "\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryGnssHdt                                                -//
//----------------------------------------------------------------------//

void CLoggerEntryGnssHdt::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\tgpsTow\theading\theadingStd\tpitch\tpitchStd\tbaseline\tnumSvTracked\tnumSvUsed\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(ms)\t(deg)\t(deg)\t(deg)\t(deg)\t(m)\t(na)\t(na)\n";
}

void CLoggerEntryGnssHdt::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogGnssHdt			&data = logData.gpsHdtData;

	m_outFile	<< context.fmtTime(data.timeStamp)										<< "\t"
				<< context.fmtStatus(data.status)										<< "\t"
				<< data.timeOfWeek														<< "\t"
				<< data.heading															<< "\t"
				<< data.headingAccuracy													<< "\t"
				<< data.pitch															<< "\t"
				<< data.pitchAccuracy													<< "\t"
				<< data.baseline														<< "\t"
				<< (uint32_t)data.numSvTracked											<< "\t"
				<< (uint32_t)data.numSvUsed												<< "\n";
}

void CLoggerEntryGnssHdt::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogGnssHdt			&data = logData.gpsHdtData;

	std::cout	<< std::setw(12) << getName()											<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << data.timeOfWeek
				<< std::setw(12) << data.heading
				<< std::setw(12) << data.headingAccuracy
				<< std::setw(12) << data.pitch
				<< std::setw(12) << data.pitchAccuracy
				<< std::setw(12) << data.baseline
				<< std::setw(12) << (uint32_t)data.numSvTracked
				<< std::setw(12) << (uint32_t)data.numSvUsed							<< "\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryGnssRaw                                                -//
//----------------------------------------------------------------------//

std::string CLoggerEntryGnssRaw::getFileName() const
{
	return getName() + ".bin";
}

bool CLoggerEntryGnssRaw::isBinaryFile() const
{
	return true;
}

void CLoggerEntryGnssRaw::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	SBG_UNUSED_PARAMETER(context);

	m_outFile.write(reinterpret_cast<const char*>(logData.gpsRawData.rawBuffer), logData.gpsRawData.bufferSize);
}

//----------------------------------------------------------------------//
//- CLoggerEntryGnssSat                                                -//
//----------------------------------------------------------------------//

void CLoggerEntryGnssSat::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogSatList					&data = logData.satGroupData;

	m_outFile	<< context.fmtTime(data.timeStamp)	<< " | Satellites: " << std::setw(3) << data.nrSatellites << "====================================================================\n";
}

void CLoggerEntryGnssSat::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogSatList					&data = logData.satGroupData;

	SBG_UNUSED_PARAMETER(context);

	std::cout	<< std::setw(12)               << getName()			<< ": "
				<< std::setw(12) << data.nrSatellites	<< "\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryGnss1Vel                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryGnss1Vel::getName() const 
{
	return "gnss1Vel";
}

//----------------------------------------------------------------------//
//- CLoggerEntryGnss1Pos                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryGnss1Pos::getName() const 
{
	return "gnss1Pos";
}

//----------------------------------------------------------------------//
//- CLoggerEntryGnss1Hdt                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryGnss1Hdt::getName() const 
{
	return "gnss1Hdt";
}

//----------------------------------------------------------------------//
//- CLoggerEntryGnss1Raw                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryGnss1Raw::getName() const 
{
	return "gnss1Raw";
}

//----------------------------------------------------------------------//
//- CLoggerEntryGnss1Sat                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryGnss1Sat::getName() const 
{
	return "gnss1Sat";
}

//----------------------------------------------------------------------//
//- CLoggerEntryGnss2Vel                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryGnss2Vel::getName() const 
{
	return "gnss2Vel";
}

//----------------------------------------------------------------------//
//- CLoggerEntryGnss2Pos                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryGnss2Pos::getName() const 
{
	return "gnss2Pos";
}

//----------------------------------------------------------------------//
//- CLoggerEntryGnss2Hdt                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryGnss2Hdt::getName() const 
{
	return "gnss2Hdt";
}

//----------------------------------------------------------------------//
//- CLoggerEntryGnss2Raw                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryGnss2Raw::getName() const 
{
	return "gnss2Raw";
}

//----------------------------------------------------------------------//
//- CLoggerEntryGnss2Sat                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryGnss2Sat::getName() const 
{
	return "gnss2Sat";
}

}; // Namespace sbg
