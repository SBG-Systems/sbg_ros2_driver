/*!
 * \file            loggerEntryVibMon.h
 * \author          SBG Systems
 * \date            March 6, 2025
 *
 * \brief           Implement vibration monitoring log handlers.
 *
 * \copyright       Copyright (C) 2007-2025, SBG Systems SAS. All rights reserved.
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

#ifndef SBG_LOGGER_ENTRY_VIB_MON_H
#define SBG_LOGGER_ENTRY_VIB_MON_H

// STL headers
#include <unordered_map>

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgEComLib.h>

// Project headers
#include <loggerManager/loggerContext.h>
#include <loggerManager/loggerEntry.h>

namespace sbg
{
    /*!
     * Key structure used for the unordered_map factory lookup.
     *
     * The key is composed of both an axis and a window type to uniquely identify
     * the corresponding vibration monitoring report specialization.
     */
    struct VibMonKey
    {
        SbgEComVibMonAxis       axis;       /*!< Vibration monitoring axis (X, Y, Z). */
        SbgEComVibMonWindow     window;     /*!< Window function type (Rectangular, Hanning, FlatTop). */

        /*!
         * Equality operator used by std::unordered_map to compare keys.
         *
         * \param[in]   other               The key instance to compare with.
         * \return                          True if both keys are identical.
         */
        bool operator==(const VibMonKey &other) const
        {
            return axis == other.axis && window == other.window;
        }
    };
}

/*!
 * Hash function specialization for VibMonKey to allow its use as key in std::unordered_map.
 */
namespace std
{
    template<>
    struct hash<sbg::VibMonKey>
    {
        /*!
         * Computes the hash value for a VibMonKey.
         *
         * \param[in]   key             The VibMonKey instance to hash.
         * \return                      The computed hash value.
         */
        size_t operator()(const sbg::VibMonKey &key) const
        {
            return static_cast<size_t>(key.axis) ^ (static_cast<size_t>(key.window) << 1);
        }
    };
}

namespace sbg
{
    //----------------------------------------------------------------------//
    //- Vibration monitoring                                               -//
    //----------------------------------------------------------------------//
    
    /*!
     * Class responsible to acctually write vibration monitoring FFT results to a file.
     */
    class CVibMonFftLog
    {
    public:
        //----------------------------------------------------------------------//
        //- Public methods                                                     -//
        //----------------------------------------------------------------------//

        /*!
         * Write the data log to the file
         *
         * \param[in]   context                             Logger context and settings.
         * \param[in]   logData                             Input sbgECom log data to write.
         */
        void process(const CLoggerContext &context, const SbgEComVibMonFft &logData);

    private:
        //----------------------------------------------------------------------//
        //- Private methods                                                    -//
        //----------------------------------------------------------------------//

        /*!
         * Indicates whether the file is currently open.
         * 
         * \return                                          True if the file is open; false otherwise.
         */
        bool isOpened() const;

        /*!
         * Opens the file for writing FFT data in text format.
         * 
         * \param[in]   context                             Logger context containing file configuration and output settings.
         * \param[in]   fileName                            Name of the file to open for writing.
         * \throw       std::runtime_error                  Thrown if the file is already open or cannot be opened.
         */
        void open(const CLoggerContext &context, const std::string &fileName);

        /*!
         * Write the header to the file and/or console.
         *
         * \param[in]   context                             Logger context and settings.
         * \param[in]   logData                             Input sbgECom log data to write.
         */
        void writeHeaderToFile(const CLoggerContext &context, const SbgEComVibMonFft &logData);

        /*!
         * Write the data log to the file
         *
         * \param[in]   context                             Logger context and settings.
         * \param[in]   logData                             Input sbgECom log data to write.
         */
        void writeDataToFile(const CLoggerContext &context, const SbgEComVibMonFft &logData);
        
    private:
        std::ofstream                   m_outFile;                          /*!< Output file to write data to. */
    };

    /*!
     * Handle SBG_ECOM_LOG_VIB_MON_FFT
     * 
     * The IMU can return a real time FFT analysis on the accelerometer signal.
     * Several axis and FFT windows might be used and each value should be stored in a dedicated file.
     */
    class CLoggerEntryVibMonFft : public ILoggerBase, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_VIB_MON_FFT>
    {
    public:
        //----------------------------------------------------------------------//
        //- Constructor                                                        -//
        //----------------------------------------------------------------------//

