# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_gnss_hdt.py											* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				25 March 2025													* |
# *																						* |
# * brief				GNSS True Heading related logs									* |
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
import struct

# Third party
import numpy as np

# Local application
from . import SbgEComLog
from sbgecom.sbgecom_ids import SbgEComLogId

class SbgEComGnssHdtStatus(Enum):
	"""
	GNSS HDT status definitions.
	"""

	SOL_COMPUTED						= 0											# A valid solution has been computed.
	INSUFFICIENT_OBS					= 1											# Not enough valid SV to compute a solution.
	INTERNAL_ERROR						= 2											# An internal error has occurred.
	HEIGHT_LIMIT						= 3											# The height limit has been exceeded.

STATUS_SHIFT							= 0											# Shift used to extract the GNSS HDT status part.
STATUS_MASK								= 0x003F									# Mask used to keep only the GNSS HDT status part.

BASELINE_VALID							= 0x0001 << 6								# Set to 1 if the baseline length field is filled and valid.

@dataclass
class SbgEComLogGnssHdt(SbgEComLog):
	"""
	Data class that stores data for the SBG_ECOM_LOG_GPS#_HDT message.
	"""

	"""
	Class members.
	"""
	time_stamp:							int		= 0									# Time in us since the sensor power up.
	status:								int		= 0									# GPS position status, type and bitmask.
	time_of_week:						int		= 0									# GPS time of week in ms.
	heading:							float	= 0									# GPS true heading in degrees (0 to 360).
	heading_accuracy:					float	= 180.0								# 1 sigma GPS true heading accuracy in degrees (0 to 180).
	pitch:								float	= 0									# GPS pitch angle measured from the master to the rover in degrees (-90 to +90).
	pitch_accuracy:						float	= 90.0								# 1 sigma GPS pitch angle accuracy in degrees (0 to 90).
	baseline:							float	= 0									# The distance between the main and aux antenna in meters (added in 2.0)
	num_sv_tracked:						int		= np.iinfo(np.uint8).max			# Number of space vehicles tracked by the GNSS - set to 0xFF if not available.
	num_sv_used:						int		= np.iinfo(np.uint8).max			# Number of space vehicles used to compute the solution - set to 0xFF if not available.

	def __post_init__(self):
		self.set_status(SbgEComGnssHdtStatus.INSUFFICIENT_OBS)

	# To identifie either GNSS1Hdt or GNSS2Hdt.
	log_id: int = field(default=None, init=False, repr=False)

	def set_log_id(self, log_id: int):
		"""
		Method to set the log_id after initialization.

		Args:
			log_id (int):					The GNSS number.
		"""
		self.log_id = log_id

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_GPS#_POS message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<IHIffff', payload[:26])
		self.time_stamp					= unpacked_data[0]
		self.status						= unpacked_data[1]
		self.time_of_week				= unpacked_data[2]
		self.heading					= unpacked_data[3]
		self.heading_accuracy			= unpacked_data[4]
		self.pitch						= unpacked_data[5]
		self.pitch_accuracy				= unpacked_data[6]

		# The baseline field have been added in version 2.0
		if len(payload) >= 30:
			self.baseline	= struct.unpack('<f', payload[26:30])[0]
		else:
			self.baseline	= 0.0

		# Read numSvTracked and numSvUsed added in version 4.0
		if len(payload) >= 31:
			self.num_sv_tracked	= struct.unpack('<B', payload[30:31])[0]
		else:
			self.num_sv_tracked	= np.iinfo(np.uint8).max

		if len(payload) >= 32:
			self.num_sv_used	= struct.unpack('<B', payload[31:32])[0]
		else:
			self.num_sv_used	= np.iinfo(np.uint8).max

	def write(self, buffer: bytearray):
		"""
		Write data for the SbgEComLogGnssHdt message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IHIfffffBB',
			self.time_stamp,
			self.status,
			self.time_of_week,
			self.heading,
			self.heading_accuracy,
			self.pitch,
			self.pitch_accuracy,
			self.baseline,
			self.num_sv_tracked,
			self.num_sv_used
		)

		buffer.extend(packed_data)

	def get_status(self) -> SbgEComGnssHdtStatus:
		"""
		Returns the true heading solution status.

		Returns:
			SbgEComGnssPosStatus:			The solution status.
		"""
		return (self.status >> STATUS_SHIFT) & STATUS_MASK

	def set_status(self, status: SbgEComGnssHdtStatus):
		"""
		Set the true heading solution status.

		Args:
			status (SbgEComGnssHdtStatus):	The solution status to set.
		"""
		assert status.value <= STATUS_MASK

		self.status &= ~(STATUS_MASK << STATUS_SHIFT)
		self.status |= (status.value&STATUS_MASK) << STATUS_SHIFT

	def heading_is_valid(self) -> bool:
		"""
		Returns true if the heading information is valid.

		Returns:
			bool:							True if the heading field is valid or false otherwise.
		"""
		if (self.heading_accuracy > 0.0) and (self.heading_accuracy < 180.0):
			return True
		else:
			return False

	def pitch_is_valid(self) -> bool:
		"""
		Returns true if the pitch information is valid.

		Some GNSS receivers don't provide pitch information between antenna 1 and 2.
		This methods checks if a valid pitch measurement is available with an associated standard deviation.

		Returns:
			bool:							True if the pitch field is valid or false otherwise.
		"""
		if (self.pitch_accuracy > 0.0) and (self.pitch_accuracy < 90.0):
			return True
		else:
			return False

	def set_baseline_valid(self, is_valid: bool):
		"""
		Set if the baseline field is provided with a valid value.

		Some GNSS receivers don't provide the baseline information between antenna 1 and 2.
		Set this flag to false to report this.

		Args:
			is_valid (bool):				True if the baseline is valid.
		"""
		if is_valid:
			self.status |= BASELINE_VALID
		else:
			self.status &= ~BASELINE_VALID

	def baseline_is_valid(self) -> bool:
		"""
		Returns true if the baseline field is valid.

		Returns:
			bool:							True if the baseline field is valid or false otherwise.
		"""
		if (self.status & BASELINE_VALID) and (self.baseline > 0.0):
			return True
		else:
			return False

	def num_sv_tracked_is_valid(self) -> bool:
		"""
		Returns true if the num_sv_tracked field is valid.

		Returns:
			bool:							True if the num_sv_tracked field is valid or false otherwise.
		"""
		if self.num_sv_tracked == np.iinfo(np.uint8).max:
			return False
		else:
			return True

	def num_sv_used_is_valid(self) -> bool:
		"""
		Returns true if the num_sv_used field is valid.

		Returns:
			bool:							True if the num_sv_used field is valid or false otherwise.
		"""
		if self.num_sv_used == np.iinfo(np.uint8).max:
			return False
		else:
			return True

	def get_name(self) -> str:
		"""
		Returns the log name.

		Returns:
			str:							Log name.
		"""
		match self.log_id:
			case SbgEComLogId.GPS1_HDT.value:
				return "gnss1Hdt"

			case SbgEComLogId.GPS2_HDT.value:
				return "gnss2Hdt"

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
			f"{self.time_of_week:>12}"
			f"{self.heading:>12.6f}"
			f"{self.heading_accuracy:>12.6f}"
			f"{self.pitch:>12.6f}"
			f"{self.pitch_accuracy:>12.6f}"
			f"{self.baseline:>12.6f}"
			f"{self.num_sv_tracked:>12}"
			f"{self.num_sv_used:>12}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'gpsTow':>12}{'heading':>12}{'headingStd':>12}{'pitch':>12}{'pitchStd':>12}{'baseline':>12}{'numSvTracked':>12}{'numSvUsed':>12}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(ms)':>12}{'(deg)':>12}{'(deg)':>12}{'(deg)':>12}{'(deg)':>12}{'(m)':>12}{'(na)':>12}{'(na)':>12}\n"
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
