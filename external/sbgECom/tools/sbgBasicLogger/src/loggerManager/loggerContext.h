/*!
 * \file            loggerContext.h
 * \author          SBG Systems
 * \date            March 06, 2023
 *
 * \brief           Define the logger context such as current UTC time.
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

#ifndef SBG_LOGGER_CONTEXT_H
#define SBG_LOGGER_CONTEXT_H

// STL headers
#include <iomanip>
#include <sstream>
#include <string>

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgEComLib.h>

// Local headers
#include "loggerSettings.h"

namespace sbg
{
    /*!
     * Logger shared context between all log handlers.
     * Used to share logger settings and convert timestamp to absolute time.
     */
    class CLoggerContext
    {
    public:
        //----------------------------------------------------------------------//
        //- Constructor / destructor                                           -//
        //----------------------------------------------------------------------//

        /*!
         * Default constructor.
         * 
         * \param[in]   settings                        Logger settings to set.
         */
        CLoggerContext(const CLoggerSettings &settings);

        //----------------------------------------------------------------------//
        //- Getters/settings                                                   -//
        //----------------------------------------------------------------------//

        /*!
         * Returns the logger settings instance (read only).
         * 
         * \return                                      logger settings reference.
         */
        const CLoggerSettings &getSettings() const;

        /*!
         * Update the UTC time with a newly received information.
         * 
         * \param[in]   utcTime                         New UTC time information.
         */
        void setUtcTime(const SbgEComLogUtc &utcTime);

        /*!
         * Returns true if at least one valid UTC time information has been set.
         *
         * \return                                      true if a valid UTC time information is available.
         */
        bool isUtcTimeValid() const;

        /*!
         * Returns for the selected time mode a header string.
         * 
         * \return                                      time header.
         */
        std::string getTimeColTitle() const;

        /*!
         * Returns for the selected time mode a header string.
         * 
         * \return                                      time header.
         */
        std::string getTimeUnit() const;

        /*!
         * Convert the INS timestamp in us to a ISO 8601 time.
         * 
         * \param[in]   timeStampUs                     Timestamp in us to convert.
         * \return                                      Corresponding time using ISO 8601 format.
         */
        std::string fmtTime(uint32_t timeStampUs) const;

        /*!
         * Convert a status unsigned integer to a string according to selected output format/
         *
         * Status can be either output using decimal or hexadecimal format.
         * 
         * \param[in]   value                           The status unsigned value to convert.
         * \param[in]   width                           Optional number of digits to use for hexadecimal format.
         * \return                                      Status formatted as a string.
         */
        template <typename T>
        std::string fmtStatus(T value, size_t width = sizeof(T)*2) const
        {   
            std::stringstream       outputStr;

            if (getSettings().getStatusFormat() == CLoggerSettings::StatusFormat::Hexadecimal)
            {
                outputStr.fill('0');
                outputStr << "0x"<< std::noshowbase << std::hex << std::setw(width) << value;
            }
            else
            {
                outputStr << static_cast<uint32_t>(value);
            }

            return outputStr.str();
        }

    private:
        CLoggerSettings                 m_settings;                         /*!< Logger settings. */
        SbgEComLogUtc                   m_lastUtcTime;                      /*!< Last received UTC time if any. */
        bool                            m_utcTimeIsValid    {false};        /*!< Set to true as soon as one valid UTC time has been received. It never goes back to false. */
    };
};

#endif // SBG_LOGGER_SETTINGS_H
