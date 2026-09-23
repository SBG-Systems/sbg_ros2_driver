# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_gnss_sat.py											* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				25 March 2025													* |
# *																						* |
# * brief				Parse space vehicles in view information log.					* |
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

# Local application
from . import SbgEComLog
from sbgecom.common.sbgecom_logging import logger
from sbgecom.defs.sbgecom_defs_gnss import SbgEComSignalId, SbgEComConstellationId, sbgecom_constellation_to_str, sbgecom_signal_to_str
from sbgecom.sbgecom_ids import SbgEComLogId

def get_field(flags: int, offset: int, mask: int) -> int:
	"""
	Get the field of the flags using the value, offset and mask.

	Args:
		flags (int):						The flags to modify.
		offset (int):						The bit offset where the value should be set.
		mask (int):							The mask to limit the bits we modify.

	Returns:
		int:								Field value.
	"""
	return (((flags) >> (offset)) & (mask))

def set_field(flags: int, value: int, offset: int, mask: int) -> int:
	"""
	Set the field of the flags using the value, offset and mask.

	Args:
		flags (int):						The flags to modify.
		value (int):						The value to set.
		offset (int):						The bit offset where the value should be set.
		mask (int):							The mask to limit the bits we modify.

	Returns:
		int:								The modified flags with the value set at the specified offset.
	"""
	flags &= ~((mask) << offset)
	flags |= ((value & mask) << offset)

	return flags

MAX_NR_SATELLITES						= 64										# Maximum number of satellites in a satellite group.
MAX_NR_SIGNALS							= 8											# Maximum number of signals per satellite.

TRACKING_STATUS_OFFSET					= 0											# Offset of the tracking status field, in bits.
TRACKING_STATUS_WIDTH					= 3											# Width of the tracking status field, in bits.
TRACKING_STATUS_MASK					= ((1 << TRACKING_STATUS_WIDTH) - 1)		# Tracking status field mask.

HEALTH_STATUS_OFFSET					= 3											# Offset of the health status field, in bits.
HEALTH_STATUS_WIDTH						= 2											# Width of the health status field, in bits.
HEALTH_STATUS_MASK						= ((1 << HEALTH_STATUS_WIDTH) - 1)			# Health status field mask.

ELEVATION_STATUS_OFFSET					= 5											# Offset of the elevation status field, in bits.
ELEVATION_STATUS_WIDTH					= 2											# Width of the elevation status field, in bits.
ELEVATION_STATUS_MASK					= ((1 << ELEVATION_STATUS_WIDTH) - 1)		# Elevation status field mask.

CONSTELLATION_ID_OFFSET					= 7											# Offset of the constellation ID field, in bits.
CONSTELLATION_ID_WIDTH					= 4											# Width of the constellation ID field, in bits.
CONSTELLATION_ID_MASK					= ((1 << CONSTELLATION_ID_WIDTH) - 1)		# Constellation ID field mask.

SIGNAL_SNR_VALID						= (1 << 5)									# Set if the SNR value is valid.

class SbgEComSatTrackingStatus(Enum):
	"""
	Tracking status.

	The tracking status embeds the solution status when the latter is known.

	All values must be strictly lower than 8.

	These are on-the-wire values.
	"""
	UNKNOWN								= 0											# Unknown tracking status such as no signal / idle.
	SEARCHING							= 1											# Signal is being searched and can't be used yet.
	TRACKING_UNKNOWN					= 2											# Signal is tracked but don't know if used or not in the solution.
	TRACKING_NOT_USED					= 3											# Signal is tracked and is not used in the solution.
	TRACKING_REJECTED					= 4											# Signal is tracked and is rejected from the solution.
	TRACKING_USED						= 5											# Signal is tracked and used in the solution.

