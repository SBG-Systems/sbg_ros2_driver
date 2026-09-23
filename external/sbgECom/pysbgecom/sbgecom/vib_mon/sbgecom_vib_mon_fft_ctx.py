# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_vib_mon_fft_ctx.py										* |
# * ingroup				vib_mon_fft														* |
# * author				SBG Systems														* |
# * date				03 April 2026													* |
# *																						* |
# * brief				Vibration monitoring FFT context management.					* |
# * description			python 3.*														* |
# *																						* |
# * copyright			Copyright (C) 2026, SBG Systems SAS. All rights reserved.		* |
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
from sbgecom.vib_mon.sbgecom_vib_mon import SbgEComVibMonFft

class SbgEComVibMonFftCtx:

	"""
	Vibration monitoring FFT buffer size, in bytes.

	The buffer contains the FFT magnitudes for each bin, as 16-bits unsigned, and some additional data.
	"""
	BUFFER_SIZE							= 8192

	def __init__(self):
		"""
		Vibration monitoring FFT context.

		This object is used to reconstruct the FFT data from received chunks as the FFT is not transmitted in a single frame.
		"""
		self.buffer						= bytearray()							# Buffer.
		self.size						= 0										# Buffer size, in bytes.
		self.page_index					= 0										# Page index.
		self.nr_pages					= 0										# Total number of pages.

	def reset(self):
		"""
		Reset a vibration monitoring FFT context.
		"""
		self.buffer							= bytearray()
		self.size							= 0
		self.page_index						= 0
		self.nr_pages						= 0

	def process(self, page_index: int, nr_pages: int, buffer: bytearray):
		"""
		Process the content of a vibration monitoring FFT log.

		Args:
			page_index (int):				Page index.
			nr_pages (int):					Total number of pages.
			buffer (bytearray):				Data buffer.
		"""
		assert page_index < nr_pages

		if self.page_index != page_index:
			if (page_index != 0) or (self.page_index != self.nr_pages):
				logger.warning("unexpected page index received, vibration monitoring context reset")
			self.reset()

		if page_index == 0:
			self.nr_pages = nr_pages

		if self.page_index == page_index:
			new_size = self.size + len(buffer)

			if new_size <= self.BUFFER_SIZE:
				self.buffer += buffer
				self.size = new_size
				self.page_index += 1
			else:
				logger.error(f"vibration monitoring too large")
				self.reset()

	def get_buffer(self) -> bytearray:
		"""
		Get the buffer from a vibration monitoring FFT context.

		If available, the stream buffer remains valid until the next call to the processing function.

		Returns:
			buffer (bytearray):				Buffer.
		"""
		buffer = None

		if (self.nr_pages != 0) and (self.page_index == self.nr_pages):
			buffer = self.buffer

		return buffer

	def prepare_process(self, vib_mon_fft_chunk) -> SbgEComVibMonFft:
		"""
		Called before processing the log entry.

		The prepare process is used to re-assemble the vibration monitoring data.

		Args:
			vib_mon_fft_chunk:				VIB Mon FFT Chunck.

		Returns:
			SbgEComVibMonFft:				VIB Mon FFT.
		"""
		self.process(vib_mon_fft_chunk.page_index, vib_mon_fft_chunk.nr_pages,vib_mon_fft_chunk.buffer[:vib_mon_fft_chunk.size])

		buffer = self.get_buffer()

		if buffer is None:
			return None

		vib_mon_fft = SbgEComVibMonFft()
		vib_mon_fft.read(buffer)

		return vib_mon_fft
