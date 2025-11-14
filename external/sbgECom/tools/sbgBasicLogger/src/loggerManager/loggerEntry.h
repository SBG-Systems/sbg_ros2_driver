/*!
 * \file            loggerEntry.h
 * \author          SBG Systems
 * \date            July 30, 2021
 *
 * \brief           All log handlers should implement ILoggerEntryKey.
 *
 * \copyright       Copyright (C) 2007-2024, SBG Systems SAS. All rights reserved.
 * \beginlicense    Proprietary license
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
     * Base interface for log handlers.
     * 
     * Provides a common foundation for all logging implementations, including
     * shared processing logic and hooks for customization.
     */
    class ILoggerBase
    {
    public:
        //----------------------------------------------------------------------//
        //- Constructor/destructor                                             -//
        //----------------------------------------------------------------------//

        /*!
         * Default destructor.
         */
        virtual ~ILoggerBase();

        //----------------------------------------------------------------------//
        //- Public getters                                                     -//
        //----------------------------------------------------------------------//

        /*!
         * Returns the log name.
         * 
         * \return                                          Log name as a string.
         */
        virtual std::string getName() const = 0;

        //----------------------------------------------------------------------//
        //- Public methods                                                     -//
        //----------------------------------------------------------------------//

        /*!
         * Processes a newly received log entry.
         * 
         * This is the main entry point for log processing. It performs validation,
         * console output (if enabled), and delegates further processing to derived classes.
         * 
         * The logger context and state may be modified during processing.
         * 
         * \param[in,out]   context                         Logger context containing configuration and state.
         * \param[in]       logData                         Incoming sbgECom log data to be processed.
         */
        void process(CLoggerContext &context, const SbgEComLogUnion &logData);

    private:
        //----------------------------------------------------------------------//
        //- Overridable Hooks (Private Template Method Pattern)                -//
        //----------------------------------------------------------------------//

        /*!
         * Determines whether the log data should be discarded, for example due to an invalid timestamp.
         * 
         * \param[in]   context                             Logger context containing configuration and state.
         * \return                                          True if the data should be discarded; false otherwise.
         */
        virtual bool shouldDiscardData(const CLoggerContext &context) const;

        /*!
         * Called before processing the log entry.
         *
         * Can be overridden to perform validation or context updates prior to further processing.
         * 
         * \param[in,out]   context                         Logger context containing configuration and state.
         * \param[in]       logData                         Incoming sbgECom log data to be processed.
         * \return                                          True to continue processing; false to skip the log entry.
         */
        virtual bool prepareProcess(CLoggerContext &context, const SbgEComLogUnion &logData);

        /*!
         * Called after console output to perform additional processing in derived classes (e.g., writing to a file).
         * 
         * \param[in]       context                         Logger context containing configuration and state.
         * \param[in]       logData                         Incoming sbgECom log data to be processed.
         */
        virtual void doProcess(const CLoggerContext &context, const SbgEComLogUnion &logData);

        /*!
         * Writes the log data to the console.
         * 
         * This method is called only if the logger context is configured to output to the console.
         * 
         * \param[in]   context                             Logger context containing configuration and state.
         * \param[in]   logData                             Incoming sbgECom log data to write to console.
         */
        virtual void writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData);
    };

    /*!
     * Interface for a log handler that writes output to a single file.
     * 
     * Inherits from ILoggerBase and provides file-specific logging behavior.
     */
    class IFileLogger : public ILoggerBase
    {
    public:
        //----------------------------------------------------------------------//
        //- Constructor/destructor                                             -//
        //----------------------------------------------------------------------//

        /*!
         * Default destructor.
         */
        virtual ~IFileLogger();

        //----------------------------------------------------------------------//
        //- Public getters                                                     -//
        //----------------------------------------------------------------------//

        /*!
         * Returns the file name to use for this log.
         * 
         * The default implementation just appends a .txt to the log name.
         * 
         * \return                                          File name for the log output.
         */
        virtual std::string getFileName() const;

        /*!
         * Indicates whether the output file should be written in binary format.
         * 
         * By default, returns false to indicate a text file.
         * 
         * \return                                          True for binary format; false for text format.
         */
        virtual bool isBinaryFile() const;

    private:
        //----------------------------------------------------------------------//
        //- Private methods                                                    -//
        //----------------------------------------------------------------------//

        /*!
         * Writes the log data to the output file.
         * 
         * This method creates the file if necessary, writes the header (once),
         * and appends the log data based on the current configuration.
         * 
         * \param[in]   context                             Logger context containing configuration and state.
         * \param[in]   logData                             Incoming sbgECom log data to be processed.
         */
        void doProcess(const CLoggerContext &context, const SbgEComLogUnion &logData) override final;

        /*!
         * Opens the output file if it is not already open.
         *
         * The appropriate file mode (text or binary) is based on the logger configuration.
         * 
         * \param[in]   context                             Logger context containing configuration and state.
         */
        void createFile(const CLoggerContext &context);

        /*!
         * Writes the header to the output file.
         * 
         * This is called only once, before the first data entry is written.
         * 
         * \param[in]   context                             Logger context containing configuration and state.
         */
        virtual void writeHeaderToFile(const CLoggerContext &context);

        /*!
         * Writes a single log entry to the file.
         * 
         * Must be implemented by derived classes to define the log format.
         * 
         * \param[in]   context                             Logger context containing configuration and state.
         * \param[in]   logData                             Incoming sbgECom log data to write to console.
         */
        virtual void writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData) = 0;

    protected:
        //----------------------------------------------------------------------//
        //- Protected members                                                  -//
        //----------------------------------------------------------------------//

        std::ofstream                   m_outFile;                          /*!< Output file stream. */

    private:
        //----------------------------------------------------------------------//
        //- Private members                                                    -//
        //----------------------------------------------------------------------//

        bool                            m_headerWritten     {false};        /*!< true if the header has already been written. */
    };

    /*!
     * Small helper macro used to compute a unique identifier based on a sbgECom message class and id.
     * 
     * \param[in]   msgClass                                sbgECom message class.
     * \param[in]   msgId                                   sbgECom message id.
     * \return                                              unique identifier for the sbgECom class/id pair.
     */
    constexpr uint32_t sbgEComComputeKey(SbgEComClass msgClass, SbgEComMsgId msgId)
    {
        return ((uint32_t)msgClass << 8) | (uint32_t)msgId;
    }

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
         * \return                                      The managed sbgECom message class.
         */
        static constexpr SbgEComClass getClass()
        {
            return msgClass;
        }

        /*!
         * Returns the sbgECom message id managed by this handler.
         * 
         * \return                                      The managed sbgECom message id.
         */
        static constexpr SbgEComMsgId getId()
        {
            return msgId;
        }

        /*!
         * Returns a unique key based on the sbgECom message class & id.
         * 
         * \return                                      Unique identifier that manages this message.
         */
        static constexpr uint32_t getKey()
        {
            return sbgEComComputeKey(msgClass, msgId);
        }
    };
};

#endif // SBG_LOGGER_ENTRY_H
