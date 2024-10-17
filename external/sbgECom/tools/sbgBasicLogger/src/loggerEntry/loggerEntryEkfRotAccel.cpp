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
#include "loggerEntryEkfRotAccel.h"

namespace sbg
{

//----------------------------------------------------------------------//
//- CLoggerEntryEkfRotAccelBody                                        -//
//----------------------------------------------------------------------//

std::string CLoggerEntryEkfRotAccelBody::getName() const
{
	return "rotAccelBody";
}

void CLoggerEntryEkfRotAccelBody::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\taccelX\taccelY\taccelZ\trateX\trateY\trateZ\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(m.s^-2)\t(m.s^-2)\t(m.s^-2)\t(deg.s^-1)\t(deg.s^-1)\t(deg.s^-1)\n";
}

void CLoggerEntryEkfRotAccelBody::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogEkfRotAccel			&data = logData.ekfRotAccel;

	m_outFile	<< context.fmtTime(data.timeStamp)									<< "\t"
				<< context.fmtStatus(data.status)									<< "\t"
				<< data.acceleration[0]												<< "\t"
				<< data.acceleration[1]												<< "\t"
				<< data.acceleration[2]												<< "\t"
				<< sbgRadToDegf(data.rate[0])										<< "\t"
				<< sbgRadToDegf(data.rate[1])										<< "\t"
				<< sbgRadToDegf(data.rate[2])										<< "\n";
}

void CLoggerEntryEkfRotAccelBody::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogEkfRotAccel			&data = logData.ekfRotAccel;

	std::cout	<< std::setw(12) << getName()										<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << data.acceleration[0]
				<< std::setw(12) << data.acceleration[1]
				<< std::setw(12) << data.acceleration[2]
				<< std::setw(12) << sbgRadToDegf(data.rate[0])
				<< std::setw(12) << sbgRadToDegf(data.rate[1])
				<< std::setw(12) << sbgRadToDegf(data.rate[2])						<< "\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryEkfRotAccelNed                                         -//
//----------------------------------------------------------------------//

std::string CLoggerEntryEkfRotAccelNed::getName() const
{
	return "rotAccelNed";
}

void CLoggerEntryEkfRotAccelNed::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\taccelN\taccelE\taccelD\trateN\trateE\trateD\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(m.s^-2)\t(m.s^-2)\t(m.s^-2)\t(deg.s^-1)\t(deg.s^-1)\t(deg.s^-1)\n";
}

void CLoggerEntryEkfRotAccelNed::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogEkfRotAccel			&data = logData.ekfRotAccel;

	m_outFile	<< context.fmtTime(data.timeStamp)									<< "\t"
				<< context.fmtStatus(data.status)									<< "\t"
				<< data.acceleration[0]												<< "\t"
				<< data.acceleration[1]												<< "\t"
				<< data.acceleration[2]												<< "\t"
				<< sbgRadToDegf(data.rate[0])										<< "\t"
				<< sbgRadToDegf(data.rate[1])										<< "\t"
				<< sbgRadToDegf(data.rate[2])										<< "\n";
}

void CLoggerEntryEkfRotAccelNed::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogEkfRotAccel			&data = logData.ekfRotAccel;

	std::cout	<< std::setw(12) << getName()										<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << data.acceleration[0]
				<< std::setw(12) << data.acceleration[1]
				<< std::setw(12) << data.acceleration[2]
				<< std::setw(12) << sbgRadToDegf(data.rate[0])
				<< std::setw(12) << sbgRadToDegf(data.rate[1])
				<< std::setw(12) << sbgRadToDegf(data.rate[2])						<< "\n";
}

}; // Namespace sbg
