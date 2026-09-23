# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_utc.py												* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				24 February 2025												* |
# *																						* |
# * brief				Parse logs used to report device UTC time.						* |
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
from enum import Enum
import math
from typing import Dict
import struct

# Third party
import numpy as np

# Local application
from . import SbgEComLog

class SbgEComClockState(Enum):
	"""
	Internal clock alignment to PPS algorithm state.
	"""

	ERROR								= 0											# An error has occurred on the clock estimation.
	RUNNING								= 1											# The clock is only based on the internal crystal using latest known clock bias and scale factor.
	STEERING							= 2											# A PPS has been detected and the clock is converging to it.
	VALID								= 3											# The internal clock is converged to the PPS signal or is still considered to be accurate.

class SbgEComUtcStatus(Enum):
	"""
	Status for the GNSS UTC time information.
	"""

	INVALID								= 0											# UTC time is not known and invalid. The INS uses the firmware default initial date/time.
	NO_LEAP_SEC							= 1											# UTC time is initialized but the leap second is not known - the default firmware value is used.
	INITIALIZED							= 2											# UTC time is initialized with a fully resolved leap second information.

"""
Clock status and UTC time status definitions.
"""
CLOCK_STATE_SHIFT						= 1											# Shift used to extract the clock state part.
CLOCK_STATE_MASK						= 0x000F									# Mask used to keep only the clock state part.
TIME_STATUS_SHIFT						= 6											# Shift used to extract the UTC status part.
TIME_STATUS_MASK						= 0x000F									# Mask used to keep only the UTC status part.

STATUS_HAS_CLOCK_INPUT					= 0x0001 << 0								# Set to 1 if a stable input clock could be used to synchronized the internal clock.
STATUS_UTC_IS_ACCURATE					= 0x0001 << 5								# The UTC time information is accurate and correctly timestamped with a PPS.

