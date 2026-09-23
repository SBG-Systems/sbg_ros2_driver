# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_diag_data.py										* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				18 July 2025													* |
# *																						* |
# * brief				Parse diagnostic logs emitted by the device.					* |
# * description			python 3.*														* |
# *																						* |
# * copyright			Copyright (C) 2025-2026, SBG Systems SAS. All rights reserved.	* |
# * beginlicense		The MIT license													* |
# *																						* |
# * Permission is hereby granted, free of charge, to any person obtaining a copy		* |
# * of this software and associated documentation files (the "Software"), to deal		* |
# * in the Software without restriction, including without limitation the rights		* |
# * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell			* |
# * copies of the Software, and to permit persons to whom the Software is				* |
# * furnished to do so, subject to the following conditions:							* |
# *																						* |
# * The above copyright notice and this permission notice shall be included in all		* |
# * copies or substantial portions of the Software.										* |
# *																						* |
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR			* |
# * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,			* |
# * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE			* |
# * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER				* |
# * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,		* |
# * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE		* |
# * SOFTWARE.																			* |
# *																						* |
# * endlicense																			* |
# *																						* |
# ---------------------------------------------------------------------------------------

# Standard library
from enum import Enum
from dataclasses import dataclass
import struct

# Local application
from . import SbgEComLog
from sbgecom.protocol.sbgecom_protocol import SbgEComProtocol

class SbgDebugLogType(Enum):
	"""
	Enum that identify the type of error / warning that has been thrown.
	"""
	ERROR									= 0									# The message to log is an error.
	WARNING									= 1									# The message to log is a warning.
	INFO									= 2									# The message to log is an information.
	DEBUG									= 3									# The message to log is a debug information.

class SbgErrorCode(Enum):
	NO_ERROR								= 0									# The operation was successfully executed.
	ERROR									= 1									# We have a generic error.
	NULL_POINTER							= 2									# A pointer is null.
	INVALID_CRC								= 3									# The received frame has an invalid CRC.
	INVALID_FRAME							= 4									# The received frame is invalid
																				#	We have received an unexpected frame (not the cmd we are waiting for or with an invalid data size.
																				#	This could be caused by a desync between questions and answers.
																				#	You should flush the serial port to fix this.
	TIME_OUT								= 5									# We have started to receive a frame but not the end.
	WRITE_ERROR								= 6									# All bytes hasn't been written.
	READ_ERROR								= 7									# All bytes hasn't been read.
	BUFFER_OVERFLOW							= 8									# A buffer is too small to contain so much data.
	INVALID_PARAMETER						= 9									# An invalid parameter has been found.
	NOT_READY								= 10								# A device isn't ready (Rx isn't ready for example).
	MALLOC_FAILED							= 11								# Failed to allocate a buffer.
	CALIB_MAG_NOT_ENOUGH_POINTS				= 12								# Not enough points were available to perform magnetometers calibration.
	CALIB_MAG_INVALID_TAKE					= 13								# The calibration procedure could not be properly executed due to insufficient precision.
	CALIB_MAG_SATURATION					= 14								# Saturation were detected when attempt to calibrate magnetos.
	CALIB_MAG_POINTS_NOT_IN_A_PLANE			= 15								# 2D calibration procedure could not be performed.
	DEVICE_NOT_FOUND						= 16								# A device couldn't be founded or opened PC only error code.
	OPERATION_CANCELLED						= 17								# An operation was canceled. PC only error code.
	NOT_CONTINUOUS_FRAME					= 18								# We have received a frame that isn't a continuous one. PC only error code.
	INCOMPATIBLE_HARDWARE					= 19								# Hence valid; the command cannot be executed because of hardware incompatibility.
	INVALID_VERSION							= 20								# Incompatible version.

MAX_STRING_SIZE = SbgEComProtocol.MAX_PAYLOAD_SIZE - 6

@dataclass
class SbgEComLogDiagData(SbgEComLog):
	"""
	Data class that stores data for the SBG_ECOM_LOG_DIAG message.
	"""

	name												= "diag"				# Log name.

	"""
	Class members.
	"""
	time_stamp:							int				= None					# Time in us since the sensor power up.
	log_type:							SbgDebugLogType	= None					# Log type.
	error_code:							SbgErrorCode	= None					# Error code.
	string:								str				= ""					# Log string.

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_DIAG message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		header_format					= '<IBB'
		header_size						= struct.calcsize(header_format)
		unpacked_data					= struct.unpack(header_format, payload[:header_size])
		self.time_stamp					= unpacked_data[0]
		self.log_type					= SbgDebugLogType(unpacked_data[1])
		self.error_code					= SbgErrorCode(unpacked_data[2])
		raw_string						= payload[header_size:]
		self.string						= raw_string.split(b'\x00', 1)[0].decode('utf-8', errors='replace')

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_DIAG message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		header_format = '<IBB'
		packed_data = struct.pack(
			header_format,
			self.time_stamp,
			self.log_type.value,
			self.error_code.value
		)
		raw_string = self.string.encode('utf-8') + b'\x00'

		buffer.extend(packed_data + raw_string)

	def get_values(self, context) -> str:
		"""
		Return formatted values shared by file and console outputs.

		Args:
			context (LoggerContext):		Logger context and settings.

		Returns:
			str:							Formatted values.
		"""
		if self.log_type == SbgDebugLogType.ERROR:
			data = (
				f" *ERR * "
				f"{self.string}"
				f" [{self.error_code.name}]"
			)
		elif self.log_type == SbgDebugLogType.WARNING:
			data = (
				f" *WARN * "
				f"{self.string}"
				f" [{self.error_code.name}]"
			)
		elif self.log_type == SbgDebugLogType.INFO:
			data = (
				f" *INFO * "
				f"{self.string}"
			)
		elif self.log_type == SbgDebugLogType.DEBUG:
			data = (
				f" *DBG * "
				f"{self.string}"
			)
		else:
			data = (
				f" *UKN * "
				f"{self.string}"
			)

		return data

	def write_to_file(self, context, file):
		"""
		Write the data log to the file.

		Args:
			context (LoggerContext):		Logger context and settings.
			file (file):					Output file to write data into.
		"""
		if file:
			file.write(
				f"{context.fmt_time(self.time_stamp)}"
				f"{self.get_values(context)}\n"
			)
			file.flush()

	def write_to_console(self, context):
		"""
		Write the data log to the console.

		Args:
			context (LoggerContext):		Logger context and settings.
		"""
		print(
			f"{self.get_values(context)}"
		)
