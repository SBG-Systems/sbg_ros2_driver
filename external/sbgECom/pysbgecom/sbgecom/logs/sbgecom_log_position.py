# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_position.py											* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				31 March 2026													* |
# *																						* |
# * brief				Parse received generic position measurement logs.				* |
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
from enum import Enum
import struct

# Third party
import numpy as np

# Local application
from . import SbgEComLog
from sbgecom.defs.sbgecom_defs_aiding import SbgEComAidingTime

@dataclass
class SbgEComLogPosition(SbgEComLog):
	"""
	DataClass that stores data for the SBG_ECOM_LOG_POSITION_# message.
	"""

	class Type(Enum):
		"""
		Position types definitions.
		"""
		INVALID				= 0													# Position is invalid.
		VALID				= 1													# Position is valid.

	TYPE_SHIFT							= (3)									# Shift used to extract the position type part.
	TYPE_MASK							= (0x0007)								# Mask used to keep only the position type part.

	TIME_TYPE_SHIFT						= (0)									# Shift used to extract the position time type part.

	name										= "position1"					# Log name.

	"""
	Class members.
	"""
	time_stamp:							int		= None							# Time in us since the INS power up OR measurement delay in us OR GPS time of the week in ms.
	status:								int		= None							# Position status bitmask.
	position:							np.ndarray = field(default_factory=lambda: np.zeros(3, dtype=np.float64))	# Latitude (positive north), longitude (positive east), and height above ellipsoid, in degrees, degrees, and meters respectively.
	cov_lat_lat:						float	= None							# Variance latitude, in m^2.
	cov_lon_lon:						float	= None							# Variance longitude, in m^2.
	cov_hgt_hgt:						float	= None							# Variance height, in m^2.
	cov_lat_lon:						float	= None							# Covariance latitude-longitude, in m^2.
	cov_lat_hgt:						float	= None							# Covariance latitude-height, in m^2.
	cov_lon_hgt:						float	= None							# Covariance longitude-height, in m^2.

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_POSITION_# message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<IH3dffffff', payload)
		self.time_stamp					= unpacked_data[0]
		self.status						= unpacked_data[1]
		self.position					= np.array(unpacked_data[2:5], dtype=np.float64)
		self.cov_lat_lat				= unpacked_data[5]
		self.cov_lon_lon				= unpacked_data[6]
		self.cov_hgt_hgt				= unpacked_data[7]
		self.cov_lat_lon				= unpacked_data[8]
		self.cov_lat_hgt				= unpacked_data[9]
		self.cov_lon_hgt				= unpacked_data[10]

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_POSITION_# message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IH3dffffff',
			self.time_stamp,
			self.status,
			self.position[0],
			self.position[1],
			self.position[2],
			self.cov_lat_lat,
			self.cov_lon_lon,
			self.cov_hgt_hgt,
			self.cov_lat_lon,
			self.cov_lat_hgt,
			self.cov_lon_hgt
		)

		buffer.extend(packed_data)

	def get_position_type(self) -> Type:
		"""
		Returns the position type.

		Returns:
			Type:							The position type.
		"""
		return self.Type((self.status >> self.TYPE_SHIFT) & self.TYPE_MASK)

	def set_position_type(self, pos_type: Type):
		"""
		Set the position type.

		Args:
			pos_type (Type):				The position type to set.
		"""
		assert isinstance(pos_type, self.Type)

		self.status &= ~(self.TYPE_MASK << self.TYPE_SHIFT)
		self.status |= ((pos_type.value & self.TYPE_MASK) << self.TYPE_SHIFT)

	def get_time_type(self) -> SbgEComAidingTime.Type:
		"""
		Returns the position time type.

		Returns:
			SbgEComLogTime.Type:			The time type.
		"""
		return SbgEComAidingTime.Type((self.status >> self.TIME_TYPE_SHIFT) & SbgEComAidingTime.TYPE_MASK)

	def set_time_type(self, time_type: SbgEComAidingTime.Type):
		"""
		Set the position time type.

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
			f"{self.position[0]:>12.6f}"
			f"{self.position[1]:>12.6f}"
			f"{self.position[2]:>12.6f}"
			f"{self.cov_lat_lat:>12.6f}"
			f"{self.cov_lon_lon:>12.6f}"
			f"{self.cov_hgt_hgt:>12.6f}"
			f"{self.cov_lat_lon:>12.6f}"
			f"{self.cov_lat_hgt:>12.6f}"
			f"{self.cov_lon_hgt:>12.6f}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'latitude':>12}{'longitude':>12}{'height':>12}{'covLatLat':>12}{'covLonLon':>12}{'covHgtHgt':>12}{'covLatLon':>12}{'covLatHgt':>12}{'covLonHgt':>12}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(deg)':>12}{'(deg)':>12}{'(m)':>12}{'(m^2)':>12}{'(m^2)':>12}{'(m^2)':>12}{'(m^2)':>12}{'(m^2)':>12}{'(m^2)':>12}\n"
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
