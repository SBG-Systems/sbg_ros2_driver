// STL headers
#include <algorithm>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <ctime>

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgEComLib.h>

// Local headers
#include "loggerContext.h"
#include "loggerSettings.h"

namespace sbg
{

//----------------------------------------------------------------------//
//- Constructor / destructor                                           -//
//----------------------------------------------------------------------//

CLoggerContext::CLoggerContext(const CLoggerSettings &settings):
m_settings(settings)
{
}

//----------------------------------------------------------------------//
//- Getters/settings                                                   -//
//----------------------------------------------------------------------//

const CLoggerSettings &CLoggerContext::getSettings() const
{
	return m_settings;
}

void CLoggerContext::setUtcTime(const SbgEComLogUtc &utcTime)
{
	m_lastUtcTime		= utcTime;

	if (sbgEComLogUtcGetUtcStatus(&m_lastUtcTime) != SBG_ECOM_UTC_STATUS_INVALID)
	{
		m_utcTimeIsValid = true;
	}
}

bool CLoggerContext::isUtcTimeValid() const
{
	return m_utcTimeIsValid;
}

std::string CLoggerContext::getTimeColTitle() const
{
	if (getSettings().getTimeMode() == CLoggerSettings::TimeMode::UtcIso8601)
	{
		return "iso8601Time";
	}
	else
	{
		return "timestamp";
	}
}

std::string CLoggerContext::getTimeUnit() const
{
	if (getSettings().getTimeMode() == CLoggerSettings::TimeMode::UtcIso8601)
	{
		return "(yyyy-mm-ddThh:mm:ss.sssZ)";
	}
	else
	{
		return "(us)";
	}
}

std::string CLoggerContext::fmtTime(uint32_t timeStampUs) const
{
	char				fullTimeStr[sizeof("yyyy-mm-ddThh:mm:ss.sssZ")];

	if (getSettings().getTimeMode() == CLoggerSettings::TimeMode::UtcIso8601)
	{	
		if (isUtcTimeValid())
		{
			char		timeSecStr[sizeof("yyyy-mm-ddThh:mm:ss")];
			std::tm		utcTime{};
		
			utcTime.tm_year = m_lastUtcTime.year-1900;
			utcTime.tm_mon	= m_lastUtcTime.month-1;
			utcTime.tm_mday	= m_lastUtcTime.day;

			utcTime.tm_hour	= m_lastUtcTime.hour;
			utcTime.tm_min	= m_lastUtcTime.minute;
			utcTime.tm_sec	= m_lastUtcTime.second;

			//
			// Output standard UTC time only
			//
			utcTime.tm_isdst = 0;
		
			//
			// Apply the offset between the latest UTC time and the current time stamp
			//
			std::time_t		utcTimeSecs		= std::mktime(&utcTime);
			int32_t			elapsedTimeUs	= timeStampUs - m_lastUtcTime.timeStamp;
			int32_t			elapsedTime		= elapsedTimeUs/1000;
			int32_t			remainingUs		= elapsedTimeUs - elapsedTime*1000;

			utcTimeSecs = utcTimeSecs + elapsedTime;
		
			//
			// Write ISO 8601 time: yyyy-mm-ddThh:mm:ss.sssZ
			//
			std::strftime(timeSecStr, sizeof(timeSecStr), "%FT%T", &utcTime);
			std::snprintf(fullTimeStr, sizeof(fullTimeStr), "%s.%03" PRId32 "Z", timeSecStr, remainingUs);
		}
		else
		{
			std::snprintf(fullTimeStr, sizeof(fullTimeStr), "%*" PRIu32, (int)sizeof(fullTimeStr)-1, timeStampUs);
		}
	}
	else
	{
		std::snprintf(fullTimeStr, sizeof(fullTimeStr), "%10" PRIu32, timeStampUs);
	}

	return fullTimeStr; 
}

}; // Namespace sbg
