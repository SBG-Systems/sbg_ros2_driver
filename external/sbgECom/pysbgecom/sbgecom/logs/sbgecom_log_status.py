# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_status.py											* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				27 March 2025													* |
# *																						* |
# * brief				Parse logs used to report device status.						* |
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
import struct

# Third party
import numpy as np

# Local application
from . import SbgEComLog

GENERAL_MAIN_POWER_OK					= 0x0001 << 0								# Set to 1 when main power supply is OK.
GENERAL_IMU_POWER_OK					= 0x0001 << 1								# Set to 1 when IMU power supply is OK.
GENERAL_GPS_POWER_OK					= 0x0001 << 2								# Set to 1 when GPS power supply is OK.
GENERAL_SETTINGS_OK						= 0x0001 << 3								# Set to 1 if settings where correctly loaded.
GENERAL_TEMPERATURE_OK					= 0x0001 << 4								# Set to 1 when temperature is within specified limits.
GENERAL_DATALOGGER_OK					= 0x0001 << 5								# Set to 1 when the datalogger is working correctly.
GENERAL_CPU_OK							= 0x0001 << 6								# Set to 1 if the CPU headroom is correct.

"""
Communication status bit mask definitions (comStatus).
"""
PORTA_VALID								= 0x00000001 << 0							# Set to 0 in case of low level communication error.
PORTB_VALID								= 0x00000001 << 1							# Set to 0 in case of low level communication error.
PORTC_VALID								= 0x00000001 << 2							# Set to 0 in case of low level communication error.
PORTD_VALID								= 0x00000001 << 3							# Set to 0 in case of low level communication error.
PORTE_VALID								= 0x00000001 << 4							# Set to 0 in case of low level communication error.

PORTA_RX_OK								= 0x00000001 << 5							# Set to 0 in case of error on PORT A input.
PORTA_TX_OK								= 0x00000001 << 6							# Set to 0 in case of error on PORT A output.
PORTB_RX_OK								= 0x00000001 << 7							# Set to 0 in case of error on PORT B input.
PORTB_TX_OK								= 0x00000001 << 8							# Set to 0 in case of error on PORT B output.
PORTC_RX_OK								= 0x00000001 << 9							# Set to 0 in case of error on PORT C input.
PORTC_TX_OK								= 0x00000001 << 10							# Set to 0 in case of error on PORT C output.
PORTD_RX_OK								= 0x00000001 << 11							# Set to 0 in case of error on PORT D input.
PORTD_TX_OK								= 0x00000001 << 12							# Set to 0 in case of error on PORT D input.
PORTE_RX_OK								= 0x00000001 << 13							# Set to 0 in case of error on PORT E input.
PORTE_TX_OK								= 0x00000001 << 14							# Set to 0 in case of error on PORT D input.

ETH0_VALID								= 0x00000001 << 15							# Set to 0 in case of error on ETH0.
ETH1_VALID								= 0x00000001 << 16							# Set to 0 in case of error on ETH1.
ETH2_VALID								= 0x00000001 << 17							# Set to 0 in case of error on ETH2.
ETH3_VALID								= 0x00000001 << 18							# Set to 0 in case of error on ETH3.
ETH4_VALID								= 0x00000001 << 19							# Set to 0 in case of error on ETH4.

CAN_VALID								= 0x00000001 << 25							# Set to 0 in case of low level communication error.
CAN_RX_OK								= 0x00000001 << 26							# Set to 0 in case of error on CAN Bus input buffer.
CAN_TX_OK								= 0x00000001 << 27							# Set to 0 in case of error on CAN Bus output buffer.

class SbgEComCanBusStatus(Enum):
	"""
	Communication status for the CAN Bus.
	"""

	OFF									= 0											# CAN bus is OFF (either not enabled or disabled due to too much errors).
	TX_RX_ERR							= 1											# CAN bus transmitter or receiver has error.
	OK									= 2											# CAN Bus is OK.
	ERROR								= 3											# CAN Bus has a general error such as busy bus.

COM2_ETH0_RX_OK							= 0x0001 << 0								# Set to 0 in case of error on ETH0 input.
COM2_ETH0_TX_OK							= 0x0001 << 1								# Set to 0 in case of error on ETH0 output.
COM2_ETH1_RX_OK							= 0x0001 << 2								# Set to 0 in case of error on ETH1 input.
COM2_ETH1_TX_OK							= 0x0001 << 3								# Set to 0 in case of error on ETH1 output.
COM2_ETH2_RX_OK							= 0x0001 << 4								# Set to 0 in case of error on ETH2 input.
COM2_ETH2_TX_OK							= 0x0001 << 5								# Set to 0 in case of error on ETH2 output.
COM2_ETH3_RX_OK							= 0x0001 << 6								# Set to 0 in case of error on ETH3 input.
COM2_ETH3_TX_OK							= 0x0001 << 7								# Set to 0 in case of error on ETH3 output.
COM2_ETH4_RX_OK							= 0x0001 << 8								# Set to 0 in case of error on ETH4 input.
COM2_ETH4_TX_OK							= 0x0001 << 9								# Set to 0 in case of error on ETH4 output.

