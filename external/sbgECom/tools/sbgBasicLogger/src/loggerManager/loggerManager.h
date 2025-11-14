/*!
 * \file            loggerManager.h
 * \author          SBG Systems
 * \date            March 03, 2023
 *
 * \brief           Basic logger implementation.
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

#ifndef SBG_LOGGER_MANAGER_H
#define SBG_LOGGER_MANAGER_H

// STL headers
#include <unordered_map>
#include <memory>

// sbgCommonLib headers
#include <sbgCommon.h>

// sbgECom headers
#include <sbgEComLib.h>

// Local headers
#include "loggerEntry.h"
#include "loggerContext.h"
#include "loggerSettings.h"

//----------------------------------------------------------------------//
//- Structure definitions                                              -//
//----------------------------------------------------------------------//

namespace sbg
{
    /*!
     * Basic logger manager implementation.
     *
     * This class parse the incoming sbgECom log and call the
     * appropriate handler to write the data to file/console.
     */
    class CLoggerManager
    {
    public:
        //----------------------------------------------------------------------//
        //- Public definitions                                                 -//
        //----------------------------------------------------------------------//

        /*!
         * Returns status about the input interface
         */
        enum class StreamStatus
        {
            HasMoreData,                    /*!< The input stream has pending data. */
            NoMoreData,                     /*!< There is not more pending data but more may arrive later. */
            EndOfStream,                    /*!< The end of stream has been reached and the processing should stop. */
        };

        //----------------------------------------------------------------------//
        //- Constructor/destructor                                             -//
        //----------------------------------------------------------------------//

        /*!
         * Default constructor.
         *
         * \param[in]   settings                                    Logger settings to use.
         * \throw                                                   std::runtime_error if
         */
        CLoggerManager(const CLoggerSettings &settings);

        /*!
         * Default destructor.
         */
        ~CLoggerManager();

        //----------------------------------------------------------------------//
        //- Public methods                                                     -//
        //----------------------------------------------------------------------//

        /*!
         * Register a new log handler.
         *
         * Example: registerLog<sbg::CLoggerEntryEventInA>();
         */
        template<typename T>
        void registerLog()
        {
            m_logList.emplace(T::getKey(), std::make_unique<T>());
        }

        /*!
         * Process one incoming log from the interface.
         *
         * \return                                                  the stream status to check if we should stop the processing.
         */
        StreamStatus processOneLog();

    private:
        //----------------------------------------------------------------------//
        //- Private methods                                                    -//
        //----------------------------------------------------------------------//

        /*!
         * Open the input interface according to configuration.
         *
         * \throw                                                   std::invalid_argument if the interface can't be opened.
         *                                                          std::logic_error if an other interface is already opened.
         */
        void openInterface();

        /*!
         * Method called by the sbgECom each time a new log is received.
         *
         * \param[in]   pHandle                                     Valid handle on the sbgECom instance that has called this callback.
         * \param[in]   msgClass                                    Received log message class.
         * \param[in]   msgId                                       Received log message id.
         * \param[in]   logData                                     Union storing the received data.
         * \param[in]   pUserArg                                    User argument that should reference the log manager.
         * \return                                                  SBG_NO_ERROR if the received log has been used successfully.
         */
        static SbgErrorCode onSbgEComLogReceived(SbgEComHandle *pHandle, SbgEComClass msgClass, SbgEComMsgId msgId, const SbgEComLogUnion *pLogData, void *pUserArg);

        /*!
         * Process an incoming log.
         *
         * \param[in]   msgClass                                    Received log message class.
         * \param[in]   msgId                                       Received log message id.
         * \param[in]   logData                                     Union storing the received data.
         */
        void processLog(SbgEComClass msgClass, SbgEComMsgId msgId, const SbgEComLogUnion &logData);

        /*!
         * Process session information data.
         *
         * \param[in]   pSessionInfoData                            Session information data.
         */
        void processSessionInformation(const SbgEComLogSessionInfo *pSessionInfoData);

    private:
        //----------------------------------------------------------------------//
        //- Private definitions                                                -//
        //----------------------------------------------------------------------//

        /*!
         * Typedef to store per sbgECom message ID the associated log handler.
         */
        typedef std::unordered_map<uint32_t, std::unique_ptr<ILoggerBase>>      LogHandlers;

        //----------------------------------------------------------------------//
        //- Private members                                                    -//
        //----------------------------------------------------------------------//
        SbgInterface                m_interface;                    /*!< interface to read log from. */
        SbgEComHandle               m_ecomHandle;                   /*!< sbgECom handler. */
        std::string                 m_ecomSessionInfoString;        /*!< Session information string. */
        SbgEComSessionInfoCtx       m_ecomSessionInfoCtx;           /*!< sbgECom session information context. */
        uint32_t                    m_sessionInfoFileId;            /*!< Identifier of the file used to log the session information. */

        LogHandlers                 m_logList;                      /*!< List of managed logs. */
        CLoggerContext              m_context;                      /*!< Logger context & settings. */
    };
}

#endif // SBG_BASIC_LOGGER_HANDLER_H
