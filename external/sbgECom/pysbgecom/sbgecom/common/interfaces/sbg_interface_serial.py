# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbg_interface_serial.py											* |
# * ingroup				common															* |
# * author				SBG Systems														* |
# * date				20 November 2025												* |
# *																						* |
# * brief				This class implements a serial interface.						* |
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
import serial

# Local application
from . import SbgInterface
from sbgecom.common.sbgecom_logging import logger

class SbgInterfaceSerial(SbgInterface):

	"""
	List of baudrates.
	"""
	VALID_BAUDRATES = [9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600, 1000000, 1152000,
					1500000, 2000000, 2500000, 3000000, 3500000, 4000000]

	TX_BUFFER_SIZE						= (4096)								# Define the transmission buffer size for the serial port.
	RX_BUFFER_SIZE						= (4096)								# Define the reception buffer size for the serial port.

	RX_TIME_OUT							= 0.012									# Default time out for new frame reception.

	def __init__(self, port: str, baudrate: int, **kwargs):
		"""
		Initialize a serial interface for read and write operations.

		Args:
			port (str):						Serial interface location (COM21 , /dev/ttyS0, depending on platform).
			baudrate (int):					Serial interface baud rate in bps.
		"""
		super().__init__(**kwargs)
		self.port							= port									# Serial interface location (COM21 , /dev/ttyS0, depending on platform).
		self.baudrate						= baudrate								# Serial interface baud rate in bps.
		self.timeout						= self.RX_TIME_OUT						# Receive buffer time out.
		self._type							= SbgInterface.Type.SERIAL				# Serial type.
		self.handle							= self.create()							# Initialize a serial interface for read and write operations.

	@SbgInterface.handle_errors
	def create(self) -> serial.Serial:
		"""
		Create a serial interface for read and write operations.

		Returns:
			Serial:							The serial interface, error otherwise.
		"""
		assert self.port
		assert self.baudrate

		ser = serial.Serial(port=self.port, baudrate=self.baudrate, timeout=self.timeout)
		return ser

	@SbgInterface.handle_errors
	def write(self, buffer: bytearray):
		"""
		Try to write data into the serial port.

		Args:
			buffer (bytearray):				Buffer that contains the data to write.
		"""
		assert self.handle
		assert self._type == SbgInterface.Type.SERIAL
		assert buffer

		self.handle.write(buffer)

	@SbgInterface.handle_errors
	def read(self) -> bytearray:
		"""
		Try to read data from the serial port.

		Returns:
			bytearray:						Buffer to read into.
		"""
		assert self.handle
		assert self._type == SbgInterface.Type.SERIAL

		buffer = None

		if self.handle.in_waiting > 0:
			buffer = self.handle.read(self.RX_BUFFER_SIZE)

		return buffer

	@SbgInterface.handle_errors
	def flush(self, flags: int):
		"""
		Make the serial interface flush pending input and/or output data.

		If flags include FLUSH_INPUT, all pending input data is discarded.
		If flags include FLUSH_OUTPUT, the output buffer is cleared

		Args:
			flags (int):					FLUSH_OUTPUT flag.
		"""
		assert self.handle
		assert self._type == SbgInterface.Type.SERIAL

		if flags & SbgInterface.FLUSH_INPUT:
			self.handle.reset_input_buffer()
		if flags & SbgInterface.FLUSH_OUTPUT:
			self.handle.reset_output_buffer()

	@SbgInterface.handle_errors
	def destroy(self):
		"""
		Destroy the interface by closing the port.
		"""
		assert self.handle
		assert self._type == SbgInterface.Type.SERIAL

		if self.handle and self.handle.is_open:
			self.handle.close()
		else:
			logger.error("no interface created")

	@SbgInterface.handle_errors
	def get_speed(self) -> int:
		"""
		Returns the current interface baud rate in bps (bit per second).

		Returns:
			int:							The current interface baud rate in bps or zero if not applicable.
		"""
		return self.baudrate

	@SbgInterface.handle_errors
	def set_speed(self, baudrate: int):
		"""
		 Change the serial interface baud rate.

		Args:
			baudrate (int):					Buffer to read into.
		"""
		assert self.handle
		assert self._type == SbgInterface.Type.SERIAL

		if baudrate in self.VALID_BAUDRATES:
			self.baudrate = baudrate
			self.handle.baudrate = baudrate

	def get_delay(self) -> int:
		"""
		Not a class method.
		"""
		pass
