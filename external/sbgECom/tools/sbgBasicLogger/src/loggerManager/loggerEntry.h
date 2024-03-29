/*!
 * \file			loggerEntry.h
 * \author			SBG Systems
 * \date			July 30, 2021
 *
 * \brief			All log handlers should implement ILoggerEntryKey.
 *
 * \copyright		Copyright (C) 2022, SBG Systems SAS. All rights reserved.
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

#ifndef SBG_LOGGER_ENTRY_H
#define SBG_LOGGER_ENTRY_H

// STL headers
#include <fstream>
#include <sstream>
#include <string>

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgEComLib.h>

// Local headers
#include "loggerContext.h"

namespace sbg
{
	/*!
	 * Small helper macro used to compute a unique identifier based on a sbgECom message class and id.
	 * 
	 * \param[in]	msgClass								sbgECom message class.
	 * \param[in]	msgId									sbgECom message id.
	 * \return												unique identifier for the sbgECom class/id pair.
	 */
	constexpr uint32_t sbgEComComputeKey(SbgEComClass msgClass, SbgEComMsgId msgId)
	{
		return ((uint32_t)msgClass << 8) | (uint32_t)msgId;
	}

	/*!
	 * Base interface to implement a log handler.
	 */
	class ILoggerEntry
	{
	public:
		//----------------------------------------------------------------------//
		//- Constructor/destructor                                             -//
		//----------------------------------------------------------------------//

		/*!
		 * Default destructor.
		 */
		virtual ~ILoggerEntry();

		//----------------------------------------------------------------------//
		//- Public getters                                                     -//
		//----------------------------------------------------------------------//

		/*!
		 * Returns the log name.
		 * 
		 * \return											log name.
		 */
		virtual std::string getName() const = 0;

		/*!
		 * Returns the file name to use.
		 * 
		 * The default implementation just appends a .txt to the log name.
		 * 
		 * \return											file name for this log.
		 */
		virtual std::string getFileName() const;

		/*!
		 * Returns false if the output file is text or true for binary.
		 * 
		 * The default base implementation consider text file (false).
		 * 
		 * \return											false for text file and true for binary.
		 */
		virtual bool isBinary() const;

		//----------------------------------------------------------------------//
		//- Public methods                                                     -//
		//----------------------------------------------------------------------//

		/*!
		 * Process a new incoming log.
		 * 
		 * \param[in/out]	context							Logger context and settings.
		 * \param[in]		logData							Received sbgECom log data to process.
		 */
		void process(CLoggerContext &context, const SbgEComLogUnion &logData);

	private:
		//----------------------------------------------------------------------//
		//- Private methods                                                    -//
		//----------------------------------------------------------------------//

		/*!
		 * Returns true if the data should be discarded because the time is invalid for example.
		 * 
		 * \param[in]	context								Logger context and settings.
		 * \return											true if data should be discarded because time is invalid or false otherwise.
		 */
		virtual bool getDiscardData(const CLoggerContext &context) const;

		/*!
		 * Open file fir written if needed using either binary of text format.
		 *
		 * \param[in]	context								Logger context and settings.
		 */
		void createFile(const CLoggerContext &context);

		/*!
		 * Called right before processing the incoming log.
		 *
		 * This method doesn't nothing and should be overriden if specific pre-processing is required.
		 * 
		 * \param[in/out]	context							Logger context and settings.
		 * \param[in]		logData							Received sbgECom log data to process.
		 * \return											true to continue processing the log or false to skip/discard it.
		 */
		virtual bool preProcess(CLoggerContext &context, const SbgEComLogUnion &logData);

		/*!
		 * Write the header to the file.
		 * 
		 * \param[in]	context								Logger context and settings.
		 */
		virtual void writeHeaderToFile(const CLoggerContext &context);

		/*!
		 * Write the data log to the file
		 * 
		 * \param[in]	context								Logger context and settings.
		 * \param[in]	logData								Input sbgECom log data to write.
		 */
		virtual void writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData) = 0;

		/*!
		 * Write the data log to the console
		 * 
		 * \param[in]	context								Logger context and settings.
		 * \param[in]	logData								Input sbgECom log data to write.
		 */
		virtual void writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData);

	protected:
		//----------------------------------------------------------------------//
		//- Protected members                                                  -//
		//----------------------------------------------------------------------//

		std::ofstream					m_outFile;							/*!< Output file to write data to. */

	private:
		//----------------------------------------------------------------------//
		//- Private members                                                    -//
		//----------------------------------------------------------------------//

		bool							m_headerWritten		{false};		/*!< true if the header has already been written. */
	};

	/*!
	 * Interface that adds a unique key identifier based on sbgECom class/id.
	 * 
	 * All sbgECom log handlers should implement this interface.
	 * Note: C++ (11) doesn't support string litterals for template non-type arguments.
	 */
	template<SbgEComClass msgClass, SbgEComMsgId msgId>
	class ILoggerEntryKey
	{
	public:
		/*!
		 * Returns the sbgECom message class managed by this handler.
		 * 
		 * \return										The managed sbgECom message class.
		 */
		static constexpr SbgEComClass getClass()
		{
			return msgClass;
		}

		/*!
		 * Returns the sbgECom message id managed by this handler.
		 * 
		 * \return										The managed sbgECom message id.
		 */
		static constexpr SbgEComMsgId getId()
		{
			return msgId;
		}

		/*!
		 * Returns a unique key based on the sbgECom message class & id.
		 * 
		 * \return										Unique identifier that manages this message.
		 */
		static constexpr uint32_t getKey()
		{
			return sbgEComComputeKey(msgClass, msgId);
		}
	};
};

#endif // SBG_LOGGER_ENTRY_H
