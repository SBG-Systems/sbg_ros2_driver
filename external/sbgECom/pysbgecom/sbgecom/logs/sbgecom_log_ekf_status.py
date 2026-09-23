# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_ekf_status.py										* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				08 July 2026													* |
# *																						* |
# * brief				Parse logs used to report EKF filter status.					* |
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

# Local application
from . import SbgEComLog

"""
EKF general status bitmask definitions.
"""
ROLL_PITCH_VALID						= (0x0001 << 0)								# Set if attitude data is reliable (Roll/Pitch error within defined criteria).
HEADING_VALID							= (0x0001 << 1)								# Set if heading data is reliable (Heading error within defined criteria).
VELOCITY_VALID							= (0x0001 << 2)								# Set if velocity data is reliable (Velocity error within defined criteria).
POSITION_VALID							= (0x0001 << 3)								# Set if position data is reliable (Position error within defined criteria).
ALIGN_VALID								= (0x0001 << 4)								# Set if the INS is aligned and sensor error estimation has converged enough to trust the solution.

NAV_SAFETY_SHIFT						= 8											# Shift used to extract the navigation safety indicator part.
NAV_SAFETY_MASK							= 0x07										# Mask used to keep only the navigation safety indicator part.

"""
GNSS extended spoofing status bitmask definitions.
"""
GNSS_SPOOFING_SHIFT						= 0											# Shift used to extract the GNSS spoofing indicator part.
GNSS_SPOOFING_MASK						= 0x07										# Mask used to keep only the GNSS spoofing indicator part.

GNSS_SPOOFING_SRC_TIME					= (0x0001 << 3)								# Set if GNSS reports inconsistent time information.
GNSS_SPOOFING_SRC_BEHAVIOR				= (0x0001 << 4)								# Set if GNSS exhibits abnormal behavior consistent with spoofing.
GNSS_SPOOFING_SRC_SOLUTION				= (0x0001 << 5)								# Set if the GNSS PVT solution is inconsistent with the INS solution.

"""
Aiding sources used bitmask definitions.
"""
AIDING_VERTICAL_REF_USED				= (0x00000001 << 0)							# Set if the vertical reference is used in the solution.
AIDING_ZUPT_USED						= (0x00000001 << 1)							# Set if a Zero Velocity Update (ZUPT) is used in the solution.
AIDING_ZARU_USED						= (0x00000001 << 2)							# Set if a Zero Angular Rate Update (ZARU) is used in the solution.
AIDING_MOTION_CONSTRAINT_USED			= (0x00000001 << 3)							# Set if vehicle motion constraints are used in the solution.
AIDING_MAGNETOMETER_USED				= (0x00000001 << 4)							# Set if the magnetometer is used in the solution.
AIDING_GNSS1_VEL_USED					= (0x00000001 << 5)							# Set if GNSS 1 velocity is used in the solution.
AIDING_GNSS1_POS_USED					= (0x00000001 << 6)							# Set if GNSS 1 position is used in the solution.
AIDING_GNSS1_HDT_USED					= (0x00000001 << 7)							# Set if GNSS 1 true heading is used in the solution.
AIDING_GNSS2_VEL_USED					= (0x00000001 << 8)							# Set if GNSS 2 velocity is used in the solution.
AIDING_GNSS2_POS_USED					= (0x00000001 << 9)							# Set if GNSS 2 position is used in the solution.
AIDING_GNSS2_HDT_USED					= (0x00000001 << 10)						# Set if GNSS 2 true heading is used in the solution.
AIDING_ODO_USED							= (0x00000001 << 11)						# Set if odometer velocity is used in the solution.
AIDING_DVL_BT_USED						= (0x00000001 << 12)						# Set if DVL bottom-track velocity is used in the solution.
AIDING_DVL_WT_USED						= (0x00000001 << 13)						# Set if DVL water-track velocity is used in the solution.
AIDING_USBL_USED						= (0x00000001 << 14)						# Set if USBL position is used in the solution.
AIDING_DEPTH_USED						= (0x00000001 << 15)						# Set if depth is used in the solution.
AIDING_AIRSPEED_USED					= (0x00000001 << 16)						# Set if true airspeed is used in the solution.
AIDING_BARO_USED						= (0x00000001 << 17)						# Set if barometric altitude is used in the solution.
AIDING_GENERIC_VEL1_USED				= (0x00000001 << 18)						# Set if generic velocity 1 is used in the solution.
AIDING_GENERIC_POS1_USED				= (0x00000001 << 19)						# Set if generic position 1 is used in the solution.

