# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_ekf_common.py										* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				27 March 2025													* |
# *																						* |
# * brief				EKF common definitions.											* |
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
from . import SbgEComLog

class SbgEComEkfCommon(SbgEComLog):

	"""
	Solution bit masks definitions.
	"""
	SOL_ATTITUDE_VALID					= 0x00000001 << 4						# Set to 1 if attitude data is reliable (Roll/Pitch error < 0,5°).
	SOL_HEADING_VALID					= 0x00000001 << 5						# Set to 1 if heading data is reliable (Heading error < 1°).
	SOL_VELOCITY_VALID					= 0x00000001 << 6						# Set to 1 if velocity data is reliable (velocity error < 1.5 m/s).
	SOL_POSITION_VALID					= 0x00000001 << 7						# Set to 1 if position data is reliable (Position error < 10m).
	SOL_VERT_REF_USED					= 0x00000001 << 8						# Set to 1 if vertical reference is used in solution (data used and valid since 3s).
	SOL_MAG_REF_USED					= 0x00000001 << 9						# Set to 1 if magnetometer is used in solution (data used and valid since 3s).
	SOL_GPS1_VEL_USED					= 0x00000001 << 10						# Set to 1 if GPS1 velocity is used in solution (data used and valid since 3s).
	SOL_GPS1_POS_USED					= 0x00000001 << 11						# Set to 1 if GPS1 Position is used in solution (data used and valid since 3s).
	SOL_GPS1_HDT_USED					= 0x00000001 << 13						# Set to 1 if GPS1 True Heading is used in solution (data used and valid since 3s).
	SOL_GPS2_VEL_USED					= 0x00000001 << 14						# Set to 1 if GPS2 velocity is used in solution (data used and valid since 3s).
	SOL_GPS2_POS_USED					= 0x00000001 << 15						# Set to 1 if GPS2 Position is used in solution (data used and valid since 3s).
	SOL_GPS2_HDT_USED					= 0x00000001 << 17						# Set to 1 if GPS2 True Heading is used in solution (data used and valid since 3s).
	SOL_ODO_USED						= 0x00000001 << 18						# Set to 1 if Odometer is used in solution (data used and valid since 3s).
	SOL_DVL_BT_USED						= 0x00000001 << 19						# Set to 1 if DVL Bottom Tracking is used in solution (data used and valid since 3s).
	SOL_DVL_WT_USED						= 0x00000001 << 20						# Set to 1 if DVL Water Tracking is used in solution (data used and valid since 3s).
	SOL_VEL1_USED						= 0x00000001 << 21						# Set to 1 if generic velocity 1 is used in solution (data used and valid since 3s).
	SOL_RESERVED_01						= 0x00000001 << 22						# Reserved for future use.
	SOL_RESERVED_02						= 0x00000001 << 23						# Reserved for future use.
	SOL_USBL_USED						= 0x00000001 << 24						# Set to 1 if USBL / LBL is used in solution (data used and valid since 3s).
	SOL_AIR_DATA_USED					= 0x00000001 << 25						# Set to 1 if AirData (altimeter and/or true airspeed) is used in solution (data used and valid since 3s).
	SOL_ZUPT_USED						= 0x00000001 << 26						# Set to 1 if a Zero Velocity Update (ZUPT) is used in solution (data used and valid since 3s).
	SOL_ALIGN_VALID						= 0x00000001 << 27						# Set to 1 if sensor alignment and calibration parameters are valid.
	SOL_DEPTH_USED						= 0x00000001 << 28						# Set to 1 if Depth sensor (for sub-sea navigation) is used in solution (data used and valid since 3s).
	SOL_ZARU_USED						= 0x00000001 << 29						# Set to 1 if a Zero Angular Rate Update (ZARU) is used in solution (data used and valid since 3s).

	"""
	Solution status mode definitions.
	"""
	SOLUTION_MODE_SHIFT					= 0										# Shift used to extract the clock status part.
	SOLUTION_MODE_MASK					= 0x0000000F							# Mask used to keep only the clock status part.

	class SolutionMode(Enum):
		"""
		Solution filter mode enum.
		"""
		UNINITIALIZED					= 0										# The Kalman filter is not initialized and the returned data are all invalid.
		VERTICAL_GYRO					= 1										# The Kalman filter only rely on a vertical reference to compute roll and pitch angles. Heading and navigation data drift freely.
		AHRS							= 2										# A heading reference is available, the Kalman filter provides full orientation but navigation data drift freely.
		NAV_VELOCITY					= 3										# The Kalman filter computes orientation and velocity. Position is freely integrated from velocity estimation.
		NAV_POSITION					= 4										# Nominal mode, the Kalman filter computes all parameters (attitude, velocity, position). Absolute position is provided.

	def get_solution_mode(self, status: int) -> SolutionMode:
		"""
		Method used to read the solution mode from a solution status field.

		Args:
			status (int):					Status value to extract the solution mode from it.

		Returns:
			SolutionMode:					The extracted solution mode.
		"""
		return self.SolutionMode((status >> self.SOLUTION_MODE_SHIFT) & self.SOLUTION_MODE_MASK)

	def build_solution_status(self, solution_mode: SolutionMode, masks: int) -> int:
		"""
		Method used to write the solution status field.

		Args:
			solution_mode (SolutionMode):	The solution mode to set.
			masks (int):					Bit mask to set.

		Returns:
			int:							The build solution status field.
		"""
		if isinstance(solution_mode, self.SolutionMode):
			mode_value = solution_mode.value
		else:
			mode_value = int(solution_mode)

		return ((mode_value & self.SOLUTION_MODE_MASK) << self.SOLUTION_MODE_SHIFT) | masks
