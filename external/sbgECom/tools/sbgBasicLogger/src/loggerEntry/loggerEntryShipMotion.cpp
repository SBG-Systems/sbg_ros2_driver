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
#include "loggerEntryShipMotion.h"

namespace sbg
{

//----------------------------------------------------------------------//
//- CLoggerEntryShipMotion                                             -//
//----------------------------------------------------------------------//

void CLoggerEntryShipMotion::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\theavePeriod\tsurge\tsway\theave\tsurgeAccel\tswayAccel\theaveAccel\tsurgeVel\tswayVel\theaveVel\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(s)\t(m)\t(m)\t(m)\t(m.s^-2)\t(m.s^-2\t(m.s^-2)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\n";
}

void CLoggerEntryShipMotion::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogShipMotion			&data = logData.shipMotionData;

	m_outFile	<< context.fmtTime(data.timeStamp)											<< "\t"
				<< context.fmtStatus(data.status)											<< "\t"
				<< data.mainHeavePeriod														<< "\t"
				<< data.shipMotion[0]														<< "\t"
				<< data.shipMotion[1]														<< "\t"
				<< data.shipMotion[2]														<< "\t"
				<< data.shipAccel[0]														<< "\t"
				<< data.shipAccel[1]														<< "\t"
				<< data.shipAccel[2]														<< "\t"
				<< data.shipVel[0]															<< "\t"
				<< data.shipVel[1]															<< "\t"
				<< data.shipVel[2]															<< "\n";
}

void CLoggerEntryShipMotion::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogShipMotion			&data = logData.shipMotionData;

	std::cout	<< std::setw(12) << getName()												<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << data.mainHeavePeriod
				<< std::setw(12) << data.shipMotion[0]
				<< std::setw(12) << data.shipMotion[1]
				<< std::setw(12) << data.shipMotion[2]
				<< std::setw(12) << data.shipAccel[0]
				<< std::setw(12) << data.shipAccel[1]
				<< std::setw(12) << data.shipAccel[2]
				<< std::setw(12) << data.shipVel[0]
				<< std::setw(12) << data.shipVel[1]
				<< std::setw(12) << data.shipVel[2]											<< "\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryShipMotionRt                                           -//
//----------------------------------------------------------------------//

std::string CLoggerEntryShipMotionRt::getName() const
{
	return "shipMotion";
}

//----------------------------------------------------------------------//
//- CLoggerEntryShipMotionHp                                           -//
//----------------------------------------------------------------------//

std::string CLoggerEntryShipMotionHp::getName() const
{
	return "shipMotionHp";
}

}; // Namespace sbg
