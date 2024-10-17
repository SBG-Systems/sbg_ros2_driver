// STL headers
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

// sbgCommonLib headers
#include <sbgCommon.h>
#include <string/sbgString.h>

// sbgECom headers
#include <sbgEComLib.h>

// Local headers
#include "loggerEntry.h"
#include "loggerContext.h"

namespace sbg
{

//----------------------------------------------------------------------//
//- Constructor/destructor                                             -//
//----------------------------------------------------------------------//

ILoggerEntry::~ILoggerEntry()
{
}

//----------------------------------------------------------------------//
//- Public getters                                                     -//
//----------------------------------------------------------------------//

std::string ILoggerEntry::getFileName() const
{
	return getName() + ".txt";
}

bool ILoggerEntry::isBinary() const
{
	return false;
}

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

void ILoggerEntry::process(CLoggerContext &context, const SbgEComLogUnion &logData)
{
	if (preProcess(context, logData))
	{
		if (!getDiscardData(context))
		{
			if (context.getSettings().getWriteToConsole())
			{
				//
				// Define the float format for console output
				// We do it every time we have to output to console in case it has been changed by any other std::cout operation
				//
				std::cout << std::fixed;

				writeDataToConsole(context, logData);
			}

			if (context.getSettings().getWriteToFile())
			{
				createFile(context);

				if (context.getSettings().getWriteHeaderToFile() && !m_headerWritten)
				{
					writeHeaderToFile(context);
					m_headerWritten = true;
				}

				writeDataToFile(context, logData);
			}
		}
	}
}

//----------------------------------------------------------------------//
//- Private methods                                                    -//
//----------------------------------------------------------------------//

bool ILoggerEntry::getDiscardData(const CLoggerContext &context) const
{
	if (context.getSettings().getDiscardInvalidTime())
	{
		return !context.isUtcTimeValid();
	}
	else
	{
		return false;
	}
}

void ILoggerEntry::createFile(const CLoggerContext &context)
{
	if (!m_outFile.is_open())
	{
		std::ofstream::openmode		mode;

		if (isBinary())
		{
			mode = std::ofstream::out | std::ofstream::binary;
		}
		else
		{
			mode = std::ofstream::out;
		}

		m_outFile.open(context.getSettings().getBasePath() + getFileName(), mode);

		//
		// Configure the file output stream float format.
		//
		m_outFile << std::setprecision(9) << std::fixed;
	}
}

bool ILoggerEntry::preProcess(CLoggerContext &context, const SbgEComLogUnion &logData)
{
	SBG_UNUSED_PARAMETER(context);
	SBG_UNUSED_PARAMETER(logData);

	return true;
}

void ILoggerEntry::writeHeaderToFile(const CLoggerContext &context)
{
	SBG_UNUSED_PARAMETER(context);
}

void ILoggerEntry::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	SBG_UNUSED_PARAMETER(context);
	SBG_UNUSED_PARAMETER(logData);
}

}; // Namespace sbg
