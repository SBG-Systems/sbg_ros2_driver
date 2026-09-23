# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_imu_data.py											* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				Parse IMU (Inertial Measurement Unit) measurement logs.			* |
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
from dataclasses import dataclass, field

# Third party
import numpy as np

# Local application
from . import SbgEComLog
from sbgecom.logs.helpers.imu_data_mean import ImuDataMean

@dataclass
class SbgEComLogImuLegacy(SbgEComLog):
	"""
	DataClass that stores data for the SBG_ECOM_LOG_IMU_DATA message.

	This message is not recommended for new designs and will be deprecated in sbgECom 5.x.
	"""

	name											= "imuData"						# Log name.

	file_values										= ImuDataMean()					# Decimated IMU values for file output.
	console_values									= ImuDataMean()					# Decimated IMU values for console output.

	"""
	Class members.
	"""
	time_stamp:							int			= None							# Time in microseconds since the sensor power up.
	status:								int			= None							# IMU status bitmask.
	accelerometers:						np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# X, Y, Z accelerometer readings in m/s².
	gyroscopes:							np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# X, Y, Z gyroscope readings in rad/s.
	temperature:						int			= None							# Internal temperature in °C.
	delta_velocity:						np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# X, Y, Z delta velocity in m/s² (same as accelerometers field).
	delta_angle:						np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# X, Y, Z delta angle in rad/s (same as gyroscopes field).

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_IMU_DATA message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<IH3f3ff3f3f', payload)
		self.time_stamp					= unpacked_data[0]
		self.status						= unpacked_data[1]
		self.accelerometers				= np.array(unpacked_data[2:5], dtype=np.float32)
		self.gyroscopes					= np.array(unpacked_data[5:8], dtype=np.float32)
		self.temperature				= unpacked_data[8]
		self.delta_velocity				= np.array(unpacked_data[9:12], dtype=np.float32)
		self.delta_angle				= np.array(unpacked_data[12:15], dtype=np.float32)

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_IMU_DATA message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IH3f3ff3f3f',
			self.time_stamp,
			self.status,
			self.accelerometers[0],
			self.accelerometers[1],
			self.accelerometers[2],
			self.gyroscopes[0],
			self.gyroscopes[1],
			self.gyroscopes[2],
			self.temperature,
			self.delta_velocity[0],
			self.delta_velocity[1],
			self.delta_velocity[2],
			self.delta_angle[0],
			self.delta_angle[1],
			self.delta_angle[2]
		)

		buffer.extend(packed_data)

	def get_values(self, context, values) -> str:
		"""
		Return formatted values shared by file and console outputs.

		Args:
			context (LoggerContext):		Logger context and settings.
			values:							Log values.

		Returns:
			str:							Formatted values.
		"""
		data = (
			f"{context.fmt_status(values.status):>12}"
			f"{values.get_accelerometer(0):>12.6f}"
			f"{values.get_accelerometer(1):>12.6f}"
			f"{values.get_accelerometer(2):>12.6f}"
			f"{values.get_gyroscope_deg(0):>12.6f}"
			f"{values.get_gyroscope_deg(1):>12.6f}"
			f"{values.get_gyroscope_deg(2):>12.6f}"
			f"{values.get_temperature():>12.6f}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'accelX':>12}{'accelY':>12}{'accelZ':>12}{'gyroX':>12}{'gyroY':>12}{'gyroZ':>12}{'temperature':>12}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(m.s^-2)':>12}{'(m.s^-2)':>12}{'(m.s^-2)':>12}{'(deg.s^-1)':>12}{'(deg.s^-1)':>12}{'(deg.s^-1)':>12}{'(degC)':>12}\n"
			)
			file.flush()

	def write_to_file(self, context, file):
		"""
		Write the data log to the file.

		Args:
			context (LoggerContext):		Logger context and settings.
			file (file):					Output file to write data into.
		"""
		acc_values = self.file_values.add_imu_data(self)

		if acc_values >= context.m_settings.m_file_decimation:
			if file:
				file.write(
					f"{context.fmt_time(self.file_values.time_stamp):>12}"
					f"{self.get_values(context, self.file_values)}\n"
				)
				file.flush()

			self.file_values.reset()

	def write_to_console(self, context):
		"""
		Write the data log to the console.

		Args:
			context (LoggerContext):		Logger context and settings.
		"""
		acc_values = self.console_values.add_imu_data(self)

		if acc_values >= context.m_settings.m_console_decimation:
			print(
				f"{self.get_name():>13}:"
				f"{self.get_values(context, self.console_values)}"
			)

			self.console_values.reset()