def select_tracking_status(tracking_status1: SbgEComSatTrackingStatus, tracking_status2: SbgEComSatTrackingStatus) -> SbgEComSatTrackingStatus:
	"""
	Returns the best tracking status between the two inputs.

	Args:
		tracking_status1 (SbgEComSatTrackingStatus): First tracking status.
		tracking_status2 (SbgEComSatTrackingStatus): Second tracking status.

	Returns:
		SbgEComSatTrackingStatus:			The best tracking status.
	"""
	tracking_status = None

	if tracking_status2.value > tracking_status1.value:
		tracking_status = tracking_status2
	else:
		tracking_status = tracking_status1

	return tracking_status

def tracking_status_to_string(tracking_status: SbgEComSatTrackingStatus) -> str:
	"""
	Returns tracking status string from enum value.

	Args:
		tracking_status (SbgEComSatTrackingStatus): Tracking status enum to convert.

	Returns:
		str:								Tracking status as a read only string.
	"""
	enum_to_str_lut = {
		SbgEComSatTrackingStatus.UNKNOWN:				"unknown",
		SbgEComSatTrackingStatus.SEARCHING:				"searching",
		SbgEComSatTrackingStatus.TRACKING_UNKNOWN:		"tracking",
		SbgEComSatTrackingStatus.TRACKING_NOT_USED:		"unused",
		SbgEComSatTrackingStatus.TRACKING_REJECTED:		"rejected",
		SbgEComSatTrackingStatus.TRACKING_USED:			"used",
	}

	return enum_to_str_lut.get(tracking_status, enum_to_str_lut[SbgEComSatTrackingStatus.UNKNOWN])

class SbgEComSatHealthStatus(Enum):
	"""
	Health status.

	All values must be strictly lower than 4.

	These are on-the-wire values.
	"""
	UNKNOWN								= 0											# Don't know the satellite or the signal health status.
	HEALTHY								= 1											# The satellite or the signal is healthy and can be used.
	UNHEALTHY							= 2											# The satellite or the signal is not healthy and can't be used.

def select_health_status(health_status1: SbgEComSatHealthStatus, health_status2: SbgEComSatHealthStatus) -> SbgEComSatHealthStatus:
	"""
	Returns the worst health status between the two inputs.

	Args:
		health_status1 (SbgEComSatHealthStatus): First health status.
		health_status2 (SbgEComSatHealthStatus): Second health status.

	Returns:
		SbgEComSatHealthStatus:				The worst health status.
	"""
	health_status = None

	if health_status2.value > health_status1.value:
		health_status = health_status2
	else:
		health_status = health_status1

	return health_status

def health_status_to_str(health_status: SbgEComSatHealthStatus) -> str:
	"""
	Returns health status string from enum value.

	Args:
		health_status (SbgEComSatHealthStatus): Health status enum to convert.

	Returns:
		str:								Health status as a read only string.
	"""
	enum_to_str_lut = {
		SbgEComSatHealthStatus.UNKNOWN:		"unknown",
		SbgEComSatHealthStatus.HEALTHY:		"healthy",
		SbgEComSatHealthStatus.UNHEALTHY:	"unhealthy",
	}

	return enum_to_str_lut.get(health_status, enum_to_str_lut[SbgEComSatHealthStatus.UNKNOWN])

class SbgEComSatElevationStatus(Enum):
	"""
	Elevation status.

	All values must be strictly lower than 4.

	These are on-the-wire values.
	"""
	UNKNOWN								= 0											# Don't know if the satellite elevation is setting or rising.
	SETTING								= 1											# The satellite elevation is setting.
	RISING								= 2											# The satellite elevation is rising.

def elevation_status_to_str(elevation_status: SbgEComSatElevationStatus) -> str:
	"""
	Returns the elevation status string from enum value.

	Args:
		elevation_status (SbgEComSatElevationStatus): Elevation status enum to convert.

	Returns:
		str:								Elevation status as a read only string.
	"""
	enum_to_str_lut = {
		SbgEComSatElevationStatus.UNKNOWN:		"unknown",
		SbgEComSatElevationStatus.SETTING:		"setting",
		SbgEComSatElevationStatus.RISING:		"rising",
	}

	return enum_to_str_lut.get(elevation_status, enum_to_str_lut[SbgEComSatElevationStatus.UNKNOWN])

