# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_ptp_status.py										* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				26 March 2025													* |
# *																						* |
# * brief				Parse logs used to report device PTP status.					* |
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
from enum import Enum
import socket
import struct

# Third party
import numpy as np

# Local application
from . import SbgEComLog

STATE_SHIFT								= 0											# Shift used to extract the PTP state part.
STATE_MASK								= 0x07										# Mask used to keep only the PTP state part.

TRANSPORT_SHIFT							= 3											# Shift used to extract the PTP transport part.
TRANSPORT_MASK							= 0x07										# Mask used to keep only the PTP transport part.

TIMESCALE_SHIFT							= 8											# Shift used to extract the PTP time scale part.
TIMESCALE_MASK							= 0x07										# Mask used to keep only the PTP time scale part.

class SbgEComLogPtpState(Enum):
	"""
	PTP states.
	"""
	DISABLED							= 0											# PTP is disabled.
	FAULTY								= 1											# The device is in the faulty state.
	MASTER								= 2											# The device is the domain master.
	PASSIVE								= 3											# The device is passive.
	UNCALIBRATED						= 4											# The device is an uncalibrated slave.
	SLAVE								= 5											# The device is a slave.

class SbgEComLogPtpTransport(Enum):
	"""
	Transport.
	"""
	UDP									= 0											# UDP.
	ETHERNET							= 1											# Ethernet (IEEE 802.3).

class SbgEComLogPtpTimeScale(Enum):
	"""
	Time scales.
	"""
	TAI									= 0											# TAI.
	UTC									= 1											# UTC
	GPS									= 2											# GPS.

