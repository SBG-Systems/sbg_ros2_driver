# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbg_interface_file.py											* |
# * ingroup				common															* |
# * author				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				This class implements a file interface.							* |
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
import os
import select

# Local application
from . import SbgInterface
from sbgecom.common.sbgecom_logging import logger

class SbgInterfaceFile(SbgInterface):

	READ_MAX_SIZE						= 4096									# Maximum size to read from the file.

	class Mode(Enum):
		"""
		Enum that defines file modes.
		"""
		READ								= 0										# Reading mode.
		WRITE								= 1										# Writing mode.

	g_file_mode = {
		Mode.READ:		"rb",
		Mode.WRITE:		"wb"
	}

	def __init__(self, file_path ,**kwargs):
		"""
		Initialize a file as an interface for read and write operations.

		Args:
			file_path (str):				File path to open.
		"""
		super().__init__(**kwargs)
		self.file_path						= file_path								# File path.
		self.mode							= self.Mode.READ						# File mode read "rb" or write "wb".
		self.file							= None									# File instance.

		self._type							= SbgInterface.Type.FILE				# File type.
		self.handle							= self.open()

	def get_mode_as_string(self) -> str:
		"""
		Returns the file mode as string.

		Retrurns:
			str:							The file mode.
		"""
		if self.mode in self.g_file_mode:
			return self.g_file_mode[self.mode]
		else:
			raise ValueError(f"unsupported mode: {self.mode}")

	@SbgInterface.handle_errors
	def open(self) -> object:
		"""
		Open a file as an interface for read or write operation.

		Returns:
			file:							The file interface, error otherwise.
		"""
		assert self.file_path
		assert self.mode in self.Mode

		file = open(self.file_path, self.get_mode_as_string())
		return file

	@SbgInterface.handle_errors
	def write(self, buffer: bytearray):
		"""
		Write data into the file.

		Args:
			buffer (bytearray):				Buffer that contains the data to write.
		"""
		assert self.handle
		assert self._type == SbgInterface.Type.FILE
		assert self.mode == self.Mode.WRITE
		assert buffer

		self.handle.write(buffer)

	@SbgInterface.handle_errors
	def read(self) -> bytearray:
		"""
		Try to read data from the file.

		Returns:
			bytearray:						Buffer to read into.
		"""
		assert self.handle
		assert self._type == SbgInterface.Type.FILE
		assert self.mode == self.Mode.READ

		buffer = None
		rlist, _, _ = select.select([self.handle], [], [], 0)

		if rlist:
			buffer = self.handle.read(self.READ_MAX_SIZE)

		return buffer

	@SbgInterface.handle_errors
	def flush(self, flags: int):
		"""
		Make the interface file flush output data.

		If flags include FLUSH_OUTPUT, the function blocks until all output data has been written out.

		Args:
			flags (int):					FLUSH_OUTPUT flag.
		"""
		assert self.handle
		assert self._type == SbgInterface.Type.FILE

		if flags & SbgInterface.FLUSH_OUTPUT:
			self.handle.flush()

	@SbgInterface.handle_errors
	def destroy(self):
		"""
		Destroy the interface by closing the file.
		"""
		assert self.handle
		assert self._type == SbgInterface.Type.FILE

		if self.handle:
			self.handle.close()
			self.handle = None
		else:
			logger.error("no interface created")

	@SbgInterface.handle_errors
	def get_size(self) -> int:
		"""
		Returns the file size in bytes.

		Returns:
			int:							The file size in bytes.
		"""
		current_pos = self.get_cursor()
		self.handle.seek(0, os.SEEK_END)
		file_size = self.handle.tell()
		self.handle.seek(current_pos, os.SEEK_SET)
		return file_size

	@SbgInterface.handle_errors
	def get_cursor(self) -> int:
		"""
		Returns the current cursor position in the file in bytes.

		Returns:
			int:							The current cursor position in bytes.
		"""
		cursor_pos = self.handle.tell()
		return cursor_pos

	def get_speed(self) -> int:
		"""
		Not a class method.
		"""
		pass

	def set_speed(self):
		"""
		Not a class method.
		"""
		pass

	def get_delay(self) -> int:
		"""
		Not a class method.
		"""
		pass
