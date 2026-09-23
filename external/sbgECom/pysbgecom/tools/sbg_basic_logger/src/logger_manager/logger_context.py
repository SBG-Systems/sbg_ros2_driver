# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				logger_context.py												* |
# * ingroup				logger_manager													* |
# * author				SBG Systems														* |
# * date				26 February 2025												* |
# *																						* |
# * brief				Define the logger context such as current UTC time.				* |
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
import datetime

# Local application
from tools.sbg_basic_logger.src.logger_manager.logger_settings import LoggerSettings
from sbgecom.logs.sbgecom_log_utc import SbgEComLogUtc, SbgEComUtcStatus

class LoggerContext:
	"""
	Logger shared context between all log handlers.

	Used to share logger settings and convert timestamp to absolute time.
	"""

	def __init__(self, settings: LoggerSettings):
		"""
		Default constructor.

		Args:
			settings (LoggerSettings):		Logger settings to set.
		"""
		self.last_utc_time:					SbgEComLogUtc							# Last received UTC time if any.
		self.settings						= settings								# Logger settings.
		self.utc_time_is_valid				= False									# Set to true as soon as one valid UTC time has been received. It never goes back to false.

	@property
	def m_settings(self) -> LoggerSettings:
		"""
		Returns the logger settings instance.

		Returns:
			LoggerSettings:					Logger settings.
		"""
		return self.settings

	def set_utc_time_unit(self, utc_time: SbgEComLogUtc):
		"""
		Update the UTC time with a newly received information.

		Args:
			utc_time (SbgEComLogUtc):		New UTC time information.
		"""
		self.last_utc_time = utc_time

		if self.last_utc_time.get_utc_status() != SbgEComUtcStatus.INVALID.value:
			self.utc_time_is_valid = True

	def is_utc_time_valid(self) -> bool:
		"""
		Returns true if at least one valid UTC time information has been set.

		Returns:
			bool:							True if a valid UTC time information is available.
		"""
		return self.utc_time_is_valid

	def get_time_col_title(self) -> str:
		"""
		Returns for the selected time mode a header string.

		Returns:
			str:							Time header.
		"""
		if self.settings.m_time_mode == LoggerSettings.TimeMode.utc_iso_8601:
			return "iso8601Time"
		else:
			return "timestamp"

	def get_time_unit(self) -> str:
		"""
		Returns for the selected time mode a header string.

		Returns:
			str:							Time header.
		"""
		if self.settings.m_time_mode == LoggerSettings.TimeMode.utc_iso_8601:
			return "(yyyy-mm-ddThh:mm:ss.ssssssZ)"
		else:
			return "(us)"

	def fmt_time(self, time_stamp_us: int) -> str:
		"""
		Convert the INS timestamp in us to a ISO 8601 time.

		Args:
			time_stamp_us (str):			Timestamp in us to convert.

		Returns:
			str:							Corresponding time using ISO 8601 format.
		"""
		if self.settings.m_time_mode == LoggerSettings.TimeMode.utc_iso_8601 and self.is_utc_time_valid():
			utc_time_tm = datetime.datetime(
				self.last_utc_time.year, self.last_utc_time.month, self.last_utc_time.day,
				self.last_utc_time.hour, self.last_utc_time.minute, self.last_utc_time.second,
				tzinfo=datetime.timezone.utc
			)
			utc_time_us = int(utc_time_tm.timestamp() * 1_000_000)
			utc_time_stamp_us = self.last_utc_time.time_stamp - (self.last_utc_time.nano_second // 1_000)
			elapsed_time_us = time_stamp_us - utc_time_stamp_us

			utc_time_us += elapsed_time_us

			utc_seconds = utc_time_us // 1_000_000
			remaining_us = utc_time_us % 1_000_000

			adjusted_utc_time = datetime.datetime.fromtimestamp(utc_seconds, tz=datetime.timezone.utc)

			full_time_str = adjusted_utc_time.strftime("%Y-%m-%dT%H:%M:%S") + f".{remaining_us // 1000:03d}Z"

			return full_time_str

		return str(time_stamp_us)

	def fmt_status(self, value: int, width=None) -> str:
		"""
		Convert a status unsigned integer to a string according to selected output format.

		Status can be either output using decimal or hexadecimal format.

		Args:
			value (int):					The status unsigned value to convert.
			width (int):					Optional number of digits to use for hexadecimal format.

		Returns:
			str:							Status formatted as a string.
		"""
		status = ""

		if width is None:
			width = len(str(abs(value))) * 2

		width = min(width, 8)

		if self.settings.m_status_format == LoggerSettings.StatusFormat.hexadecimal:

			status = f"0x{value:0{width}X}"
		else:
			status = str(value)

		return status
