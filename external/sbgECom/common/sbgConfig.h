/*!
 *	\file		sbgConfig.h
 *  \author		SBG Systems (Raphael Siryani)
 *	\date		17 March 2015
 *
 *	\brief		Header file used to configure the framework.
 *
 *	You can configure for example the logging system.
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
#ifndef SBG_CONFIG_H
#define SBG_CONFIG_H

//----------------------------------------------------------------------//
//- Platform specific configurations                                   -//
//----------------------------------------------------------------------//

/*!
 * Windows x86 & x64 support both aligned and unaligned access
 */
#define SBG_CONFIG_UNALIGNED_ACCESS_AUTH			(0)

/*!
 * Windows is using little endianess
 */
#define SBG_CONFIG_BIG_ENDIAN						(0)

//----------------------------------------------------------------------//
//- Debug / logging Configurations                                     -//
//----------------------------------------------------------------------//

/*!
 *	Define the error log configuration for debug and release modes.
 *	You should setup your C preprocessor to define SBG_NDEBUG in release mode.
 */
#ifdef NDEBUG
	#define SBG_CONFIG_ENABLE_ASSERT				(0)						/*!< Set to 1 to enable all assertion checks. */
	#define SBG_CONFIG_ENABLE_LOG_ERROR				(1)						/*!< Set to 1 to enable log of all errors thrown using SBG_LOG_ERROR. */
	#define SBG_CONFIG_ENABLE_LOG_WARNING			(1)						/*!< Set to 1 to enable log of all errors thrown using SBG_LOG_WARNING. */
	#define SBG_CONFIG_ENABLE_LOG_INFO				(0)						/*!< Set to 1 to enable log of all errors thrown using SBG_LOG_INFO. */
	#define SBG_CONFIG_ENABLE_LOG_DEBUG				(0)						/*!< Set to 1 to enable log of all errors thrown using SBG_LOG_DEBUG */
#else
	#define SBG_CONFIG_ENABLE_ASSERT				(1)					/*!< Set to 1 to enable all assertion checks. */
	#define SBG_CONFIG_ENABLE_LOG_ERROR				(1)					/*!< Set to 1 to enable log of all errors thrown using SBG_LOG_ERROR. */
	#define SBG_CONFIG_ENABLE_LOG_WARNING			(1)					/*!< Set to 1 to enable log of all errors thrown using SBG_LOG_WARNING. */
	#define SBG_CONFIG_ENABLE_LOG_INFO				(1)					/*!< Set to 1 to enable log of all errors thrown using SBG_LOG_INFO. */
	#define SBG_CONFIG_ENABLE_LOG_VERBOSE			(1)					/*!< Set to 1 to enable log of all errors thrown using SBG_LOG_DEBUG. */
#endif

#endif	/* SBG_CONFIG_H */
