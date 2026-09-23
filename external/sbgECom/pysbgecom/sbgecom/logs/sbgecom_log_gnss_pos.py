# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_gnss_pos.py											* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				GNSS position related logs.										* |
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
import math
import numpy as np

# Local application
from . import SbgEComLog
from sbgecom.sbgecom_ids import SbgEComLogId

"""
GNSS signals definitions.
"""
GPS_L1_USED								= (0x00000001 << 12)						# Set to 1 if GPS L1CA/L1P is used in solution.
GPS_L2_USED								= (0x00000001 << 13)						# Set to 1 if GPS L2P/L2C is used in solution.
GPS_L5_USED								= (0x00000001 << 14)						# Set to 1 if GPS L5 is used in solution.

GLO_L1_USED								= (0x00000001 << 15)						# Set to 1 if GLONASS L1CA is used in solution.
GLO_L2_USED								= (0x00000001 << 16)						# Set to 1 if GLONASS L2C/L2P is used in solution.
GLO_L3_USED								= (0x00000001 << 17)						# Set to 1 if GLONASS L3 is used in solution.

GAL_E1_USED								= (0x00000001 << 18)						# Set to 1 if Galileo E1 is used in solution.
GAL_E5A_USED							= (0x00000001 << 19)						# Set to 1 if Galileo E5a is used in solution.
GAL_E5B_USED							= (0x00000001 << 20)						# Set to 1 if Galileo E5b is used in solution.
GAL_E5ALT_USED							= (0x00000001 << 21)						# Set to 1 if Galileo E5 AltBoc is used in solution.
GAL_E6_USED								= (0x00000001 << 22)						# Set to 1 if Galileo E6 is used in solution.

BDS_B1_USED								= (0x00000001 << 23)						# Set to 1 if BeiDou B1 is used in solution.
BDS_B2_USED								= (0x00000001 << 24)						# Set to 1 if BeiDou B2 is used in solution.
BDS_B3_USED								= (0x00000001 << 25)						# Set to 1 if BeiDou B3 is used in solution.

QZSS_L1_USED							= (0x00000001 << 26)						# Set to 1 if QZSS L1CA is used in solution.
QZSS_L2_USED							= (0x00000001 << 27)						# Set to 1 if QZSS L2C is used in solution.
QZSS_L5_USED							= (0x00000001 << 28)						# Set to 1 if QZSS L5 is used in solution.
QZSS_L6_USED							= (0x00000001 << 29)						# Set to 1 if QZSS L6 is used in solution.

class SbgEComGnssPosStatus(Enum):
	"""
	GNSS position status definitions.
	"""

	SOL_COMPUTED						= 0											# A valid solution has been computed.
	INSUFFICIENT_OBS					= 1											# Not enough valid SV to compute a solution.
	INTERNAL_ERROR						= 2											# An internal error has occurred.
	HEIGHT_LIMIT						= 3											# The height limit has been exceeded.

class SbgEComGnssPosType(Enum):
	"""
	GNSS position types definitions.
	"""

	NO_SOLUTION							= 0											# No valid solution available.
	UNKNOWN								= 1											# An unknown solution type has been computed.
	SINGLE								= 2											# Single point solution position.
	PSRDIFF								= 3											# Standard Pseudorange Differential Solution (DGPS).
	SBAS								= 4											# SBAS satellite used for differential corrections.
	OMNISTAR							= 5											# Omnistar VBS Position (L1 sub-meter).
	RTK_FLOAT							= 6											# Floating RTK ambiguity solution (20 cms RTK).
	RTK_INT								= 7											# Integer RTK ambiguity solution (2 cms RTK).
	PPP_FLOAT							= 8											# Precise Point Positioning with float ambiguities.
	PPP_INT								= 9											# Precise Point Positioning with fixed ambiguities.
	FIXED								= 10										# Fixed location solution position.

