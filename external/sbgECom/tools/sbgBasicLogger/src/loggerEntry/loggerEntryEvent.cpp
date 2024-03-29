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
#include "loggerEntryEvent.h"

namespace sbg
{
//----------------------------------------------------------------------//
//- CLoggerEntryEvent                                                  -//
//----------------------------------------------------------------------//

void CLoggerEntryEvent::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\ttimeOffset0\ttimeOffset1\ttimeOffset2\ttimeOffset3\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(us)\t(us)\t(us)\t(us)\n";
}

void CLoggerEntryEvent::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogEvent			&data = logData.eventMarker;
	
	m_outFile	<< context.fmtTime(data.timeStamp)											<< "\t"
				<< context.fmtStatus(data.status)											<< "\t"
				<< (uint32_t)data.timeOffset0												<< "\t"
				<< (uint32_t)data.timeOffset1												<< "\t"
				<< (uint32_t)data.timeOffset2												<< "\t"
				<< (uint32_t)data.timeOffset3												<< "\n";
}

void CLoggerEntryEvent::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogEvent			&data = logData.eventMarker;

	std::cout	<< std::setw(12) << getName()												<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << (uint32_t)data.timeOffset0
				<< std::setw(12) << (uint32_t)data.timeOffset1
				<< std::setw(12) << (uint32_t)data.timeOffset2
				<< std::setw(12) << (uint32_t)data.timeOffset3								<< "\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryEventInA                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryEventInA::getName() const
{
	return "eventInA";
}

//----------------------------------------------------------------------//
//- CLoggerEntryEventInB                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryEventInB::getName() const
{
	return "eventInB";
}

//----------------------------------------------------------------------//
//- CLoggerEntryEventInC                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryEventInC::getName() const
{
	return "eventInC";
}

//----------------------------------------------------------------------//
//- CLoggerEntryEventInD                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryEventInD::getName() const
{
	return "eventInD";
}

//----------------------------------------------------------------------//
//- CLoggerEntryEventInE                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryEventInE::getName() const
{
	return "eventInE";
}

//----------------------------------------------------------------------//
//- CLoggerEntryEventOutA                                              -//
//----------------------------------------------------------------------//

std::string CLoggerEntryEventOutA::getName() const
{
	return "eventOutA";
}

//----------------------------------------------------------------------//
//- CLoggerEntryEventOutB                                              -//
//----------------------------------------------------------------------//

std::string CLoggerEntryEventOutB::getName() const
{
	return "eventOutB";
}

}; // Namespace sbg
