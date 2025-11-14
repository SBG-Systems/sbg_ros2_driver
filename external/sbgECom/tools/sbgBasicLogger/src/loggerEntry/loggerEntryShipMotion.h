/*!
 * \file            loggerEntryShipMotion.h
 * \author          SBG Systems
 * \date            March 14, 2023
 *
 * \brief           Implemented Ship Motion logs (heave/surge/sway).
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

#ifndef SBG_LOGGER_ENTRY_SHIP_MOTION_H
#define SBG_LOGGER_ENTRY_SHIP_MOTION_H

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
    //- Ship Motion Generic Implementation                                 -//
    //----------------------------------------------------------------------//

    /*!
     * Generic Handle SBG_ECOM_LOG_SHIP_MOTION_##
     */
    class CLoggerEntryShipMotion : public IFileLogger
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
    //- Ship Motion Specializations                                        -//
    //----------------------------------------------------------------------//

    /*!
     * Handle SBG_ECOM_LOG_SHIP_MOTION (real time)
     */
    class CLoggerEntryShipMotionRt : public CLoggerEntryShipMotion, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_SHIP_MOTION>
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
     * Handle SBG_ECOM_LOG_SHIP_MOTION_HP (delayed heave)
     */
    class CLoggerEntryShipMotionHp : public CLoggerEntryShipMotion, public ILoggerEntryKey<SBG_ECOM_CLASS_LOG_ECOM_0, SBG_ECOM_LOG_SHIP_MOTION_HP>
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

#endif // SBG_LOGGER_ENTRY_SHIP_MOTION_H
