# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_gnss_vel.py											* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				25 March 2025													* |
# *																						* |
# * brief				GNSS velocity logs.												* |
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

class SbgEComGnssVelStatus(Enum):
	"""
	GNSS velocity status definitions.
	"""

	SOL_COMPUTED						= 0											# A valid solution has been computed.
	INSUFFICIENT_OBS					= 1											# Not enough valid SV to compute a solution.
	INTERNAL_ERROR						= 2											# An internal error has occurred.
	LIMIT								= 3											# Velocity limit exceeded.

class SbgEComGnssVelType(Enum):
	"""
	GNSS velocity types definitions.
	"""

	NO_SOLUTION							= 0											# No valid velocity solution available.
	UNKNOWN								= 1											# An unknown solution type has been computed.
	DOPPLER								= 2											# A Doppler velocity has been computed.
	DIFFERENTIAL						= 3											# A differential velocity has been computed between two positions.

STATUS_SHIFT							= 0											# Shift used to extract the GPS velocity status part.
STATUS_MASK								= 0x0000003F								# Mask used to keep only the GPS velocity status part.

TYPE_SHIFT								= 6											# Shift used to extract the GPS velocity type part.
TYPE_MASK								= 0x0000003F								# Mask used to keep only the GPS velocity type part.

@dataclass
class SbgEComLogGnssVel(SbgEComLog):
	"""
	DataClass that stores data for the SBG_ECOM_LOG_GPS#_VEL message.
	"""

	"""
	Class members.
	"""
	time_stamp:							int			= 0								# Time in microseconds since the sensor power up.
	status:								int			= 0								# GPS velocity status, type and bitmask.
	time_of_week:						int			= 0								# GPS time of week in milliseconds.
	velocity:							np.ndarray	= field(default_factory=lambda: np.full(3, 9999.0, dtype=np.float32))	# GPS North, East, Down velocity in m/s.
	velocity_acc:						np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.int32))			# GPS North, East, Down velocity 1 sigma accuracy in m/s (0 to 9999).
	course:								float		= 0								# Track ground course in degrees (0 to 360).
	course_acc:							float		= 180.0							# Course accuracy in degrees (0 to 180).

	def __post_init__(self):
		self.set_status(SbgEComGnssVelStatus.INSUFFICIENT_OBS)
		self.set_type(SbgEComGnssVelType.NO_SOLUTION)

	# To identifie either GNSS1Vel or GNSS2Vel.
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
		Parse data for the SBG_ECOM_LOG_GPS#_VEL message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<III3f3fff', payload)
		self.time_stamp					= unpacked_data[0]
		self.status						= unpacked_data[1]
		self.time_of_week				= unpacked_data[2]
		self.velocity					= np.array(unpacked_data[3:6], dtype=np.float32)
		self.velocity_acc				= np.array(unpacked_data[6:9], dtype=np.float32)
		self.course						= unpacked_data[9]
		self.course_acc					= unpacked_data[10]

	def write(self, buffer: bytearray):
		"""
		Write data for the SbgEComLogGnssVel message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<III3f3fff',
			self.time_stamp,
			self.status,
			self.time_of_week,
			self.velocity[0],
			self.velocity[1],
			self.velocity[2],
			self.velocity_acc[0],
			self.velocity_acc[1],
			self.velocity_acc[2],
			self.course,
			self.course_acc
		)

		buffer.extend(packed_data)

	def get_status(self) -> SbgEComGnssVelStatus:
		"""
		Returns the GNSS velocity solution status.

		Returns:
			SbgEComGnssVelStatus:			The solution status.
		"""
		return (self.status >> STATUS_SHIFT) & STATUS_MASK

	def set_status(self, status: SbgEComGnssVelStatus):
		"""
		Set the GNSS velocity solution status.

		Args:
			status (SbgEComGnssVelStatus):	The solution status to set.
		"""
		assert status.value <= STATUS_MASK

		self.status &= ~(STATUS_MASK << STATUS_SHIFT)
		self.status |= (status.value&STATUS_MASK) << STATUS_SHIFT

	def get_type(self) -> SbgEComGnssVelType:
		"""
		Returns the GNSS velocity solution type.

		Returns:
			SbgEComGnssVelType:				The solution type.
		"""
		return (self.status >> TYPE_SHIFT) & TYPE_MASK

	def set_type(self, pos_type: SbgEComGnssVelType):
		"""
		Set the GNSS velocity solution type.

		Args:
			pos_type (SbgEComGnssVelType):	The solution type to set.
		"""
		assert pos_type.value <= TYPE_MASK

		self.status &= ~(TYPE_MASK << TYPE_SHIFT)
		self.status |= (pos_type.value & TYPE_MASK) << TYPE_SHIFT

	def down_velocity_is_valid(self) -> bool:
		"""
		Returns true if the velocity down component is available/valid.

		Some GNSS receivers such as the ones using NMEA protocols can't provide down velocity.
		This methods checks the velocity standard deviation down component.

		Returns:
			bool:							True if the velocity down component is available/valid.
		"""
		if self.velocity_acc[2] < (9999.0 - np.finfo(float).eps):
			return True
		else:
			return False

	def get_name(self) -> str:
		"""
		Returns the log name.

		Returns:
			str:							Log name.
		"""
		match self.log_id:
			case SbgEComLogId.GPS1_VEL.value:
				return "gnss1Vel"

			case SbgEComLogId.GPS2_VEL.value:
				return "gnss2Vel"

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
			f"{self.velocity[0]:>12.6f}"
			f"{self.velocity[1]:>12.6f}"
			f"{self.velocity[2]:>12.6f}"
			f"{self.velocity_acc[0]:>12.6f}"
			f"{self.velocity_acc[1]:>12.6f}"
			f"{self.velocity_acc[2]:>12.6f}"
			f"{self.course:>12.6f}"
			f"{self.course_acc:>12.6f}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'gpsTow':>12}{'velN':>12}{'velE':>12}{'velD':>12}{'velStdN':>12}{'velStdE':>12}{'velStdD':>12}{'track':>12}{'trackStd':>12}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(ms)':>12}{'(m.s^-1)':>12}{'(m.s^-1)':>12}{'(m.s^-1)':>12}{'(m.s^-1)':>12}{'(m.s^-1)':>12}{'(m.s^-1)':>12}{'(deg)':>12}{'(deg)':>12}\n"
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
