/*!
 * \file            loggerSettings.h
 * \author          SBG Systems
 * \date            March 06, 2023
 *
 * \brief           Define the logger settings.
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

#ifndef SBG_LOGGER_SETTINGS_H
#define SBG_LOGGER_SETTINGS_H

// STL headers
#include <string>

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgEComLib.h>

namespace sbg
{
    /*!
     * General logger settings as set by the CLI
     */
    class CLoggerSettings
    {
    public:
        //----------------------------------------------------------------------//
        //- Public definitions                                                 -//
        //----------------------------------------------------------------------//

        /*!
         * Defines time field output format.
         */
        enum class TimeMode
        {
            TimeStamp,                                                              /*!< Use the internal timestamp value in us. */
            UtcIso8601                                                              /*!< Use an absolute UTC time using ISO 8601 format. */
        };

        /*!
         * Defines status field output format.
         */
        enum class StatusFormat
        {
            Decimal,                                                                /*!< Output status using a decimal format. */
            Hexadecimal                                                             /*!< Output status using an hexadecimal format (ie 0x0000AABB). */
        };
        
        /*!
         * Defines the interface mode to use.
         */
        enum class InterfaceMode
        {
            Unknown,                                                                /*!< Unknown / undefined logger interface type. */
            Serial,                                                                 /*!< Logger is setup to read data from a serial interface. */
            Udp,                                                                    /*!< Logger is setup to read data from an Ethernet UDP interface. */
            File                                                                    /*!< Logger is setup to read data from a file. */
        };

        /*!
         * Settings for a serial interface.
         */
        struct Serial
        {
            std::string         location                {};                         /*!< Serial port location. */
            uint32_t            baudRate                {0};                        /*!< Serial baud rate. */
        };

        /*!
         * Settings for an UDP interface
         */
        struct Udp
        {
            std::string         remoteIp                {};                         /*!< device IPv4 address to connect to ("a.b.c.d"). */
            uint32_t            remotePort              {0};                        /*!< device port number to send data to. */
            uint32_t            listenPort              {0};                        /*!< computer listen port on which we should receive data. */
        };

        //----------------------------------------------------------------------//
        //- Public getters & setters                                           -//
        //----------------------------------------------------------------------//

        /*!
         * Define the base path to store all logs to
         * 
         * \param[in]   basePath                            The base path to store logs to.
         */
        void setBasePath(const std::string &basePath);

        /*!
         * Returns the base path.
         * 
         * \return                                          The base path ending with '\'
         */
        const std::string &getBasePath() const;

        /*!
         * Set if logs should be written to the console.
         * 
         * \param[in]   writeToConsole                      true ti write logs to console.
         */
        void setWriteToConsole(bool writeToConsole);

        /*!
         * Returns if the log should be written to the console.
         * 
         * \return                                          true if logs should be written to the console.
         */
        bool getWriteToConsole() const;

        /*!
         * Set if logs should be written to files.
         * 
         * \param[in]   writeToFile                         true ti write logs to file.
         */
        void setWriteToFile(bool writeToFile);

        /*!
         * Returns if the log should be written to files.
         * 
         * \return                                          true if logs should be written to file.
         */
        bool getWriteToFile() const;

        /*!
         * Set if the headers should be written to file.
         * 
         * \param[in]   writeHeaderToFile                   true to write headers to file or false otherwise.
         */
        void setWriteHeaderToFile(bool writeHeaderToFile);

        /*!
         * Returns if headers should be written to file.
         * 
         * \return                                          true if headers should be written to file or false otherwise.
         */
        bool getWriteHeaderToFile() const;

        /*!
         * Set the decimation factor for the file output.
         * 
         * \param[in]   consoleDecimation                   Decimation factor from 1 to 10 000.
         * \throw                                           std::invalid_argument if the decimation is invalid.
         */
        void setFileDecimation(int32_t fileDecimation);

        /*!
         * Returns the decimation factor for the file output.
         * 
         * \return                                          The decimation factor.
         */
        int32_t getFileDecimation() const;

        /*!
         * Set the decimation factor for the console output.
         * 
         * \param[in]   consoleDecimation                   Decimation factor from 1 to 10 000.
         * \throw                                           std::invalid_argument if the decimation is invalid.
         */
        void setConsoleDecimation(int32_t consoleDecimation);
        
        /*!
         * Returns the decimation factor for the console output.
         * 
         * \return                                          The decimation factor.
         */
        int32_t getConsoleDecimation() const;

        /*!
         * Set if data with invalid time information should be discarded.
         * 
         * \param[in]   discardInvalidTime                  true to discard data with an invalid time information.
         */
        void setDiscardInvalidTime(bool discardInvalidTime);

        /*!
         * Returns true if data with invalid time should be discarded or false otherwise.
         * 
         * \return                                          true if data with invalid time should be discarded.
         */
        bool getDiscardInvalidTime() const;

        /*!
         * Set the time mode/format to use.
         * 
         * \param[in]   timeMode                            the time mode to use.
         */
        void setTimeMode(TimeMode timeMode);

        /*!
         * Returns the time mode to use.
         * 
         * \return                                          the time mode to use.
         */
        TimeMode getTimeMode() const;

        /*!
         * Set the status output format.
         * 
         * \param[in]   statusFormat                        the status output format.
         */
        void setStatusFormat(StatusFormat statusFormat);

        /*!
         * Returns the status output format.
         * 
         * \return                                          the status output format.
         */
        StatusFormat getStatusFormat() const;

        /*!
         * Returns true if a valid output configuration is set.
         * 
         * A valid output configuration is either the console or the file or both.
         * 
         * \return                                          true if there is a valid output configuration.
         */
        bool isOutputConfValid() const;

        /*!
         * Returns the configured interface mode.
         * 
         * \return                                          the interface mode that is setup.
         */
        InterfaceMode getInterfaceMode() const;

        /*!
         * Returns true if a valid serial or UDP or file interface is configured.
         *
         * \return                                          true if there is a valid interface configuration.
         */
        bool hasInterfaceConf() const;
        
        /*!
         * Define the configuration for a serial interface.
         * 
         * \param[in]   serialConf                          The serial configuration to set.
         * \throw                                           std::logic_error if this configuration overrides a previous one.
         */
        void setSerialConf(const Serial &serialConf);

        /*!
         * Returns the serial configuration if any.
         *
         * \return                                          The serial configuration if any.
         * \throw                                           std::logic_error if no valid serial configuration has been set.
         */
        const Serial &getSerialConf() const;

        /*!
         * Define the configuration for an UDP interface.
         * 
         * \param[in]   udpConf                             The UDP configuration to set.
         * \throw                                           std::logic_error if this configuration overrides a previous one.
         */
        void setUdpConf(const Udp &udpConf);

        /*!
         * Returns the UDP configuration if any.
         *
         * \return                                          The UDP configuration if any.
         * \throw                                           std::logic_error if no valid UDP configuration has been set.
         */
        const Udp &getUdpConf() const;

        /*!
         * Define the configuration for a file interface.
         * 
         * \param[in]   fileConf                            The file configuration to set (ie file path to open).
         * \throw                                           std::logic_error if this configuration overrides a previous one.
         */
        void setFileConf(const std::string &fileConf);

        /*!
         * Returns the file configuration if any.
         *
         * \return                                          The file configuration if any.
         * \throw                                           std::logic_error if no valid file configuration has been set.
         */
        const std::string &getFileConf() const;

        /*!
         * Returns true if the settings are valid and can be used.
         * 
         * \return                                          true if settings are valid.
         */
        bool isValid() const;

    private:
        //----------------------------------------------------------------------//
        //- Private members                                                    -//
        //----------------------------------------------------------------------//

        std::string             m_basePath              {};                             /*!< Base path to output all files to. */
        bool                    m_writeToConsole        {false};                        /*!< Set to true to output the received logs to the console. */
        bool                    m_writeToFile           {false};                        /*!< Set to true to output the receive logs to the file. */
        bool                    m_writeHeaderToFile     {false};                        /*!< Set to true to write the header to the file. */
        int32_t                 m_fileDecimation        {1};                            /*!< File decimation factor >= 1. */
        int32_t                 m_consoleDecimation     {1};                            /*!< Console decimation factor >= 1. */
        bool                    m_discardInvalidTime    {false};                        /*!< If set to true, don't output data with an invalid time. */
        TimeMode                m_timeMode              {TimeMode::TimeStamp};          /*!< Define how to output time information in files. */
        StatusFormat            m_statusFormat          {StatusFormat::Hexadecimal};    /*!< Define the status output format. */

        //
        // Interface configuration are exclusive but in C++ 14 we don't have variant
        // TODO: Might be reworked with a proper IInterfaceSettings base class and so on
        //
        InterfaceMode           m_interfaceMode         {InterfaceMode::Unknown};   /*!< Define the interface mode to use. */
        std::string             m_fileConf              {};                         /*!< File interface configuration (ie file name). */
        Serial                  m_serialConf            {};                         /*!< Serial interface configuration. */
        Udp                     m_udpConf               {};                         /*!< UDP interface configuration. */
    };
};

#endif // SBG_LOGGER_SETTINGS_H
