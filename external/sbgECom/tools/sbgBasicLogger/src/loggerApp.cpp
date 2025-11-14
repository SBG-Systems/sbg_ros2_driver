// STL headers
#include <functional>
#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgECom.h>
#include <sbgEComGetVersion.h>

// Argtable3 headers
#include <argtable3.h>

// Project headers
#include <loggerManager/loggerContext.h>
#include <loggerManager/loggerManager.h>
#include <loggerManager/loggerSettings.h>
#include <loggerEntry/loggerEntryAidings.h>
#include <loggerEntry/loggerEntryDvl.h>
#include <loggerEntry/loggerEntryEkf.h>
#include <loggerEntry/loggerEntryEkfRotAccel.h>
#include <loggerEntry/loggerEntryEvent.h>
#include <loggerEntry/loggerEntryGeneral.h>
#include <loggerEntry/loggerEntryGnss.h>
#include <loggerEntry/loggerEntryImu.h>
#include <loggerEntry/loggerEntryMag.h>
#include <loggerEntry/loggerEntryShipMotion.h>
#include <loggerEntry/loggerEntryVibMon.h>


// Local headers
#include "loggerApp.h"

namespace sbg
{
//----------------------------------------------------------------------//
//- Private methods                                                    -//
//----------------------------------------------------------------------//

std::string CLoggerApp::getApplicationName() const
{
	return "sbgBasicLogger";
}

void CLoggerApp::createLogger(const CLoggerSettings &settings)
{
	m_manager = std::make_unique<CLoggerManager>(settings);

	//
	// Register all aiding logs
	//
	m_manager->registerLog<sbg::CLoggerEntryOdometer>();
	m_manager->registerLog<sbg::CLoggerEntryAirData>();
	m_manager->registerLog<sbg::CLoggerEntryDepth>();
	m_manager->registerLog<sbg::CLoggerEntryUsbl>();
	m_manager->registerLog<sbg::CLoggerEntryVelocity1>();

	//
	// DVL aiding
	///
	m_manager->registerLog<sbg::CLoggerEntryDvlBottom>();
	m_manager->registerLog<sbg::CLoggerEntryDvlWater>();

	//
	// Register EKF logs
	//
	m_manager->registerLog<sbg::CLoggerEntryEkfEuler>();
	m_manager->registerLog<sbg::CLoggerEntryEkfQuat>();
	m_manager->registerLog<sbg::CLoggerEntryEkfNav>();
	m_manager->registerLog<sbg::CLoggerEntryEkfVelBody>();
	m_manager->registerLog<sbg::CLoggerEntryEkfRotAccelBody>();
	m_manager->registerLog<sbg::CLoggerEntryEkfRotAccelNed>();

	//
	// Register event in/out logs
	//
	m_manager->registerLog<sbg::CLoggerEntryEventInA>();
	m_manager->registerLog<sbg::CLoggerEntryEventInB>();
	m_manager->registerLog<sbg::CLoggerEntryEventInC>();
	m_manager->registerLog<sbg::CLoggerEntryEventInD>();
	m_manager->registerLog<sbg::CLoggerEntryEventInE>();
	m_manager->registerLog<sbg::CLoggerEntryEventOutA>();
	m_manager->registerLog<sbg::CLoggerEntryEventOutB>();

	//
	// Register general logs
	//
	m_manager->registerLog<sbg::CLoggerEntryUtcTime>();
	m_manager->registerLog<sbg::CLoggerEntryStatus>();
	m_manager->registerLog<sbg::CLoggerEntryDiag>();
	m_manager->registerLog<sbg::CLoggerEntryPtpStatus>();
	m_manager->registerLog<sbg::CLoggerEntryRtcmRaw>();

	//
	// Register GNSS 1 logs
	//
	m_manager->registerLog<sbg::CLoggerEntryGnss1Vel>();
	m_manager->registerLog<sbg::CLoggerEntryGnss1Pos>();
	m_manager->registerLog<sbg::CLoggerEntryGnss1Hdt>();
	m_manager->registerLog<sbg::CLoggerEntryGnss1Raw>();
	m_manager->registerLog<sbg::CLoggerEntryGnss1Sat>();

	//
	// Register GNSS 2 logs
	//
	m_manager->registerLog<sbg::CLoggerEntryGnss2Vel>();
	m_manager->registerLog<sbg::CLoggerEntryGnss2Pos>();
	m_manager->registerLog<sbg::CLoggerEntryGnss2Hdt>();
	m_manager->registerLog<sbg::CLoggerEntryGnss2Raw>();
	m_manager->registerLog<sbg::CLoggerEntryGnss2Sat>();

	//
	// IMU logs
	//
	m_manager->registerLog<sbg::CLoggerEntryImuData>();
	m_manager->registerLog<sbg::CLoggerEntryImuShort>();
	m_manager->registerLog<sbg::CLoggerEntryImuFast>();

	//
	// Magnetometer logs
	//
	m_manager->registerLog<sbg::CLoggerEntryMag>();
	m_manager->registerLog<sbg::CLoggerEntryMagCalib>();

	//
	// Ship Motion Logs
	//
	m_manager->registerLog<sbg::CLoggerEntryShipMotionRt>();
	m_manager->registerLog<sbg::CLoggerEntryShipMotionHp>();

	//
	// Vibration monitoring logs
	//
	m_manager->registerLog<sbg::CLoggerEntryVibMonFft>();
	m_manager->registerLog<sbg::CLoggerEntryVibMonReport>();
	
}

void CLoggerApp::process()
{
	CLoggerManager::StreamStatus streamStatus;

	//
	// Stop when the end of stream is reached or the user cancels
	// If there is no more data in the input stream, just wait until new data arrives
	//
	do
	{
		streamStatus = m_manager->processOneLog();

		if (streamStatus == CLoggerManager::StreamStatus::NoMoreData)
		{
			sbgSleep(1);
		}
	} while ( (streamStatus != CLoggerManager::StreamStatus::EndOfStream) && m_continueCb() );
}

CLoggerSettings CLoggerApp::processArgs(int argc, char **argv)
{
	CLoggerSettings						 settings;

	struct arg_lit						*pHelpArg;
	struct arg_lit						*pVersionArg;
	struct arg_str						*pUdpAddrArg;
	struct arg_int						*pUdpPortInArg;
	struct arg_int						*pUdpPortOutArg;
	struct arg_str						*pSerialDeviceArg;
	struct arg_int						*pSerialBaudrateArg;
	struct arg_file						*pInputFileArg;
	struct arg_lit						*pWriteLogsArg;
	struct arg_str						*pWriteLogsDirArg;
	struct arg_int						*pFileDecimationArg;
	struct arg_int						*pScreenDecimationArg;
	struct arg_lit						*pPrintLogsArg;
	struct arg_lit						*pLogsHeaderArg;
	struct arg_str						*pStatusFormatArg;
	struct arg_str						*pTimeModeArg;
	struct arg_lit						*pDiscardInvalidTimeArg;
	struct arg_end						*pEndArg;

	void								*argTable[] =
	{
		pHelpArg				= arg_lit0(		"h",	"help",												"display this help and exit"),
		pVersionArg				= arg_lit0(		"v",	"version",											"display sbgECom version and exit"),

		pUdpAddrArg				= arg_str0(		"a",	"addr-ip",				"IP address",				"open an UDP interface"),
		pUdpPortInArg			= arg_int0(		"I",	"udp-port-in",			"UDP port in",				"UDP port in"),
		pUdpPortOutArg			= arg_int0(		"O",	"udp-port-out",			"UDP port out",				"UDP port out"),

		pSerialDeviceArg		= arg_str0(		"s",	"serial-device",		"SERIAL_DEVICE",			"open a serial interface"),
		pSerialBaudrateArg		= arg_int0(		"r",	"serial-baudrate",		"SERIAL_BAUDRATE",			"serial baudrate"),

		pInputFileArg			= arg_file0(	"i",	"input-file",			"INPUT-FILE",				"input file"),

		pWriteLogsArg			= arg_lit0(		"w",	"write-logs",										"write logs in different files"),
		pWriteLogsDirArg		= arg_str0(		"o",	"dir",					"DIRECTORY",				"directory to write logs into"),

		pFileDecimationArg		= arg_int0(		"d",	"file-decimation",		"FILE DECIMATION",			"file decimation"),
		pScreenDecimationArg	= arg_int0(		"c",	"console-decimation",	"CONSOLE DECIMATION",		"output stream decimation"),

		pPrintLogsArg			= arg_lit0(		"p",	"print-logs",										"print the logs on the output stream"),
		pLogsHeaderArg			= arg_lit0(		"H",	"disable-header",									"disable header for files"),

		pStatusFormatArg		= arg_str0(		"f",	"status-format",		"decimal or hexadecimal",	"select the format to use for status fields"),
		pTimeModeArg			= arg_str0(		"m",	"time-mode",			"timestamp or utcIso8601",	"select time base to output"),
		pDiscardInvalidTimeArg	= arg_lit0(		"t",	"discard-invalid-time",								"discard data without a valid UTC time"),

		pEndArg					= arg_end(20),
	};

	if (arg_nullcheck(argTable) != 0)
	{
		throw std::bad_alloc();
	}

	//
	// Parse input arguments and the extract settings
	//
	int argError = arg_parse(argc, argv, argTable);

	try
	{
		//
		// special case: '--help' and '--version' take precedence over error reporting
		//
		if (pHelpArg->count != 0)
		{
			std::cout << "Usage: " << getApplicationName();
			arg_print_syntax(stdout, argTable, "\n\n");

			std::cout	<< "Manage sbgECom logs in text format.\n\n"
						<< "Serial example: " << getApplicationName() << " -s <SERIAL-PORT> -r <BAUDRATE> -p\n"
						<< "   UDP example: " << getApplicationName() << " -a <IP_ADDR> -I <UDP_PORT_IN> -O <UDP_PORT_OUT> -p\n"
						<< "  File example: " << getApplicationName() << " -i <BINARY_FILE> -p\n"
						<< std::endl;

			std::cout	<< "Logs affected by decimation:\n"
						<< "  - IMU short\n"
						<< "  - IMU fast\n"
						<< "  - IMU data\n"
						<< std::endl;

			arg_print_glossary(stdout, argTable, "  %-50s %s\n");
		}
		else if (pVersionArg->count != 0)
		{
			std::cout << sbgEComGetVersionAsString() << std::endl;
		}
		else if (argError != 0)
		{
			throw std::invalid_argument("");
		}
		else
		{
			//
			// Parse and set all output related configuration such as decimation, headers etc
			//
			if (pLogsHeaderArg->count == 0)
			{
				settings.setWriteHeaderToFile(true);
			}

			if (pWriteLogsArg->count != 0)
			{
				settings.setWriteToFile(true);

				if (pFileDecimationArg->count != 0)
				{
					settings.setFileDecimation(pFileDecimationArg->ival[0]);
				}

				if (pWriteLogsDirArg->count != 0)
				{
					settings.setBasePath(pWriteLogsDirArg->sval[0]);
				}
			}

			if (pPrintLogsArg->count != 0)
			{
				settings.setWriteToConsole(true);

				if (pScreenDecimationArg->count != 0)
				{
					settings.setConsoleDecimation(pScreenDecimationArg->ival[0]);
				}
			}

			if (pStatusFormatArg->count != 0)
			{
				const std::string statusFmt = pStatusFormatArg->sval[0];

				if (statusFmt == "decimal")
				{
					settings.setStatusFormat(CLoggerSettings::StatusFormat::Decimal);
				}
				else if (statusFmt == "hexadecimal")
				{
					settings.setStatusFormat(CLoggerSettings::StatusFormat::Hexadecimal);
				}
				else
				{
					throw std::invalid_argument("invalid status-format argument.");
				}
			}

			if (pTimeModeArg->count != 0)
			{
				const std::string timeMode = pTimeModeArg->sval[0];

				if (timeMode == "timestamp")
				{
					settings.setTimeMode(CLoggerSettings::TimeMode::TimeStamp);
				}
				else if (timeMode == "utcIso8601")
				{
					settings.setTimeMode(CLoggerSettings::TimeMode::UtcIso8601);
				}
				else
				{
					throw std::invalid_argument("invalid time-mode argument.");
				}
			}

			if (pDiscardInvalidTimeArg->count != 0)
			{
				settings.setDiscardInvalidTime(true);
			}

			if (!settings.isOutputConfValid())
			{
				throw std::invalid_argument("Invalid mode, missing \"-p\" and/or \"-w\" arguments");
			}

			//
			// Parse and set input interface related configuration, user should select only one interface
			//
			try
			{
				if ( (pSerialDeviceArg->count != 0) && (pSerialBaudrateArg->count != 0) )
				{
					CLoggerSettings::Serial	serialConf;

					serialConf.location = pSerialDeviceArg->sval[0];
					serialConf.baudRate	= pSerialBaudrateArg->ival[0];

					settings.setSerialConf(serialConf);
				}

				if ( (pUdpAddrArg->count != 0) && (pUdpPortInArg->count != 0) && (pUdpPortOutArg->count != 0) )
				{
					CLoggerSettings::Udp	udpConf;

					udpConf.remoteIp	= pUdpAddrArg->sval[0];
					udpConf.remotePort	= pUdpPortOutArg->ival[0];
					udpConf.listenPort	= pUdpPortInArg->ival[0];

					settings.setUdpConf(udpConf);
				}

				if (pInputFileArg->count != 0)
				{
					settings.setFileConf(pInputFileArg->filename[0]);
				}
			}
			catch (...)
			{
				throw std::invalid_argument("Please select only one input interface.");
			}

			if (!settings.hasInterfaceConf())
			{
				throw std::invalid_argument("Please select at least one input interface among serial, file or UDP.");
			}
		}
	}
	catch (std::exception &e)
	{
		arg_print_errors(stderr, pEndArg, getApplicationName().c_str());
		arg_freetable(argTable, SBG_ARRAY_SIZE(argTable));

		throw e;
	}

	arg_freetable(argTable, SBG_ARRAY_SIZE(argTable));

	return settings;
}

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

void CLoggerApp::setContinueCallback(std::function<bool()> continueCb) noexcept
{
	m_continueCb = continueCb;
}

int CLoggerApp::exec(int argc, char **argv) noexcept
{
	int				exitCode = EXIT_SUCCESS;

	try
	{
		CLoggerSettings settings = processArgs(argc, argv);

		if (settings.isValid())
		{
			createLogger(settings);
			process();
			m_manager.reset(nullptr);
		}
	}
	catch (std::exception &e)
	{
		if (strlen(e.what()) > 0)
		{
			std::cout << getApplicationName() << ": " << e.what() << std::endl;
		}

		std::cout << "Try '" << getApplicationName() << " --help' for more information." << std::endl;
		exitCode = EXIT_FAILURE;
	}

	return exitCode;
}

}; // namespace sbg
