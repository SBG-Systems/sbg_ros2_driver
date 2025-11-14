// STL headers
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <inttypes.h>
#include <string>
#include <stdexcept>
#include <type_traits>

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
		return "(yyyy-mm-ddThh:mm:ss.ssssssZ)";
	}
	else
	{
		return "(us)";
	}
}

std::string CLoggerContext::fmtTime(uint32_t timeStampUs) const
{
	char				fullTimeStr[sizeof("yyyy-mm-ddThh:mm:ss.ssssssZ")];

	//
	// We have to do arithemtics on time_t so make sure it is defined as an integer type
	//
	static_assert(std::is_integral<time_t>::value);

	if (getSettings().getTimeMode() == CLoggerSettings::TimeMode::UtcIso8601)
	{	
		if (isUtcTimeValid())
		{
			char			timeSecStr[sizeof("yyyy-mm-ddThh:mm:ss")];
			std::tm			utcTimeTm{};
			uint32_t		remainingUs;

			//
			// Convert the latest UTC date time to a time_t to ease date/time arithmetics
			//
			utcTimeTm.tm_year	= m_lastUtcTime.year-1900;
			utcTimeTm.tm_mon	= m_lastUtcTime.month-1;
			utcTimeTm.tm_mday	= m_lastUtcTime.day;

			utcTimeTm.tm_hour	= m_lastUtcTime.hour;
			utcTimeTm.tm_min	= m_lastUtcTime.minute;
			utcTimeTm.tm_sec	= m_lastUtcTime.second;

			utcTimeTm.tm_isdst	= 0;

			//
			// As the utcTime is only able to represent plain seconds, correct for any residual nanoSecond value
			//
			std::time_t		utcTime			= std::mktime(&utcTimeTm);
			int64_t			utcTimeUs		= utcTime * 1000000;
			uint32_t		utcTimeStampUs	= (m_lastUtcTime.timeStamp - m_lastUtcTime.nanoSecond/1000);
			int32_t			elapsedTimeUs	= timeStampUs - utcTimeStampUs;
			
			//
			// Apply the offset between the latest UTC time and the current timestamp
			//
			utcTimeUs		= utcTimeUs + elapsedTimeUs;
			utcTime			= (time_t)(utcTimeUs/1000000);
			remainingUs		= (uint32_t)(utcTimeUs - (int64_t)utcTime*1000000);

			//
			// Write ISO 8601 time: yyyy-mm-ddThh:mm:ss.sssZ
			//
			if (std::strftime(timeSecStr, sizeof(timeSecStr), "%FT%T", std::gmtime(&utcTime)) == 0)
			{
				throw std::overflow_error("timeSecStr is too small");
			}

			int ret = std::snprintf(fullTimeStr, sizeof(fullTimeStr), "%s.%06" PRIu32 "Z", timeSecStr, remainingUs);

			if (ret >= sizeof(fullTimeStr))
			{
				throw std::overflow_error("fullTimeStr is too small");
			}
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
