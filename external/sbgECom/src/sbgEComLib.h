/*!
 *	\file		sbgEComLib.h
 *  \author		SBG Systems (Raphael Siryani)
 *	\date		05 February 2013
 *
 *	\brief		Main header file for the SBG Systems Enhanced Communication Library.
 *
 *	Only this main header file should be included to use the library.
 *
 *	\section CodeCopyright Copyright Notice 
 *  The MIT license
 *  
 *  Copyright (C) 2007-2020, SBG Systems SAS. All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

/*!
 *	\mainpage SBG Systems Enhanced Communication library documentation
 *	Welcome to the sbgECom library documentation.<br>
 *	This documentation describes all functions implemented in the sbgECom library.
 */

#ifndef __SBG_ECOM_LIB_H__
#define __SBG_ECOM_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <sbgCommon.h>
#include <crc/sbgCrc.h>
#include <interfaces/sbgInterface.h>
#include <interfaces/sbgInterfaceUdp.h>
#include <interfaces/sbgInterfaceSerial.h>
#include <interfaces/sbgInterfaceFile.h>
#include <splitBuffer/sbgSplitBuffer.h>
#include <streamBuffer/sbgStreamBuffer.h>
#include <network/sbgNetwork.h>
#include <swap/sbgSwap.h>
#include "sbgECanId.h"
#include "sbgEComIds.h"
#include "commands/sbgEComCmd.h"
#include "protocol/sbgEComProtocol.h"
#include "binaryLogs/sbgEComBinaryLogs.h"
#include "sbgEComVersion.h"
#include "sbgEComGetVersion.h"

//----------------------------------------------------------------------//
//- Footer (close extern C block)                                      -//
//----------------------------------------------------------------------//
#ifdef __cplusplus
}
#endif

#endif	/* __SBG_ECOM_LIB_H__ */
