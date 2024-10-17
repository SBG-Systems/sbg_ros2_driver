/*!
 * \file		sbgDebug.c
 * \author		SBG Systems
 *
 * \brief		Error logging for the SBG Systems common C library.
 *
 * \section CodeCopyright Copyright Notice
 * Copyright (C) 2019, SBG Systems SAS. All rights reserved.
 *
 * This source code is intended for use only by SBG Systems SAS and
 * those that have explicit written permission to use it from
 * SBG Systems SAS.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

// Standard headers
#include <ctype.h>

// sbgCommonLib headers
#include <sbgCommon.h>

// Local headers
#include "sbgDebug.h"

//----------------------------------------------------------------------//
//- Constant definitions                                               -//
//----------------------------------------------------------------------//

/*!
 * Number of bytes translated per line.
 */
#define SBG_DEBUG_NR_BYTES_PER_LINE							(16)

/*!
 * Size of the buffer used to generate lines, in bytes.
 */
#define SBG_DEBUG_LINE_BUFFER_SIZE							(256)

//----------------------------------------------------------------------//
//- Private functions                                                  -//
//----------------------------------------------------------------------//

/*!
 * Dump the given buffer on a single text line.
 *
 * \param[out]	pLine						Output line buffer.
 * \param[in]	lineSize					Size of the ouput line buffer.
 * \param[in]	pBuffer						Buffer.
 * \param[in]	size						Buffer size.
 */
static void sbgDebugHexDumpGenerateLine(char *pLine, size_t lineSize, const uint8_t *pBuffer, size_t size)
{
	size_t								 length;

	assert(pLine);
	assert(lineSize != 0);
	assert(pBuffer);
	assert(size <= SBG_DEBUG_NR_BYTES_PER_LINE);

	for (size_t i = 0; i < size; i++)
	{
		length = snprintf(pLine, lineSize, "%02x ", pBuffer[i]);
		assert(length < lineSize);

		pLine		= &pLine[length];
		lineSize	-= length;
	}

	for (size_t i = size; i < SBG_DEBUG_NR_BYTES_PER_LINE; i++)
	{
		length = snprintf(pLine, lineSize, "   ");
		assert(length < lineSize);

		pLine		= &pLine[length];
		lineSize	-= length;
	}

	length = snprintf(pLine, lineSize, " | ");
	assert(length < lineSize);

	pLine		= &pLine[length];
	lineSize	-= length;

	for (size_t i = 0; i < size; i++)
	{
		char								 c;

		if (isprint(pBuffer[i]))
		{
			c = pBuffer[i];
		}
		else
		{
			c = '.';
		}

		length = snprintf(pLine, lineSize, "%c", c);
		assert(length < lineSize);

		pLine		= &pLine[length];
		lineSize	-= length;
	}
}

//----------------------------------------------------------------------//
//- Public functions                                                   -//
//----------------------------------------------------------------------//

void sbgDebugHexDump(const char *pPrefix, const void *pBuffer, size_t size)
{
	char								 line[SBG_DEBUG_LINE_BUFFER_SIZE];
	size_t								 index = 0;
	bool								 multiLine = false;

	assert(pPrefix);
	assert(pBuffer || (size == 0));

	if (size > SBG_DEBUG_NR_BYTES_PER_LINE)
	{
		SBG_LOG_DEBUG("%s: multi-line dump start (%zu bytes)", pPrefix, size);
		multiLine = true;
	}

	while (size != 0)
	{
		const uint8_t						*pByteBuffer = pBuffer;
		size_t								 rangeSize;

		if (size < SBG_DEBUG_NR_BYTES_PER_LINE)
		{
			rangeSize = size;
		}
		else
		{
			rangeSize = SBG_DEBUG_NR_BYTES_PER_LINE;
		}

		sbgDebugHexDumpGenerateLine(line, sizeof(line), &pByteBuffer[index], rangeSize);
		SBG_LOG_DEBUG("%s: %s", pPrefix, line);

		size	-= rangeSize;
		index	+= rangeSize;
	}

	if (multiLine)
	{
		SBG_LOG_DEBUG("%s: multi-line dump end", pPrefix);
	}
}
