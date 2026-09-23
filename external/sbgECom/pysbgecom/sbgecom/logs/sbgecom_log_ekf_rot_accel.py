# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_ekf_rot_accel.py									* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				06 May 2025														* |
# *																						* |
# * brief				Write and parse SBG_ECOM_LOG_EKF_ROT_ACCEL_XXXX messages.		* |
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

# Third party
import numpy as np

# Local application
from . import SbgEComLog
from sbgecom.sbgecom_ids import SbgEComLogId

@dataclass
class SbgEComLogEkfRotAccel(SbgEComLog):
	"""
	Data class that stores data for the SBG_ECOM_LOG_EKF_ROT_ACCEL_XXXX message.

	INS compensated body or North, East, Down rotation rates and linear accelerations.
	"""

	"""
	Class members.
	"""
	time_stamp:							int = 0									# Time in us since the sensor power up.
	status:								int = 0									# Status - reserved for future use.
	rate:								np.ndarray = field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# X,Y,Z or North/East/Down INS rotation rate free from earth rotation and sensor bias/scale errors (rad.s^-1).
	acceleration:						np.ndarray = field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# X,Y,Z or North/East/Down INS accelerations and free from gravity and sensor bias/scale errors (m.s^-2).

	# To identifie either rotAccelBody or rotAccelNed.
	log_id: int = field(default=None, init=False, repr=False)

	def set_log_id(self, log_id: int):
		"""
		Method to set the log_id after initialization.

		Args:
			log_id (int):					The rotation acceleration type.
		"""
		self.log_id = log_id

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_EKF_ROT_ACCEL_XXXX message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<II3f3f', payload)
		self.time_stamp					= unpacked_data[0]
		self.status						= unpacked_data[1]
		self.rate						= np.array(unpacked_data[2:5], dtype=np.float32)
		self.acceleration				= np.array(unpacked_data[5:8], dtype=np.float32)

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_EKF_ROT_ACCEL_XXXX message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<II3f3f',
			self.time_stamp,
			self.status,
			self.rate[0],
			self.rate[1],
			self.rate[2],
			self.acceleration[0],
			self.acceleration[1],
			self.acceleration[2],
		)

		buffer.extend(packed_data)

	def get_name(self) -> str:
		"""
		Returns the log name.

		Returns:
			str:							Log name.
		"""
		match self.log_id:
			case SbgEComLogId.EKF_ROT_ACCEL_BODY.value:
				return "rotAccelBody"

			case SbgEComLogId.EKF_ROT_ACCEL_NED.value:
				return "rotAccelNed"

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
			f"{self.acceleration[0]:>12.6f}"
			f"{self.acceleration[1]:>12.6f}"
			f"{self.acceleration[2]:>12.6f}"
			f"{np.degrees(self.rate[0]):>12.6f}"
			f"{np.degrees(self.rate[1]):>12.6f}"
			f"{np.degrees(self.rate[2]):>12.6f}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'accelX':>12}{'accelY':>12}{'accelZ':>12}{'rateX':>12}{'rateY':>12}{'rateZ':>12}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(m.s^-2)':>12}{'(m.s^-2)':>12}{'(m.s^-2)':>12}{'(deg.s^-1)':>12}{'(deg.s^-1)':>12}{'(deg.s^-1)':>12}\n"
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