AIDING_GPS1_POS_RECV					= 0x00000001 << 0							# Set to 1 when valid GPS 1 position data is received.
AIDING_GPS1_VEL_RECV					= 0x00000001 << 1							# Set to 1 when valid GPS 1 velocity data is received.
AIDING_GPS1_HDT_RECV					= 0x00000001 << 2							# Set to 1 when valid GPS 1 true heading data is received.
AIDING_GPS1_UTC_RECV					= 0x00000001 << 3							# Set to 1 when valid GPS 1 UTC time data is received.
AIDING_GPS2_POS_RECV					= 0x00000001 << 4							# Set to 1 when valid GPS 2 position data is received.
AIDING_GPS2_VEL_RECV					= 0x00000001 << 5							# Set to 1 when valid GPS 2 velocity data is received.
AIDING_GPS2_HDT_RECV					= 0x00000001 << 6							# Set to 1 when valid GPS 2 true heading data is received.
AIDING_GPS2_UTC_RECV					= 0x00000001 << 7							# Set to 1 when valid GPS 2 UTC time data is received.
AIDING_MAG_RECV							= 0x00000001 << 8							# Set to 1 when valid Magnetometer data is received.
AIDING_ODO_RECV							= 0x00000001 << 9							# Set to 1 when Odometer pulse is received.
AIDING_DVL_RECV							= 0x00000001 << 10							# Set to 1 when valid DVL data is received.
AIDING_USBL_RECV						= 0x00000001 << 11							# Set to 1 when valid USBL data is received.
AIDING_DEPTH_RECV						= 0x00000001 << 12							# Set to 1 when valid Depth Log data is received.
AIDING_AIR_DATA_RECV					= 0x00000001 << 13							# Set to 1 when valid Air Data (altitude and/or true airspeed) is received.
AIDING_VEL1_RECV						= 0x00000001 << 14							# Set to 1 when valid generic velocity 1 data is received.

CAN_BUS_STATUS_SHIFT					= 28										# Shift used to access the CAN status part.
CAN_BUS_STATUS_MASK						= 0x00000007								# Mask used to keep only the CAN status part.

@dataclass
class SbgEComLogStatus(SbgEComLog):
	"""
	Data Class that stores data for the SBG_ECOM_LOG_STATUS message.
	"""

	name									= "status"								# Log name.

	"""
	Class members.
	"""
	time_stamp:							int = None									# Time in us since the sensor power up.
	general_status:						int = None									# General status bitmask and enum.
	com_status:							int = 0										# Communication status bitmask and enum.
	com_status2:						int = 0										# Second communication status bitmask and enum (used since v4.0).
	aiding_status:						int = None									# Aiding equipments status bitmask and enum.
	reserved2:							int = None									# Reserved status field for future use.
	reserved3:							int = None									# Reserved status field for future use.
	uptime:								int = None									# System uptime in seconds - 0 if N/A (added v1.7).
	cpu_usage:							int = None									# Main CPU usage in percent - 0xFF if N/A (added v5.0).

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_STATUS message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<IHHIIIH', payload[:22])
		self.time_stamp					= unpacked_data[0]
		self.general_status				= unpacked_data[1]
		self.com_status2				= unpacked_data[2]
		self.com_status					= unpacked_data[3]
		self.aiding_status				= unpacked_data[4]
		self.reserved2					= unpacked_data[5]
		self.reserved3					= unpacked_data[6]

		# Device up time added in version 1.7
		if len(payload[22:]) >= 4:
			self.uptime					= struct.unpack('<I', payload[22:26])[0]
		else:
			self.uptime					= 0

		# CPU usage added in version 5.0
		if len(payload[26:]) >= 1:
			self.cpu_usage				= payload[26]
		else:
			self.cpu_usage				= np.iinfo(np.uint8).max

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_STATUS message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IHHIIIHIB',
			self.time_stamp,
			self.general_status,
			self.com_status2,
			self.com_status,
			self.aiding_status,
			self.reserved2,
			self.reserved3,
			self.uptime,
			self.cpu_usage
		)

		buffer.extend(packed_data)

	def get_can_bus_status(self) -> SbgEComCanBusStatus:
		"""
		Returns the CAN bus status.

		Returns:
			SbgEComCanBusStatus:			SbgEComCanBusStatus
		"""
		return SbgEComCanBusStatus((self.com_status >> CAN_BUS_STATUS_SHIFT) & CAN_BUS_STATUS_MASK)

	def set_can_bus_status(self, status: SbgEComCanBusStatus):
		"""
		Defines the CAN bus status.

		Args:
			status (SbgEComCanBusStatus):	CAN bus status to set.
		"""
		assert status.value <= CAN_BUS_STATUS_MASK

		self.com_status &= ~(CAN_BUS_STATUS_MASK << CAN_BUS_STATUS_SHIFT)
		self.com_status2 |= (status.value & CAN_BUS_STATUS_MASK) << CAN_BUS_STATUS_SHIFT

	def is_cpu_usage_available(self) -> bool:
		"""
		Returns true if the CPU usage in percent is available.

		Returns:
			bool:							True if available.
		"""
		if self.cpu_usage != np.iinfo(np.uint8).max:
			return True
		else:
			return False

	def get_values(self, context) -> str:
		"""
		Return formatted values shared by file and console outputs.

		Args:
			context (LoggerContext):		Logger context and settings.

		Returns:
			str:							Formatted values.
		"""
		data = (
			f"{context.fmt_status(self.general_status):>12}"
			f"{context.fmt_status(self.com_status):>12}"
			f"{context.fmt_status(self.com_status2):>12}"
			f"{context.fmt_status(self.aiding_status):>12}"
			f"{self.cpu_usage:>12}"
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
				f"{context.get_time_col_title():>12}{'general':>8}{'com':>12}{'com2':>12}{'aiding':>12}{'cpuUsage':>12}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(na)':>12}{'(na)':>12}{'(na)':>12}{'(%%)':>12}\n"
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
