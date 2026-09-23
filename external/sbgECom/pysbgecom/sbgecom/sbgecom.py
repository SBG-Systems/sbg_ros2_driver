# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom.py														* |
# * ingroup				src																* |
# * author				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				Contains main sbgECom methods.									* |
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
from typing import Callable, Any

# Local application
from sbgecom.common.interfaces import SbgInterface
from sbgecom.common.sbgecom_logging import logger
from sbgecom.logs import *
from sbgecom.protocol.sbgecom_protocol import SbgEComProtocol
from sbgecom.sbgecom_ids import SbgEComClass, SbgEComLogId, sbgecom_msg_class_is_a_log

class SbgEComHandle:
	"""
	Main sbgECom library initialization and parsing methods.
	"""

	DEFAULT_CMD_TIME_OUT_MS				= 500										# Default time out in miliseconds for commands reception.

	def __init__(self, interface: SbgInterface):
		"""
		Handler constructor.

		Args:
			interface (SbgInterface):		Interface to use for read/write operations.
		"""
		assert interface

		self.protocol_handle				= SbgEComProtocol(interface)			# Protocol system.

		self.receive_log_call_back			= None									# Log callback.
		self.user_arg						= None									# Optional user argument.

		self.num_trials						= 3										# Number of trials when a command is sent.
		self.cmd_default_time_out			= self.DEFAULT_CMD_TIME_OUT_MS			# Default time out in miliseconds to get an answer from the device.

	def close(self):
		"""
		Close the protocol system.
		"""
		self.protocol_handle.close()

	def log_parse(self, msg_class: SbgEComClass, msg_id: SbgEComLogId, payload: bytearray):
		"""
		Parse an incoming log and return the object.

		Args:
			msg_class (SbgEComClass):		Received message class.
			msg_id (SbgEComMsgId):			Received message ID.
			payload (bytearray):			payload data.

		Returns:
			SbgEComLog:						Stores parsed data, None otherwise.
		"""
		log = None
		#
		# Handle the different classes of messages differently
		#
		if msg_class == SbgEComClass.LOG_ECOM_0.value:
			#
			# Parse the incoming log according to its type
			#
			match msg_id:
				case SbgEComLogId.STATUS.value:
					log = SbgEComLogStatus()
				case SbgEComLogId.IMU_DATA.value:
					log = SbgEComLogImuLegacy()
				case SbgEComLogId.IMU_SHORT.value:
					log = SbgEComLogImuShort()
				case SbgEComLogId.EKF_EULER.value:
					log = SbgEComLogEkfEuler()
				case SbgEComLogId.EKF_QUAT.value:
					log = SbgEComLogEkfQuat()
				case SbgEComLogId.EKF_NAV.value:
					log = SbgEComLogEkfNav()
				case SbgEComLogId.EKF_VEL_BODY.value:
					log = SbgEComLogEkfVelBody()
				case SbgEComLogId.EKF_ROT_ACCEL_BODY.value | SbgEComLogId.EKF_ROT_ACCEL_NED.value:
					log = SbgEComLogEkfRotAccel()
					log.set_log_id(msg_id)
				case SbgEComLogId.SHIP_MOTION.value | SbgEComLogId.SHIP_MOTION_HP.value:
					log = SbgEComLogShipMotion()
					log.set_log_id(msg_id)
				case SbgEComLogId.ODO_VEL.value:
					log = SbgEComLogOdometer()
				case SbgEComLogId.UTC_TIME.value:
					log = SbgEComLogUtc()
				case SbgEComLogId.PTP_STATUS.value:
					log = SbgEComLogPtpStatus()
				case SbgEComLogId.VIB_MON_FFT.value:
					log = SbgEComLogVibMon.Fft()
				case SbgEComLogId.VIB_MON_REPORT.value:
					log = SbgEComLogVibMon.Report()
				case SbgEComLogId.GPS1_VEL.value | SbgEComLogId.GPS2_VEL.value:
					log = SbgEComLogGnssVel()
					log.set_log_id(msg_id)
				case SbgEComLogId.GPS1_POS.value | SbgEComLogId.GPS2_POS.value:
					log = SbgEComLogGnssPos()
					log.set_log_id(msg_id)
				case SbgEComLogId.GPS1_HDT.value | SbgEComLogId.GPS2_HDT.value:
					log = SbgEComLogGnssHdt()
					log.set_log_id(msg_id)
				case SbgEComLogId.GPS1_RAW.value | SbgEComLogId.GPS2_RAW.value:
					log = SbgEComLogRawData()
					log.set_log_id(msg_id)
				case SbgEComLogId.GPS1_SAT.value | SbgEComLogId.GPS2_SAT.value:
					log = SbgEComLogSatList()
					log.set_log_id(msg_id)
				case SbgEComLogId.GPS1_DOP.value | SbgEComLogId.GPS2_DOP.value:
					log = SbgEComLogGnssDop()
					log.set_log_id(msg_id)
				case SbgEComLogId.RTCM_RAW.value:
					log = SbgEComLogRawData()
					log.set_log_id(msg_id)
				case SbgEComLogId.MAG.value:
					log = SbgEComLogMag()
				case SbgEComLogId.MAG_CALIB.value:
					log = SbgEComLogMagCalib()
				case SbgEComLogId.DVL_BOTTOM_TRACK.value | SbgEComLogId.DVL_WATER_TRACK.value:
					log = SbgEComLogDvl()
					log.set_log_id(msg_id)
				case SbgEComLogId.AIR_DATA.value:
					log = SbgEComLogAirData()
				case SbgEComLogId.USBL.value:
					log = SbgEComLogUsbl()
				case SbgEComLogId.DEPTH.value:
					log = SbgEComLogDepth()
				case SbgEComLogId.EVENT_A.value | SbgEComLogId.EVENT_B.value | SbgEComLogId.EVENT_C.value | SbgEComLogId.EVENT_D.value | SbgEComLogId.EVENT_E.value | SbgEComLogId.EVENT_OUT_A.value | SbgEComLogId.EVENT_OUT_B.value:
					log = SbgEComLogEvent()
					log.set_log_id(msg_id)
				case SbgEComLogId.DIAG.value:
					log = SbgEComLogDiagData()
				case SbgEComLogId.SESSION_INFO.value:
					log = SbgEComLogSessionInfo()
				case SbgEComLogId.VELOCITY_1.value:
					log = SbgEComLogVelocity()
				case SbgEComLogId.EKF_AIR_DATA.value:
					log = SbgEComLogEkfAirData()
				case SbgEComLogId.POSITION_1.value:
					log = SbgEComLogPosition()
				case SbgEComLogId.EKF_STATUS.value:
					log = SbgEComLogEkfStatus()

			if log:
				log.read(payload)

			return log

		logger.error("error in message class")

		return log

	def handle_one_log(self):
		"""
		Handle available log in the input interface.

		Returns:
			bytearray:						Data payload, None otherwise.
		"""
		payload, received_msg_class, received_msg_id = self.protocol_handle.receive()

		if payload:

			if sbgecom_msg_class_is_a_log(received_msg_class):
				log_data = self.log_parse(received_msg_class, received_msg_id, payload)

				if self.receive_log_call_back and log_data:
					self.receive_log_call_back(received_msg_class, received_msg_id, log_data, self.user_arg)
			else:
				logger.warning("we have received a command, it shouldn't happen")

		return payload

	def set_receive_log_call_back(self, receive_log_call_back: Callable[[int, int, Any], None], user_arg: Any):
		"""
		Define the callback that should be called each time a new binary log is received.

		Args:
			receive_log_call_back (Callable):	Reference on the callback to call when a new log is received.
			user_arg (Any):					Optional user argument that will be passed to the callback method.
		"""
		self.receive_log_call_back	= receive_log_call_back
		self.user_arg				= user_arg

	def set_cmd_trials_and_time_out(self, num_trials: int, cmd_default_time_out: int):
		"""
		Define the default number of trials that should be done when a command is send to the device as well as the time out.

		Args:
			num_trials (int):				Number of trials when a command is sent (starting at 1).
			cmd_default_time_out (int):		Default time out in milliseconds to wait to receive an answer from the device.
		"""
		assert num_trials > 0
		assert cmd_default_time_out > 0

		self.num_trials				= num_trials
		self.cmd_default_time_out	= cmd_default_time_out
