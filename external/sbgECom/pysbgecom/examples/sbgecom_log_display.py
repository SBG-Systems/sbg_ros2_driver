# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_display.py											* |
# * ingroup				examples														* |
# * author				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				An example of datas display										* |
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

# Local application
from sbgecom import SbgEComClass, SbgEComLogAirData, SbgEComLogGnssPos, SbgEComLogId, SbgEComLogImuLegacy, SbgEComLogImuShort, SbgEComLogUtc, logger, sbgecom_msg_class_is_a_log

class SbgEComLogDisplay:

	def on_log_received(self, msg_class: SbgEComClass, msg_id: SbgEComLogId, payload: bytearray, user_arg):
		"""
		Parse an incoming log and display.

		Args:
			msg_class (SbgEComClass):		Received message class.
			msg_id (SbgEComMsgId):			Received message ID.
			payload (bytearray):			payload data.
		"""
		#
		# Handle the different classes of messages differently.
		#
		if msg_class == SbgEComClass.LOG_ECOM_0.value:
			#
			# Parse the incoming log according to its type.
			#
			match msg_id:
				case SbgEComLogId.IMU_SHORT.value:
					self.display_imu_short(payload)
				case SbgEComLogId.AIR_DATA.value:
					self.display_air_data(payload)
		else:
			logger.error("error in message class")

	def display_imu_short(self, imu_short: SbgEComLogImuShort):
		message_type = "imuShort:"
		print(f"{message_type:<10}"
				f"{imu_short.time_stamp:>12}"
				f"{imu_short.status:>8}"
				f"{imu_short.get_delta_velocity(0):>12.6f}"
				f"{imu_short.get_delta_velocity(1):>12.6f}"
				f"{imu_short.get_delta_velocity(2):>12.6f}"
				f"{imu_short.get_delta_angle(0):>12.6f}"
				f"{imu_short.get_delta_angle(1):>12.6f}"
				f"{imu_short.get_delta_angle(2):>12.6f}"
				f"{imu_short.get_temperature():>13.6f}")

	def display_air_data(self, air_data: SbgEComLogAirData):
		message_type = "airData:"
		print(f"{message_type:<10}"
				f"{air_data.time_stamp:>12}"
				f"{air_data.status:>8}"
				f"{air_data.pressure_abs:>12.6f}"
				f"{air_data.altitude:>12.6f}"
				f"{air_data.pressure_diff:>12.6f}"
				f"{air_data.true_air_speed:>12.6f}"
				f"{air_data.air_temperature:>12.6f}")
