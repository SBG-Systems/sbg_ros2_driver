// STL headers
#include <algorithm>
#include <string>
#include <stdexcept>

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgEComLib.h>

// Local headers
#include "loggerSettings.h"

namespace sbg
{
//----------------------------------------------------------------------//
//- Public getters & setters                                           -//
//----------------------------------------------------------------------//

void CLoggerSettings::setBasePath(const std::string &basePath)
{
	m_basePath = basePath;

	std::replace(m_basePath.begin(), m_basePath.end(), '\\', '/');

	if (m_basePath.back() != '/')
	{
		m_basePath.append("/");
	}
}

const std::string &CLoggerSettings::getBasePath() const
{
	return m_basePath;
}

void CLoggerSettings::setWriteToConsole(bool writeToConsole)
{
	m_writeToConsole = writeToConsole;
}

bool CLoggerSettings::getWriteToConsole() const
{
	return m_writeToConsole;
}

void CLoggerSettings::setWriteToFile(bool writeToFile)
{
	m_writeToFile = writeToFile;
}

bool CLoggerSettings::getWriteToFile() const
{
	return m_writeToFile;
}

void CLoggerSettings::setWriteHeaderToFile(bool writeHeaderToFile)
{
	m_writeHeaderToFile = writeHeaderToFile;
}

bool CLoggerSettings::getWriteHeaderToFile() const
{
	return m_writeHeaderToFile;
}

void CLoggerSettings::setFileDecimation(int32_t fileDecimation)
{
	if ( (fileDecimation > 0) && (fileDecimation < 10000) )
	{
		m_fileDecimation = fileDecimation;
	}
	else
	{
		throw std::invalid_argument("file decimation should be within 0 to 10000");
	}
}

int32_t CLoggerSettings::getFileDecimation() const
{
	return m_fileDecimation;
}

void CLoggerSettings::setConsoleDecimation(int32_t consoleDecimation)
{
	if ( (consoleDecimation > 0) && (consoleDecimation < 10000) )
	{
		m_consoleDecimation = consoleDecimation;
	}
	else
	{
		throw std::invalid_argument("console decimation should be within 0 to 10000");
	}
}

int32_t CLoggerSettings::getConsoleDecimation() const
{
	return m_consoleDecimation;
}

void CLoggerSettings::setDiscardInvalidTime(bool discardInvalidTime)
{
	m_discardInvalidTime = discardInvalidTime;
}

bool CLoggerSettings::getDiscardInvalidTime() const
{
	return m_discardInvalidTime;
}

void CLoggerSettings::setTimeMode(TimeMode timeMode)
{
	m_timeMode = timeMode;
}

CLoggerSettings::TimeMode CLoggerSettings::getTimeMode() const
{
	return m_timeMode;
}

void CLoggerSettings::setStatusFormat(CLoggerSettings::StatusFormat statusFormat)
{
	m_statusFormat = statusFormat;
}

CLoggerSettings::StatusFormat CLoggerSettings::getStatusFormat() const
{
	return m_statusFormat;
}

bool CLoggerSettings::isOutputConfValid() const
{
	if (m_writeToFile || m_writeToConsole)
	{
		return true;
	}
	else
	{
		return false;
	}
}

CLoggerSettings::InterfaceMode CLoggerSettings::getInterfaceMode() const
{
	return m_interfaceMode;
}

bool CLoggerSettings::hasInterfaceConf() const
{
	return (m_interfaceMode != InterfaceMode::Unknown);
}

void CLoggerSettings::setSerialConf(const Serial &serialConf)
{
	if (hasInterfaceConf())
	{
		throw std::logic_error("serial conf overrides a previous interface conf.");
	}

	m_serialConf		= serialConf;
	m_interfaceMode		= InterfaceMode::Serial;
}

const CLoggerSettings::Serial &CLoggerSettings::getSerialConf() const
{
	if (m_interfaceMode != InterfaceMode::Serial)
	{
		throw std::logic_error("no valid serial conf set.");
	}

	return m_serialConf;
}

void CLoggerSettings::setUdpConf(const Udp &udpConf)
{
	if (hasInterfaceConf())
	{
		throw std::logic_error("UDP conf overrides a previous interface conf.");
	}

	m_udpConf			= udpConf;
	m_interfaceMode		= InterfaceMode::Udp;
}

const CLoggerSettings::Udp &CLoggerSettings::getUdpConf() const
{
	if (m_interfaceMode != InterfaceMode::Udp)
	{
		throw std::logic_error("no valid UDP conf set.");
	}

	return m_udpConf;
}

void CLoggerSettings::setFileConf(const std::string &fileConf)
{
	if (hasInterfaceConf())
	{
		throw std::logic_error("file conf overrides a previous interface conf.");
	}

	m_fileConf			= fileConf;
	m_interfaceMode		= InterfaceMode::File;
}

const std::string &CLoggerSettings::getFileConf() const
{
	if (m_interfaceMode != InterfaceMode::File)
	{
		throw std::logic_error("no valid file conf set.");
	}

	return m_fileConf;
}

bool CLoggerSettings::isValid() const
{
	return isOutputConfValid() && hasInterfaceConf();
}

}; // Namespace sbg
