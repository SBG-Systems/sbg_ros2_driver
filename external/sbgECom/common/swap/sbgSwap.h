/*!
 *	\file		sbgSwap.h
 *  \author		SBG Systems (Raphael Siryani)
 *	\date		14 January 2013
 *
 *	\brief		Set of functions used to swap numbers.
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

#ifndef SBG_SWAP_H
#define SBG_SWAP_H

//----------------------------------------------------------------------//
//- Header (open extern C block)                                       -//
//----------------------------------------------------------------------//
#ifdef __cplusplus
extern "C" {
#endif
	
#include <sbgCommon.h>

//----------------------------------------------------------------------//
//- Internal swap functions                                            -//
//----------------------------------------------------------------------//

/*!
 *	Swap a uint16_t number.
 *	\param[in]	x					The uint16_t to swap.
 *	\return							The swapped value.
 */
SBG_INLINE uint16_t sbgSwap16(uint16_t x)
{
	return ((x<<8)|(x>>8));
}

/*!
 *	Swap a uint32_t number.
 *	\param[in]	x					The uint32_t to swap.
 *	\return							The swapped value.
 */
SBG_INLINE uint32_t sbgSwap32(uint32_t x)
{
	return ((x << 24) | ((x << 8) & (0xFF0000)) | ((x >> 8) & (0xFF00)) | (x >> 24));
}

/*!
 *	Swap a uint64_t number.
 *	\param[in]	x					The uint64_t to swap.
 *	\return							The swapped value.
 */
SBG_INLINE uint64_t sbgSwap64(uint64_t x)
{
	uint32_t hi, lo;

	//
	// Separate into high and low 32-bit values
	//
	lo = (uint32_t)(x&0xFFFFFFFF);
	x >>= 32;
	hi = (uint32_t)(x&0xFFFFFFFF);

	//
	// Swap each part and rebuild our 64 bit vale
	//
	x = sbgSwap32(lo);
	x <<= 32;
	x |= sbgSwap32(hi);

	return x;
}

/*!
 * Swap a float number.
 * \param[in]	val					The float to swap.
 * \return							The swapped value.
 */
SBG_INLINE float sbgSwapFloat(float val)
{
	FloatNint	tmpFloat;

	//
	// We use a union to do the type punning
	//
	tmpFloat.valF = val;
	tmpFloat.valU = sbgSwap32(tmpFloat.valU);

	//
	// Return the swapped float
	//
	return tmpFloat.valF;
}

/*!
 * Swap a double number.
 * \param[in]	val					The double to swap.
 * \return							The swapped value.
 */
SBG_INLINE double sbgSwapDouble(double val)
{
	DoubleNint	tmpDouble;

	//
	// We use a union to do the type punning
	//
	tmpDouble.valF = val;
	tmpDouble.valU = sbgSwap64(tmpDouble.valU);

	//
	// Return the swapped double
	//
	return tmpDouble.valF;
}

//----------------------------------------------------------------------//
//- Footer (close extern C block)                                      -//
//----------------------------------------------------------------------//
#ifdef __cplusplus
}
#endif

#endif /* SBG_SWAP_H */