@dataclass
class SbgEComLogPtpStatus(SbgEComLog):
	"""
	Data Class that stores data for the SBG_ECOM_LOG_PTP_STATUS message.
	"""

	name									= "ptpStatus"							# Log name.

	"""
	Class members.
	"""
	time_stamp:							int = 0										# Timestamp, in us.
	status:								int = 0										# State, transport and internal time scale status.

	time_scale_offset:					float = 0.0									# Internal time scale offset, in seconds.

	local_clock_identity:				int = np.iinfo(np.uint64).max				# Local clock identity.
	local_clock_priority1:				int = 0										# Local clock priority1 attribute.
	local_clock_priority2:				int = 0										# Local clock priority2 attribute.
	local_clock_class:					int = 0										# Local clock class attribute.
	local_clock_accuracy:				int = 0										# Local clock accuracy.
	local_clock_log_2_variance:			int = 0										# Local clock PTP variance.
	local_clock_time_source:			int = 0										# Local clock time source.

	master_clock_identity:				int = np.iinfo(np.uint64).max				# Master clock identity.
	master_clock_priority1:				int = 0										# Master clock priority1 attribute.
	master_clock_priority2:				int = 0										# Master clock priority2 attribute.
	master_clock_class:					int = 0										# Master clock class attribute.
	master_clock_accuracy:				int = 0										# Master clock accuracy.
	master_clock_log_2_variance:		int = 0										# Master clock PTP variance.
	master_clock_time_source:			int = 0										# Master clock time source.
	master_ip_address:					int = np.iinfo(np.uint32).max				# Master clock IP address.

	mean_path_delay:					float = np.nan								# Mean path delay to/from the master clock, in seconds.
	mean_path_delay_std_dev:			float = np.nan								# Mean path delay standard deviation, in seconds.
	clock_offset:						float = np.nan								# Offset between the local and master clocks, in seconds.
	clock_offset_std_dev:				float = np.nan								# Master clock offset standard deviation, in seconds.
	clock_freq_offset:					float = np.nan								# Offset between the frequency of the local and master clocks, in Hz.
	clock_freq_offset_std_dev:			float = np.nan								# Frequency offset standard deviation, in Hz.

	master_mac_address:					np.ndarray = field(default_factory=lambda: np.zeros(6, dtype=np.uint8))	# Master clock 48 bits mac address, all field at UINT8_MAX if invalid.

	def __post_init__(self):
		self.set_state(SbgEComLogPtpState.FAULTY)
		self.set_transport(SbgEComLogPtpTransport.UDP)
		self.set_time_scale(SbgEComLogPtpTimeScale.TAI)

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_PTP_STATUS message and fill the corresponding structure.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<IHdQBBBBHBQBBBBHBIffdfff', payload[:76])
		self.time_stamp					= unpacked_data[0]
		self.status						= unpacked_data[1]

		self.time_scale_offset			= unpacked_data[2]

		self.local_clock_identity		= unpacked_data[3]
		self.local_clock_priority1		= unpacked_data[4]
		self.local_clock_priority2		= unpacked_data[5]
		self.local_clock_class			= unpacked_data[6]
		self.local_clock_accuracy		= unpacked_data[7]
		self.local_clock_log_2_variance	= unpacked_data[8]
		self.local_clock_time_source	= unpacked_data[9]

		self.master_clock_identity		= unpacked_data[10]
		self.master_clock_priority1		= unpacked_data[11]
		self.master_clock_priority2		= unpacked_data[12]
		self.master_clock_class			= unpacked_data[13]
		self.master_clock_accuracy		= unpacked_data[14]
		self.master_clock_log_2_variance = unpacked_data[15]
		self.master_clock_time_source	= unpacked_data[16]
		self.master_ip_address			= unpacked_data[17]

		self.mean_path_delay			= unpacked_data[18]
		self.mean_path_delay_std_dev	= unpacked_data[19]
		self.clock_offset				= unpacked_data[20]
		self.clock_offset_std_dev		= unpacked_data[21]
		self.clock_freq_offset			= unpacked_data[22]
		self.clock_freq_offset_std_dev	= unpacked_data[23]

		# Added in sbgECom 5.2
		if (len(payload) - 76) >= len(self.master_mac_address):
			self.master_mac_address[:] = struct.unpack('<6B', payload[76:82])
		else:
			self.master_mac_address[:] = np.iinfo(np.uint8).max

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_PTP_STATUS message to the output stream buffer from the provided structure.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IHdQBBBBHBQBBBBHBIffdfff',
			self.time_stamp,
			self.status,

			self.time_scale_offset,

			self.local_clock_identity,
			self.local_clock_priority1,
			self.local_clock_priority2,
			self.local_clock_class,
			self.local_clock_accuracy,
			self.local_clock_log_2_variance,
			self.local_clock_time_source,

			self.master_clock_identity,
			self.master_clock_priority1,
			self.master_clock_priority2,
			self.master_clock_class,
			self.master_clock_accuracy,
			self.master_clock_log_2_variance,
			self.master_clock_time_source,
			self.master_ip_address,

			self.mean_path_delay,
			self.mean_path_delay_std_dev,
			self.clock_offset,
			self.clock_offset_std_dev,
			self.clock_freq_offset,
			self.clock_freq_offset_std_dev
		)

		# Added in sbgECom 5.2
		self.mac_address_data = struct.pack('<6B', *self.master_mac_address)

		buffer.extend(packed_data)
		buffer.extend(self.mac_address_data)

	def get_state(self) -> SbgEComLogPtpState:
		"""
		Returns the PTP solution state.

		Returns:
			SbgEComLogPtpState:				The solution state.
		"""
		return SbgEComLogPtpState((self.status >> STATE_SHIFT) & STATE_MASK)

	def set_state(self, state: SbgEComLogPtpState):
		"""
		Set the PTP state.

		Args:
			state (SbgEComLogPtpState):		The solution state to set.
		"""
		assert state.value <= STATE_MASK

		self.status &= ~(STATE_MASK << STATE_SHIFT)
		self.status |= (state.value & STATE_MASK) << STATE_SHIFT

	def get_transport(self) -> SbgEComLogPtpTransport:
		"""
		Returns the PTP solution transport.

		Returns:
			SbgEComLogPtpTransport:			The solution transport.
		"""
		return SbgEComLogPtpTransport((self.status >> TRANSPORT_SHIFT) & TRANSPORT_MASK)

	def set_transport(self, transport: SbgEComLogPtpTransport):
		"""
		Set the PTP transport.

		Args:
			transport (SbgEComLogPtpTransport):	The solution transport to set.
		"""
		assert transport.value <= TRANSPORT_MASK

		self.status &= ~(TRANSPORT_MASK << TRANSPORT_SHIFT)
		self.status |= (transport.value & TRANSPORT_MASK) << TRANSPORT_SHIFT

	def get_time_scale(self) -> SbgEComLogPtpTimeScale:
		"""
		Returns the PTP solution time scale.

		Returns:
			SbgEComLogPtpTimeScale:			The solution time scale.
		"""
		return SbgEComLogPtpTimeScale((self.status >> TIMESCALE_SHIFT) & TIMESCALE_MASK)

	def set_time_scale(self, time_scale: SbgEComLogPtpTimeScale):
		"""
		Set the PTP time scale.

		Args:
			time_scale (SbgEComLogPtpTimeScale):	The solution time scale to set.
		"""
		assert time_scale.value <= TIMESCALE_MASK

		self.status &= ~(TIMESCALE_MASK << TIMESCALE_SHIFT)
		self.status |= (time_scale.value & TIMESCALE_MASK) << TIMESCALE_SHIFT

	def convert_state(self, state: SbgEComLogPtpState) -> str:
		"""
		Convert a PTP state to a string.

		Args:
			state (SbgEComLogPtpState):		State.

		Returns:
			str:							State string.
		"""
		state_str = "unknow"

		match state:
			case SbgEComLogPtpState.DISABLED:
				state_str = "disabled"
			case SbgEComLogPtpState.FAULTY:
				state_str = "faulty"
			case SbgEComLogPtpState.MASTER:
				state_str = "master"
			case SbgEComLogPtpState.PASSIVE:
				state_str = "passive"
			case SbgEComLogPtpState.UNCALIBRATED:
				state_str = "uncalibrated"
			case SbgEComLogPtpState.SLAVE:
				state_str = "slave"

		return state_str

	def convert_transport(self, transport: SbgEComLogPtpTransport) -> str:
		"""
		Convert a PTP transport to a string.

		Args:
			transport (SbgEComLogPtpTransport): Transport.

		Returns:
			str:							Transport string.
		"""
		transport_str = "unknow"

		match transport:
			case SbgEComLogPtpTransport.UDP:
				transport_str = "udp"
			case SbgEComLogPtpTransport.ETHERNET:
				transport_str = "ethernet"

		return transport_str

	def convert_time_scale(self, time_scale: SbgEComLogPtpTimeScale) -> str:
		"""
		Convert a PTP time scale to a string.

		Args:
			time_scale (SbgEComLogPtpTimeScale): Time scale.

		Returns:
			str:							Time scale string.
		"""
		time_scale_str = "unknow"

		match time_scale:
			case SbgEComLogPtpTimeScale.TAI:
				time_scale_str = "tai"
			case SbgEComLogPtpTimeScale.UTC:
				time_scale_str = "utc"
			case SbgEComLogPtpTimeScale.GPS:
				time_scale_str = "gps"

		return time_scale_str

	def convert_mac_address(self, mac_addr: np.ndarray) -> str:
		"""
		onvert a PTP MAC address to a string.

		Args:
			mac_addr (np.ndarray):			MAC address array.

		Returns:
			str:							MAC address string formatted as "XX:XX:XX:XX:XX:XX"..
		"""
		return ":".join(f"{byte:02X}" for byte in mac_addr)

	def convert_ip_address(self, ip_int: int) -> str:
		"""
		Converts an IP address stored as an integer to an IPv4 string.

		Args:
			ip_int (int):					IP address as an integer.

		Returns:
			str:							IP address as a string.
		"""
		return socket.inet_ntoa(struct.pack("!I", ip_int))

	def get_values(self, context) -> str:
		"""
		Return formatted values shared by file and console outputs.

		Args:
			context (LoggerContext):		Logger context and settings.

		Returns:
			str:							Formatted values.
		"""
		data = (
			f"{self.convert_state(self.get_state()):>12}"
			f"{self.convert_transport(self.get_transport()):>12}"
			f"{self.convert_time_scale(self.get_time_scale()):>12}"
			f"{self.time_scale_offset:>12.6f}"
			f"{self.local_clock_identity:20X}"
			f"{self.local_clock_priority1:>12}"
			f"{self.local_clock_priority2:>12}"
			f"{self.local_clock_class:>12}"
			f"{self.local_clock_accuracy:>12}"
			f"{self.local_clock_log_2_variance:>12}"
			f"{self.local_clock_time_source:>12}"
			f"{self.master_clock_identity:20X}"
			f"{self.master_clock_priority1:>12}"
			f"{self.master_clock_priority2:>12}"
			f"{self.master_clock_class:>12}"
			f"{self.master_clock_accuracy:>12}"
			f"{self.master_clock_log_2_variance:>12}"
			f"{self.master_clock_time_source:>12}"
			f"{self.convert_ip_address(self.master_ip_address):>20}"
			f"{self.mean_path_delay:>12.6f}"
			f"{self.mean_path_delay_std_dev:>12.6f}"
			f"{self.clock_offset:>12.6f}"
			f"{self.clock_offset_std_dev:>12.6f}"
			f"{self.clock_freq_offset:>12.6f}"
			f"{self.clock_freq_offset_std_dev:>12.6f}"
			f"{self.convert_mac_address(self.master_mac_address):>20}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'timeScale':>12}{'timeScaleOffset':>20}{'localClockIdentity':>25}{'localClockPriority1':>25}{'localClockPriority2':>25}{'localClockClass':>25}{'localClockAccuracy':>25}{'localClockLog2Variance':>25}{'localClockTimeSource':>25}{'masterClockIdentity':>25}{'masterClockPriority1':>25}{'masterClockPriority2':>25}{'masterClockClass':>25}{'masterClockAccuracy':>25}{'masterClockLog2Variance':>25}{'masterClockTimeSource':>25}{'masterIpAddress':>25}{'meanPathDelay':>25}{'meanPathDelayStdDev':>25}{'clockOffset':>25}{'clockOffsetStdDev':>25}{'clockFreqOffset':>25}{'clockFreqOffsetStdDev':>25}{'masterMacAddress':>25}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(na)':>12}{'(s)':>12}{'(na)':>12}{'(na)':>12}{'(na)':>12}{'(na)':>12}{'(na)':>12}{'(na)':>12}{'(na)':>12}{'(na)':>12}{'(na)':>12}{'(na)':>12}{'(na)':>12}{'(na)':>12}{'(na)':>12}{'(na)':>12}{'(na)':>12}{'(s)':>12}{'(s)':>12}{'(s)':>12}{'(s)':>12}{'(s)':>12}{'(s)':>12}{'(na)':>12}\n"
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
