# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_velocity.py											* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				07 May 2025														* |
# *																						* |
# * brief				Parse received generic velocity measurement logs.				* |
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
from sbgecom.defs.sbgecom_defs_aiding import SbgEComAidingTime

@dataclass
class SbgEComLogVelocity(SbgEComLog):
	"""
	Data class that stores data for the SBG_ECOM_LOG_VELOCITY_# message.
	"""

	"""
	Velocity status mask definitions.
	"""
	VELOCITY_0_VALID					= (0x0001 << 3)								# Set to 1 if the X or North velocity information is valid.
	VELOCITY_1_VALID					= (0x0001 << 4)								# Set to 1 if the Y or East velocity information is valid.
	VELOCITY_2_VALID					= (0x0001 << 5)								# Set to 1 if the Z or Down velocity information is valid.
	VELOCITY_STD_VALID					= (0x0001 << 6)								# Set to 1 if the velocity standard deviation information is valid.

	"""Timestamp time type has been added in 5.4."""
	TIME_TYPE_SHIFT						= (0)										# Shift used to extract the velocity time type part.

	name											= "velocity1"					# Log name.

	"""
	Class members.
	"""
	time_stamp:							int			= None							# Time in us since the INS power up OR measurement delay in us OR GPS time of the week in ms.
	status:								int			= None							# Velocity status bitmask.
	velocity:							np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# X,Y,Z or North/East/Down velocities in m/s.
	velocity_std:						np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# X,Y,Z or North/East/Down velocities standard deviation in m/s.

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_VELOCITY_# message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<IH3f3f', payload)
		self.time_stamp					= unpacked_data[0]
		self.status						= unpacked_data[1]
		self.velocity					= np.array(unpacked_data[2:5], dtype=np.float32)
		self.velocity_std				= np.array(unpacked_data[5:8], dtype=np.float32)

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_VELOCITY_# message to the output stream buffer from the provided object.

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
			self.velocity_std[0],
			self.velocity_std[1],
			self.velocity_std[2]
		)

		buffer.extend(packed_data)

	def get_time_type(self) -> SbgEComAidingTime.Type:
		"""
		Returns the velocity time type.

		Returns:
			SbgEComLogTime.Type:			The time type.
		"""
		return SbgEComAidingTime.Type((self.status >> self.TIME_TYPE_SHIFT) & SbgEComAidingTime.TYPE_MASK)

	def set_time_type(self, time_type: SbgEComAidingTime.Type):
		"""
		Set the velocity time type.

		Args:
			time_type (SbgEComLogTime.Type): The time type to set.
		"""
		assert isinstance(time_type, SbgEComAidingTime.Type)

		self.status &= ~(SbgEComAidingTime.TYPE_MASK << self.TIME_TYPE_SHIFT)
		self.status |= ((time_type.value & SbgEComAidingTime.TYPE_MASK) << self.TIME_TYPE_SHIFT)

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
			f"{self.velocity_std[0]:>12.6f}"
			f"{self.velocity_std[1]:>12.6f}"
			f"{self.velocity_std[2]:>12.6f}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'vel0':>12}{'vel1':>12}{'vel2':>12}{'velStd0':>12}{'velStd1':>12}{'velStd2':>12}\n"
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
