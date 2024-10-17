/*!
 * \file			loggerEntryGeneral.h
 * \author			SBG Systems
 * \date			March 07, 2023
 *
 * \brief			Implement general sbgECom log such as status/utc/etc
 *
 * \copyright		Copyright (C) 2023, SBG Systems SAS. All rights reserved.
 * \beginlicense	Proprietary license
 *
 * This source code is intended for use only by SBG Systems SAS and
 * those that have explicit written permission to use it from
 * SBG Systems SAS.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * \endlicense
 */

#ifndef SBG_LOGGER_ENTRY_GENERAL_H
#define SBG_LOGGER_ENTRY_GENERAL_H

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgEComLib.h>

// Project headers
#include <loggerManager/loggerEntry.h>
#include <loggerManager/loggerContext.h>

namespace sbg
{
	/*!
	 * Handle SBG_ECOM_LOG_UTC_TIME
	 */
	class CLoggerEntryUtcTime : public ILoggerEntry, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_UTC_TIME>
	{
	public:

		//----------------------------------------------------------------------//
		//- Public getters                                                     -//
		//----------------------------------------------------------------------//

		/*!
		 * Returns the log name.
		 * 
		 * \return											log name.
		 */
		std::string getName() const override;

	private:
		
		//----------------------------------------------------------------------//
		//- Private methods                                                    -//
		//----------------------------------------------------------------------//

		/*!
		 * Process the incoming log to update the context with latest UTC time information.
		 * 
		 * \param[in/out]	context							Logger context and settings.
		 * \param[in]		logData							Received sbgECom log data to process.
		 * \return											true to continue processing the log or false to skip/discard it.
		 */
		bool preProcess(CLoggerContext &context, const SbgEComLogUnion &logData) override;

		/*!
		 * Write the header to the file and/or console.
		 *
		 * \param[in]	context								Logger context and settings.
		 */
		void writeHeaderToFile(const CLoggerContext &context) override;

		/*!
		 * Write the data log to the file
		 * 
		 * \param[in]	context								Logger context and settings.
		 * \param[in]	logData								Input sbgECom log data to write.
		 */
		void writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData) override;

		/*!
		 * Write the data log to the console
		 * 
		 * \param[in]	context								Logger context and settings.
		 * \param[in]	logData								Input sbgECom log data to write.
		 */
		void writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData) override;

	};

	/*!
	 * Handle SBG_ECOM_LOG_STATUS
	 */
	class CLoggerEntryStatus : public ILoggerEntry, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_STATUS>
	{
	public:

		//----------------------------------------------------------------------//
		//- Public getters                                                     -//
		//----------------------------------------------------------------------//

		/*!
		 * Returns the log name.
		 * 
		 * \return											log name.
		 */
		std::string getName() const override;

	private:
		
		//----------------------------------------------------------------------//
		//- Private methods                                                    -//
		//----------------------------------------------------------------------//

		/*!
		 * Write the header to the file and/or console.
		 *
		 * \param[in]	context								Logger context and settings.
		 */
		void writeHeaderToFile(const CLoggerContext &context) override;

		/*!
		 * Write the data log to the file
		 * 
		 * \param[in]	context								Logger context and settings.
		 * \param[in]	logData								Input sbgECom log data to write.
		 */
		void writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData) override;

		/*!
		 * Write the data log to the console
		 * 
		 * \param[in]	context								Logger context and settings.
		 * \param[in]	logData								Input sbgECom log data to write.
		 */
		void writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData) override;
	};

	/*!
	 * Handle SBG_ECOM_LOG_DIAG
	 */
	class CLoggerEntryDiag : public ILoggerEntry, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_DIAG>
	{
	public:

		//----------------------------------------------------------------------//
		//- Public getters                                                     -//
		//----------------------------------------------------------------------//

		/*!
		 * Returns the log name.
		 * 
		 * \return											log name.
		 */
		std::string getName() const override;

	private:
		//----------------------------------------------------------------------//
		//- Private methods                                                    -//
		//----------------------------------------------------------------------//

		/*!
		 * For diagnostic messages, never discard data when time is invalid.
		 * 
		 * \param[in]	context								Logger context and settings.
		 * \return											always returns false.
		 */
		bool getDiscardData(const CLoggerContext &context) const override;

		/*!
		 * Write the data log to the file
		 * 
		 * \param[in]	context								Logger context and settings.
		 * \param[in]	logData								Input sbgECom log data to write.
		 */
		void writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData) override;

		/*!
		 * Write the data log to the console
		 * 
		 * \param[in]	context								Logger context and settings.
		 * \param[in]	logData								Input sbgECom log data to write.
		 */
		void writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData) override;
	};
		
	/*!
	 * Handle SBG_ECOM_LOG_RTCM_RAW
	 */
	class CLoggerEntryRtcmRaw : public ILoggerEntry, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_RTCM_RAW>
	{
	public:
		//----------------------------------------------------------------------//
		//- Settings/getters                                                   -//
		//----------------------------------------------------------------------//

		/*!
		 * Returns the log name.
		 * 
		 * \return											log name.
		 */
		std::string getName() const override;

		/*!
		 * Returns the file name to use.
		 * 
		 * The default implementation just appends a .txt to the log name.
		 * 
		 * \return											file name for this log.
		 */
		std::string getFileName() const override;

		/*!
		 * Returns false if the output file is text or true for binary.
		 * 
		 * The default base implementation consider text file (false).
		 * 
		 * \return											false for text file and true for binary.
		 */
		bool isBinary() const override;

	private:
		
		//----------------------------------------------------------------------//
		//- Private methods                                                    -//
		//----------------------------------------------------------------------//

		/*!
		 * Write the data log to the file
		 * 
		 * \param[in]	context								Logger context and settings.
		 * \param[in]	logData								Input sbgECom log data to write.
		 */
		void writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData) override;
	};
};

#endif // SBG_LOGGER_ENTRY_GENERAL_H
