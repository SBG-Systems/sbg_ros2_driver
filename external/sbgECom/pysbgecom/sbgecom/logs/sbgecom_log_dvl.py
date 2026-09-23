# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_dvl.py												* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				07 May 2025														* |
# *																						* |
# * brief				Parse received DVL (Doppler Velocity Logger) measurement logs.	* |
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
class SbgEComLogDvl(SbgEComLog):
	"""
	Data class that stores data for the SBG_ECOM_LOG_DVL_BOTTOM_TRACK / SBG_ECOM_LOG_DVL_WATER_TRACK messages.

	The DVL quality indicator represents residual error information rather than an actual standard deviation measurement.
	"""

	VELOCITY_VALID						= (0x0001 << 0)							# Set to 1 if the DVL equipment was able to measure a valid velocity.
	TIME_SYNC							= (0x0001 << 1)							# Set to 1 if the DVL data is correctly synchronized.

	"""
	Class members.
	"""
	time_stamp:							int			= None						# Time in us since the sensor power up.
	status:								int			= None						# DVL status bitmask.
	velocity:							np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# X, Y, Z velocities in m/s expressed in the DVL instrument frame.
	velocityQuality:					np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# Quality indicators for X, Y, Z velocities provided by the DVL sensor, expressed in m/s.

	# To identifie either dvlBottom or dvlWater.
	log_id: int = field(default=None, init=False, repr=False)

	def set_log_id(self, log_id: int):
		"""
		Method to set the log_id after initialization.

		Args:
			log_id (int):					The DVL type.
		"""
		self.log_id = log_id

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_DVL_BOTTOM_TRACK / SBG_ECOM_LOG_DVL_WATER_TRACK message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<IH3f3f', payload)
		self.time_stamp					= unpacked_data[0]
		self.status						= unpacked_data[1]
		self.velocity					= np.array(unpacked_data[2:5], dtype=np.float32)
		self.velocityQuality			= np.array(unpacked_data[5:8], dtype=np.float32)

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_DVL_BOTTOM_TRACK / SBG_ECOM_LOG_DVL_WATER_TRACK message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IH3f3f',
			self.time_stamp,
			self.status,
			self.velocity[0],
			self.velocity[1],
			self.velocity[2],
			self.velocityQuality[0],
			self.velocityQuality[1],
			self.velocityQuality[2]
		)

		buffer.extend(packed_data)

	def get_name(self) -> str:
		"""
		Returns the log name.

		Returns:
			str:							Log name.
		"""
		match self.log_id:
			case SbgEComLogId.DVL_BOTTOM_TRACK.value:
				return "dvlBottom"

			case SbgEComLogId.DVL_WATER_TRACK.value:
				return "dvlWater"

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
			f"{self.velocity[0]:>12.6f}"
			f"{self.velocity[1]:>12.6f}"
			f"{self.velocity[2]:>12.6f}"
			f"{self.velocityQuality[0]:>12.6f}"
			f"{self.velocityQuality[1]:>12.6f}"
			f"{self.velocityQuality[2]:>12.6f}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'velX':>12}{'velY':>12}{'velZ':>12}{'velQualX':>12}{'velQualY':>12}{'velQualZ':>12}\n"
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
