/*!
 *	\file		sbgCrc.h
 *  \author		SBG Systems (Raphael Siryani)
 *	\date		15 January 2013
 *
 *	\brief		This file provides CRC-32 and CRC-16 methods.
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

#ifndef SBG_CRC_H
#define SBG_CRC_H

//----------------------------------------------------------------------//
//- Header (open extern C block)                                       -//
//----------------------------------------------------------------------//
#ifdef __cplusplus
extern "C" {
#endif

#include <sbgCommon.h>

//----------------------------------------------------------------------//
//- Types definitions                                                  -//
//----------------------------------------------------------------------//

/*!< Type used to compute a 32 bit Ethernet CRC. */
typedef uint32_t SbgCrc32;

/*!< Type used to compute a 16 bit CRC. */
typedef uint16_t SbgCrc16;

//----------------------------------------------------------------------//
//- 32 bits Ethernet CRC                                               -//
//----------------------------------------------------------------------//

/*!
 *	Initialize the 32 bit CRC computation system.
 *	\param[in]	pInstance				Pointer on an allocated but non initialized Crc32 object.
 */
SBG_COMMON_LIB_API void sbgCrc32Initialize(SbgCrc32 *pInstance);

/*!
 *	Compute a 32 bit CRC using an Ethernet polynome.
 *	Warning: the buffer size should be at least 4 bytes long.
 *	\param[in]	pInstance				Read only pointer on a valid Crc32 object.
 *	\param[in]	pData					Read only pointer on the data buffer to compute CRC on.
 *	\param[in]	dataSize				Data size in bytes of the buffer, has to be greater or equals to 4.
 */
SBG_COMMON_LIB_API void sbgCrc32Update(SbgCrc32 *pInstance, const void *pData, size_t dataSize);

/*!
 *	Returns the computed 32 bit CRC value.
 *	\param[in]	pInstance				Read only pointer on a valid Crc32 object.
 *	\return								The computed CRC.
 */
SBG_INLINE uint32_t sbgCrc32Get(const SbgCrc32 *pInstance)
{
	return *pInstance;
}

/*!
 *	Compute a 32 Bit CRC using an Ethernet polynome.
 *	Warning: the buffer size should be at least 4 bytes long.
 *	\param[in]	pData					Read only pointer on the data buffer to compute CRC on.
 *	\param[in]	dataSize				Data size in bytes of the buffer, has to be greater or equals to 4.
 *	\return								The computed CRC.
 */
SBG_COMMON_LIB_API uint32_t sbgCrc32Compute(const void *pData, size_t dataSize);

//----------------------------------------------------------------------//
//- CRC-16 operations                                                  -//
//----------------------------------------------------------------------//

/*!
 *	Initialize the 16 bit CRC computation system.
 *	\param[in]	pInstance				Pointer on an allocated but non initialized Crc16 object.
 */
SBG_COMMON_LIB_API void sbgCrc16Initialize(SbgCrc16 *pInstance);

/*!
 *	Compute a 16 bit CRC using an the polynome 0x8408.
 *	\param[in]	pInstance				Read only pointer on a valid Crc16 object.
 *	\param[in]	pData					Read only pointer on the data buffer to compute CRC on.
 *	\param[in]	dataSize				Data size in bytes of the buffer.
 */
SBG_COMMON_LIB_API void sbgCrc16Update(SbgCrc16 *pInstance, const void *pData, size_t dataSize);

/*!
 *	Returns the computed 32 bit CRC value.
 *	\param[in]	pInstance				Read only pointer on a valid Crc16 object.
 *	\return								The computed CRC.
 */
SBG_INLINE uint16_t sbgCrc16Get(const SbgCrc16 *pInstance)
{
	return *pInstance;
}

/*!
 *	Compute a 32 Bit CRC using an the polynome 0x8408.
 *	\param[in]	pData					Read only pointer on the data buffer to compute CRC on.
 *	\param[in]	dataSize				Data size in bytes of the buffer.
 *	\return								The computed CRC.
 */
SBG_COMMON_LIB_API uint16_t sbgCrc16Compute(const void *pData, size_t dataSize);

//----------------------------------------------------------------------//
//- Footer (close extern C block)                                      -//
//----------------------------------------------------------------------//
#ifdef __cplusplus
}
#endif

#endif /* SBG_CRC_H */
