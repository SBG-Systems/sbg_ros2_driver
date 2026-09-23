# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				imu_data_mean.py												* |
# * ingroup				helpers															* |
# * author				SBG Systems														* |
# * date				12 March 2025													* |
# *																						* |
# * brief				Implemented simple moving average filter for IMU data entries.	* |
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
import numpy as np

class ImuDataMean:
	"""
	Helper used to accumulate and decimate IMU data.
	The timestamp represents the latest added measurement.
	The status represents the OR combinaison of all accumulated status.
	"""

	def __init__(self):
		"""
		Default constructor.
		"""
		self.time_stamp						= 0										# Latest added measurement timestamp in us.
		self.status							= 0										# Status bitmask.
		self.accelerometer					= np.zeros(3)							# X,Y,Z accelerometer accumulator in m.s^-2.
		self.gyroscope						= np.zeros(3)							# X,y,Z rotation rate accumulator in rad.s^-1.
		self.temperature					= 0.0									# Temperature accumulator in degC.
		self.num_acc_values					= 0										# Number of accumulated values.

	def add_imu_data(self, imu_data) -> int:
		"""
		Accumulate a IMU measurement and return the number of accumulated values.

		Args:
			imu_data (SbgEComLogImuLegacy):	IMU data to accumulate.

		Returns:
			int:							Number of accumulated data so far.
		"""
		self.time_stamp						= imu_data.time_stamp
		self.status							|= imu_data.status

		self.accelerometer[0]				+= imu_data.delta_velocity[0]
		self.accelerometer[1]				+= imu_data.delta_velocity[1]
		self.accelerometer[2]				+= imu_data.delta_velocity[2]

		self.gyroscope[0]					+= imu_data.delta_angle[0]
		self.gyroscope[1]					+= imu_data.delta_angle[1]
		self.gyroscope[2]					+= imu_data.delta_angle[2]

		self.temperature					+= imu_data.temperature
		self.num_acc_values					+= 1

		return self.num_acc_values

	def add_imu_short(self, imu_short) -> int:
		"""
		Accumulate a IMU measurement and return the number of accumulated values.

		Args:
			imu_short (SbgEComLogImuShort):	IMU data to accumulate.

		Returns:
			int:							Number of accumulated data so far.
		"""
		self.time_stamp						= imu_short.time_stamp
		self.status							|= imu_short.status

		self.accelerometer[0]				+= imu_short.get_delta_velocity(0)
		self.accelerometer[1]				+= imu_short.get_delta_velocity(1)
		self.accelerometer[2]				+= imu_short.get_delta_velocity(2)

		self.gyroscope[0]					+= imu_short.get_delta_angle(0)
		self.gyroscope[1]					+= imu_short.get_delta_angle(1)
		self.gyroscope[2]					+= imu_short.get_delta_angle(2)

		self.temperature					+= imu_short.get_temperature()
		self.num_acc_values					+= 1

		return self.num_acc_values

	def reset(self):
		"""
		Reset the accumulators.
		"""
		self.__init__()

	def get_accelerometer(self, idx: int) -> float:
		"""
		Returns the mean accelerometer value.

		Args:
			idx (int):						Axis index from 0 to 2.

		Returns:
			float:							Mean accelerometer value in m.s^-2.
		"""
		assert idx < self.accelerometer.size
		assert self.num_acc_values > 0

		return self.accelerometer[idx] / self.num_acc_values

	def get_gyroscope(self, idx: int) -> float:
		"""
		Returns the mean rotation rate value.

		Args:
			idx (int):						Axis index from 0 to 2.

		Returns:
			float:							Mean gyroscope value in rad.s^-1.
		"""
		assert idx < self.gyroscope.size
		assert self.num_acc_values > 0

		return self.gyroscope[idx] / self.num_acc_values

	def get_gyroscope_deg(self, idx: int) -> float:
		"""
		Returns the mean rotation rate value.

		Args:
			idx (int):						Axis index from 0 to 2.

		Returns:
			float:							Mean gyroscope value in deg.s^-1.
		"""
		assert idx < self.gyroscope.size
		assert self.num_acc_values > 0

		return np.degrees(self.get_gyroscope(idx))

	def get_temperature(self) -> float:
		"""
		Returns the mean temperature value.

		Returns:
			float:							Mean temperature in degC.
		"""
		assert self.num_acc_values > 0

		return self.temperature / self.num_acc_values if self.num_acc_values > 0 else 0.0
