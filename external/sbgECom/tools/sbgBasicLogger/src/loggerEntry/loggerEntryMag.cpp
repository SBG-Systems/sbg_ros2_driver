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
#include "loggerEntryMag.h"

namespace sbg
{

//----------------------------------------------------------------------//
//- CLoggerEntryMag                                                    -//
//----------------------------------------------------------------------//

std::string CLoggerEntryMag::getName() const
{
	return "mag";
}

void CLoggerEntryMag::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\tmagX\tmagY\tmagZ\taccelX\taccelY\taccelZ\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(au)\t(au)\t(au)\t(m.s^-2)\t(m.s^-2)\t(m.s^-2)\n";
}

void CLoggerEntryMag::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogMag					&data = logData.magData;

	m_outFile	<< context.fmtTime(data.timeStamp)									<< "\t"
				<< context.fmtStatus(data.status)									<< "\t"
				<< data.magnetometers[0]											<< "\t"
				<< data.magnetometers[1]											<< "\t"
				<< data.magnetometers[2]											<< "\t"
				<< data.accelerometers[0]											<< "\t"
				<< data.accelerometers[1]											<< "\t"
				<< data.accelerometers[2]											<< "\n";
}

void CLoggerEntryMag::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogMag					&data = logData.magData;

	std::cout	<< std::setw(12) << getName()										<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << data.magnetometers[0]
				<< std::setw(12) << data.magnetometers[1]
				<< std::setw(12) << data.magnetometers[2]
				<< std::setw(12) << data.accelerometers[0]
				<< std::setw(12) << data.accelerometers[1]
				<< std::setw(12) << data.accelerometers[2]							<< "\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryMagCalib                                               -//
//----------------------------------------------------------------------//
	
std::string CLoggerEntryMagCalib::getName() const
{
	return "magCalib";
}

std::string CLoggerEntryMagCalib::getFileName() const
{
	return getName() + ".bin";
}

bool CLoggerEntryMagCalib::isBinaryFile() const
{
	return true;
}

void CLoggerEntryMagCalib::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	SBG_UNUSED_PARAMETER(context);
	
	m_outFile.write(reinterpret_cast<const char*>(logData.magCalibData.magData), sizeof(logData.magCalibData.magData));
}

}; // Namespace sbg
