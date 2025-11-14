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
#include "loggerEntryEkf.h"

namespace sbg
{

//----------------------------------------------------------------------//
//- CLoggerEntryEkfEuler                                               -//
//----------------------------------------------------------------------//

std::string CLoggerEntryEkfEuler::getName() const
{
	return "euler";
}

void CLoggerEntryEkfEuler::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\troll\tpitch\tyaw\trollStd\tpitchStd\tyawStd\tmagHeading\tmagDecl\tmagIncl\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(deg)\t(deg)\t(deg)\t(deg)\t(deg)\t(deg)\t(deg)\t(deg)\t(deg)\n";
}

void CLoggerEntryEkfEuler::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogEkfEuler			&data = logData.ekfEulerData;

	m_outFile	<< context.fmtTime(data.timeStamp)									<< "\t"
				<< context.fmtStatus(data.status)									<< "\t"
				<< sbgRadToDegf(data.euler[0])										<< "\t"
				<< sbgRadToDegf(data.euler[1])										<< "\t"
				<< sbgRadToDegf(data.euler[2])										<< "\t"
				<< sbgRadToDegf(data.eulerStdDev[0])								<< "\t"
				<< sbgRadToDegf(data.eulerStdDev[1])								<< "\t"
				<< sbgRadToDegf(data.eulerStdDev[2])								<< "\t"
				<< sbgRadToDegf(sbgEComLogEkfEulerGetMagneticHeading(&data))		<< "\t"
				<< sbgRadToDegf(data.magDeclination)								<< "\t"
				<< sbgRadToDegf(data.magInclination)								<< "\n";
}

void CLoggerEntryEkfEuler::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogEkfEuler			&data = logData.ekfEulerData;

	std::cout	<< std::setw(12) << getName()										<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << sbgRadToDegf(data.euler[0])
				<< std::setw(12) << sbgRadToDegf(data.euler[1])
				<< std::setw(12) << sbgRadToDegf(data.euler[2])
				<< std::setw(12) << sbgRadToDegf(data.eulerStdDev[0])
				<< std::setw(12) << sbgRadToDegf(data.eulerStdDev[1])
				<< std::setw(12) << sbgRadToDegf(data.eulerStdDev[2])
				<< std::setw(12) << sbgRadToDegf(sbgEComLogEkfEulerGetMagneticHeading(&data))
				<< std::setw(12) << sbgRadToDegf(data.magDeclination)
				<< std::setw(12) << sbgRadToDegf(data.magInclination)				<< "\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryEkfQuat                                                -//
//----------------------------------------------------------------------//

std::string CLoggerEntryEkfQuat::getName() const
{
	return "quat";
}

void CLoggerEntryEkfQuat::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\tqW\tqX\tqY\tqZ\trollStd\tpitchStd\tyawStd\tmagDecl\tmagIncl\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(au)\t(au)\t(au)\t(au)\t(deg)\t(deg)\t(deg)\t(deg)\t(deg)\n";
}

void CLoggerEntryEkfQuat::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogEkfQuat				&data = logData.ekfQuatData;

	m_outFile	<< context.fmtTime(data.timeStamp)									<< "\t"
				<< context.fmtStatus(data.status)									<< "\t"
				<< data.quaternion[0]												<< "\t"
				<< data.quaternion[1]												<< "\t"
				<< data.quaternion[2]												<< "\t"
				<< data.quaternion[3]												<< "\t"
				<< sbgRadToDegf(data.eulerStdDev[0])								<< "\t"
				<< sbgRadToDegf(data.eulerStdDev[1])								<< "\t"
				<< sbgRadToDegf(data.eulerStdDev[2])								<< "\t"
				<< sbgRadToDegf(data.magDeclination)								<< "\t"
				<< sbgRadToDegf(data.magInclination)								<< "\n";
}

void CLoggerEntryEkfQuat::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogEkfQuat				&data = logData.ekfQuatData;

	std::cout	<< std::setw(12) << getName()										<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << data.quaternion[0]
				<< std::setw(12) << data.quaternion[1]
				<< std::setw(12) << data.quaternion[2]
				<< std::setw(12) << data.quaternion[3]
				<< std::setw(12) << sbgRadToDegf(data.eulerStdDev[0])
				<< std::setw(12) << sbgRadToDegf(data.eulerStdDev[1])
				<< std::setw(12) << sbgRadToDegf(data.eulerStdDev[2])
				<< std::setw(12) << sbgRadToDegf(data.magDeclination)
				<< std::setw(12) << sbgRadToDegf(data.magInclination)				<< "\n";
}

