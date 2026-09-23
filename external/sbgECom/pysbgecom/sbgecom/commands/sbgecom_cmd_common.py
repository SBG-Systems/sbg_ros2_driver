# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_cmd_common.py											* |
# * ingroup				commands														* |
# * author				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				Definitions and methods common to all commands.					* |
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
import struct
import time

# Local application
from sbgecom.common.sbgecom_logging import logger
from sbgecom.sbgecom import SbgEComHandle
from sbgecom.sbgecom_ids import SbgEComClass, SbgEComCmd, sbgecom_msg_class_is_a_log

class SbgEComRejectionMode(Enum):
	"""
	List of all rejection modes for aiding inputs.
	"""
	NEVER_ACCEPT_MODE		= 0				# Measurement is not taken into account.
	AUTOMATIC_MODE			= 1				# Measurement is accepted and rejected automatically depending on consistency checks.
	ALWAYS_ACCEPT_MODE		= 2				# Measurement is always accepted. Should be used with caution.

class SbgEComAxisDirection(Enum):
	"""
	List of all axis directions for modules/sensor alignment.
	"""
	ALIGNMENT_FORWARD		= 0				# IMU/module Axis is turned in vehicle's forward direction.
	ALIGNMENT_BACKWARD		= 1				# IMU/module Axis is turned in vehicle's backward direction.
	ALIGNMENT_LEFT			= 2				# IMU/module Axis is turned in vehicle's left direction.
	ALIGNMENT_RIGHT			= 3				# IMU/module Axis is turned in vehicle's right direction.
	ALIGNMENT_UP			= 4				# IMU/module Axis is turned in vehicle's up direction.
	ALIGNMENT_DOWN			= 5				# IMU/module Axis is turned in vehicle's down direction.

class SbgEComCmdCommon:

	def receive_any_cmd(handle: SbgEComHandle, time_out: int):
		"""
		Receive a command message.

		All binary logs received are handled trough the standard callback system.

		Args:
			time_out (int):					Time-out, in ms.

		Retruns:
			tuple:							(Data payload, message class, message ID), (None, -1, -1) otherwise.
		"""
		if time_out > 0:
			start = time.time()
		else:
			start = 0

		while True:
			payload, received_msg_class, received_msg_id = handle.protocol_handle.receive()

			if payload:
				if sbgecom_msg_class_is_a_log(received_msg_class):
					log_data = handle.log_parse(received_msg_class, received_msg_id, payload)

					if handle.receive_log_call_back:
						handle.receive_log_call_back(received_msg_class, received_msg_id, log_data, handle.user_arg)
				else:
					break

			if time_out > 0:
				if not payload:
					time.sleep(0.001)

				now = time.time()

				if (now - start) >= time_out:
					break
			else:
				break

		return payload, received_msg_class, received_msg_id

	def receive_cmd(handle: SbgEComHandle, msg_class: int, msg_id: int, time_out: int) -> bytearray:
		"""
		Receive a specific command message.

		This function also processes ACK messages for the given class and ID.

		All binary logs received during this time are handled trough the standard callback system.

		Args:
			handle (SbgEComHandle):			SbgECom handle.
			msg_class (int):				Message class.
			msg_id (int):					Message ID.
			time_out (int):					Time-out, in ms.

		Retruns:
			payload (buffer):				Data payload, None otherwise.
		"""
		payload = None
		start = time.time()

		while ((time.time() - start) < (time_out / 1000)):
			try:
				payload, received_msg_class, received_msg_id = SbgEComCmdCommon.receive_any_cmd(handle, 0)
			except Exception:
				time.sleep(0.001)
				continue

			if payload:
				if (received_msg_class == msg_class) and (received_msg_id == msg_id):
					break
				elif (received_msg_class == SbgEComClass.LOG_CMD_0.value) and (received_msg_id == SbgEComCmd.ACK.value):
					ack_msg_id, ack_msg_class, ack_error_code = struct.unpack('<2BH', payload)
					if (ack_msg_class == msg_class) and (ack_msg_id == msg_id):

						if ack_error_code != 0:
							logger.error(f"{ack_error_code}")

						break
			else:
				time.sleep(0.001)

		return payload