@dataclass
class SbgEComLogSatSignal:
	"""
	Information and status for a single signal for a Satellite.
	"""

	"""
	Class members.
	"""
	id:									SbgEComSignalId	= -1						# Signal ID.
	flags:								int				= -1						# Flags.
	snr:								int				= -1						# Signal-to-noise ratio, in dB.

	def read(self, payload: bytearray):
		"""
		Parse a satellite signal instance from a stream buffer.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<BBB', payload)
		self.id							= unpacked_data[0]
		self.flags						= unpacked_data[1]
		self.snr						= unpacked_data[2]

	def write(self) -> bytearray:
		"""
		Write a satellite signal instance to a stream buffer.

		Returns:
			bytearray:						Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<BBB',
			self.id,
			self.flags,
			self.snr,
		)

		return bytearray(packed_data)

	def get_signal_id_as_string(self) -> str:
		"""
		Get a signal id as a read only string.

		Returns:
			str:							Signal id as a read only string.
		"""
		return sbgecom_signal_to_str(self.id)

	def snr_is_valid(self) -> bool:
		"""
		Returns true if the SNR value is valid.

		Returns:
			bool:							True if the SNR value is valid.
		"""
		return True if self.flags & SIGNAL_SNR_VALID else False

	def get_health_status(self) -> SbgEComSatHealthStatus:
		"""
		Get the health status of signal data.

		Returns:
			SbgEComSatHealthStatus:			Health status.
		"""
		return SbgEComSatHealthStatus(get_field(self.flags, HEALTH_STATUS_OFFSET, HEALTH_STATUS_MASK))

	def get_health_status_as_str(self) -> str:
		"""
		Get the health status of signal data as a read only string.

		Returns:
			str:							Health status as a read only string.
		"""
		return health_status_to_str(self.get_health_status())

	def get_tracking_status(self) -> SbgEComSatTrackingStatus:
		"""
		Get the tracking status of signal data.

		Returns:
			SbgEComSatTrackingStatus:		Tracking status.
		"""
		return SbgEComSatTrackingStatus(get_field(self.flags, TRACKING_STATUS_OFFSET, TRACKING_STATUS_MASK))

	def get_tracking_status_as_str(self) -> str:
		"""
		Get the tracking status of signal data as a read only string.

		Returns:
			str:							Tracking status as a read only string.
		"""
		return tracking_status_to_string(self.get_tracking_status())

