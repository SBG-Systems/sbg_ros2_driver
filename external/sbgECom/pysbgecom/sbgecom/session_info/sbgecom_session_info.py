# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_session_info.py											* |
# * ingroup				session_info													* |
# * author				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				Session information management.									* |
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
from sbgecom.common.sbgecom_logging import logger

class SbgEComSessionInfoCtx:

	BUFFER_SIZE							= 32768									# Session information buffer size, in bytes.

	def __init__(self):
		"""
		Session information context constructor.

		On return, the session information string of the context is empty.
		"""
		self.string							= b""									# Session information string.
		self.length							= 0										# Session information string length, in bytes.
		self.page_index						= 0										# Page index.
		self.nr_pages						= 0										# Total number of pages.

	def reset(self):
		"""
		Reset a session information context.
		"""
		self.string							= b""
		self.length							= 0
		self.page_index						= 0
		self.nr_pages						= 0

	def process(self, page_index: int, nr_pages: int, buffer: bytearray):
		"""
		Process the content of a session information log.

		If successful, the session information string remains valid until the next process operation.

		Args:
			page_index (int):				Page index.
			nr_pages (int):					Total number of pages.
			buffer (bytearray):				Data buffer.
		"""
		assert page_index < nr_pages

		if self.page_index != page_index:
			if (page_index != 0) or (self.page_index != self.nr_pages):
				logger.warning("unexpected page index received, session information context reset")
			self.reset()

		if page_index == 0:
			self.nr_pages = nr_pages

		if self.page_index == page_index:
			new_size = self.length + len(buffer)

			if new_size <= self.BUFFER_SIZE:
				self.string += buffer
				self.length = new_size
				self.page_index += 1
			else:
				logger.error(f"session information too large")
				self.reset()

	def get_string(self) -> bytearray:
		"""
		Get the string of a session information context.

		If valid, the session information string remains so until the next process operation.

		Returns:
			string (bytearray):				Session information string, b"" if not complete.
		"""
		string = b""

		if (self.nr_pages != 0) and (self.page_index == self.nr_pages):
			string = self.string

		return string
