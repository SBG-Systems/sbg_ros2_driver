// STL headers
#include <iomanip>
#include <iostream>

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgEComLib.h>

// Project headers
#include <loggerManager/loggerContext.h>
#include <loggerManager/loggerEntry.h>

// Local headers
#include "loggerEntryVibMon.h"

namespace sbg
{

//----------------------------------------------------------------------//
//- Private helpers                                                    -//
//----------------------------------------------------------------------//

/*!
 * Convert an axis to a string.
 *
 * \param[in]	axis								Axis.
 * \return											Axis string.
 */
std::string convertAxis(SbgEComVibMonAxis axis)
{
	std::string							 axisStr = "unknown";

	switch (axis)
	{
	case SBG_ECOM_VIB_MON_AXIS_X:
		axisStr = "x";
		break;
	case SBG_ECOM_VIB_MON_AXIS_Y:
		axisStr = "y";
		break;
	case SBG_ECOM_VIB_MON_AXIS_Z:
		axisStr = "z";
		break;
	}

	return axisStr;
}

/*!
 * Convert a window to a string.
 *
 * \param[in]	window								Window.
 * \return											Window string.
 */
std::string convertWindow(SbgEComVibMonWindow window)
{
	std::string							 windowStr = "unknown";

	switch (window)
	{
	case SBG_ECOM_VIB_MON_WINDOW_RECTANGULAR:
		windowStr = "rectangular";
		break;
	case SBG_ECOM_VIB_MON_WINDOW_HANNING:
		windowStr = "hanning";
		break;
	case SBG_ECOM_VIB_MON_WINDOW_FLAT_TOP:
		windowStr = "flatTop";
		break;
	}

	return windowStr;
}

//----------------------------------------------------------------------//
//- Public methods (CVibMonFftLog)                                     -//
//----------------------------------------------------------------------//

void CVibMonFftLog::process(const CLoggerContext &context, const SbgEComVibMonFft &logData)
{
	if (!isOpened())
	{
		const std::string	axis		=  convertAxis(sbgEComVibMonFftGetAxis(&logData));
		const std::string	window		=  convertWindow(sbgEComVibMonFftGetWindow(&logData));
		const std::string	fileName	= "vibFftAcf_" + axis + "_" + window + ".txt";

		open(context, fileName);

		//
		// Write the headers if needed
		//
		if (context.getSettings().getWriteHeaderToFile())
		{
			writeHeaderToFile(context, logData);
		}
	}

	writeDataToFile(context, logData);
}

//----------------------------------------------------------------------//
//- Private methods (CVibMonFftLog)                                    -//
//----------------------------------------------------------------------//

bool CVibMonFftLog::isOpened() const
{
	return m_outFile.is_open();
}

void CVibMonFftLog::open(const CLoggerContext &context, const std::string &fileName)
{
	if (isOpened())
	{
		throw std::runtime_error("file already opened");
	}

	m_outFile.open(context.getSettings().getBasePath() + fileName, std::ofstream::out);
	m_outFile << std::setprecision(9) << std::fixed;
}

void CVibMonFftLog::writeHeaderToFile(const CLoggerContext &context, const SbgEComVibMonFft &logData)
{
	const std::string	axis	= convertAxis(sbgEComVibMonFftGetAxis(&logData));
	const std::string	window	= convertWindow(sbgEComVibMonFftGetWindow(&logData));

	m_outFile	<< "vibMonFft (" << axis << ", " << window << ") - ACF (Amplitude Correction Factor) - "
				<< "fSampling: " << logData.fSampling << "Hz - binCount: " << logData.binCount << "\n";

	//
	// First line, col name and then bin frequency
	//
	m_outFile	<< std::setprecision(2)			<< std::fixed;
	m_outFile	<< context.getTimeColTitle()	<< "\t";
	
	for (size_t i = 0; i < logData.binCount; i++)
	{
		m_outFile << sbgEComVibMonFftGetFrequencyFromBinIndex(&logData, i);

		if (i < logData.binCount-1)
		{
			m_outFile << "\t";
		}
		else
		{
			m_outFile << "\n";
		}
	}

	m_outFile << std::setprecision(9) << std::fixed;

	//
	// Second line, display each col unit
	//
	m_outFile	<< context.getTimeUnit()		<< "\t";

	for (size_t i = 0; i < logData.binCount; i++)
	{
		m_outFile << "(m.s^-2)";

		if (i < logData.binCount-1)
		{
			m_outFile << "\t";
		}
		else
		{
			m_outFile << "\n";
		}
	}
}

void CVibMonFftLog::writeDataToFile(const CLoggerContext &context, const SbgEComVibMonFft &logData)
{
	m_outFile	<< context.fmtTime(logData.timestampUs)								<< "\t";

	for (size_t i = 0; i < logData.binCount; i++)
	{
		m_outFile << sbgEComVibMonFftGetMagnitudeFromBinIndex(&logData, i);

		if (i < logData.binCount-1)
		{
			m_outFile << "\t";
		}
		else
		{
			m_outFile << "\n";
		}
	}
}

//----------------------------------------------------------------------//
//- Constructor (CLoggerEntryVibMonFft)                                -//
//----------------------------------------------------------------------//

CLoggerEntryVibMonFft::CLoggerEntryVibMonFft()
{
	sbgEComVibMonFftCtxConstruct(&m_ecomVibMonFftCtx);
}

//----------------------------------------------------------------------//
//- Public getters (CLoggerEntryVibMonFft)                             -//
//----------------------------------------------------------------------//

std::string CLoggerEntryVibMonFft::getName() const
{
	return "vibMonFft";
}

//----------------------------------------------------------------------//
//- Private methods (CLoggerEntryVibMonFft)                            -//
//----------------------------------------------------------------------//

bool CLoggerEntryVibMonFft::prepareProcess(CLoggerContext &context, const SbgEComLogUnion &logData)
{
	SbgErrorCode						 errorCode;

	SBG_UNUSED_PARAMETER(context);

	errorCode = sbgEComVibMonFftCtxProcess(&m_ecomVibMonFftCtx, logData.vibMonFft.pageIndex, logData.vibMonFft.nrPages, logData.vibMonFft.buffer, logData.vibMonFft.size);
	
	if (errorCode == SBG_NO_ERROR)
	{
		SbgStreamBuffer		streamBuffer;

		errorCode = sbgEComVibMonFftCtxGetStreamBuffer(&m_ecomVibMonFftCtx, &streamBuffer);

		if (errorCode != SBG_NO_ERROR)
		{
			throw std::runtime_error("unable to extract stream buffer for vibration monitoring FFT");
		}

		errorCode = sbgEComVibMonFftReadFromStream(&m_cachedVibMonFft, &streamBuffer);

		if (errorCode != SBG_NO_ERROR)
		{
			throw std::runtime_error("unable to parse vibration monitoring FFT");
		}

		return true;
	}
	else if (errorCode != SBG_NOT_READY)
	{
		throw std::runtime_error("unable to process vibration monitoring FFT chunk");
	}
	
	return false;
}

void CLoggerEntryVibMonFft::doProcess(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	SBG_UNUSED_PARAMETER(logData);

	if (context.getSettings().getWriteToFile())
	{
		SbgEComVibMonAxis		axis	= sbgEComVibMonFftGetAxis(&m_cachedVibMonFft);
		SbgEComVibMonWindow		window	= sbgEComVibMonFftGetWindow(&m_cachedVibMonFft);
			
		m_loggers[VibMonKey{axis, window}].process(context, m_cachedVibMonFft);
	}
}

void CLoggerEntryVibMonFft::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	SBG_UNUSED_PARAMETER(context);
	SBG_UNUSED_PARAMETER(logData);

