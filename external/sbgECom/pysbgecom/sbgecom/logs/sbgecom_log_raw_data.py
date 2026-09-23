# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------------------
# *																									* |
# * file				sbgecom_log_raw_data.py														* |
# * ingroup				logs																		* |
# * author				SBG Systems																	* |
# * date				25 March 2025																* |
# *																									* |
# * brief				Parse logs used to store a binary stream such as RAW GNSS or RTCM stream.	* |
# * description			python 3.*																	* |
# *																									* |
# * copyright			Copyright (C) 2025-2026, SBG Systems SAS. All rights reserved.				* |
# * beginlicense		The MIT license																* |
# *																									* |
# * Permission is hereby granted, free of charge, to any person obtaining a copy					* |
# * of this software and associated documentation files (the "Software"), to deal					* |
# * in the Software without restriction, including without limitation the rights					* |
# * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell						* |
# * copies of the Software, and to permit persons to whom the Software is							* |
# * furnished to do so, subject to the following conditions:										* |
# *																									* |
# * The above copyright notice and this permission notice shall be included in all					* |
# * copies or substantial portions of the Software.													* |
# *																									* |
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR						* |
# * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,						* |
# * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE						* |
# * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER							* |
# * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,					* |
# * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE					* |
# * SOFTWARE.																						* |
# *																									* |
# * endlicense																						* |
# *																									* |
# ---------------------------------------------------------------------------------------------------

# Standard library
from dataclasses import dataclass, field

# Local application
from . import SbgEComLog
from sbgecom.common.sbgecom_logging import logger
from sbgecom.sbgecom_ids import SbgEComLogId

MAX_BUFFER_SIZE							= 4086										# Maximum buffer size in bytes that can be stored in the raw data log.

@dataclass
class SbgEComLogRawData(SbgEComLog):
	"""
	DataClass that stores data for the SBG_ECOM_LOG_GPS#_RAW message.
	"""

	is_binary									= True								# Binary file.

	"""
	Class members.
	"""
	raw_buffer:							bytearray = None							# Buffer that contains raw data.

	# To identifie either GNSS1Raw or GNSS2Raw or RTCM.
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
		Parse raw data message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		if len(payload) <= MAX_BUFFER_SIZE:
			self.raw_buffer = payload
		else:
			logger.error("buffer overflow")

	def write(self, buffer: bytearray):
		"""
		Write raw data message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		buffer.extend(self.raw_buffer)

	def get_name(self) -> str:
		"""
		Returns the log name.

		Returns:
			str:							Log name.
		"""
		match self.log_id:
			case SbgEComLogId.GPS1_RAW.value:
				return "gnss1Raw"

			case SbgEComLogId.GPS2_RAW.value:
				return "gnss2Raw"

			case SbgEComLogId.RTCM_RAW.value:
				return "rtcm"

	def write_to_file(self, context, file):
		"""
		Write the data log to the file.

		Args:
			context (LoggerContext):		Logger context and settings.
			file (file):					Output file to write data into.
		"""
		if file:
			file.write(self.raw_buffer)
			file.flush()
