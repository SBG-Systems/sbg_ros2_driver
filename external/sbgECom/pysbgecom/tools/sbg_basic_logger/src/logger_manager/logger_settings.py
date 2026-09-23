# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				logger_settings.py												* |
# * ingroup				logger_manager													* |
# * author				SBG Systems														* |
# * date				26 February 2025												* |
# *																						* |
# * brief				Basic logger implementation.									* |
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
from enum import Enum

# Local application
from sbgecom.common.sbgecom_logging import logger

class Serial:
	"""
	Settings for a serial interface.
	"""
	location			= ""				# Serial port location.
	baudrate			= 0					# Serial baud rate.

class Udp:
	"""
	Settings for an UDP interface.
	"""
	remote_ip			= ""				# Device IPv4 address to connect to ("a.b.c.d").
	remote_port			= 0					# Device port number to send data to.
	listen_port			= 0					# Computer listen port on which we should receive data.

class LoggerSettings:
	"""
	General logger settings as set by the CLI
	"""

	class TimeMode(Enum):
		"""
		Defines time field output format.
		"""
		time_stamp			= object()		# Use the internal timestamp value in us.
		utc_iso_8601		= object()		# Use an absolute UTC time using ISO 8601 format.

	class StatusFormat(Enum):
		"""
		Defines status field output format.
		"""
		decimal				= object()		# Output status using a decimal format.
		hexadecimal			= object()		# Output status using an hexadecimal format (ie 0x0000AABB).

	class InterfaceMode(Enum):
		"""
		Defines the interface mode to use.
		"""
		unknown				= object()		# Unknown / undefined logger interface type.
		serial				= object()		# Logger is setup to read data from a serial interface.
		udp					= object()		# Logger is setup to read data from an Ethernet UDP interface.
		file				= object()		# Logger is setup to read data from a file.

	def __init__(self):
		self.base_path						= ""									# Base path to output all files to.
		self.write_to_console				= False									# Set to true to output the received logs to the console.
		self.write_to_file					= False									# Set to true to output the receive logs to the file.
		self.write_header_to_file			= False									# Set to true to write the header to the file.
		self.file_decimation				= 1										# File decimation factor >= 1.
		self.console_decimation				= 1										# Console decimation factor >= 1.
		self.discard_invalid_time			= False									# If set to true, don't output data with an invalid time.
		self.time_mode						= self.TimeMode.time_stamp				# Define how to output time information in files.
		self.status_format					= self.StatusFormat.hexadecimal			# Define the status output format.

		"""
		TODO: Might be reworked with a proper InterfaceSettings base class and so on.
		"""
		self.interface_mode					= self.InterfaceMode.unknown			# Define the interface mode to use.
		self.file_conf						= ""									# File interface configuration (ie file name).
		self.serial_conf					= None									# Serial interface configuration.
		self.udp_conf						= None									# UDP interface configuration.

	@property
	def m_base_path(self) -> str:
		"""
		Returns the base path.

		Returns:
			str:							The base path ending with '\'.
		"""
		return self.base_path

	@m_base_path.setter
	def m_base_path(self, base_path: str):
		"""
		Define the base path to store all logs to.

		Args:
			base_path (str):				The base path to store logs to.
		"""
		self.base_path = base_path

	@property
	def m_write_to_console(self) -> bool:
		"""
		Returns if the log should be written to the console.

		Returns:
			bool:							True if logs should be written to the console.
		"""
		return self.write_to_console

	@m_write_to_console.setter
	def m_write_to_console(self, write_to_console: bool):
		"""
		Set if logs should be written to the console.

		Args:
			write_to_console (bool):		True to write logs to console.
		"""
		self.write_to_console = write_to_console

	@property
	def m_write_to_file(self) -> bool:
		"""
		Returns if the log should be written to files.

		Returns:
			bool:							True if logs should be written to file.
		"""
		return self.write_to_file

	@m_write_to_file.setter
	def m_write_to_file(self, write_to_file: bool):
		"""
		Set if logs should be written to files.

		Args:
			write_to_file (bool):			True to write logs to file.
		"""
		self.write_to_file = write_to_file

	@property
	def m_write_header_to_file(self) -> bool:
		"""
		Returns if headers should be written to file.

		Returns:
			bool:							True if headers should be written to file or false otherwise.
		"""
		return self.write_header_to_file

	@m_write_header_to_file.setter
	def m_write_header_to_file(self, write_header_to_file: bool):
		"""
		Set if the headers should be written to file.

		Args:
			write_header_to_file (bool):	True to write headers to file or false otherwise.
		"""
		self.write_header_to_file = write_header_to_file

	@property
	def m_file_decimation(self) -> int:
		"""
		Returns the decimation factor for the file output.

		Returns:
			int:							The decimation factor.
		"""
		return self.file_decimation

	@m_file_decimation.setter
	def m_file_decimation(self, file_decimation: int):
		"""
		Set the decimation factor for the file output.

		Args:
			file_decimation (int):			Decimation factor from 1 to 10 000.
		"""
		if 0 < file_decimation < 10000:
			self.file_decimation = file_decimation
		else:
			raise ValueError("file decimation should be within 0 to 10000")

	@property
	def m_console_decimation(self) -> int:
		"""
		Returns the decimation factor for the console output.

		Returns:
			int:							The decimation factor.
		"""
		return self.console_decimation

	@m_console_decimation.setter
	def m_console_decimation(self, console_decimation: int):
		"""
		Set the decimation factor for the console output.

		Args:
			console_decimation (int):		Decimation factor from 1 to 10 000.
		"""
		if 0 < console_decimation < 10000:
			self.console_decimation = console_decimation
		else:
			raise ValueError("console decimation should be within 0 to 10000")

	@property
	def m_discard_invalid_time(self) -> bool:
		"""
		Returns true if data with invalid time should be discarded or false otherwise.

		Returns:
			bool:							True if data with invalid time should be discarded.
		"""
		return self.discard_invalid_time

	@m_discard_invalid_time.setter
	def m_discard_invalid_time(self, discard_invalid_time: bool):
		"""
		Set if data with invalid time information should be discarded.

		Args:
			discard_invalid_time (bool):	True to discard data with an invalid time information.
		"""
		self.discard_invalid_time = discard_invalid_time

	@property
	def m_time_mode(self) -> TimeMode:
		"""
		Returns the time mode to use.

		Returns:
			TimeMode:						The time mode to use.
		"""
		return self.time_mode

	@m_time_mode.setter
	def m_time_mode(self, time_mode: TimeMode):
		"""
		Set the time mode/format to use.

		Args:
			time_mode (TimeMode):			The time mode to use.
		"""
		self.time_mode = time_mode

	@property
	def m_status_format(self) -> StatusFormat:
		"""
		Returns the status output format.

		Returns:
			StatusFormat):					The status output format.
		"""
		return self.status_format

	@m_status_format.setter
	def m_status_format(self, status_format: StatusFormat):
		"""
		Set the status output format.

		Args:
			status_format (StatusFormat):	The status output format.
		"""
		self.status_format = status_format

	def is_output_conf_valid(self) -> bool:
		"""
		Returns true if a valid output configuration is set.

		A valid output configuration is either the console or the file or both.

		Returns:
			bool:							True if there is a valid output configuration.
		"""
		if self.write_to_file or self.write_to_console:
			return True
		else:
			return False

	@property
	def m_interface_mode(self) -> InterfaceMode:
		"""
		Returns the configured interface mode.

		Returns:
			InterfaceMode:					The interface mode that is setup.
		"""
		return self.interface_mode

	def has_interface_conf(self) -> bool:
		"""
		Returns true if a valid serial or UDP or file interface is configured.

		Returns:
			bool:							True if there is a valid interface configuration.
		"""
		return self.interface_mode != self.InterfaceMode.unknown

	@property
	def m_serial_conf(self) -> Serial:
		"""
		Returns the serial configuration if any.

		Returns:
			Serial:							The serial configuration if any.
		"""
		if self.interface_mode != self.InterfaceMode.serial:
			logger.error("no valid serial conf set.")

		return self.serial_conf

	@m_serial_conf.setter
	def m_serial_conf(self, serial_conf: Serial):
		"""
		Define the configuration for a serial interface.

		Args:
			serial_conf (Serial):			The serial configuration to set.
		"""
		if self.has_interface_conf():
			raise ValueError("serial conf overrides a previous interface conf.")

		self.serial_conf = serial_conf
		self.interface_mode = self.InterfaceMode.serial

	@property
	def m_udp_conf(self) -> Udp:
		"""
		Returns the UDP configuration if any.

		Returns:
			Udp:							The UDP configuration if any.
		"""
		if self.interface_mode != self.InterfaceMode.udp:
			raise ValueError("no valid UDP conf set.")

		return self.udp_conf

	@m_udp_conf.setter
	def m_udp_conf(self, udp_conf: Udp):
		"""
		Define the configuration for an UDP interface.

		Args:
			udp_conf (Udp):					The UDP configuration to set.
		"""
		if self.has_interface_conf():
			raise ValueError("udp conf overrides a previous interface conf.")

		self.udp_conf = udp_conf
		self.interface_mode = self.InterfaceMode.udp

	@property
	def m_file_conf(self) -> str:
		"""
		Returns the file configuration if any.

		Returns:
			str:							The file configuration if any.
		"""
		if self.interface_mode != self.InterfaceMode.file:
			raise ValueError("no valid file conf set.")

		return self.file_conf

	@m_file_conf.setter
	def m_file_conf(self, file_conf: str):
		"""
		Define the configuration for a file interface.

		Args:
			file_conf (str):				The file configuration to set (ie file path to open).
		"""
		if self.has_interface_conf():
			raise ValueError("file conf overrides a previous interface conf.")

		self.file_conf = file_conf
		self.interface_mode = self.InterfaceMode.file

	def is_valid(self) -> bool:
		"""
		Returns true if the settings are valid and can be used.

		Returns:
			bool:							True if settings are valid.
		"""
		return self.is_output_conf_valid() and self.has_interface_conf()