        /*!
         * Default constructor.
         */
        CLoggerEntryVibMonFft();

        //----------------------------------------------------------------------//
        //- Public getters                                                     -//
        //----------------------------------------------------------------------//

        /*!
         * Returns the log name.
         * 
         * \return                                          Log name as a string.
         */
        std::string getName() const override;

    private:

        //----------------------------------------------------------------------//
        //- Private methods                                                    -//
        //----------------------------------------------------------------------//

        /*!
         * Called before processing the log entry.
         *
         * The prepare process is used to re-assemble the vibration monitoring data.
         * 
         * \param[in,out]   context                         Logger context containing configuration and state.
         * \param[in]       logData                         Incoming sbgECom log data to be processed.
         * \return                                          True to continue processing; false to skip the log entry.
         */
        bool prepareProcess(CLoggerContext &context, const SbgEComLogUnion &logData) override final;

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
         * Writes the log data to the console.
         * 
         * This method is called only if the logger context is configured to output to the console.
         * 
         * \param[in]   context                             Logger context containing configuration and state.
         * \param[in]   logData                             Incoming sbgECom log data to write to console.
         */
        void writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData) override final;

    private:
        SbgEComVibMonFft                                m_cachedVibMonFft;      /*!< Latest received and parsed FFT data. */
        SbgEComVibMonFftCtx                             m_ecomVibMonFftCtx;     /*!< sbgECom vibration monitoring FFT context. */
        std::unordered_map<VibMonKey, CVibMonFftLog>    m_loggers;              /*!< Maps each axis/window combination (as a key) to its corresponding FFT log writer instance. */
    };

    /*!
     * Class responsible to acctually write vibration monitoring report results to a file.
     */
    class CVibMonReportLog
    {
    public:
        //----------------------------------------------------------------------//
        //- Public methods                                                     -//
        //----------------------------------------------------------------------//

        /*!
         * Write the data log to the file
         *
         * \param[in]   context                             Logger context and settings.
         * \param[in]   data                                Input sbgECom log data to write.
         */
        void process(const CLoggerContext &context, const SbgEComLogVibMonReport &data);

    private:
        //----------------------------------------------------------------------//
        //- Private methods                                                    -//
        //----------------------------------------------------------------------//

        /*!
         * Indicates whether the file is currently open.
         * 
         * \return                                          True if the file is open; false otherwise.
         */
        bool isOpened() const;

        /*!
         * Opens the file for writing FFT data in text format.
         * 
         * \param[in]   context                             Logger context containing file configuration and output settings.
         * \param[in]   fileName                            Name of the file to open for writing.
         * \throw       std::runtime_error                  Thrown if the file is already open or cannot be opened.
         */
        void open(const CLoggerContext &context, const std::string &fileName);

        /*!
         * Write the header to the file and/or console.
         *
         * \param[in]   context                             Logger context and settings.
         */
        void writeHeaderToFile(const CLoggerContext &context);

        /*!
         * Write the data log to the file
         *
         * \param[in]   context                             Logger context and settings.
         * \param[in]   data                                Input sbgECom log data to write.
         */
        void writeDataToFile(const CLoggerContext &context, const SbgEComLogVibMonReport &data);
        
    private:
        std::ofstream                   m_outFile;                          /*!< Output file to write data to. */
    };

    /*!
     * Handle SBG_ECOM_LOG_VIB_MON_REPORT
     */
    class CLoggerEntryVibMonReport : public ILoggerBase, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_VIB_MON_REPORT>
    {
    public:
        //----------------------------------------------------------------------//
        //- Public getters                                                     -//
        //----------------------------------------------------------------------//

        /*!
         * Returns the log name.
         * 
         * \return                                          Log name as a string.
         */
        std::string getName() const override;

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
         * Writes the log data to the console.
         * 
         * This method is called only if the logger context is configured to output to the console.
         * 
         * \param[in]   context                             Logger context containing configuration and state.
         * \param[in]   logData                             Incoming sbgECom log data to write to console.
         */
        void writeDataToConsole(const CLoggerContext &context, const SbgEComLogUnion &logData) override final;

    private:
        std::unordered_map<VibMonKey, CVibMonReportLog> m_loggers;          /*!< Maps each axis/window combination (as a key) to its corresponding  log writer instance. */
    };
};

#endif // SBG_LOGGER_ENTRY_VIB_MON_H
