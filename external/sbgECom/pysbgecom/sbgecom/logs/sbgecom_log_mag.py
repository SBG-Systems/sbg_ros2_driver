# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_mag.py												* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				07 May 2025														* |
# *																						* |
# * brief				Parse magnetic field measurements logs.							* |
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
class SbgEComLogMag(SbgEComLog):
	"""
	Data class that stores data for the SBG_ECOM_LOG_MAG message.

	This structure encapsulates the magnetic field data, which is calibrated to correct for soft and hard iron effects.
	It provides 3D magnetic field readings intended for use by the INS filter.
	"""

	"""
	Log magnetometer data status mask definitions.
	"""
	MAG_X_BIT							= (0x00000001 << 0)							# Set to 1 if the magnetometer X passes Built In Test.
	MAG_Y_BIT							= (0x00000001 << 1)							# Set to 1 if the magnetometer Y passes Built In Test.
	MAG_Z_BIT							= (0x00000001 << 2)							# Set to 1 if the magnetometer Z passes Built In Test.

	ACCEL_X_BIT							= (0x00000001 << 3)							# Set to 1 if the accelerometer X passes Built In Test.
	ACCEL_Y_BIT							= (0x00000001 << 4)							# Set to 1 if the accelerometer Y passes Built In Test.
	ACCEL_Z_BIT							= (0x00000001 << 5)							# Set to 1 if the accelerometer Z passes Built In Test.

	MAGS_IN_RANGE						= (0x00000001 << 6)							# Set to 1 if all magnetometers are within operating range.
	ACCELS_IN_RANGE						= (0x00000001 << 7)							# Set to 1 if all accelerometers are within operating range.

	CALIBRATION_OK						= (0x00000001 << 8)							# Set to 1 if the magnetometers seems to be calibrated.

	"""Timestamp time type has been added in 5.4."""
	TIME_TYPE_SHIFT						= (9)										# Shift used to extract the magnetometer time type part.

	name											= "mag"							# Log name.

	"""
	Class members.
	"""
	time_stamp:							int			= None							# Time in us since the sensor power up.
	status:								int			= None							# Magnetometer status bitmask.
	magnetometers:						np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# X, Y, Z magnetometer data in arbitrary units (A.U.).
	accelerometers:						np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# X, Y, Z accelerometer data in (m/s^2).

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_MAG message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<IH3f3f', payload)
		self.time_stamp					= unpacked_data[0]
		self.status						= unpacked_data[1]
		self.magnetometers				= np.array(unpacked_data[2:5], dtype=np.float32)
		self.accelerometers				= np.array(unpacked_data[5:8], dtype=np.float32)

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_MAG message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""

		packed_data = struct.pack(
			'<IH3f3f',
			self.time_stamp,
			self.status,
			self.magnetometers[0],
			self.magnetometers[1],
			self.magnetometers[2],
			self.accelerometers[0],
			self.accelerometers[1],
			self.accelerometers[2]
		)

		buffer.extend(packed_data)

	def get_time_type(self) -> SbgEComAidingTime.Type:
		"""
		Returns the magnetometer time type.

		Returns:
			SbgEComLogTime.Type:			The time type.
		"""
		return SbgEComAidingTime.Type((self.status >> self.TIME_TYPE_SHIFT) & SbgEComAidingTime.TYPE_MASK)

	def set_time_type(self, time_type: SbgEComAidingTime.Type):
		"""
		Set the magnetometer time type.

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
			f"{self.magnetometers[0]:>12.6f}"
			f"{self.magnetometers[1]:>12.6f}"
			f"{self.magnetometers[2]:>12.6f}"
			f"{self.accelerometers[0]:>12.6f}"
			f"{self.accelerometers[1]:>12.6f}"
			f"{self.accelerometers[2]:>12.6f}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'magX':>12}{'magY':>12}{'magZ':>12}{'accelX':>12}{'accelY':>12}{'accelZ':>12}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(au)':>12}{'(au)':>12}{'(au)':>12}{'(m.s^-2)':>12}{'(m.s^-2)':>12}{'(m.s^-2)':>12}\n"
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