	const std::string	axis	= convertAxis(sbgEComVibMonFftGetAxis(&m_cachedVibMonFft));
	const std::string	window	= convertWindow(sbgEComVibMonFftGetWindow(&m_cachedVibMonFft));

	std::cout	<< std::setw(12) << getName()														<< " "
				<< "(" << axis << ", " << window													<< "): "
				<< "fSampling: "	<< m_cachedVibMonFft.fSampling									<< "Hz "
				<< "binCount: "		<< m_cachedVibMonFft.binCount									<< " "
				<< "acf: "			<< m_cachedVibMonFft.acf										<< " "
				<< "ecf: "			<< m_cachedVibMonFft.ecf										<< "\n";
}

//----------------------------------------------------------------------//
//- Public methods (CVibMonReportLog)                                  -//
//----------------------------------------------------------------------//

void CVibMonReportLog::process(const CLoggerContext &context, const SbgEComLogVibMonReport &data)
{
	if (!isOpened())
	{
		const std::string	axis		=  convertAxis(sbgEComLogVibMonReportGetAxis(&data));
		const std::string	window		=  convertWindow(sbgEComLogVibMonReportGetWindow(&data));
		const std::string	fileName	= "vibMonReport_" + axis + "_" + window + ".txt";

		open(context, fileName);

		//
		// Write the headers if needed
		//
		if (context.getSettings().getWriteHeaderToFile())
		{
			writeHeaderToFile(context);
		}
	}

	writeDataToFile(context, data);
}

//----------------------------------------------------------------------//
//- Private methods (CVibMonReportLog)                                 -//
//----------------------------------------------------------------------//

bool CVibMonReportLog::isOpened() const
{
	return m_outFile.is_open();
}

void CVibMonReportLog::open(const CLoggerContext &context, const std::string &fileName)
{
	if (isOpened())
	{
		throw std::runtime_error("file already opened");
	}

	m_outFile.open(context.getSettings().getBasePath() + fileName, std::ofstream::out);
	m_outFile << std::setprecision(9) << std::fixed;
}

