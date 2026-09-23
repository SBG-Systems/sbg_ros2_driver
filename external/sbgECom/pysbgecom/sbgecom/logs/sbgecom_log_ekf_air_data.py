# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_ekf_euler.py										* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				02 April 2026													* |
# *																						* |
# * brief				Parse EKF air dta logs.											* |
# * description			python 3.*														* |
# *																						* |
# * copyright			Copyright (C) 2026, SBG Systems SAS. All rights reserved.		* |
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

# Third party
import numpy as np

# Local application
from . import SbgEComLog
from sbgecom.logs import SbgEComEkfCommon

@dataclass
class SbgEComLogEkfAirData(SbgEComEkfCommon, SbgEComLog):
	"""
	Data class that stores data for the SBG_ECOM_LOG_EKF_AIR_DATA message.
	"""

	name									= "ekfAirData"							# Log name.

	"""
	Class members.
	"""
	time_stamp:							int = 0										# Time in us since the sensor power up.
	status:								int = 0										# Status, reserved for future use.
	wind:								np.ndarray = field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# North, East, Down air velocity, in m.s^-1.
	wind_std_dev:						np.ndarray = field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# North, East, Down air velocity standard deviation, in m.s^-1.

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_EKF_AIR_DATA message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<IH3f3f', payload)
		self.time_stamp					= unpacked_data[0]
		self.status						= unpacked_data[1]
		self.wind						= np.array(unpacked_data[2:5], dtype=np.float32)
		self.wind_std_dev				= np.array(unpacked_data[5:8], dtype=np.float32)

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_EKF_AIR_DATA message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IH3f3f',
			self.time_stamp,
			self.status,
			self.wind[0],
			self.wind[1],
			self.wind[2],
			self.wind_std_dev[0],
			self.wind_std_dev[1],
			self.wind_std_dev[2]
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
			f"{self.wind[0]:>12.6f}"
			f"{self.wind[1]:>12.6f}"
			f"{self.wind[2]:>12.6f}"
			f"{self.wind_std_dev[0]:>12.6f}"
			f"{self.wind_std_dev[1]:>12.6f}"
			f"{self.wind_std_dev[2]:>12.6f}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'windN':>12}{'windE':>12}{'windD':>12}{'windStdN':>12}{'windStdE':>12}{'windStdD':>12}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(m.s^-1)':>12}{'(m.s^-1)':>12}{'(m.s^-1)':>12}{'(m.s^-1)':>12}{'(m.s^-1)':>12}{'(m.s^-1)':>12}\n"
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