class SbgEComGnssIfmStatus(Enum):
	"""
	GNSS interference monitoring and mitigation indicator.
	"""

	ERROR								= 0											# Interference monitoring system is in error and doesn't work.
	UNKNOWN								= 1											# Interference monitoring is either disabled or not available.
	CLEAN								= 2											# Interference are monitored and the environment is clean.
	MITIGATED							= 3											# Interference are detected and mitigated, the PVT is OK.
	CRITICAL							= 4											# Interference are detected and couldn't be mitigated, the PVT is invalid.

class SbgEComGnssSpoofingStatus(Enum):
	"""
	GNSS spoofing monitoring and mitigation indicator.
	"""

	ERROR								= 0											# Spoofing detection system is in error and doesn't work.
	UNKNOWN								= 1											# Spoofing detection is either disabled or not available.
	CLEAN								= 2											# Spoofing detection is enabled and not spoofer is detected.
	SINGLE								= 3											# Probable spoofing identified by one method only.
	MULTIPLE							= 4											# Confirmed spoofing identified by several methods such as Galileo OSNMA + Built In.

class SbgEComGnssOsnmaStatus(Enum):
	"""
	GNSS Galileo OSNMA Status.
	"""

	ERROR								= 0											# OSNMA is in error state and doesn't work.
	DISABLED							= 1											# OSNMA is either disabled or not available.
	INITIALIZING						= 2											# OSNMA initialization in progress.
	WAITING_NTP							= 3											# OSNMA strict is used and is waiting to receive valid time from NTP server.
	VALID								= 4											# OSNMA is actively authenticating Galileo signals and not spoofing is detected.
	SPOOFED								= 5											# OSNMA has detected spoofing on Galileo signals.

"""
Private definitions for status field.
"""
STATUS_SHIFT							= (0)										# Shift used to extract the GNSS position status part.
STATUS_MASK								= (0x0000003F)								# Mask used to keep only the GNSS position status part.

TYPE_SHIFT								= (6)										# Shift used to extract the GNSS position type part.
TYPE_MASK								= (0x0000003F)								# Mask used to keep only the GNSS position type part.

SIGNALS_MASK							= (0xFFFFF000)								# Mask used to keep only the signals used in solution part.

"""
Private definitions for statusExt field.
"""
IFM_SHIFT								= (0)										# Shift used to extract the GNSS interference monitoring and mitigation indicator.
IFM_MASK								= (0x0000000F)								# Mask used to keep only the GNSS interference monitoring and mitigation indicator.

SPOOFING_SHIFT							= (4)										# Shift used to extract the GNSS spoofing indicator.
SPOOFING_MASK							= (0x0000000F)								# Mask used to keep only the GNSS spoofing indicator.

OSNMA_SHIFT								= (8)										# Shift used to extract the Galileo OSNMA status.
OSNMA_MASK								= (0x0000000F)								# Mask used to keep only the Galileo OSNMA status.

