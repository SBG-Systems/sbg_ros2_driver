/*!
 * \file            loggerEntryEvent.h
 * \author          SBG Systems
 * \date            March 14, 2023
 *
 * \brief           Implement Sync In/Out event handlers.
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

#ifndef SBG_LOGGER_ENTRY_EVENT_H
#define SBG_LOGGER_ENTRY_EVENT_H

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgEComLib.h>

// Project headers
#include <loggerManager/loggerEntry.h>
#include <loggerManager/loggerContext.h>

namespace sbg
{
    //----------------------------------------------------------------------//
    //- Generic Event implementations                                      -//
    //----------------------------------------------------------------------//

    /*!
     * Handle SBG_ECOM_LOG_EVENT_# & SBG_ECOM_LOG_EVENT_OUT_#
     */
    class CLoggerEntryEvent : public IFileLogger
    {
    private:
        
        //----------------------------------------------------------------------//
        //- Private methods                                                    -//
        //----------------------------------------------------------------------//

        /*!
         * Write the header to the file and/or console.
         * 
         * \param[in]   context                             Logger context and settings.
         */
        void writeHeaderToFile(const CLoggerContext &context) override;

        /*!
         * Write the data log to the file
         * 
         * \param[in]   context                             Logger context and settings.
         * \param[in]   logData                             Input sbgECom log data to write.
         */
        void writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData) override;

        /*!
         * Write the data log to the console
         * 
         * \param[in]   context                             Logger context and settings.
         * \param[in]   logData                             Input sbgECom log data to write.
         */
        void writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData) override;
    };

    //----------------------------------------------------------------------//
    //- Event specialization/implementations                               -//
    //----------------------------------------------------------------------//

    /*!
     * Handle SBG_ECOM_LOG_EVENT_A
     */
    class CLoggerEntryEventInA : public CLoggerEntryEvent, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_EVENT_A>
    {
    public:

        /*!
         * Returns the log name.
         * 
         * \return                                          log name.
         */
        std::string getName() const override;
    };

    /*!
     * Handle SBG_ECOM_LOG_EVENT_B
     */
    class CLoggerEntryEventInB : public CLoggerEntryEvent, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_EVENT_B>
    {
    public:

        /*!
         * Returns the log name.
         * 
         * \return                                          log name.
         */
        std::string getName() const override;
    };

    /*!
     * Handle SBG_ECOM_LOG_EVENT_C
     */
    class CLoggerEntryEventInC : public CLoggerEntryEvent, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_EVENT_C>
    {
    public:

        /*!
         * Returns the log name.
         * 
         * \return                                          log name.
         */
        std::string getName() const override;
    };

    /*!
     * Handle SBG_ECOM_LOG_EVENT_D
     */
    class CLoggerEntryEventInD : public CLoggerEntryEvent, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_EVENT_D>
    {
    public:

        /*!
         * Returns the log name.
         * 
         * \return                                          log name.
         */
        std::string getName() const override;
    };

    /*!
     * Handle SBG_ECOM_LOG_EVENT_E
     */
    class CLoggerEntryEventInE : public CLoggerEntryEvent, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_EVENT_E>
    {
    public:

        /*!
         * Returns the log name.
         * 
         * \return                                          log name.
         */
        std::string getName() const override;
    };

    /*!
     * Handle SBG_ECOM_LOG_EVENT_OUT_A
     */
    class CLoggerEntryEventOutA : public CLoggerEntryEvent, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_EVENT_OUT_A>
    {
    public:

        /*!
         * Returns the log name.
         * 
         * \return                                          log name.
         */
        std::string getName() const override;
    };

    /*!
     * Handle SBG_ECOM_LOG_EVENT_OUT_B
     */
    class CLoggerEntryEventOutB : public CLoggerEntryEvent, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_EVENT_OUT_B>
    {
    public:

        /*!
         * Returns the log name.
         * 
         * \return                                          log name.
         */
        std::string getName() const override;
    };

};

#endif // SBG_LOGGER_ENTRY_EVENT_H