class SbgEComEkfStatusSolutionType(Enum):
	"""
	Solution types.
	"""
	INVALID								= 0											# EKF is not initialized. All outputs are invalid.
	VG									= 1											# Vertical gyro mode with roll and pitch estimated from the vertical reference.
	AHRS								= 2											# AHRS mode with roll, pitch, and heading estimated from magnetometers, dual-GNSS heading, or gyro-compassing.
	NAV_INERTIAL						= 3											# Full INS solution using inertial data only.
	NAV_MOTION_CONSTRAINT				= 4											# Full INS solution using vehicle motion constraints.
	NAV_ZUPT							= 5											# Full INS solution using Zero Velocity Updates (ZUPT).
	NAV_GNSS_VEL						= 6											# Full INS solution using GNSS velocity.
	NAV_GNSS_UNKNOWN					= 7											# Full INS solution using GNSS position of unknown type.
	NAV_SINGLE_POINT					= 8											# Full INS solution using GNSS single-point position.
	NAV_DGPS							= 9											# Full INS solution using GNSS DGPS position.
	NAV_SBAS							= 10										# Full INS solution using GNSS SBAS position.
	NAV_RTK_FLOAT						= 11										# Full INS solution using GNSS RTK with float position.
	NAV_RTK_FIXED						= 12										# Full INS solution using GNSS RTK with fixed position.
	NAV_PPP_FLOAT						= 13										# Full INS solution using GNSS PPP with float position.
	NAV_PPP_FIXED						= 14										# Full INS solution using GNSS PPP with fixed position.
	NAV_ODO								= 15										# Full INS solution using odometer velocity.
	NAV_DVL								= 16										# Full INS solution using DVL velocity.
	NAV_USBL							= 17										# Full INS solution using USBL position.
	NAV_AIRSPEED						= 18										# Full INS solution using true airspeed.
	NAV_VEL1							= 19										# Full INS solution using generic velocity 1.
	NAV_POS1							= 20										# Full INS solution using generic position 1.

class SbgEComEkfStatusNavSafety(Enum):
	"""
	Navigation safety indicator.
	"""
	SAFETY_UNKNOWN						= 0											# Safety cannot be assessed. Anti-spoofing is disabled, or navigation uncertainty is too high to reliably detect or mitigate spoofing.
	NOMINAL								= 1											# No spoofing is detected, and navigation operates under nominal conditions.
	MITIGATED							= 2											# Spoofing is detected or suspected, but navigation remains protected thanks to mitigation using inertial and/or other aiding sources.
	UNPROTECTED							= 3											# Navigation is operating with limited or no spoofing mitigation capability (e.g. due to high uncertainty, or insufficient aiding).

class SbgEComEkfStatusSpoofing(Enum):
	"""
	Spoofing indicator.
	"""
	UNKNOWN								= 0											# Undetermined spoofing status.
	RELIABLE							= 1											# GNSS solution is considered as reliable.
	INDICATED							= 2											# One spoofing detector indicated potential spoofing event.
	CONFIRMED							= 3											# Multiple indications of spoofing.