//----------------------------------------------------------------------//
//- CLoggerEntryEkfNav                                                 -//
//----------------------------------------------------------------------//

std::string CLoggerEntryEkfNav::getName() const
{
	return "nav";
}

void CLoggerEntryEkfNav::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\tvelN\tvelE\tvelD\tvelStdN\tvelStdE\tvelStdD\tlatitude\tlongitude\taltitude\tlatitudeStd\tlongitudeStd\taltitudeStd\tundulation\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(deg)\t(deg)\t(m)\t(m)\t(m)\t(m)\t(m)\n";
}

void CLoggerEntryEkfNav::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogEkfNav				&data = logData.ekfNavData;

	m_outFile	<< context.fmtTime(data.timeStamp)									<< "\t"
				<< context.fmtStatus(data.status)									<< "\t"
				<< data.velocity[0]													<< "\t"
				<< data.velocity[1]													<< "\t"
				<< data.velocity[2]													<< "\t"
				<< data.velocityStdDev[0]											<< "\t"
				<< data.velocityStdDev[1]											<< "\t"
				<< data.velocityStdDev[2]											<< "\t"
				<< data.position[0]													<< "\t"
				<< data.position[1]													<< "\t"
				<< data.position[2]													<< "\t"
				<< data.positionStdDev[0]											<< "\t"
				<< data.positionStdDev[1]											<< "\t"
				<< data.positionStdDev[2]											<< "\t"
				<< data.undulation													<< "\n";
}

void CLoggerEntryEkfNav::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogEkfNav				&data = logData.ekfNavData;

	std::cout	<< std::setw(12) << getName()										<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << data.velocity[0]
				<< std::setw(12) << data.velocity[1]
				<< std::setw(12) << data.velocity[2]
				<< std::setw(12) << data.velocityStdDev[0]
				<< std::setw(12) << data.velocityStdDev[1]
				<< std::setw(12) << data.velocityStdDev[2]
				<< std::setw(12) << data.position[0]
				<< std::setw(12) << data.position[1]
				<< std::setw(12) << data.position[2]
				<< std::setw(12) << data.positionStdDev[0]
				<< std::setw(12) << data.positionStdDev[1]
				<< std::setw(12) << data.positionStdDev[2]
				<< std::setw(12) << data.undulation									<< "\n";
}


//----------------------------------------------------------------------//
//- CLoggerEntryEkfVelBody                                             -//
//----------------------------------------------------------------------//

std::string CLoggerEntryEkfVelBody::getName() const
{
	return "velBody";
}

void CLoggerEntryEkfVelBody::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<< "\tstatus\tvelX\tvelY\tvelZ\tvelStdX\tvelStdY\tvelStdZ\n";
	m_outFile	<< context.getTimeUnit()		<< "\t(na)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\t(m.s^-1)\n";
}

void CLoggerEntryEkfVelBody::writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogEkfVelBody				&data = logData.ekfVelBody;

	m_outFile	<< context.fmtTime(data.timeStamp)									<< "\t"
				<< context.fmtStatus(data.status)									<< "\t"
				<< data.velocity[0]													<< "\t"
				<< data.velocity[1]													<< "\t"
				<< data.velocity[2]													<< "\t"
				<< data.velocityStdDev[0]											<< "\t"
				<< data.velocityStdDev[1]											<< "\t"
				<< data.velocityStdDev[2]											<< "\n";
}

void CLoggerEntryEkfVelBody::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogEkfVelBody				&data = logData.ekfVelBody;

	std::cout	<< std::setw(12) << getName()										<< ": "
				<< std::setw(12) << context.fmtStatus(data.status)
				<< std::setw(12) << data.velocity[0]
				<< std::setw(12) << data.velocity[1]
				<< std::setw(12) << data.velocity[2]
				<< std::setw(12) << data.velocityStdDev[0]
				<< std::setw(12) << data.velocityStdDev[1]
				<< std::setw(12) << data.velocityStdDev[2]							<< "\n";
}

}; // Namespace sbg
