# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbg_interface_udp.py											* |
# * ingroup				common															* |
# * author				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				This class implements an UDP interface.							* |
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
import select
import socket

# Local application
from . import SbgInterface
from sbgecom.common.sbgecom_logging import logger

class SbgInterfaceUdp(SbgInterface):

	PACKET_MAX_SIZE						= 1400									# Maximum packet size to read form the interface.

	def __init__(self, remote_addr: str, remote_port: int, local_port: int, broadcast: bool = False, **kwargs):
		"""
		Initialize an UDP interface for read and write operations.

		Args:
			remote_addr (str):				IP address to send data to.
			remote_port (int):				Ethernet port to send data to.
			local_port (int):				Ethernet port on which the interface is listening.
			broadcast (bool):				Set to true to allow sending to a broadcast address.
		"""
		super().__init__(**kwargs)
		self.use_connected					= False									# Set to true to use connected mode or false for unconnected.
		self.remote_addr					= remote_addr							# IP address to send data to.
		self.local_addr						= ""									# IP address to receive data from.
		self.remote_port					= remote_port							# Ethernet port to send data to.
		self.local_port						= local_port							# Ethernet port on which the interface is listening.
		self.broadcast						= broadcast								# Set to true to allow sending to a broadcast address.

		self._type							= SbgInterface.Type.ETH_UDP				# UDP type.
		self.handle							= self.create()							# Initialize an UDP interface for read and write operations.

	@SbgInterface.handle_errors
	def create(self) -> socket.socket:
		"""
		Create an UDP interface for read and write operations.

		Returns:
			socket:							The UDP interface, error otherwise.
		"""
		assert self.local_port

		sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		if self.broadcast:
			sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
		sock.bind((self.local_addr, self.local_port))
		return sock

	@SbgInterface.handle_errors
	def write(self, buffer: bytearray):
		"""
		Try to write data into the ethernet port.

		Args:
			buffer (bytearray):				Buffer that contains the data to write.
		"""
		assert self.handle
		assert self._type == SbgInterface.Type.ETH_UDP
		assert buffer

		self.handle.sendto(buffer, (self.remote_addr, self.remote_port))

	@SbgInterface.handle_errors
	def read(self) -> bytearray:
		"""
		Try to read data from the ethernet port.

		Returns:
			bytearray:						Buffer to read into.
		"""
		assert self.handle
		assert self._type == SbgInterface.Type.ETH_UDP

		buffer = None
		rlist, _, _ = select.select([self.handle], [], [], 0)

		if rlist:
			buffer, addr = self.handle.recvfrom(self.PACKET_MAX_SIZE)

		return buffer

	@SbgInterface.handle_errors
	def destroy(self):
		"""
		Destroy the interface by closing the port.
		"""
		assert self.handle
		assert self._type == SbgInterface.Type.ETH_UDP

		if self.handle:
			self.handle.close()
		else:
			logger.error("no interface created")

	@SbgInterface.handle_errors
	def set_connected_mode(self, use_connected: bool):
		"""
		Define if the UDP socket should use connected or unconnected mode.

		Args:
			use_connected (bool):			Set to true to enable connected mode and false for unconnected.
		"""
		self.use_connected = use_connected

	@SbgInterface.handle_errors
	def get_connected_mode(self) -> bool:
		"""
		Returns if the UDP socket is using or not connected mode.

		Returns:
			bool:							True if connected mode is enabled or false otherwise.
		"""
		return self.use_connected

	def flush(self):
		"""
		Not a class method.
		"""
		pass

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
