/*!
 * \file            loggerApp.h
 * \author          SBG Systems
 * \date            March 10, 2023
 *
 * \brief           Basic Logger main application entry point.
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

#ifndef SBG_LOGGER_APP_H
#define SBG_LOGGER_APP_H

// STL headers
#include <functional>
#include <memory>

// sbgCommonLib headers
#include <sbgCommon.h>

// Project headers
#include <loggerManager/loggerContext.h>
#include <loggerManager/loggerManager.h>
#include <loggerManager/loggerSettings.h>

//----------------------------------------------------------------------//
//- Structure definitions                                              -//
//----------------------------------------------------------------------//

namespace sbg
{
    /*!
     * Application entry point.
     */
    class CLoggerApp
    {
    private:
        std::unique_ptr<CLoggerManager>      m_manager;                     /*!< Logger manager instance. */    
        std::function<bool()>                m_continueCb;                  /*!< Optional callback used to either continue or stop the process. */

        //----------------------------------------------------------------------//
        //- Private methods                                                    -//
        //----------------------------------------------------------------------//
        
        /*!
         * Returns the application name.
         * 
         * \return                              application name as a read only string.
         */
        std::string getApplicationName() const;

        /*!
         * Instantiate the logger and register all log handlers.
         * 
         * \param[in]   settings                Logger settings to use.
         */
        void createLogger(const CLoggerSettings &settings);

        /*!
         * Run the logger until interface EOF is reached or user request to exit.
         */
        void process();

        /*!
         * Parse command line arguments and returns a filled settings instance.
         *
         * \param[in]   argc                    Number of input arguments.
         * \param[in]   argv                    Input arguments as an array of strings.
         * \throw                               std::invalid_argument if input command line argument are invalid.
         *                                      std::bad_alloc if arg3 malloc failed.
         */
        CLoggerSettings processArgs(int argc, char **argv);

    public:
        //----------------------------------------------------------------------//
        //- Public methods                                                     -//
        //----------------------------------------------------------------------//

        /*!
         * Define an optional callback used to continue or stop the processing.
         *
         * The function is called periodically from the application processing loop.
         * 
         * If the callback returns true, the processing continues normally.
         * If the callback returns false, the processing stops and program flow returns from exec().
         * 
         * Please keep in mind this callback belongs to the same thread as exec().
         * 
         * \param[in]   continueCb              optional and user provided continue function to call.
         */
        void setContinueCallback(std::function<bool()> continueCb) noexcept;

        /*!
         * Execute the console application and returns once processing is done or stoped.
         * 
         * The processing can be cancelled at anytime by registering a function with setContinueCallback
         *
         * \param[in]   argc                    Number of input arguments.
         * \param[in]   argv                    Input arguments as an array of strings.
         * \return                              EXIT_SUCCESS if successful.
         *                                      EXIT_FAILURE if an error has occurred.
         */
        int exec(int argc, char **argv) noexcept;
    };
}

#endif // SBG_LOGGER_APP_H