@dataclass
class SbgEComLogUtc(SbgEComLog):
	"""
	Data class that stores data for the SBG_ECOM_LOG_UTC_TIME message.
	"""

	name										= "utcTime"							# Log name.

	"""
	Class members.
	"""
	time_stamp:							int		= 0									# Time in us since the sensor power up.
	status:								int		= 0									# Clock state and UTC time status.
	year:								int		= 0									# Year for example: 2013.
	month:								int		= 0									# Month in year [1 .. 12].
	day:								int		= 0									# Day in month [1 .. 31].
	hour:								int		= 0									# Hour in day [0 .. 23].
	minute:								int		= 0									# Minute in hour [0 .. 59].
	second:								int		= 0									# Second in minute [0 .. 60]. (60 is used only when a leap second is added)
	nano_second:						int		= 0									# Nanosecond of current second in ns.
	gps_time_of_week:					int		= 0									# GPS time of week in ms.
	clk_bias_std:						float	= np.nan							# Estimated internal clock bias standard deviation in seconds - set to None if not available. (added in 4.0)
	clk_sf_error_std:					float	= np.nan							# Estimated internal clock scale factor error standard deviation - set to None if not available. (added in 4.0)
	clk_residual_error:					float	= np.nan							# Latest residual clock error from the GNSS PPS signal in seconds - set to None if not available. (added in 4.0)

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_UTC_DATA message and fill the corresponding structure.

		Args:
			payload (bytearray):			Input stream buffer to read the log from.
		"""
		unpacked_data					= struct.unpack('<IHHbbbbbiI', payload[:21])
		self.time_stamp					= unpacked_data[0]
		self.status						= unpacked_data[1]
		self.year						= unpacked_data[2]
		self.month						= unpacked_data[3]
		self.day						= unpacked_data[4]
		self.hour						= unpacked_data[5]
		self.minute						= unpacked_data[6]
		self.second						= unpacked_data[7]
		self.nano_second				= unpacked_data[8]
		self.gps_time_of_week			= unpacked_data[9]

		if (len(payload) - 21) >= 12:
			unpacked_data				= struct.unpack('<fff', payload[21:33])
			self.clk_bias_std			= unpacked_data[0]
			self.clk_sf_error_std		= unpacked_data[1]
			self.clk_residual_error		= unpacked_data[2]
		else:
			self.clk_bias_std			= np.nan
			self.clk_sf_error_std		= np.nan
			self.clk_residual_error		= np.nan

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_UTC_DATA message to the output stream buffer from the provided structure.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IHHbbbbbiIfff',
			self.time_stamp,
			self.status,
			self.year,
			self.month,
			self.day,
			self.hour,
			self.minute,
			self.second,
			self.nano_second,
			self.gps_time_of_week,
			self.clk_bias_std,
			self.clk_sf_error_std,
			self.clk_residual_error
		)

		buffer.extend(packed_data)

	def get_clock_state(self) -> SbgEComClockState:
		"""
		Returns the clock alignment state.

		Returns:
			SbgEComClockState:				The clock status.
		"""
		return SbgEComClockState((self.status >> CLOCK_STATE_SHIFT) & CLOCK_STATE_MASK)

	def set_clock_state(self, status: SbgEComClockState):
		"""
		Set the clock alignment state.

		Args:
			state (SbgEComClockState):		The status to set.
		"""
		assert status.value <= CLOCK_STATE_MASK

		self.status &= ~(CLOCK_STATE_MASK << CLOCK_STATE_SHIFT)
		self.status |= (status.value & CLOCK_STATE_MASK) << CLOCK_STATE_SHIFT

	def get_clock_state_as_string(self) -> str:
		"""
		Returns the clock alignment state as a string.

		Returns:
			SbgEComClockState:				The clock state as a string.
		"""
		CLOCK_STATE_STR: Dict[SbgEComClockState, str] = {
			SbgEComClockState.ERROR			: "error",
			SbgEComClockState.RUNNING		: "free",
			SbgEComClockState.STEERING		: "steering",
			SbgEComClockState.VALID			: "valid"
		}

		clock_state = self.get_clock_state()

		return CLOCK_STATE_STR.get(clock_state, "undefined")

	def get_utc_status(self) -> SbgEComUtcStatus:
		"""
		Returns the GNSS UTC time information status.

		Returns:
			SbgEComUtcStatus:				The UTC time information status.
		"""
		return SbgEComUtcStatus((self.status >> TIME_STATUS_SHIFT) & TIME_STATUS_MASK)

	def set_utc_status(self, status: SbgEComUtcStatus):
		"""
		Set the UTC time information status.

		Args:
			status (SbgEComUtcStatus):		The status to set.
		"""
		assert status.value <= TIME_STATUS_MASK

		self.status &= ~(TIME_STATUS_MASK << TIME_STATUS_SHIFT)
		self.status |= (status.value & TIME_STATUS_MASK) << TIME_STATUS_SHIFT

	def get_utc_status_as_string(self) -> str:
		"""
		Returns the GNSS UTC time information status as a string.

		Returns:
			SbgEComClockState:				The UTC time information status as a string.
		"""
		UTC_STATUS_STR: Dict[SbgEComUtcStatus, str] = {
			SbgEComUtcStatus.INVALID		: "invalid",
			SbgEComUtcStatus.NO_LEAP_SEC	: "noLeapSec",
			SbgEComUtcStatus.INITIALIZED	: "initialized"
		}

		utc_status = self.get_utc_status()

		return UTC_STATUS_STR.get(utc_status, "undefined")

	def set_has_clock_input(self, has_clock_input: bool):
		"""
		Set if a valid PPS signal is received and can be used to align the internal clock.

		Args:
			has_clock_input (bool):			True if a valid PPS signal is received.
		"""
		if has_clock_input:
			self.status |= STATUS_HAS_CLOCK_INPUT
		else:
			self.status &= ~STATUS_HAS_CLOCK_INPUT

	def has_clock_input(self) -> bool:
		"""
		Returns true if a valid and stable PPS signal is being received to align the INS clock.

		Use this status to determine if the INS is constantly correcting the clock with a PPS signal
		or propagating the clock with its internal crystal using latest estimated bias and scale factor corrections.

		During nominal INS operations with good GNSS reception, this method should return true.
		During GNSS outages, no PPS signal is received and this method should return false.

		Returns:
			bool:							True if a valid PPS signal is received and false if no PPS signal is received.
		"""
		return self.status & STATUS_HAS_CLOCK_INPUT

	def time_set_is_accurate(self, utc_is_accurate: bool):
		"""
		Set if the reported UTC time is accurate or not (ie timestamped with a PPS).

		Args:
			utc_is_accurate (bool):			True if the UTC time is accurate or false otherwise.
		"""
		if utc_is_accurate:
			self.status |= STATUS_UTC_IS_ACCURATE
		else:
			self.status &=  ~STATUS_UTC_IS_ACCURATE

	def time_is_accurate(self) -> bool:
		"""
		Returns true if the UTC time information is considered to be accurate.

		The PPS signal is used to compensate for processing and transmission delays to
		precisely timestamp the received UTC time information.

		During nominal INS operations with good GNSS reception, this method should return true.

		If the PPS signal is lost for too much time, this method should return false indicating
		a degraded UTC time information accuracy.

		Returns:
			bool:							True if the UTC time information is accurate.
		"""
		return True if self.status & STATUS_UTC_IS_ACCURATE else False

	def time_clk_bias_std_is_valid(self) -> bool:
		"""
		Returns true if the clkBiasStd field is filled and valid.

		Returns:
			bool:							True if the field is valid.
		"""
		return not math.isnan(self.clk_bias_std)

	def time_clk_bias_sf_error_std_is_valid(self) -> bool:
		"""
		Returns true if the clkSfErrorStd field is filled and valid.

		Returns:
			bool:							True if the field is valid.
		"""
		return not math.isnan(self.clk_sf_error_std)

	def time_clk_residual_error_is_valid(self) -> bool:
		"""
		Returns true if the clkResidualError field is filled and valid.

		Returns:
			bool:							True if the field is valid.
		"""
		return not math.isnan(self.clk_residual_error)

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
			f"{self.gps_time_of_week:>12}"
			f"{self.year:>12}"
			f"{self.month:>12}"
			f"{self.day:>12}"
			f"{self.hour:>12}"
			f"{self.minute:>12}"
			f"{self.second:>12}"
			f"{self.nano_second:>12}"
			f"{self.clk_bias_std * 1e6:>12.6f}"
			f"{self.clk_sf_error_std * 1e9:>12.6f}"
			f"{self.clk_residual_error * 1e6:>12.6f}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'gpsTimeOfWeek':>12}{'year':>12}{'month':>12}{'day':>12}{'hour':>12}{'minute':>12}{'second':>12}{'nanosecond':>12}{'clkBiasStd':>12}{'clkSfErrorStd':>12}{'clkResidualError':>12}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(ms)':>12}{'(yyyy)':>12}{'(mm)':>12}{'(dd)':>12}{'(hh)':>12}{'(mm)':>12}{'(s)':>12}{'(ns)':>12}{'(us)':>12}{'(ppm)':>12}{'(us)':>12}\n"
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
