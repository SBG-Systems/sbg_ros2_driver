/*!
 * \file            loggerEntryImu.h
 * \author          SBG Systems
 * \date            March 14, 2023
 *
 * \brief           Implemented IMU with optional decimation.
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

#ifndef SBG_LOGGER_ENTRY_IMU_H
#define SBG_LOGGER_ENTRY_IMU_H

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgEComLib.h>

// Project headers
#include <loggerManager/loggerEntry.h>
#include <loggerManager/loggerContext.h>

// Local headers
#include "helpers/imuDataMean.h"

namespace sbg
{
    /*!
     * Generic handler for IMU data Handle
     */
    class CLoggerEntryImu : public IFileLogger
    {
    protected:

        /*!
         * Write the IMU data or short log to the file
         * 
         * \param[in]   context                             Logger context and settings.
         * \param[in]   imuData                             Input sbgECom IMU data to write.
         */
        template<typename T>
        void writeImuDataToFile(const CLoggerContext &context, const T &imuData);

        /*!
         * Write the IMU data or short log to the file
         * 
         * \param[in]   context                             Logger context and settings.
         * \param[in]   imuData                             Input sbgECom IMU data to write.
         */
        template<typename T>
        void writeImuDataToConsole(const CLoggerContext &context, const T &imuData);

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

        //----------------------------------------------------------------------//
        //- Private members                                                    -//
        //----------------------------------------------------------------------//

        CImuDataMean            m_fileValues;                       /*!< decimated IMU values for file output. */
        CImuDataMean            m_consoleValues;                    /*!< decimated IMU values for console output. */
    };

    /*!
     * Handle SBG_ECOM_LOG_IMU_DATA
     */
    class CLoggerEntryImuData : public CLoggerEntryImu, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_IMU_DATA>
    {   
    public:
        //----------------------------------------------------------------------//
        //- Public getters                                                     -//
        //----------------------------------------------------------------------//

        /*!
         * Returns the log name.
         * 
         * \return                                          log name.
         */
        std::string getName() const override;
        
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

    /*!
     * Handle SBG_ECOM_LOG_IMU_SHORT
     */
    class CLoggerEntryImuShort : public CLoggerEntryImu, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_IMU_SHORT>
    {
    public:
        //----------------------------------------------------------------------//
        //- Public getters                                                     -//
        //----------------------------------------------------------------------//

        /*!
         * Returns the log name.
         * 
         * \return                                          log name.
         */
        std::string getName() const override;

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

    /*!
     * Handle SBG_ECOM_LOG_FAST_IMU_DATA
     */
    class CLoggerEntryImuFast : public CLoggerEntryImu, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_1, SBG_ECOM_LOG_FAST_IMU_DATA>
    {
    public:
        //----------------------------------------------------------------------//
        //- Public getters                                                     -//
        //----------------------------------------------------------------------//

        /*!
         * Returns the log name.
         * 
         * \return                                          log name.
         */
        std::string getName() const override;

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
};

#endif // SBG_LOGGER_ENTRY_IMU_H
