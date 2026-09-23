# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_ekf_euler.py										* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				27 March 2025													* |
# *																						* |
# * brief				Parse EKF euler logs.											* |
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
class SbgEComLogEkfEuler(SbgEComEkfCommon):
	"""
	Data class that stores data for the SBG_ECOM_LOG_EKF_EULER message.
	"""

	name									= "euler"							# Log name.

	"""
	Class members.
	"""
	time_stamp:							int = None								# Time in us since the sensor power up.
	euler:								np.ndarray = field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# Roll [-pi;+pi], pitch [-pi/2;+pi/2], yaw [-pi;+pi] in radians.
	euler_std_dev:						np.ndarray = field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# Roll std. [0;+pi], pitch std. [0;+pi/2] and yaw std. [0;+pi] estimated 1-sigma accuracy in radians.
	status:								int = None								# EKF solution status bitmask and enum.
	mag_declination:					float = None							# Local magnetic declination in radians (positive east - set to NAN if not available).
	mag_inclination:					float = None							# Local magnetic inclination in radians (positive down - set to NAN if not available).

	def get_magnetic_heading(self) -> float:
		"""
		Returns the magnetic heading from the true heading by applying the local declination.

		Returns:
			float:							Magnetic heading [-pi;+pi] in radians.
		"""
		heading = self.euler[2]

		if (self.mag_declination is not None) and not np.isnan(self.mag_declination):
			heading -= self.mag_declination

			if heading > np.pi:
				heading -= 2 * np.pi
			elif heading < -np.pi:
				heading += 2 * np.pi

		return heading

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_EKF_EULER message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<I3f3fI', payload[:32])
		self.time_stamp					= unpacked_data[0]
		self.euler						= np.array(unpacked_data[1:4], dtype=np.float32)
		self.euler_std_dev				= np.array(unpacked_data[4:7], dtype=np.float32)
		self.status						= unpacked_data[7]

		if len(payload[32:]) >= 8:
			self.mag_declination, self.mag_inclination = struct.unpack('<ff', payload[32:40])
		else:
			self.mag_declination = np.nan
			self.mag_inclination = np.nan

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_EKF_EULER message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<I3f3fIff',
			self.time_stamp,
			self.euler[0],
			self.euler[1],
			self.euler[2],
			self.euler_std_dev[0],
			self.euler_std_dev[1],
			self.euler_std_dev[2],
			self.status,
			self.mag_declination,
			self.mag_inclination
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
			f"{np.degrees(self.euler[0]):>12.6f}"
			f"{np.degrees(self.euler[1]):>12.6f}"
			f"{np.degrees(self.euler[2]):>12.6f}"
			f"{np.degrees(self.euler_std_dev[0]):>12.6f}"
			f"{np.degrees(self.euler_std_dev[1]):>12.6f}"
			f"{np.degrees(self.euler_std_dev[2]):>12.6f}"
			f"{np.degrees(self.get_magnetic_heading()):>12.6f}"
			f"{np.degrees(self.mag_declination):>12.6f}"
			f"{np.degrees(self.mag_inclination):>12.6f}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'roll':>12}{'pitch':>12}{'yaw':>12}{'rollStd':>12}{'pitchStd':>12}{'yawStd':>12}{'magHeading':>12}{'magDecl':>12}{'magIncl':>12}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(deg)':>12}{'(deg)':>12}{'(deg)':>12}{'(deg)':>12}{'(deg)':>12}{'(deg)':>12}{'(deg)':>12}{'(deg)':>12}{'(deg)':>12}\n"
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
