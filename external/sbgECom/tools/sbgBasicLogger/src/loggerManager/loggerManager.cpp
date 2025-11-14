// STL headers
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <memory>

// sbgCommonLib headers
#include <sbgCommon.h>
#include <interfaces/sbgInterface.h>
#include <interfaces/sbgInterfaceSerial.h>
#include <interfaces/sbgInterfaceUdp.h>
#include <network/sbgNetwork.h>
#include <streamBuffer/sbgStreamBuffer.h>

// sbgECom headers
#include <sbgEComLib.h>

// Local headers
#include "loggerContext.h"
#include "loggerEntry.h"
#include "loggerManager.h"
#include "loggerSettings.h"

namespace sbg
{

//----------------------------------------------------------------------//
//- Constructor/destructor                                             -//
//----------------------------------------------------------------------//

CLoggerManager::CLoggerManager(const CLoggerSettings &settings):
m_context(settings)
{
	openInterface();

	if (sbgEComInit(&m_ecomHandle, &m_interface) != SBG_NO_ERROR)
	{
		throw std::runtime_error("unable to init sbgECom library");
	}

	sbgEComSetReceiveLogCallback(&m_ecomHandle, onSbgEComLogReceived, this);

	sbgEComSessionInfoCtxConstruct(&m_ecomSessionInfoCtx);
	
	m_sessionInfoFileId	= 0;
}

CLoggerManager::~CLoggerManager()
{
	sbgEComClose(&m_ecomHandle);
	sbgInterfaceDestroy(&m_interface);
}

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

CLoggerManager::StreamStatus CLoggerManager::processOneLog()
{
	SbgErrorCode	errorCode;
	StreamStatus	streamStatus;

	errorCode = sbgEComHandleOneLog(&m_ecomHandle);

	if (errorCode == SBG_NOT_READY)
	{
		if (sbgInterfaceTypeGet(&m_interface) == SBG_IF_TYPE_FILE)
		{
			streamStatus = StreamStatus::EndOfStream;
		}
		else
		{
			streamStatus = StreamStatus::NoMoreData;
		}
	}
	else
	{
		streamStatus = StreamStatus::HasMoreData;
	}

	return streamStatus;
}

//----------------------------------------------------------------------//
//- Private methods                                                    -//
//----------------------------------------------------------------------//

void CLoggerManager::openInterface()
{
	auto			interfaceMode = m_context.getSettings().getInterfaceMode();

	if (interfaceMode == CLoggerSettings::InterfaceMode::Serial)
	{
		SbgErrorCode errorCode = sbgInterfaceSerialCreate(&m_interface,	m_context.getSettings().getSerialConf().location.c_str(),
																		m_context.getSettings().getSerialConf().baudRate);

		if (errorCode != SBG_NO_ERROR)
		{
			throw std::invalid_argument("unable to open serial interface " + m_context.getSettings().getSerialConf().location);
		}
	}
	else if (interfaceMode == CLoggerSettings::InterfaceMode::Udp)
	{
		SbgErrorCode errorCode = sbgInterfaceUdpCreate(&m_interface,	sbgNetworkIpFromString(m_context.getSettings().getUdpConf().remoteIp.c_str()),
																		m_context.getSettings().getUdpConf().remotePort,
																		m_context.getSettings().getUdpConf().listenPort);

		if (errorCode == SBG_NO_ERROR)
		{
			//
			// Enable connected mode to only keep data from the specified host/port
			//
			sbgInterfaceUdpSetConnectedMode(&m_interface, true);
		}
		else
		{
			throw std::invalid_argument("unable to open UDP interface " + m_context.getSettings().getUdpConf().remoteIp);
		}
	}
	else if (interfaceMode == CLoggerSettings::InterfaceMode::File)
	{
		SbgErrorCode errorCode = sbgInterfaceFileOpen(&m_interface,		m_context.getSettings().getFileConf().c_str());

		if (errorCode != SBG_NO_ERROR)
		{
			throw std::invalid_argument("unable to open file " + m_context.getSettings().getFileConf());
		}
	}
	else
	{
		throw std::logic_error("no valid interface configuration!");
	}
}

SbgErrorCode CLoggerManager::onSbgEComLogReceived(SbgEComHandle *pHandle, SbgEComClass msgClass, SbgEComMsgId msgId, const SbgEComLogUnion *pLogData, void *pUserArg)
{
	CLoggerManager		*pLogManager;

	assert(pHandle);
	assert(pLogData);
	assert(pUserArg);

	pLogManager = static_cast<CLoggerManager*>(pUserArg);

	if ((msgClass == SBG_ECOM_CLASS_LOG_ECOM_0) && (msgId == SBG_ECOM_LOG_SESSION_INFO))
	{
		pLogManager->processSessionInformation(&pLogData->sessionInfoData);
	}
	else
	{
		pLogManager->processLog(msgClass, msgId, *pLogData);
	}

	return SBG_NO_ERROR;
}

void CLoggerManager::processLog(SbgEComClass msgClass, SbgEComMsgId msgId, const SbgEComLogUnion &logData)
{
	try
	{
		m_logList.at(sbgEComComputeKey(msgClass, msgId))->process(m_context, logData);
	}
	catch (std::out_of_range &)
	{
		SBG_LOG_WARNING(SBG_ERROR, "Unknown log %u:%u", msgClass, msgId);
	}
}

void CLoggerManager::processSessionInformation(const SbgEComLogSessionInfo *pSessionInfoData)
{
	SbgErrorCode						 errorCode;

	assert(pSessionInfoData);

	errorCode = sbgEComSessionInfoCtxProcess(&m_ecomSessionInfoCtx, pSessionInfoData->pageIndex, pSessionInfoData->nrPages, pSessionInfoData->buffer, pSessionInfoData->size);

	if (errorCode == SBG_NO_ERROR)
	{
		std::string							 string;

		string = sbgEComSessionInfoCtxGetString(&m_ecomSessionInfoCtx);

		if (string != m_ecomSessionInfoString)
		{
			m_ecomSessionInfoString = string;

			if (m_context.getSettings().getWriteToConsole())
			{
				std::cout << "session information received, size:" << m_ecomSessionInfoString.length() << "\n";
			}

			if (m_context.getSettings().getWriteToFile())
			{
				std::ofstream						outFile;

				outFile.open(m_context.getSettings().getBasePath() + "sessionInfo_" + std::to_string(m_sessionInfoFileId) + ".json", std::ofstream::out | std::ofstream::trunc);
				outFile << m_ecomSessionInfoString;
				outFile.close();

				m_sessionInfoFileId++;
			}
		}
	}
}

}; // namespace sbg
