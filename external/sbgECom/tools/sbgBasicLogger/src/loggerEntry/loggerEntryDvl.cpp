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
#include "loggerEntryDvl.h"

namespace sbg
{
//----------------------------------------------------------------------//
//- CLoggerEntryDvl                                                    -//
//----------------------------------------------------------------------//

void CLoggerEntryDvl::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\tvelX\tvelY\tvelZ\tvelQualX\tvelQualY\tvelQualZ\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(deg)\t(deg)\n";
}

void CLoggerEntryDvl::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogDvl					&data = logData.dvlData;

	m_outFile	<< context.fmtTime(data.timeStamp)										<< "\t"
				<< context.fmtStatus(data.status)										<< "\t"
				<< data.velocity[0]														<< "\t"
				<< data.velocity[1]														<< "\t"
				<< data.velocity[2]														<< "\t"
				<< data.velocityQuality[0]												<< "\t"
				<< data.velocityQuality[1]												<< "\t"
				<< data.velocityQuality[2]												<< "\n";
}

void CLoggerEntryDvl::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogDvl					&data = logData.dvlData;

	std::cout	<< std::setw(12) << getName()											<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << data.velocity[0]
				<< std::setw(12) << data.velocity[1]
				<< std::setw(12) << data.velocity[2]
				<< std::setw(12) << data.velocityQuality[0]
				<< std::setw(12) << data.velocityQuality[1]
				<< std::setw(12) << data.velocityQuality[2]								<< "\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryDvlBottom                                              -//
//----------------------------------------------------------------------//

std::string CLoggerEntryDvlBottom::getName() const
{
	return "dvlBottom";
}

//----------------------------------------------------------------------//
//- CLoggerEntryDvlWater                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryDvlWater::getName() const
{
	return "dvlWater";
}

}; // Namespace sbg
