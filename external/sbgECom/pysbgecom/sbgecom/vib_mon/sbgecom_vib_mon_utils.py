# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_vib_mon_utils.py  										* |
# * ingroup				vib_mon_fft														* |
# * author				SBG Systems														* |
# * date				28 May 2026 													* |
# *																						* |
# * brief				Vibration monitoring utils.										* |
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

# Standard library
import os

class SbgEComVibMonUtils:

	@staticmethod
	def is_opened(file) -> bool:
		"""
		Indicates whether the file is currently open.

		Args:
			file (file):					File handle.

		Returns:
			bool:							True if the file is open.
		"""
		return (file is not None) and (not file.closed)

	@staticmethod
	def open_vib_mon_file(context, file_name: str):
		"""
		Opens the file for writing FFT data in text format.

		Args:
			context (LoggerContext):		Logger context and settings.
			file_name (str):				Name of the file to open for writing.

		Returns:
			file:							Opened file object.
		"""
		base_path = context.m_settings.m_base_path
		full_path = os.path.join(base_path, file_name)

		return open(full_path, "w", encoding="utf-8")
