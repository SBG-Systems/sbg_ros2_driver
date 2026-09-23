# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbg_interface.py												* |
# * ingroup				common															* |
# * author				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				This class implements the base template for all interfaces.		* |
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
from abc import ABC, abstractmethod
from enum import Enum
import os
import serial
import socket

# Local application
from sbgecom.common.sbgecom_logging import logger

class SbgInterface(ABC):
	"""
	Interface class that stores methods used to communicate on the interface.

	The interface class is designed to allow custom user implementations. The type member stores
	a type identifier allowing the identification of the underlying type, including custom
	implementations. Standard interfaces provided by this library use types from 1 up to
	and including IF_TYPE_LAST_RESERVED. Greater values are intended to identify custom
	types that are normally specific to the project using this library. The value 0 identifies
	an unknown interface type, usually indicating that the interface was not correctly initialized.
	"""

	NAME_MAX_SIZE						= 48									# Maximum size in bytes for the interface name string.

	class Type(Enum):
		"""
		Type values reserved for standard interface types.
		"""
		UNKNOWN				= 0										# The interface type is not defined.
		SERIAL				= 1										# The interface is a serial com port.
		ETH_UDP				= 2										# The interface is an UDP one.
		ETH_TCP_IP			= 3										# The interface is an TCP/IP one.
		FILE				= 4										# The interface is a file.
		LAST_RESERVED		= 999									# Last reserved value for standard types.

	"""
	Flags for the flush operation.
	"""
	FLUSH_INPUT							= 1 << 0								# Flush input data flag.
	FLUSH_OUTPUT						= 1 << 1								# Flush output data flag.
	FLUSH_ALL							= FLUSH_INPUT | FLUSH_OUTPUT			# Flag combination to flush both input and output data.

	g_interface_type = {
		Type.UNKNOWN:		"unknown",
		Type.SERIAL:		"serial",
		Type.ETH_UDP:		"eth UDP",
		Type.ETH_TCP_IP:	"eth TCP",
		Type.FILE:			"file"
	}

	def __init__(self):
		"""
		Interface contructor.
		"""
		self.handle								= None								# Internal interface handle used to access the media.
		self._type:							int = self.Type.UNKNOWN					# Opaque interface type.
		self._name:							str = ""								# The interface name as passed during the creation

	@property
	def type(self) -> int:
		"""
		Returns the interface type.

		Returns:
			int:							The interface type.
		"""
		return self._type

	def get_type_as_string(self) -> str:
		"""
		Returns the interface type as string.

		Retrurns:
			str:							The interface type.
		"""
		if self._type in self.g_interface_type:
			return self.g_interface_type[self.type]
		else:
			logger.error(f"unknown interface type: {self.type}")
			return self.g_interface_type[self.Type.UNKNOWN]

	@property
	def name(self) -> str:
		"""
		Returns the interface name.

		Returns:
			str:							The interface name.
		"""
		return self._name

	@name.setter
	def name(self, name: str):
		"""
		Define the interface name.

		If the interface name you would like to set is too long, only the end of the string will be kept.

		Args:
			name (str):						The interface name to set.
		"""
		assert name

		if len(name) < self.NAME_MAX_SIZE:
			self._name = name
		else:
			self._name = name[-self.NAME_MAX_SIZE:]

	@abstractmethod
	def write(self):
		"""
		Method to implement to write a buffer to an interface.

		This method should return an error only if all bytes were not written successfully.
		If you try to write zero byte, the method shouldn't return any error.

		Args:
			buffer (bytearray):				Buffer that contains the data to write.

		Raises:
			Exception:						If an error occurs while written.
		"""
		pass

	@abstractmethod
	def read(self) -> bytearray:
		"""
		Method to implement to read data from an interface.

		This method returns an error only if there is a 'low level' error on the interface.

		Returns:
			bytearray:						buffer that can hold bytes data.

		Raises:
			Exception:						If an error occurs while reading.
		"""
		pass

	@abstractmethod
	def destroy(self):
		"""
		Method to implement that close an interface.

		Raises:
			Exception:						If an error occurs while closing the interface.
		"""
		pass

	@abstractmethod
	def flush(self):
		"""
		Make an interface flush pending input and/or output data.

		Args:
			flags (int):					Combination of the FLUSH_INPUT and FLUSH_OUTPUT flags.

		Raises:
			Exception:						If an error occurs while flushing the interface.
		"""
		pass

	@abstractmethod
	def get_speed(self) -> int:
		"""
		Returns the current interface baud rate in bps (bit per second).

		Returns:
			int:							The current interface baud rate in bps or zero if not applicable.

		Raises:
			Exception:						If an error occurs.
		"""
		pass

	@abstractmethod
	def set_speed(self):
		"""
		Change an interface input and output speed in bps (bit per second).

		This method will try to change the speed immediately even if there are
		sent before changing the speed, please flush the interface before.

		Args:
			speed (int):					The new interface speed to set in bps.

		Raises:
			Exception:						If an error occurs.
		"""
		pass

	@abstractmethod
	def get_delay(self) -> int:
		"""
		Compute and return the delay needed by the interface to transmit / receive X number of bytes.

		Args:
			num_bytes (int):				The number of bytes to transmit / receive to evaluate the needed delay.

		Returns:
			int:							The expected delay in us needed to transmit / receive the specified number of bytes or 0 if not applicable.

		Raises:
			Exception:						If an error occurs.
		"""
		pass

	def handle_errors(func):
		"""
		Handle exceptions.
		"""
		def wrapper(self, *args, **kwargs):
			try:
				return func(self, *args, **kwargs)
			except (serial.SerialException, socket.error, OSError) as e:
				logger.error(f"{func.__name__} error: {e}")
				raise
			except Exception as e:
				logger.error(f"unexpected error in {func.__name__}: {e}")
				raise
		return wrapper
