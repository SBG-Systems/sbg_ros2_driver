# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_air_data.py											* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				Parse received air data measurement logs.						* |
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
from dataclasses import dataclass
import struct

# Local application
from . import SbgEComLog
from sbgecom.defs.sbgecom_defs_aiding import SbgEComAidingTime

@dataclass
class SbgEComLogAirData(SbgEComLog):
	"""
	DataClass that stores data for the SBG_ECOM_LOG_AIR_DATA message.
	"""

	"""Timestamp time type has been added in 5.4."""
	TIME_TYPE_SHIFT						= (6)									# Shift used to extract the air data time type part.

	"""
	Air Data sensor status mask definitions
	"""
	TIME_IS_DELAY						= (0x0001 << 0)							# Set to 1 if the timeStamp field represents a delay instead of an absolute timestamp.
	PRESSURE_ABS_VALID					= (0x0001 << 1)							# Set to 1 if the pressure field is filled and valid.
	ALTITUDE_VALID						= (0x0001 << 2)							# Set to 1 if the barometric altitude field is filled and valid.
	PRESSURE_DIFF_VALID					= (0x0001 << 3)							# Set to 1 if the differential pressure field is filled and valid.
	AIRPSEED_VALID						= (0x0001 << 4)							# Set to 1 if the true airspeed field is filled and valid.
	TEMPERATURE_VALID					= (0x0001 << 5)							# Set to 1 if the output air temperature field is filled and valid.

	name										= "airData"						# Log name.

	"""
	Class members.
	"""
	time_stamp:							int		= None							# Time in us since the sensor power up OR measurement delay in us.
	status:								int		= None							# Airdata sensor status bitmask.
	pressure_abs:						float	= None							# Raw absolute pressure measured by the barometer sensor in Pascals.
	altitude:							float	= None							# Altitude computed from barometric altimeter in meters and positive upward.
	pressure_diff:						float	= 0								# Raw differential pressure measured by the pitot tube in Pascal.
	true_air_speed:						float	= 0								# True airspeed measured by a pitot tube in m.s^-1 and positive forward.
	air_temperature:					float	= 0								# Outside air temperature in °C that could be used to compute true airspeed from differential pressure.

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_AIR_DATA message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<IHfffff', payload)
		self.time_stamp					= unpacked_data[0]
		self.status						= unpacked_data[1]
		self.pressure_abs				= unpacked_data[2]
		self.altitude					= unpacked_data[3]
		self.pressure_diff				= unpacked_data[4]
		self.true_air_speed				= unpacked_data[5]
		self.air_temperature			= unpacked_data[6]

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_AIR_DATA message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IHfffff',
			self.time_stamp,
			self.status,
			self.pressure_abs,
			self.altitude,
			self.pressure_diff,
			self.true_air_speed,
			self.air_temperature,
		)

		buffer.extend(packed_data)

	def get_time_type(self) -> SbgEComAidingTime.Type:
		"""
		Returns the air data time type.

		Returns:
			SbgEComLogTime.Type:			The time type.
		"""
		return SbgEComAidingTime.Type((self.status >> self.TIME_TYPE_SHIFT) & SbgEComAidingTime.TYPE_MASK)

	def set_time_type(self, time_type: SbgEComAidingTime.Type):
		"""
		Set the air data time type.

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
			f"{self.pressure_abs:>15.6f}"
			f"{self.altitude:>12.6f}"
			f"{self.pressure_diff:>12.6f}"
			f"{self.true_air_speed:>12.6f}"
			f"{self.air_temperature:>12.6f}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'pressureAbs':>12}{'altitude':>12}{'pressureDiff':>12}{'trueAirspeed':>12}{'airTemperature':>12}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(Pa)':>12}{'(m)':>12}{'(Pa)':>12}{'(ms.^-1)':>12}{'(degC)':>12}\n"
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