@dataclass
class SbgEComLogSatEntry:
	"""
	Information and status for a single satellite.
	"""

	"""
	Class members.
	"""
	id:									int = -1									# Satellite ID.
	elevation:							int = -1									# Elevation, in degrees [-90; +90], valid if and only if the elevation is known.
	azimuth:							int = -1									# Azimuth, in degrees [0; 359], valid if and only if the elevation is known.
	flags:								int = -1									# Flags.
	nr_signals:							int = -1									# Number of signals.
	signal_data:						list[SbgEComLogSatSignal] = field(
											default_factory=lambda: [SbgEComLogSatSignal() for _ in range(MAX_NR_SIGNALS)])		# Signal data list.

	def construct(self, id: int, elevation: int, azimuth: int, constellation_id: SbgEComConstellationId, elevation_status: SbgEComSatElevationStatus, health_status: SbgEComSatHealthStatus, tracking_status: SbgEComSatTrackingStatus):
		"""
		Satellite entry constructor.

		Args:
			id (int):						Satellite ID.
			elevation (int):				Elevation, in degrees.
			azimuth (int):					Azimuth, in degrees.
			constellation_id (SbgEComConstellationId): Constellation ID.
			elevation_status (SbgEComSatElevationStatus): Elevation status.
			health_status (SbgEComSatHealthStatus): Health status.
			tracking_status (SbgEComSatTrackingStatus): Tracking status.
		"""
		flags = 0

		flags = set_field(flags,	constellation_id.value,		CONSTELLATION_ID_OFFSET,	CONSTELLATION_ID_MASK)
		flags = set_field(flags,	elevation_status.value,		ELEVATION_STATUS_OFFSET,	ELEVATION_STATUS_MASK)
		flags = set_field(flags,	health_status.value,		HEALTH_STATUS_OFFSET,		HEALTH_STATUS_MASK)
		flags = set_field(flags,	tracking_status.value,		TRACKING_STATUS_OFFSET,		TRACKING_STATUS_MASK)

		self.id							= id
		self.elevation					= elevation
		self.azimuth					= azimuth
		self.flags						= flags
		self.nr_signals					= 0

	def update_status(self, health_status: SbgEComSatHealthStatus, tracking_status: SbgEComSatTrackingStatus):
		"""
		Update the health and tracking summary statuses of satellite data.

		The SbgEComLogSatEntry stores a synthetic health and tracking statues.
		This synthetic status is computed from each signal status using priority rules.

		For example:

		if at least one signal reports an unhealthy signal, the whole SV is flagged unhealthy.
		if at least one signal is tracked and used in the solution, the whole SV is flagged as tracked+used.

		Args:
			health_status (SbgEComSatHealthStatus): Health status.
			tracking_status (SbgEComSatTrackingStatus): Tracking status.
		"""
		flags = self.flags

		health_status	= select_health_status(self.get_health_status(), health_status)
		tracking_status	= select_tracking_status(self.get_tracking_status(), tracking_status)

		flags = set_field(flags,	health_status.value,		HEALTH_STATUS_OFFSET,		HEALTH_STATUS_MASK)
		flags = set_field(flags,	tracking_status.value,		TRACKING_STATUS_OFFSET,		TRACKING_STATUS_MASK)

		self.flags		= flags

	def read(self, payload: bytearray, offset: int):
		"""
		Read a satellite entry instance from a stream buffer.

		Args:
			payload (bytearray):			Data payload.
			offset (int):					Offset.
		"""
		header_size = struct.calcsize('<BbHHB')

		unpacked_data					= struct.unpack('<BbHHB', payload[offset:offset + header_size])
		self.id							= unpacked_data[0]
		self.elevation					= unpacked_data[1]
		self.azimuth					= unpacked_data[2]
		self.flags						= unpacked_data[3]
		self.nr_signals					= unpacked_data[4]

		offset += header_size

		if unpacked_data:

			if self.nr_signals <= len(self.signal_data):

				signal_size = struct.calcsize('<BBB')

				for i in range(self.nr_signals):
					if len(payload) < offset + signal_size:
						raise ValueError("payload too short for sat signal")

					self.signal_data[i].read(payload[offset:offset + signal_size])
					offset += signal_size
			else:
				logger.error(f"invalid number of signals: {self.nr_signals}")

		return offset

	def write(self, buffer: bytearray):
		"""
		Write a satellite entry instance to a stream buffer.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<BbHHB',
			self.id,
			self.elevation,
			self.azimuth,
			self.flags,
			self.nr_signals
		)

		buffer.extend(packed_data)

		for i in range(self.nr_signals):
			buffer.extend(self.signal_data[i].write())

	def add(self, id: SbgEComSignalId, health_status: SbgEComSatHealthStatus, tracking_status: SbgEComSatTrackingStatus, snr_valid: bool, snr: int) -> SbgEComLogSatSignal:
		"""
		Add signal data to satellite data.

		The health and tracking statuses of the satellite data are updated according to their respective priority rules.

		Args:
			id (SbgEComSignalId):			Signal ID.
			health_status (SbgEComSatHealthStatus): Health status.
			tracking_status (SbgEComSatTrackingStatus): Tracking status.
			snr_valid (bool):				Set to true if the SNR value is valid.
			snr (int):						Signal-to-noise ratio, in dB.

		Returns:
			SbgEComLogSatSignal:			Signal data.
		"""
		signal_data = None

		if self.nr_signals < len(self.signal_data):
			flags = 0

			flags = set_field(flags,	health_status.value,		HEALTH_STATUS_OFFSET,		HEALTH_STATUS_MASK)
			flags = set_field(flags,	tracking_status.value,		TRACKING_STATUS_OFFSET,		TRACKING_STATUS_MASK)

			if snr_valid:
				flags |= SIGNAL_SNR_VALID

			signal_data = self.signal_data[self.nr_signals]
			self.nr_signals += 1

			signal_data.id		= id
			signal_data.flags	= flags
			signal_data.snr		= snr

			self.update_status(health_status, tracking_status)
		else:
			logger.error("not free slot to add a new signal")

		return signal_data

	def get(self, id: SbgEComSignalId) -> SbgEComLogSatSignal:
		"""
		Get signal data from satellite data.

		Args:
			id (SbgEComSignalId):			Signal ID.

		Returns:
			SbgEComLogSatSignal:			Signal data, None if not found.
		"""
		signal_data = None

		for signal in self.signal_data:

			if signal.id == id:
				signal_data = signal
				break

		return signal_data

	def get_constellation_id(self) -> SbgEComConstellationId:
		"""
		Get the constellation ID of satellite data.

		Returns:
			SbgEComConstellationId:			Constellation ID.
		"""
		return SbgEComConstellationId(get_field(self.flags, CONSTELLATION_ID_OFFSET, CONSTELLATION_ID_MASK))

	def get_constellation_id_as_str(self) -> str:
		"""
		Get the constellation ID of satellite data as a read only string.

		Returns:
			str:							Constellation ID as a read only string.
		"""
		return sbgecom_constellation_to_str(self.get_constellation_id())

	def get_elevation_status(self) -> SbgEComSatElevationStatus:
		"""
		Get the elevation status of satellite data.

		Returns:
			SbgEComSatElevationStatus:		Elevation status.
		"""
		return SbgEComSatElevationStatus(get_field(self.flags, ELEVATION_STATUS_OFFSET, ELEVATION_STATUS_MASK))

	def get_elevation_status_as_str(self) -> str:
		"""
		Get the elevation status of satellite data as a read only string.

		Returns:
			str:							Elevation status as a read only string.
		"""
		return elevation_status_to_str(self.get_elevation_status())

	def get_health_status(self) -> SbgEComSatHealthStatus:
		"""
		Get the health status of satellite data.

		Returns:
			SbgEComSatHealthStatus:			Health status.
		"""
		return SbgEComSatHealthStatus(get_field(self.flags, HEALTH_STATUS_OFFSET, HEALTH_STATUS_MASK))

	def get_health_status_as_str(self) -> str:
		"""
		Get the health status of satellite data as a read only string.

		Returns:
			str:							Health status as a read only string.
		"""
		return health_status_to_str(self.get_health_status())

	def get_tracking_status(self) -> SbgEComSatTrackingStatus:
		"""
		Get the tracking status of satellite data.

		Returns:
			SbgEComSatTrackingStatus:		Tracking status.
		"""
		return SbgEComSatTrackingStatus(get_field(self.flags, TRACKING_STATUS_OFFSET, TRACKING_STATUS_MASK))

	def get_tracking_status_as_str(self) -> str:
		"""
		Get the tracking status of satellite data as a read only string.

		Returns:
			str:							Tracking status as a read only string.
		"""
		return tracking_status_to_string(self.get_tracking_status())

@dataclass
class SbgEComLogSatList(SbgEComLog):
	"""
	DataClass that stores data for the SBG_ECOM_LOG_GPS#_SAT message.

	List of visible satellites.
	"""

	"""
	Class members.
	"""
	time_stamp:							int = -1									# Time since the sensor power up, in us.
	reserved:							int = -1									# Reserved for future use.
	nr_satellites:						int = -1									# Number of satellites.
	sat_data:							list[SbgEComLogSatEntry] = field(
											default_factory=lambda: [SbgEComLogSatEntry() for _ in range(MAX_NR_SATELLITES)])	# Satellite data list.

	# To identifie either GNSS1Hdt or GNSS2Hdt.
	log_id: int = field(default=None, init=False, repr=False)

	def set_log_id(self, log_id: int):
		"""
		Method to set the log_id after initialization.

		Args:
			log_id (int):					The GNSS number.
		"""
		self.log_id = log_id

	def construct(self, timestamp: int):
		"""
		Satellite list constructor.

		Args:
			timestamp (int):				Timestamp, in us.
		"""
		self.time_stamp					= timestamp
		self.reserved					= 0
		self.nr_satellites				= 0

	def add(self, id: int, elevation: int, azimuth: int, constellation_id: SbgEComConstellationId, elevation_status: SbgEComSatElevationStatus, health_status: SbgEComSatHealthStatus, tracking_status: SbgEComSatTrackingStatus)-> SbgEComLogSatEntry:
		"""
		Add a satellite entry to the list of satellites.

		Args:
			id (int):						Satellite ID.
			elevation (int):				Elevation, in degrees.
			azimuth (int):					Azimuth, in degrees.
			constellation_id (SbgEComConstellationId): Constellation ID.
			elevation_status (SbgEComSatElevationStatus): Elevation status.
			health_status (SbgEComSatHealthStatus): Health status.
			tracking_status (SbgEComSatTrackingStatus): Tracking status.

		Returns:
			SbgEComLogSatEntry:				Satellite data.
		"""
		sat_data = None

		if self.nr_satellites < len(self.sat_data):
			sat_data = self.sat_data[self.nr_satellites]
			self.nr_satellites += 1

			sat_data.construct(id, elevation, azimuth, constellation_id, elevation_status, health_status, tracking_status)
		else:
			logger.error("no free slot to add a new satellite.")

		return sat_data

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_GPS#_SAT message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		header_size = struct.calcsize('<IIB')

		unpacked_data					= struct.unpack('<IIB', payload[:header_size])
		self.time_stamp					= unpacked_data[0]
		self.reserved					= unpacked_data[1]
		self.nr_satellites				= unpacked_data[2]

		if unpacked_data:

			if self.nr_satellites <= len(self.sat_data):

				offset = header_size

				for i in range(self.nr_satellites):
					offset = self.sat_data[i].read(payload, offset)
			else:
				logger.error(f"invalid number of satellite: {self.nr_satellites}")

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_GPS#_SAT message to the output buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IIB',
			self.time_stamp,
			self.reserved,
			self.nr_satellites
		)

		buffer.extend(packed_data)

		for i in range(self.nr_satellites):
			self.sat_data[i].write(buffer)

	def get(self, id: int) -> SbgEComLogSatEntry:
		"""
		Get a satellite entry from its ID.

		Args:
			id (int):						Satellite ID.

		Returns:
			SbgEComLogSatEntry:				Satellite data.
		"""
		sat_data = None

		for satellite in self.sat_data:

			if satellite.id == id:
				sat_data = satellite
				break

		return sat_data

	def get_name(self) -> str:
		"""
		Returns the log name.

		Returns:
			str:							Log name.
		"""
		match self.log_id:
			case SbgEComLogId.GPS1_SAT.value:
				return "gnss1Sat"

			case SbgEComLogId.GPS2_SAT.value:
				return "gnss2Sat"

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
				f" | Satellites: {self.nr_satellites}====================================================================\n"
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
			f"{self.nr_satellites:>12}"
		)
