# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_ekf_quat.py											* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				27 March 2025													* |
# *																						* |
# * brief				Parse EKF quaternion logs.										* |
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
from sbgecom.logs import SbgEComEkfCommon

@dataclass
class SbgEComLogEkfNav(SbgEComEkfCommon):
	"""
	Data class that stores data for the SBG_ECOM_LOG_EKF_NAV message.
	"""

	name											= "nav"							# Log name.

	"""
	Class members.
	"""
	time_stamp:							int			= None															# Time in us since the sensor power up.
	velocity:							np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# North, East, Down velocity in m.s^-1.
	velocity_std_dev:					np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# North, East, Down velocity 1-sigma standard deviation in m.s^-1.
	position:							np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float64))	# Latitude, Longitude in degrees positive North and East.
	undulation:							float		= None															# Altitude difference between the geoid and the Ellipsoid in meters (Height above Ellipsoid = altitude + undulation).
	position_std_dev:					np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# Latitude, longitude and altitude 1 sigma standard deviation in meters.
	status:								int			= None															# EKF solution status bitmask and enum.

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_EKF_NAV message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<I3f3f3df3fI', payload)
		self.time_stamp					= unpacked_data[0]
		self.velocity					= np.array(unpacked_data[1:4], dtype=np.float32)
		self.velocity_std_dev			= np.array(unpacked_data[4:7], dtype=np.float32)
		self.position					= np.array(unpacked_data[7:10], dtype=np.float64)
		self.undulation					= unpacked_data[10]
		self.position_std_dev			= np.array(unpacked_data[11:14], dtype=np.float32)
		self.status						= unpacked_data[14]

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_EKF_NAV message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<I3f3f3df3fI',
			self.time_stamp,
			self.velocity[0],
			self.velocity[1],
			self.velocity[2],
			self.velocity_std_dev[0],
			self.velocity_std_dev[1],
			self.velocity_std_dev[2],
			self.position[0],
			self.position[1],
			self.position[2],
			self.undulation,
			self.position_std_dev[0],
			self.position_std_dev[1],
			self.position_std_dev[2],
			self.status
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
			f"{self.velocity[0]:>12.6f}"
			f"{self.velocity[1]:>12.6f}"
			f"{self.velocity[2]:>12.6f}"
			f"{self.velocity_std_dev[0]:>12.6f}"
			f"{self.velocity_std_dev[1]:>12.6f}"
			f"{self.velocity_std_dev[2]:>12.6f}"
			f"{self.position[0]:>12.6f}"
			f"{self.position[1]:>12.6f}"
			f"{self.position[2]:>12.6f}"
			f"{self.position_std_dev[0]:>12.6f}"
			f"{self.position_std_dev[1]:>12.6f}"
			f"{self.position_std_dev[2]:>12.6f}"
			f"{self.undulation:>12.6f}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'velN':>12}{'velE':>12}{'velD':>12}{'velStdN':>12}{'velStdE':>12}{'velStdD':>12}{'latitude':>12}{'longitude':>12}{'altitude':>12}{'latitudeStd':>12}{'longitudeStd':>12}{'altitudeStd':>12}{'undulation':>12}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(m.s^-1)':>12}{'(m.s^-1)':>12}{'(m.s^-1)':>12}{'(m.s^-1)':>12}{'(m.s^-1)':>12}{'(m.s^-1)':>12}{'(deg)':>12}{'(deg)':>12}{'(m)':>12}{'(m)':>12}{'(m)':>12}{'(m)':>12}{'(m)':>12}\n"
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
