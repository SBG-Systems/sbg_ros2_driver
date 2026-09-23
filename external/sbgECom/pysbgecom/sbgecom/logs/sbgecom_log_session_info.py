# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_session_info.py										* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				Parse logs used to report session information.					* |
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
import struct

# Local application
from sbgecom.common.sbgecom_logging import logger

@dataclass
class SbgEComLogSessionInfo:
	"""
	Session information log class.
	"""

	"""
	Class members.
	"""
	page_index:							int			= None							# Page index.
	nr_pages:							int			= None							# Total number of pages.
	buffer:								bytearray	= None							# Data buffer

	def read(self, log_data: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_SESSION_INFO message and fill the corresponding object.

		Args:
			log_data (bytearray):			Log object instance to fill.
		"""
		assert log_data

		header_format					= '<3H'
		header_size						= struct.calcsize(header_format)

		if len(log_data) < header_size:
			logger.error("payload is too short to unpack session info header")
			self.page_index = 0
			self.nr_pages = 0
			self.buffer = bytearray()
			return

		unpacked_data					= struct.unpack(header_format, log_data[:header_size])
		self.page_index					= unpacked_data[0]
		self.nr_pages					= unpacked_data[1]
		size							= unpacked_data[2]

		if self.page_index >= self.nr_pages:
			logger.error(f"invalid session information: page_index {self.page_index} >= nr_pages {self.nr_pages}")

		if size > 256:
			logger.error(f"invalid session information size: {size}")

		if len(log_data) < header_size + size:
			logger.error("payload is too short for the buffer")

		max_size = max(0, len(log_data) - header_size)
		self.buffer = log_data[header_size:header_size + min(size, max_size)]

	def write(self, buffer: bytearray):
		"""
			Write data for the SBG_ECOM_LOG_SESSION_INFO message to the output stream buffer from the provided object.

		Returns:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		raw_buffer = bytes(self.buffer) if self.buffer else b""
		size = len(raw_buffer)

		packed_data = struct.pack(
			'<3H',
			self.page_index,
			self.nr_pages,
			size
		)

		buffer.extend(packed_data)
		buffer.extend(raw_buffer)