@dataclass
class SbgEComLogGnssPos(SbgEComLog):
	"""
	Data class that stores data for the SBG_ECOM_LOG_GPS#_POS message.
	"""

	"""
	Class members.
	"""
	time_stamp:							int		= 0									# Time in us since the sensor power up.
	status:								int		= 0									# GPS position status, type and bitmask.
	time_of_week:						int		= 0									# GPS time of week in ms.
	latitude:							float	= 0									# Latitude in degrees, positive north.
	longitude:							float	= 0									# Longitude in degrees, positive east.
	altitude:							float	= 0									# Altitude above Mean Sea Level in meters.
	undulation:							float	= 0									# Altitude difference between the geoid and the Ellipsoid in meters (Height above Ellipsoid = altitude + undulation).
	latitude_accuracy:					float	= 9999.0							# 1 sigma latitude accuracy in meters (0 to 9999).
	longitude_accuracy:					float	= 9999.0							# 1 sigma longitude accuracy in meters (0 to 9999).
	altitude_accuracy:					float	= 9999.0							# 1 sigma altitude accuracy in meters (0 to 9999).
	num_sv_used:						int		= np.iinfo(np.uint8).max			# Number of space vehicles used to compute the solution - set to 0xFF if not available.
	base_station_id:					int		= np.iinfo(np.uint16).max			# Base station id for differential corrections (0-4095) - set to 0xFFFF if differential are not used or not available.
	differential_age:					int		= np.iinfo(np.uint16).max			# Differential correction age in 0.01 seconds - set to 0xFFFF if differential are not used or not available.
	num_sv_tracked:						int		= np.iinfo(np.uint8).max			# Number of space vehicles tracked by the GNSS - set to 0xFF if not available.
	status_ext:							int		= 0									# Additional status for interference, spoofing and OSNMA.

	def __post_init__(self):
		self.set_status(SbgEComGnssPosStatus.INSUFFICIENT_OBS)
		self.set_type(SbgEComGnssPosType.NO_SOLUTION)
		self.set_ifm_status(SbgEComGnssIfmStatus.UNKNOWN)
		self.set_spoofing_status(SbgEComGnssSpoofingStatus.UNKNOWN)
		self.set_osnma_status(SbgEComGnssOsnmaStatus.DISABLED)

	# To identifie either GNSS1Pos or GNSS2Pos.
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
		should_continue = False

		unpacked_data					= struct.unpack('<IIIdddffff', payload[:52])
		self.time_stamp					= unpacked_data[0]
		self.status						= unpacked_data[1]
		self.time_of_week				= unpacked_data[2]
		self.latitude					= unpacked_data[3]
		self.longitude					= unpacked_data[4]
		self.altitude					= unpacked_data[5]
		self.undulation					= unpacked_data[6]
		self.latitude_accuracy			= unpacked_data[7]
		self.longitude_accuracy			= unpacked_data[8]
		self.altitude_accuracy			= unpacked_data[9]

		# Read additional fields added in version 1.4
		if (len(payload) - 52) >= 5:
			unpacked_data				= struct.unpack('<BHH', payload[52:57])
			self.num_sv_used			= unpacked_data[0]
			self.base_station_id		= unpacked_data[1]
			self.differential_age		= unpacked_data[2]
			should_continue				= True
		else:
			self.num_sv_used			= np.iinfo(np.uint8).max
			self.base_station_id		= np.iinfo(np.uint16).max
			self.differential_age		= np.iinfo(np.uint16).max
			should_continue				= False

		# Read additional status added in version 4.0
		if should_continue and (len(payload) - 57) >= 5:
			unpacked_data				= struct.unpack('<BI', payload[57:62])
			self.num_sv_tracked			= unpacked_data[0]
			self.status_ext				= unpacked_data[1]
		else:
			self.num_sv_tracked			= np.iinfo(np.uint8).max
			self.status_ext				= 0

			self.set_ifm_status(SbgEComGnssIfmStatus.UNKNOWN)
			self.set_spoofing_status(SbgEComGnssSpoofingStatus.UNKNOWN)
			self.set_osnma_status(SbgEComGnssOsnmaStatus.DISABLED)

	def write(self, buffer:bytearray):
		"""
		Write data for the SbgEComLogGnssPos message to the output stream buffer from the provided object

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IIIdddffffBHHBI',
			self.time_stamp,
			self.status,
			self.time_of_week,
			self.latitude,
			self.longitude,
			self.altitude,
			self.undulation,
			self.latitude_accuracy,
			self.longitude_accuracy,
			self.altitude_accuracy,
			self.num_sv_used,
			self.base_station_id,
			self.differential_age,
			self.num_sv_tracked,
			self.status_ext
		)

		buffer.extend(packed_data)

	def get_status(self) -> SbgEComGnssPosStatus:
		"""
		Returns the GNSS position solution status.

		Returns:
			SbgEComGnssPosStatus:			The solution status.
		"""
		return (self.status >> STATUS_SHIFT)&STATUS_MASK

	def set_status(self, status: SbgEComGnssPosStatus):
		"""
		Set the GNSS position solution status.

		Args:
			status (SbgEComGnssPosStatus):	The solution status to set.
		"""
		assert status.value <= STATUS_MASK

		self.status &= ~(STATUS_MASK << STATUS_SHIFT)
		self.status |= (status.value&STATUS_MASK) << STATUS_SHIFT

	def get_type(self) -> SbgEComGnssPosType:
		"""
		Returns the GNSS position solution type.

		Returns:
			SbgEComGnssPosType:				The solution type.
		"""
		return (self.status >> TYPE_SHIFT)&TYPE_MASK

	def set_type(self, pos_type: SbgEComGnssPosType):
		"""
		Set the GNSS position solution type.

		Args:
			pos_type (SbgEComGnssPosType):	The solution type to set.
		"""
		assert pos_type.value <= TYPE_MASK

		self.status &= ~(TYPE_MASK << TYPE_SHIFT)
		self.status |= (pos_type.value&TYPE_MASK) << TYPE_SHIFT

	def set_signals_unused(self, signal_mask: int):
		"""
		Set the GNSS signals used in solution bitmask.

		Args:
			signal_mask (int):				Bitmak of signals used in solution to set.
		"""
		assert signal_mask&SIGNALS_MASK == signal_mask

		self.status &= ~SIGNALS_MASK
		self.status |= signal_mask&SIGNALS_MASK

	def signal_are_used(self, signal_mask: int) -> bool:
		"""
		Returns true if the signal(s) are used in the solution.

		Args:
			signal_mask (int):				Bitmask of signals to check if there are used in solution.

		Returns:
			bool:							True if the signal(s) are used in solution.
		"""
		if (self.status & signal_mask) == signal_mask:
			return True
		else:
			return False

	def num_sv_tracked_is_valid(self) -> bool:
		"""
		Returns true if the numSvTracked field is valid.

		Returns:
			bool:							True if the numSvTracked field is valid or false otherwise.
		"""
		if self.num_sv_tracked == np.iinfo(np.uint8).max:
			return False
		else:
			return True

	def num_sv_used_is_valid(self) -> bool:
		"""
		Returns true if the numSvUsed field is valid.

		Returns:
			bool:							True if the numSvUsed field is valid or false otherwise.
		"""
		if self.num_sv_used == np.iinfo(np.uint8).max:
			return False
		else:
			return True

	def base_station_id_is_valid(self) -> bool:
		"""
		Returns true if the baseStationId field is valid.

		Returns:
			bool:							True if the baseStationId field is valid or false otherwise.
		"""
		if self.base_station_id == np.iinfo(np.uint16).max:
			return False
		else:
			return True

	def differential_age_is_valid(self) -> bool:
		"""
		Returns true if the differentialAge field is valid.

		Returns:
			bool:							True if the differentialAge field is valid or false otherwise.
		"""
		if self.differential_age == np.iinfo(np.uint16).max:
			return False
		else:
			return True

	def get_differential_age(self) -> float:
		"""
		Returns the differential correction age in seconds.

		If there is no valid differential age, returns nan.

		Returns:
			diff_age_seconds (float):		Differential correction age in seconds.
		"""
		if self.differential_age == np.iinfo(np.uint16).max:
			diff_age_seconds = np.nan
		else:
			diff_age_seconds = self.differential_age / 100.0
		return diff_age_seconds

	def set_differential_age(self, differential_age: float):
		"""
		Set the differential correction age in seconds.

		If differential correction age is greater than 655.340 seconds or nan the field is set to invalid/not available.

		Args:
			differential_age (float):		Differential correction age in seconds (>= 0).
		"""
		assert math.isnan(differential_age) or differential_age >= 0.0

		if not math.isnan(differential_age):
			diff_age_scaled = int(round(differential_age * 100.0))

			if diff_age_scaled >= np.iinfo(np.uint16).max:
				self.differential_age = np.iinfo(np.uint16).max
			else:
				self.differential_age = diff_age_scaled
		else:
			self.differential_age = np.iinfo(np.uint16).max

	def get_ifm_status(self) -> SbgEComGnssIfmStatus:
		"""
		Returns the GNSS position solution status.

		Returns:
			SbgEComGnssIfmStatus:			The interference monitoring and mitigation status.
		"""
		return (self.status_ext >> IFM_SHIFT)&IFM_MASK

	def set_ifm_status(self, status: SbgEComGnssIfmStatus):
		"""
		Set the GNSS interference monitoring and mitigation status.

		Args:
			status (SbgEComGnssIfmStatus):	The status to set.
		"""
		assert status.value <= IFM_MASK

		self.status_ext &= ~(IFM_MASK << IFM_SHIFT)
		self.status_ext |= (status.value&IFM_MASK) << IFM_SHIFT

	def get_spoofing_status(self) -> SbgEComGnssSpoofingStatus:
		"""
		Returns the GNSS spoofing status.

		Returns:
			SbgEComGnssSpoofingStatus:		The spoofing status.
		"""
		return (self.status_ext >> SPOOFING_SHIFT)&SPOOFING_MASK

	def set_spoofing_status(self, status: SbgEComGnssSpoofingStatus):
		"""
		Set the GNSS spoofing status.

		Args:
			status (SbgEComGnssSpoofingStatus):	The status to set.
		"""
		assert status.value <= SPOOFING_MASK

		self.status_ext &= ~(SPOOFING_MASK << SPOOFING_SHIFT)
		self.status_ext |= (status.value&SPOOFING_MASK) << SPOOFING_SHIFT

	def get_osnma_status(self) -> SbgEComGnssOsnmaStatus:
		"""
		Returns the GNSS Galileo OSNMA status.

		Returns:
			SbgEComGnssOsnmaStatus:			The Galileo OSNMA status.
		"""
		return (self.status_ext >> OSNMA_SHIFT)&OSNMA_MASK

	def set_osnma_status(self, status: SbgEComGnssOsnmaStatus):
		"""
		Set the GNSS Galileo OSNMA status.

		Args:
			status (SbgEComGnssOsnmaStatus): The status to set.
		"""
		assert status.value <= OSNMA_MASK

		self.status_ext &= ~(OSNMA_MASK << OSNMA_SHIFT)
		self.status_ext |= (status.value&OSNMA_MASK) << OSNMA_SHIFT

	def get_name(self) -> str:
		"""
		Returns the log name.

		Returns:
			str:							Log name.
		"""
		match self.log_id:
			case SbgEComLogId.GPS1_POS.value:
				return "gnss1Pos"

			case SbgEComLogId.GPS2_POS.value:
				return "gnss2Pos"

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
			f"{context.fmt_status(self.status_ext):>12}"
			f"{self.time_of_week:>12}"
			f"{self.latitude:>12.6f}"
			f"{self.longitude:>12.6f}"
			f"{self.altitude:>12.6f}"
			f"{self.undulation:>12.6f}"
			f"{self.latitude_accuracy:>12.6f}"
			f"{self.longitude_accuracy:>12.6f}"
			f"{self.altitude_accuracy:>12.6f}"
			f"{self.num_sv_tracked:>12}"
			f"{self.num_sv_used:>12}"
			f"{self.base_station_id:>12}"
			f"{self.differential_age:>12}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'statusExt':>12}{'gpsTow':>12}{'latitude':>12}{'longitude':>12}{'altitude':>12}{'undulation':>12}{'latitudeStd':>12}{'longitudeStd':>12}{'altitudeStd':>12}{'numSvTracked':>12}{'numSvUsed':>12}{'baseStationId':>12}{'diffAge':>12}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(na)':>12}{'(ms)':>12}{'(deg)':>12}{'(deg)':>12}{'(m)':>12}{'(m)':>12}{'(m)':>12}{'(m)':>12}{'(m)':>12}{'(na)':>12}{'(na)':>12}{'(na)':>12}{'(0.01s)':>12}\n"
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
