# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_ship_motion.py										* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				06 May 2025														* |
# *																						* |
# * brief				Parse logs that returns ship motion values such as heave.		* |
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
import struct

# Third party
import numpy as np

# Local application
from . import SbgEComLog
from sbgecom.sbgecom_ids import SbgEComLogId

HEAVE_VALID								= (0x0001 << 0)								# Set to 1 after heave convergence time.
HEAVE_VEL_AIDED							= (0x0001 << 1)								# Set to 1 if heave output is compensated for transient accelerations.
HEAVE_SURGE_SWAY_INCLUDED				= (0x0001 << 2)								# Set to 1 if surge and sway channels are provided in this output.
HEAVE_PERIOD_INCLUDED					= (0x0001 << 3)								# Set to 1 if the heave period is provided in this output.
HEAVE_PERIOD_VALID						= (0x0001 << 4)								# Set to 1 if the returned heave period is assumed to be valid.
HEAVE_SWELL_MODE						= (0x0001 << 5)								# Set to 1 if the real time heave filter is using the swell mode computations.

@dataclass
class SbgEComLogShipMotion(SbgEComLog):
	"""
	Data class that stores data for the SBG_ECOM_LOG_SHIP_MOTION or SBG_ECOM_LOG_SHIP_MOTION_HP message.

	The data are expressed in the standard NED Inertial Navigation System coordinate frame.
	Surge is positive forward, sway is positive right and heave is positive down.
	"""

	"""
	Class members.
	"""
	time_stamp:							int			= None							# Time in us since the sensor power up.
	status:								int			= None							# Ship Motion data status bitmask.
	main_heave_period:					float		= None							# Main heave period in seconds.
	ship_motion:						np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# Surge, sway and heave in meters.
	ship_accel:							np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# Surge, sway and heave ship Acceleration in m.s^-2.
	ship_vel:							np.ndarray	= field(default_factory=lambda: np.zeros(3, dtype=np.float32))	# Surge, sway and heave velocities.

	# To identifie either shipMotion or shipMotionHp.
	log_id: int = field(default=None, init=False, repr=False)

	def set_log_id(self, log_id: int):
		"""
		Method to set the log_id after initialization.

		Args:
			log_id (int):					The ship motion or ship motion hp.
		"""
		self.log_id = log_id

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_SHIP_MOTION or SBG_ECOM_LOG_SHIP_MOTION_HP message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<If3f3f', payload[:32])
		self.time_stamp					= unpacked_data[0]
		self.main_heave_period			= unpacked_data[1]
		self.ship_motion				= np.array(unpacked_data[2:5], dtype=np.float32)
		self.ship_accel					= np.array(unpacked_data[5:8], dtype=np.float32)

		# Test if we have a additional information such as ship velocity and status (since version 1.4)
		if (len(payload) - 32) >= 14:
			unpacked_data				= struct.unpack('<3fH', payload[32:46])
			self.ship_vel				= np.array(unpacked_data[0:3], dtype=np.float32)
			self.status					= unpacked_data[3]
		else:
			# Those outputs are not available in previous versions
			self.ship_vel				= np.zeros(3)
			self.status					= 0

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_SHIP_MOTION or SBG_ECOM_LOG_SHIP_MOTION_HP message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<If3f3f3fH',
			self.time_stamp,
			self.main_heave_period,
			self.ship_motion[0],
			self.ship_motion[1],
			self.ship_motion[2],
			self.ship_accel[0],
			self.ship_accel[1],
			self.ship_accel[2],

			# Write additional fields added in version 1.4
			self.ship_vel[0],
			self.ship_vel[1],
			self.ship_vel[2],
			self.status
		)

		buffer.extend(packed_data)

	def get_name(self) -> str:
		"""
		Returns the log name.

		Returns:
			str:							Log name.
		"""
		match self.log_id:
			case SbgEComLogId.SHIP_MOTION.value:
				return "shipMotion"

			case SbgEComLogId.SHIP_MOTION_HP.value:
				return "shipMotionHp"

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
			f"{self.main_heave_period:>12.6f}"
			f"{self.ship_motion[0]:>12.6f}"
			f"{self.ship_motion[1]:>12.6f}"
			f"{self.ship_motion[2]:>12.6f}"
			f"{self.ship_accel[0]:>12.6f}"
			f"{self.ship_accel[1]:>12.6f}"
			f"{self.ship_accel[2]:>12.6f}"
			f"{self.ship_vel[0]:>12.6f}"
			f"{self.ship_vel[1]:>12.6f}"
			f"{self.ship_vel[2]:>12.6f}"
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
				f"{context.get_time_col_title():>12}{'status':>8}{'heavePeriod':>12}{'surge':>12}{'sway':>12}{'heave':>12}{'surgeAccel':>12}{'swayAccel':>12}{'heaveAccel':>12}{'surgeVel':>12}{'swayVel':>12}{'heaveVel':>12}\n"
				f"{context.get_time_unit():>12}{'(na)':>8}{'(s)':>12}{'(m)':>12}{'(m)':>12}{'(m)':>12}{'(m.s^-2)':>12}{'(m.s^-2)':>12}{'(m.s^-2)':>12}{'(m.s^-1)':>12}{'(m.s^-1)':>12}{'(m.s^-1)':>12}\n"
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
