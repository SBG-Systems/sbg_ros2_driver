# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file 				sbgecom_log_imu_short.py										* |
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
from dataclasses import dataclass, field
import struct

# Third party
import numpy as np

# Local application
from . import SbgEComLog
from sbgecom.logs.helpers.imu_data_mean import ImuDataMean

@dataclass
class SbgEComLogImuShort(SbgEComLog):
	"""
	Data class that stores data for the SBG_ECOM_LOG_IMU_SHORT message.

	This message is sent asynchronously and must be used for post processing.

	The delta angle values are scaled based on the gyroscopes output. If any output exceeds
	a predefined limit, the scale factor switches from standard to high range to prevent saturation.
	"""

	"""
	Standard gyroscope scale factor.
	"""
	GYRO_SCALE_STD						= 67108864.0

	"""
	High range gyroscope scale factor derived from 10000 degrees per second maximum range.

	Calculation: (2^31 - 1) / (10000 * π / 180)
	"""
	GYRO_SCALE_HIGH						= 12304174.0

	"""
	Maximum value for the standard scale factor, in radians per second.

	Approximately equivalent to 1833 °/s.
	"""
	GYRO_SCALE_STD_MAX_RAD				= float(np.iinfo(np.uint32).max) / GYRO_SCALE_STD

	"""
	Standard accelerometer scale factor.
	"""
	ACCEL_SCALE_STD						= 1048576.0

	"""
	Standard temperature scale factor.
	"""
	TEMP_SCALE_STD						= 256.0

	"""
	Log inertial data status mask definitions.
	"""
	COM_OK								= (0x00000001 << 0)						# Set to 1 if the communication with the IMU is ok.
	STATUS_BIT							= (0x00000001 << 1)						# Set to 1 if the IMU passes general Built in Tests (calibration, CPU, ...).

	ACCEL_X_BIT							= (0x00000001 << 2)						# Set to 1 if the accelerometer X passes Built In Test.
	ACCEL_Y_BIT							= (0x00000001 << 3)						# Set to 1 if the accelerometer Y passes Built In Test.
	ACCEL_Z_BIT							= (0x00000001 << 4)						# Set to 1 if the accelerometer Z passes Built In Test.

	GYRO_X_BIT							= (0x00000001 << 5)						# Set to 1 if the gyroscope X passes Built In Test.
	GYRO_Y_BIT							= (0x00000001 << 6)						# Set to 1 if the gyroscope Y passes Built In Test.
	GYRO_Z_BIT							= (0x00000001 << 7)						# Set to 1 if the gyroscope Z passes Built In Test.

	ACCELS_IN_RANGE						= (0x00000001 << 8)						# Set to 1 if all accelerometers are within operating range.
	GYROS_IN_RANGE						= (0x00000001 << 9)						# Set to 1 if all gyroscopes are within operating range.
	GYROS_USE_HIGH_SCALE				= (0x00000001 << 10)					# Set if the gyroscope scale factor range is high. Applicable only for SBG_ECOM_LOG_IMU_SHORT logs.

	name									= "imuShort"						# Log name.

	file_values								= ImuDataMean()						# Decimated IMU values for file output.
	console_values							= ImuDataMean()						# Decimated IMU values for console output.

	"""
	Class members.
	"""
	time_stamp:							int = None								# Time in us since the sensor power up.
	status:								int = None								# IMU status bitmask.
	delta_velocity:						np.ndarray = field(default_factory=lambda: np.zeros(3, dtype=np.int32))		# X, Y, Z delta velocity. Unit is 1048576 LSB for 1 m.s^-2.
	delta_angle:						np.ndarray = field(default_factory=lambda: np.zeros(3, dtype=np.int32))		# X, Y, Z delta angle. Unit is either 67108864 LSB for 1 rad.s^-1 (standard) or 12304174 LSB for 1 rad.s^-1 (high range), managed automatically.
	temperature:						int = None								# IMU average temperature. Unit is 256 LSB for 1°C.

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_IMU_SHORT message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<IH3i3ih', payload)
		self.time_stamp					= unpacked_data[0]
		self.status						= unpacked_data[1]
		self.delta_velocity				= np.array(unpacked_data[2:5], dtype=np.int32)
		self.delta_angle				= np.array(unpacked_data[5:8], dtype=np.int32)
		self.temperature				= unpacked_data[8]

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_IMU_SHORT message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IH3i3ih',
			self.time_stamp,
			self.status,
			self.delta_velocity[0],
			self.delta_velocity[1],
			self.delta_velocity[2],
			self.delta_angle[0],
			self.delta_angle[1],
			self.delta_angle[2],
			self.temperature
		)

		buffer.extend(packed_data)

	def get_delta_velocity(self, idx: int) -> float:
		"""
		Return from an IMU Short log, the X, Y or Z delta velocity value in m.s^-2

		Args:
			idx (int):						The component to return from 0 to 2.

		Returns:
			float:							The delta velocity value converted, in m.s^-2.
		"""
		if idx < 0 or idx >= 3:
			raise IndexError("index must be between 0 and 2")

		return self.delta_velocity[idx] / self.ACCEL_SCALE_STD

	def set_delta_velocity(self, array: np.ndarray):
		"""
		Set the delta velocity values.

		Args:
			array (np.ndarray):				Array of delta velocity values, in m.s^-2.
		"""
		if array is None:
			raise ValueError("array must not be None")

		if len(array) != 3:
			raise ValueError("array must have exactly 3 elements")

		self.delta_velocity = np.array([int(array[i] * self.ACCEL_SCALE_STD) for i in range(3)], dtype=np.int32)

	def get_delta_angle(self, idx: int) -> float:
		"""
		Return from an IMU Short log, the X, Y or Z delta angle value in rad.s^-1.

		Args:
			idx (int):						The component to return from 0 to 2.

		Returns:
			float:							The delta angle value converted in rad.s^-1.
		"""
		scale_factor = self.GYRO_SCALE_STD

		if idx < 0 or idx >= 3:
			raise IndexError("index must be between 0 and 2")

		if self.status & self.GYROS_USE_HIGH_SCALE:
			scale_factor = self.GYRO_SCALE_HIGH

		return self.delta_angle[idx] / scale_factor

	def set_delta_angle(self, array: np.ndarray):
		"""
		Set the delta angle values.

		Args:
			array (np.ndarray):				Array of delta angle values, in rad.s^-1.
		"""
		scale_factor = self.GYRO_SCALE_STD

		if array is None:
			raise ValueError("array must not be None")

		if len(array) != 3:
			raise ValueError("array must have exactly 3 elements")

		self.status &= ~self.GYROS_USE_HIGH_SCALE

		for angle in array:
			if abs(angle) > self.GYRO_SCALE_STD_MAX_RAD:
				scale_factor = self.GYRO_SCALE_HIGH
				self.status |= self.GYROS_USE_HIGH_SCALE
				break

		self.delta_angle = np.array([int(angle * scale_factor) for angle in array], dtype=np.int32)

	def get_temperature(self) -> float:
		"""
		Return from an IMU Short log, the temperature in °C.

		Returns:
			float:							The converted temperature in °C.
		"""
		return self.temperature / self.TEMP_SCALE_STD

	def set_temperature(self, temperature: float):
		"""
		Set the temperature.

		Args:
			temperature (float):			Temperature value to set, in °C.
		"""
		if temperature is None:
			raise ValueError("temperature must not be None")

		self.temperature = temperature * self.TEMP_SCALE_STD

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
		acc_values = self.file_values.add_imu_short(self)

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
		acc_values = self.console_values.add_imu_short(self)

		if acc_values >= context.m_settings.m_console_decimation:
			print(
				f"{self.get_name():>13}:"
				f"{self.get_values(context, self.console_values)}"
			)

			self.console_values.reset()
