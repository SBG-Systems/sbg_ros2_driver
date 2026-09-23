# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_event.py											* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				17 July 2025													* |
# *																						* |
# * brief				Parse event markers logs used to timestamp external signals.	* |
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
from dataclasses import dataclass, field
import struct

# Local application
from . import SbgEComLog
from sbgecom.sbgecom_ids import SbgEComLogId

"""
Log market events status mask definitions.
"""
OVERFLOW								= (0x00000001 << 0)							# Set to 1 if we have received events at a higher rate than 1 kHz.
OFFSET_0_VALID							= (0x00000001 << 0)							# Set to 1 if at least two events have been received.
OFFSET_1_VALID							= (0x00000001 << 0)							# Set to 1 if at least three events have been received.
OFFSET_2_VALID							= (0x00000001 << 0)							# Set to 1 if at least four events have been received.
OFFSET_3_VALID							= (0x00000001 << 0)							# Set to 1 if at least five events have been received.

@dataclass
class SbgEComLogEvent(SbgEComLog):
	"""
	Data class that stores data for the SBG_ECOM_LOG_EVENT_# message.
	"""

	"""
	Class members.
	"""
	time_stamp:							int = None									# Time in us since the sensor power up.
	status:								int = None									# Magnetometer status bitmask.
	time_offset0:						int = None									# Time offset for the second received event.
	time_offset1:						int = None									# Time offset for the third received event.
	time_offset2:						int = None									# Time offset for the fourth received event.
	time_offset3:						int = None									# Time offset for the fifth received event.

	# To identifie which event.
	log_id: int = field(default=None, init=False, repr=False)

	def set_log_id(self, log_id: int):
		"""
		Method to set the log_id after initialization.

		Args:
			log_id (int):					The EVENT type.
		"""
		self.log_id = log_id

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_EVENT_# message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<IHHHHH', payload)
		self.time_stamp					= unpacked_data[0]
		self.status						= unpacked_data[1]
		self.time_offset0				= unpacked_data[2]
		self.time_offset1				= unpacked_data[3]
		self.time_offset2				= unpacked_data[4]
		self.time_offset3				= unpacked_data[5]

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_EVENT_# message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IHHHHH',
			self.time_stamp,
			self.status,
			self.time_offset0,
			self.time_offset1,
			self.time_offset2,
			self.time_offset3
		)

		buffer.extend(packed_data)

	def get_name(self) -> str:
		"""
		Returns the log name.

		Returns:
			str:							Log name.
		"""
		match self.log_id:
			case SbgEComLogId.EVENT_A.value:
				return "eventInA"

			case SbgEComLogId.EVENT_B.value:
				return "eventInB"

			case SbgEComLogId.EVENT_C.value:
				return "eventInC"

			case SbgEComLogId.EVENT_D.value:
				return "eventInD"

			case SbgEComLogId.EVENT_E.value:
				return "eventInE"

			case SbgEComLogId.EVENT_OUT_A.value:
				return "eventOutA"

			case SbgEComLogId.EVENT_OUT_B.value:
				return "eventOutB"

	def get_values(self, context) -> str:
		"""
		Return formatted values shared by file and console outputs.

		Args:
			context (LoggerContext):		Logger context and settings.

		Returns:
			str:							Formatted values.
		"""
		data = (
			f"{context.fmt_status(self.status):>12}"
			f"{self.time_offset0:>12}"
			f"{self.time_offset1:>12}"
			f"{self.time_offset2:>12}"
			f"{self.time_offset3:>12}"
		)

		return data

	def write_header_to_file(self, context, file):
		"""
		Write the data header to the file.

		Args:
			context (LoggerContext):		Logger context and settings.
			file (file):					Output file to write data into.
		"""
		if file:
			file.write(
				f"{context.get_time_col_title():>12}{'status':>8}{'timeOffset0':>12}{'timeOffset1':>12}{'timeOffset2':>12}{'timeOffset3':>12}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(us)':>12}{'(us)':>12}{'(us)':>12}{'(us)':>12}\n"
			)
			file.flush()

	def write_to_file(self, context, file):
		"""
		Write the data log to the file.

		Args:
			context (LoggerContext):		Logger context and settings.
			file (file):					Output file to write data into.
		"""
		if file:
			file.write(
				f"{context.fmt_time(self.time_stamp):>12}"
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
			f"{self.get_name():>13}:"
			f"{self.get_values(context)}"
		)
