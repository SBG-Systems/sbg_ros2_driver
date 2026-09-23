# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_depth.py											* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				17 July 2025													* |
# *																						* |
# * brief				Parse received sub-sea depth measurement logs.					* |
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
import struct
from dataclasses import dataclass

# Local application
from . import SbgEComLog

@dataclass
class SbgEComLogDepth(SbgEComLog):
	"""
	Data class that stores data for the SBG_ECOM_LOG_DEPTH message.
	"""

	"""
	Depth sensor status mask definitions.
	"""
	TIME_IS_DELAY						= (0x0001 << 0)							# Set to 1 if the timeStamp field represents a delay instead of an absolute timestamp.
	PRESSURE_ABS_VALID					= (0x0001 << 1)							# Set to 1 if the pressure field is filled and valid.
	ALTITUDE_VALID						= (0x0001 << 2)							# Set to 1 if the depth altitude field is filled and valid.

	name										= "depth"						# Log name.

	"""
	Class members.
	"""
	time_stamp:							int		= None							# Time in us since the sensor power up.
	status:								int		= None							# Magnetometer status bitmask.
	pressure_abs:						float	= None							# Raw absolute pressure measured by the depth sensor in Pascals.
	altitude:							float	= None							# Altitude computed from depth sensor in meters and positive upward.

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_DEPTH message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<IHff', payload)
		self.time_stamp					= unpacked_data[0]
		self.status						= unpacked_data[1]
		self.pressure_abs				= unpacked_data[2]
		self.altitude					= unpacked_data[3]

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_DEPTH message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IHff',
			self.time_stamp,
			self.status,
			self.pressure_abs,
			self.altitude
		)

		buffer.extend(packed_data)

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
			f"{self.pressure_abs:>12.6f}"
			f"{self.altitude:>12.6f}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'pressureAbs':>12}{'altitude':>12}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(Pa)':>12}{'(m)':>12}\n"
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
