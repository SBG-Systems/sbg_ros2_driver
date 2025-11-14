/*!
 * \file            loggerEntryEkfRotAccel.h
 * \author          SBG Systems
 * \date            June 12, 2023
 *
 * \brief           Implements body/NED rotation rate and accelerations.
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

#ifndef SBG_LOGGER_ENTRY_EKF_ROT_ACCEL_H
#define SBG_LOGGER_ENTRY_EKF_ROT_ACCEL_H

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
     * Handle SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY
     */
    class CLoggerEntryEkfRotAccelBody : public IFileLogger, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_EKF_ROT_ACCEL_BODY>
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
     * Handle SBG_ECOM_LOG_EKF_ROT_ACCEL_NED
     */
    class CLoggerEntryEkfRotAccelNed : public IFileLogger, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_EKF_ROT_ACCEL_NED>
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
};

#endif // SBG_LOGGER_ENTRY_EKF_ROT_ACCEL_H