@dataclass
class SbgEComLogEkfStatus(SbgEComLog):
	"""
	Data class that stores data for the SBG_ECOM_LOG_EKF_STATUS message.
	"""

	name									= "ekfStatus"							# Log name.

	"""
	Class members.
	"""
	time_stamp:							int = 0										# Time since the sensor power up, in us.
	ekf_status:							int = 0										# EKF general status.
	solution_type:						int = 0										# INS solution type.
	alignment_progress:					int = 0										# Alignment progress, in percent from 0 to 100.
	gnss1_spoofing_status:				int = 0										# GNSS 1 extended spoofing status.
	gnss2_spoofing_status:				int = 0										# GNSS 2 extended spoofing status.
	aidings_used:						int = 0										# Aiding sources currently used by the EKF solution.

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_EKF_STATUS message and fill the corresponding structure.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<IIBBHHI', payload[:18])
		self.time_stamp					= unpacked_data[0]
		self.ekf_status					= unpacked_data[1]
		self.solution_type				= unpacked_data[2]
		self.alignment_progress			= unpacked_data[3]
		self.gnss1_spoofing_status		= unpacked_data[4]
		self.gnss2_spoofing_status		= unpacked_data[5]
		self.aidings_used				= unpacked_data[6]

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_EKF_STATUS message to the output stream buffer from the provided structure.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IIBBHHI',
			self.time_stamp,
			self.ekf_status,
			self.solution_type,
			self.alignment_progress,
			self.gnss1_spoofing_status,
			self.gnss2_spoofing_status,
			self.aidings_used
		)

		buffer.extend(packed_data)

	def get_nav_safety(self) -> SbgEComEkfStatusNavSafety:
		"""
		Returns the navigation safety indicator.

		Returns:
			SbgEComEkfStatusNavSafety:		The navigation safety indicator.
		"""
		return SbgEComEkfStatusNavSafety((self.ekf_status >> NAV_SAFETY_SHIFT) & NAV_SAFETY_MASK)

	def set_nav_safety(self, nav_safety: SbgEComEkfStatusNavSafety):
		"""
		Set the navigation safety indicator.

		Args:
			nav_safety (SbgEComEkfStatusNavSafety):	The navigation safety indicator to set.
		"""
		assert nav_safety.value <= NAV_SAFETY_MASK

		self.ekf_status &= ~(NAV_SAFETY_MASK << NAV_SAFETY_SHIFT)
		self.ekf_status |= (nav_safety.value & NAV_SAFETY_MASK) << NAV_SAFETY_SHIFT

	def get_gnss1_spoofing_indicator(self) -> SbgEComEkfStatusSpoofing:
		"""
		Returns the GNSS 1 spoofing indicator.

		Returns:
			SbgEComEkfStatusSpoofing:		The GNSS 1 spoofing indicator.
		"""
		return SbgEComEkfStatusSpoofing((self.gnss1_spoofing_status >> GNSS_SPOOFING_SHIFT) & GNSS_SPOOFING_MASK)

	def set_gnss1_spoofing_indicator(self, spoofing: SbgEComEkfStatusSpoofing):
		"""
		Set the GNSS 1 spoofing indicator.

		Args:
			spoofing (SbgEComEkfStatusSpoofing):	The GNSS 1 spoofing indicator to set.
		"""
		assert spoofing.value <= GNSS_SPOOFING_MASK

		self.gnss1_spoofing_status &= ~(GNSS_SPOOFING_MASK << GNSS_SPOOFING_SHIFT)
		self.gnss1_spoofing_status |= (spoofing.value & GNSS_SPOOFING_MASK) << GNSS_SPOOFING_SHIFT

	def get_gnss2_spoofing_indicator(self) -> SbgEComEkfStatusSpoofing:
		"""
		Returns the GNSS 2 spoofing indicator.

		Returns:
			SbgEComEkfStatusSpoofing:		The GNSS 2 spoofing indicator.
		"""
		return SbgEComEkfStatusSpoofing((self.gnss2_spoofing_status >> GNSS_SPOOFING_SHIFT) & GNSS_SPOOFING_MASK)

	def set_gnss2_spoofing_indicator(self, spoofing: SbgEComEkfStatusSpoofing):
		"""
		Set the GNSS 2 spoofing indicator.

		Args:
			spoofing (SbgEComEkfStatusSpoofing):	The GNSS 2 spoofing indicator to set.
		"""
		assert spoofing.value <= GNSS_SPOOFING_MASK

		self.gnss2_spoofing_status &= ~(GNSS_SPOOFING_MASK << GNSS_SPOOFING_SHIFT)
		self.gnss2_spoofing_status |= (spoofing.value & GNSS_SPOOFING_MASK) << GNSS_SPOOFING_SHIFT

	def get_values(self, context) -> str:
		"""
		Return formatted values shared by file and console outputs.

		Args:
			context (LoggerContext):		Logger context and settings.

		Returns:
			str:							Formatted values.
		"""
		data = (
			f"{context.fmt_status(self.ekf_status):>12}"
			f"{context.fmt_status(self.solution_type):>14}"
			f"{self.alignment_progress:>19}"
			f"{context.fmt_status(self.gnss1_spoofing_status):>21}"
			f"{context.fmt_status(self.gnss2_spoofing_status):>21}"
			f"{context.fmt_status(self.aidings_used):>13}"
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
				f"{context.get_time_col_title():>12}{'ekfStatus':>12}{'solutionType':>14}{'alignmentProgress':>19}{'gnss1SpoofingStatus':>21}{'gnss2SpoofingStatus':>21}{'aidingsUsed':>13}\n"
				f"{context.get_time_unit():>12}{'(na)':>12}{'(na)':>14}{'(%)':>19}{'(na)':>21}{'(na)':>21}{'(na)':>13}\n"
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