void CVibMonReportLog::writeHeaderToFile(const CLoggerContext &context)
{
	m_outFile	<< context.getTimeColTitle()	<<	"\tmean\trms"
													"\tband1Freq1\tband1Freq2\tband1Rms\tband1peakHarmonic\tband1peakMagnitude"
													"\tband2Freq1\tband2Freq2\tband2Rms\tband2peakHarmonic\tband2peakMagnitude"
													"\tband3Freq1\tband3Freq2\tband3Rms\tband3peakHarmonic\tband3peakMagnitude"
													"\tband4Freq1\tband4Freq2\tband4Rms\tband4peakHarmonic\tband4peakMagnitude\n";

	m_outFile	<< context.getTimeUnit()		<<	"\t(m.s^-2)\t(m.s^-2)"
													"\t(Hz)\t(Hz)\t(m.s^-2)\t(Hz)\t(m.s^-2)"
													"\t(Hz)\t(Hz)\t(m.s^-2)\t(Hz)\t(m.s^-2)"
													"\t(Hz)\t(Hz)\t(m.s^-2)\t(Hz)\t(m.s^-2)"
													"\t(Hz)\t(Hz)\t(m.s^-2)\t(Hz)\t(m.s^-2)\n";
}

void CVibMonReportLog::writeDataToFile(const CLoggerContext &context, const SbgEComLogVibMonReport &data)
{
	assert(SBG_ARRAY_SIZE(data.bands) == 4);

	m_outFile	<< context.fmtTime(data.timeStamp)											<< "\t"
				<< data.mean																<< "\t"
				<< data.rms																	<< "\t"
				<< data.bands[0].freq1														<< "\t"
				<< data.bands[0].freq2														<< "\t"
				<< data.bands[0].rms														<< "\t"
				<< data.bands[0].peakHarmonic												<< "\t"
				<< data.bands[0].peakMagnitude												<< "\t"
				<< data.bands[1].freq1														<< "\t"
				<< data.bands[1].freq2														<< "\t"
				<< data.bands[1].rms														<< "\t"
				<< data.bands[1].peakHarmonic												<< "\t"
				<< data.bands[1].peakMagnitude												<< "\t"
				<< data.bands[2].freq1														<< "\t"
				<< data.bands[2].freq2														<< "\t"
				<< data.bands[2].rms														<< "\t"
				<< data.bands[2].peakHarmonic												<< "\t"
				<< data.bands[2].peakMagnitude												<< "\t"
				<< data.bands[3].freq1														<< "\t"
				<< data.bands[3].freq2														<< "\t"
				<< data.bands[3].rms														<< "\t"
				<< data.bands[3].peakHarmonic												<< "\t"
				<< data.bands[3].peakMagnitude												<< "\n";
}

//----------------------------------------------------------------------//
//- Public getters (CLoggerEntryVibMonReport)                          -//
//----------------------------------------------------------------------//

std::string CLoggerEntryVibMonReport::getName() const
{
	return "vibMonReport";
}

//----------------------------------------------------------------------//
//- Private methods (CLoggerEntryVibMonReport)                         -//
//----------------------------------------------------------------------//

void CLoggerEntryVibMonReport::doProcess(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	if (context.getSettings().getWriteToFile())
	{
		const SbgEComLogVibMonReport	&vibMonReport = logData.vibMonReport;

		SbgEComVibMonAxis		axis	= sbgEComLogVibMonReportGetAxis(&vibMonReport);
		SbgEComVibMonWindow		window	= sbgEComLogVibMonReportGetWindow(&vibMonReport);
			
		m_loggers[VibMonKey{axis, window}].process(context, vibMonReport);
	}
}

void CLoggerEntryVibMonReport::writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData)
{
	const SbgEComLogVibMonReport	&data = logData.vibMonReport;

	SBG_UNUSED_PARAMETER(context);
	
	const std::string	axis	= convertAxis(sbgEComLogVibMonReportGetAxis(&data));
	const std::string	window	= convertWindow(sbgEComLogVibMonReportGetWindow(&data));

	//
	// To reduce console overload, pickup the peak magnitude and display the attached frequency
	//
	const SbgEComLogVibMonBand	&maxBand = *sbgEComLogVibMonReportGetMaxPeakBand(&data);

	std::cout	<< std::setw(12) << getName()												<< " "
				<< "(" << axis << ", " << window											<< "): "
				<< std::setw(12) << data.mean												<< "\t"
				<< std::setw(12) << data.rms												<< "\t"
				<< std::setw(12) << maxBand.freq1											<< "\t"
				<< std::setw(12) << maxBand.freq2											<< "\t"
				<< std::setw(12) << maxBand.rms												<< "\t"
				<< std::setw(12) << maxBand.peakHarmonic									<< "\t"
				<< std::setw(12) << maxBand.peakMagnitude									<< "\n";
}

}; // Namespace sbg
