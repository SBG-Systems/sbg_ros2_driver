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
//- Constructor/destructor (ILoggerBase)                               -//
//----------------------------------------------------------------------//

ILoggerBase::~ILoggerBase()
{
}

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

void ILoggerBase::process(CLoggerContext &context, const SbgEComLogUnion &logData)
{
	if (prepareProcess(context, logData))
	{
		if (!shouldDiscardData(context))
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

			//
			// Perform additional processing specific to the derived class (e.g., file, etc.)
			//
			doProcess(context, logData);
		}
	}
}

//----------------------------------------------------------------------//
//- Overridable Hooks (Private Template Method Pattern)                -//
//----------------------------------------------------------------------//

bool ILoggerBase::shouldDiscardData(const CLoggerContext &context) const
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

bool ILoggerBase::prepareProcess(CLoggerContext &context, const SbgEComLogUnion &logData)
{
	SBG_UNUSED_PARAMETER(context);
	SBG_UNUSED_PARAMETER(logData);

	return true;
}

void ILoggerBase::doProcess(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	SBG_UNUSED_PARAMETER(context);
	SBG_UNUSED_PARAMETER(logData);
}

void ILoggerBase::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	SBG_UNUSED_PARAMETER(context);
	SBG_UNUSED_PARAMETER(logData);
}

//----------------------------------------------------------------------//
//- Constructor/destructor (IFileLogger)                               -//
//----------------------------------------------------------------------//

IFileLogger::~IFileLogger()
{
}

//----------------------------------------------------------------------//
//- Public getters                                                     -//
//----------------------------------------------------------------------//

std::string IFileLogger::getFileName() const
{
	return getName() + ".txt";
}

bool IFileLogger::isBinaryFile() const
{
	return false;
}

void IFileLogger::doProcess(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
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

void IFileLogger::createFile(const CLoggerContext &context)
{
	if (!m_outFile.is_open())
	{
		std::ofstream::openmode		mode;

		if (isBinaryFile())
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

void IFileLogger::writeHeaderToFile(const CLoggerContext &context)
{
	SBG_UNUSED_PARAMETER(context);
}

}; // Namespace sbg
