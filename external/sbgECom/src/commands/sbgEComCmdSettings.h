/*!
 *	\file		sbgEComCmdSettings.h
 *  \author		SBG Systems (Maxime Renaudet)
 *	\date		11 June 2014
 *
 *	\brief		This file implements SbgECom commands related to settings.
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
#ifndef __SBG_ECOM_CMD_SETTINGS_H__
#define __SBG_ECOM_CMD_SETTINGS_H__

#include "sbgEComCmdCommon.h"

//----------------------------------------------------------------------//
//- Settings action definition										   -//
//----------------------------------------------------------------------//

/*!
 * Defintion of all the settings actions available.
 */
typedef enum _SbgEComSettingsAction
{
	SBG_ECOM_REBOOT_ONLY 				= 0,		/*!< Only reboot the device. */
	SBG_ECOM_SAVE_SETTINGS				= 1,		/*!< Save the settings to non-volatile memory and then reboot the device. */
	SBG_ECOM_RESTORE_DEFAULT_SETTINGS	= 2			/*!< Restore default settings, save them to non-volatile memory and reboot the device. */
} SbgEComSettingsAction;

//----------------------------------------------------------------------//
//- Settings commands                                                  -//
//----------------------------------------------------------------------//

/*!
 *	Execute one of the available settings action : <BR>
 *			- SBG_ECOM_REBOOT_ONLY : Only reboot the device.<BR>
 *			- SBG_ECOM_SAVE_SETTINGS : Save the settings to non-volatile memory and then reboot the device.<BR>
 *			- SBG_ECOM_RESTORE_DEFAULT_SETTINGS : Restore default settings, save them to non-volatile memory and reboot the device.<BR>
 *	\param[in]	pHandle						A valid sbgECom handle.
 *	\param[in]	action						One of the available SbgEComSettingsAction.
 *	\return									SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdSettingsAction(SbgEComHandle *pHandle, SbgEComSettingsAction action);

/*!
 *	Send a complete set of settings to the device and store them into the FLASH memory.
 *	The device will reboot automatically to use the new settings.
 *	\param[in]	pHandle						A valid sbgECom handle.
 *	\param[in]	pBuffer						Read only buffer containing the settings.
 *	\param[in]	size						Size of the buffer.
 *	\return									SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdImportSettings(SbgEComHandle *pHandle, const void *pBuffer, size_t size);

/*!
 *	Retrieve a complete set of settings from the device as a buffer.
 *	\param[in]	pHandle						A valid sbgECom handle.
 *	\param[in]	pBuffer						Allocated buffer that can hold the received settings.
 *	\param[out]	pSize						The number of bytes that have been stored into pBuffer.
 *	\param[in]	maxSize						The maximum buffer size in bytes that can be stored into pBuffer.
 *	\return									SBG_NO_ERROR if the command has been executed successfully.
 */
SbgErrorCode sbgEComCmdExportSettings(SbgEComHandle *pHandle, void *pBuffer, size_t *pSize, size_t maxSize);

#endif
