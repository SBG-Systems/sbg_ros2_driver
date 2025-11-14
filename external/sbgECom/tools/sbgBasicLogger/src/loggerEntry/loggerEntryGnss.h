/*!
 * \file            loggerEntryGnss.h
 * \author          SBG Systems
 * \date            March 06, 2023
 *
 * \brief           Implement GNSS log handlers
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

#ifndef SBG_LOGGER_ENTRY_GNSS_H
#define SBG_LOGGER_ENTRY_GNSS_H

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
    //- Generic GNSS implementations                                       -//
    //----------------------------------------------------------------------//

    /*!
     * Handle SBG_ECOM_LOG_GNSS#_VEL
     */
    class CLoggerEntryGnssVel : public IFileLogger
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

    /*!
     * Handle SBG_ECOM_LOG_GNSS#_POS
     */
    class CLoggerEntryGnssPos : public IFileLogger
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

    /*!
     * Handle SBG_ECOM_LOG_GNSS#_HDT
     */
    class CLoggerEntryGnssHdt : public IFileLogger
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

    /*!
     * Handle SBG_ECOM_LOG_GNSS#_RAW
     */
    class CLoggerEntryGnssRaw : public IFileLogger
    {
    public:
        //----------------------------------------------------------------------//
        //- Settings/getters                                                   -//
        //----------------------------------------------------------------------//

        /*!
         * Returns the file name to use.
         * 
         * The default implementation just appends a .txt to the log name.
         * 
         * \return                                          file name for this log.
         */
        std::string getFileName() const override;

        /*!
         * Returns false if the output file is text or true for binary.
         * 
         * The default base implementation consider text file (false).
         * 
         * \return                                          false for text file and true for binary.
         */
        bool isBinaryFile() const override;

    private:
        
        //----------------------------------------------------------------------//
        //- Private methods                                                    -//
        //----------------------------------------------------------------------//

        /*!
         * Write the data log to the file
         * 
         * \param[in]   context                             Logger context and settings.
         * \param[in]   logData                             Input sbgECom log data to write.
         */
        void writeDataToFile(const CLoggerContext &context, const SbgEComLogUnion &logData) override;
    };

    /*!
     * Handle SBG_ECOM_LOG_GNSS#_SAT
     */
    class CLoggerEntryGnssSat : public IFileLogger
    {
    private:
        
        //----------------------------------------------------------------------//
        //- Private methods                                                    -//
        //----------------------------------------------------------------------//

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
    //- GNSS1 implementations                                              -//
    //----------------------------------------------------------------------//

    /*!
     * Handle SBG_ECOM_LOG_GNSS1_VEL
     */
    class CLoggerEntryGnss1Vel : public CLoggerEntryGnssVel, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_GPS1_VEL>
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
     * Handle SBG_ECOM_LOG_GNSS1_POS
     */
    class CLoggerEntryGnss1Pos : public CLoggerEntryGnssPos, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_GPS1_POS>
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
     * Handle SBG_ECOM_LOG_GNSS1_HDT
     */
    class CLoggerEntryGnss1Hdt : public CLoggerEntryGnssHdt, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_GPS1_HDT>
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
     * Handle SBG_ECOM_LOG_GNSS1_RAW
     */
    class CLoggerEntryGnss1Raw : public CLoggerEntryGnssRaw, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_GPS1_RAW>
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
     * Handle SBG_ECOM_LOG_GNSS1_SAT
     */
    class CLoggerEntryGnss1Sat : public CLoggerEntryGnssSat, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_GPS1_SAT>
    {
    public:

        /*!
         * Returns the log name.
         * 
         * \return                                          log name.
         */
        std::string getName() const override;
    };

    //----------------------------------------------------------------------//
    //- GNSS2 implementations                                              -//
    //----------------------------------------------------------------------//

    /*!
     * Handle SBG_ECOM_LOG_GNSS2_VEL
     */
    class CLoggerEntryGnss2Vel : public CLoggerEntryGnssVel, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_GPS2_VEL>
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
     * Handle SBG_ECOM_LOG_GNSS2_POS
     */
    class CLoggerEntryGnss2Pos : public CLoggerEntryGnssPos, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_GPS2_POS>
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
     * Handle SBG_ECOM_LOG_GNSS2_HDT
     */
    class CLoggerEntryGnss2Hdt : public CLoggerEntryGnssHdt, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_GPS2_HDT>
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
     * Handle SBG_ECOM_LOG_GNSS2_RAW
     */
    class CLoggerEntryGnss2Raw : public CLoggerEntryGnssRaw, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_GPS2_RAW>
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
     * Handle SBG_ECOM_LOG_GNSS2_SAT
     */
    class CLoggerEntryGnss2Sat : public CLoggerEntryGnssSat, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_GPS2_SAT>
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

#endif // SBG_LOGGER_ENTRY_GNSS_H
