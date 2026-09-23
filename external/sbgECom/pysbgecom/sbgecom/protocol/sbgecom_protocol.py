# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_protocol.py												* |
# * ingroup				protocol														* |
# * author				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				Implementation of the sbgECom binary communication protocol.	* |
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
import math
import struct
from typing import Tuple

# Third party
import numpy as np

# Local application
from sbgecom.common.crc import SbgCrc
from sbgecom.common.interfaces import SbgInterface
from sbgecom.common.sbgecom_logging import logger

class SbgEComProtocol:
	"""
	This class defines the low level protocol method to receive and send frames.
	"""

	MAX_BUFFER_SIZE						= 4096									# Maximum reception buffer size in bytes.
	MAX_PAYLOAD_SIZE					= 4086									# Maximum payload size in bytes.
	MAX_EXTENDED_PAYLOAD_SIZE			= 4081									# Maximum payload size in an extended frame, in bytes.
	SYNC_1								= 0xFF									# First synchronization char of the frame.
	SYNC_2								= 0x5A									# Second synchronization char of the frame.
	ETX									= 0x33									# End of frame byte.

	def __init__(self, linked_interface: 'SbgInterface'):
		"""
		Initialize the protocol system used to communicate with the product and return the created handle.
		"""
		self.linked_interface				= linked_interface						# Associated interface used by the protocol to read/write bytes.
		self.msg_id							= -1									# Message ID for the current large transfer.
		self.msg_class						= -1									# Message class for the current large transfer.
		self.transfer_id					= -1									# ID of the current large transfer.
		self.page_index						= -1									# Expected page index of the next frame.
		self.nr_pages						= -1									# Number of pages in the current transfer.
		self.rx_buffer						= bytearray()							# The reception buffer.
		self.large_payload					= bytearray()							# Buffer for large transfers.

	def read(self):
		"""
		Read data from the underlying interface into the work buffer of a protocol.
		"""
		assert self.linked_interface

		if len(self.rx_buffer) < self.MAX_BUFFER_SIZE:
			data = self.linked_interface.read()

			if data:
				self.rx_buffer.extend(data)

	def find_sync_bytes(self) -> bool:
		"""
		Find SYNC bytes in the work buffer of a protocol.

		This function scans the `rx_buffer` to locate the sequence of synchronization bytes defined by `SYNC_1` and `SYNC_2`:

		Returns:
			bool:							True if the SYNC sequence is found, False otherwise.
		"""
		found = False

		if self.rx_buffer:
			for i in range(len(self.rx_buffer) - 1):
				if self.rx_buffer[i] == self.SYNC_1 and self.rx_buffer[i + 1] == self.SYNC_2:
					self.rx_buffer = self.rx_buffer[i:]
					found = True
					break

			if not found:
				#
				# The SYNC bytes were not found, but check if the last byte in the work buffer is the first SYNC byte,
				# as it could result from receiving a partial frame.
				#
				if self.rx_buffer[-1] == self.SYNC_1:
					#
					# The first SYNC byte was found, but not the second. It may be a valid
					# frame, so keep the SYNC byte but have all preceding bytes discarded
					# on the next read.
					#
					self.rx_buffer = self.rx_buffer[-1:]
				else:
					#
					# No SYNC byte found, discard all data.
					#
					self.rx_buffer.clear()

		return found

	def parse_frame(self) -> Tuple[bytearray, int, int, int, int, int]:
		"""
		Parse a frame in the work buffer of a protocol.

		A non-zero number of pages indicates the reception of an extended frame.

		Returns:
			tuple:							(payload, msg_class, msg_id, transfer_id, page_index, nr_pages), (None, -1, -1, -1, -1, -1) otherwise.

		Raises:
			RuntimeError:					If invalid frame.
		"""
		payload							= None
		msg_class						= -1
		msg_id							= -1
		transfer_id						= -1
		page_index						= -1
		nr_pages						= -1

		if len(self.rx_buffer[2:6]) >= 4:

			msg_id, msg_class, standard_payload_size = struct.unpack('<BBH', self.rx_buffer[2:6])

			if standard_payload_size > self.MAX_PAYLOAD_SIZE:
				raise RuntimeError(f"invalid payload size: {standard_payload_size}")

			if len(self.rx_buffer) >= standard_payload_size + 9:
				payload_offset	= 6
				payload_size	= standard_payload_size

				if msg_class & 0x80 == 0:
					transfer_id	= 0
					page_index	= 0
					nr_pages	= 0
				else:
					msg_class	&= 0x7f

					transfer_id, page_index, nr_pages = struct.unpack('<BHH', self.rx_buffer[6:11])

					if transfer_id & 0xf0 != 0:
						logger.warning("invalid frame, reserved bits set in extended headers")
						transfer_id &= 0xf

					if page_index >= nr_pages:
						raise RuntimeError(f"invalid page information: {page_index} >= {nr_pages}")

					#
					# In extended frames, the payload size includes the extended headers.
					#
					payload_size	-= 5
					payload_offset	+= 5

				frame_crc, last_byte = struct.unpack('<HB', self.rx_buffer[payload_offset + payload_size : payload_offset + payload_size + 3])

				if last_byte != self.ETX:
					raise RuntimeError(f"invalid end-of-frame, byte: {last_byte}")

				#
				# The CRC spans from the header (excluding the SYNC bytes) up to the CRC bytes.
				#
				computed_crc = SbgCrc.compute(self.rx_buffer[2 : payload_offset + payload_size])

				if frame_crc != computed_crc:
					raise RuntimeError(f"invalid CRC, farme: {frame_crc} computed {computed_crc}")

				payload = self.rx_buffer[payload_offset : payload_offset + payload_size]

				#
				# Valid frame found, discard all data up to and including that frame
				# on the next read.
				#
				self.rx_buffer = self.rx_buffer[standard_payload_size + 9:]

		return payload, msg_class, msg_id, transfer_id, page_index, nr_pages

	def find_frame(self) -> Tuple[bytearray, int, int, int, int, int]:
		"""
		Find a frame in the work buffer of a protocol.

		If an extended frame is received, the number of pages is set to a non-zero value.

		Returns:
			tuple:							(payload, msg_class, msg_id, transfer_id, page_index, nr_pages), (None, -1, -1, -1, -1, -1) otherwise.

		Raises:
			Exception:						If a frame is detected but is invalid during parsing.
		"""
		payload								= None
		msg_class							= -1
		msg_id								= -1
		transfer_id							= -1
		page_index							= -1
		nr_pages							= -1

		while True:
			found = self.find_sync_bytes()

			if not found:
				break

			try:
				payload, msg_class, msg_id, transfer_id, page_index, nr_pages = self.parse_frame()
				break
			except Exception:
				#
				# Not a valid frame, skip SYNC bytes and try again.
				#
				self.rx_buffer = self.rx_buffer[2:]
				raise

		return payload, msg_class, msg_id, transfer_id, page_index, nr_pages

	def large_transfer_in_progress(self) -> bool:
		"""
		Check if a large transfer is in progress.

		Returns:
			bool:							True if a large transfer is in progress, False otherwise.
		"""
		return len(self.large_payload) != 0

	def clear_large_transfer(self):
		"""
		Clear any large transfer in progress and reset the large transfer member variables of a protocol.
		"""
		self.large_payload.clear()
		self.transfer_id					= 0
		self.page_index						= 0
		self.nr_pages						= 0

	def process_extended_frame(self, msg_class: int, msg_id: int, transfer_id: int, page_index: int, nr_pages: int, payload: bytearray) -> bytearray:
		"""
		Process an extended frame.

		Args:
			msg_class (int):				Message class.
			msg_id (int):					Message ID.
			transfer_id (int):				Transfer ID.
			page_index (int):				Page index.
			nr_pages (int):					Number of pages.
			payload (bytearray):			Payload data

		Returns:
			bytearray:						large_payload, None otherwise.
		"""
		assert (transfer_id & 0xf0) == 0
		assert page_index < nr_pages
		assert len(payload) <= self.MAX_EXTENDED_PAYLOAD_SIZE

		large_payload = bytearray()

		if page_index == 0:
			if self.large_transfer_in_progress():
				logger.error("large transfer started while a large transfer is in progress")
				logger.error("terminating large transfer")

				self.clear_large_transfer()

			self.msg_class						= msg_class
			self.msg_id							= msg_id
			self.transfer_id					= transfer_id
			self.page_index						= 0
			self.nr_pages						= nr_pages
		else:
			if not self.large_transfer_in_progress():
				logger.error("extended frame received while no large transfer in progress")
				large_payload = None

		if large_payload is not None:
			if msg_class != self.msg_class:
				logger.error("message class mismatch in extended frame")
				logger.error("terminating large transfer")

				self.clear_large_transfer()
				large_payload = None

		if large_payload is not None:
			if msg_id != self.msg_id:
				logger.error("message ID mismatch in extended frame")
				logger.error("terminating large transfer")

				self.clear_large_transfer()
				large_payload = None

		if large_payload is not None:
			if transfer_id != self.transfer_id:
				logger.error("transfer ID mismatch in extended frame")
				logger.error("terminating large transfer")

				self.clear_large_transfer()
				large_payload = None

		if large_payload is not None:
			if nr_pages != self.nr_pages:
					logger.error("extended frame received out of sequence")
					logger.error("terminating large transfer")

					self.clear_large_transfer()
					large_payload = None

		if large_payload is not None:
			if page_index != self.page_index:
				logger.error("page count mismatch in extended frame")
				logger.error("terminating large transfer")

				self.clear_large_transfer()
				large_payload = None

		if large_payload is not None:
			self.large_payload	+= payload
			self.page_index		+= 1

			if self.page_index == self.nr_pages:
				large_payload = self.large_payload
			else:
				large_payload = None

		return large_payload

	def receive(self) -> Tuple[bytearray, int, int]:
		"""
		Receive a frame.

		Returns:
			tuple:							(payload, msg_class, msg_id), (None, -1, -1) otherwise.
		"""
		self.read()

		# TODO Optimize, i.e. don't call if the receiver buffer hasn't changed.
		payload, msg_class, msg_id, transfer_id, page_index, nr_pages = self.find_frame()

		if payload is not None:
			if nr_pages == 0:

				if self.large_transfer_in_progress():
					logger.error("standard frame received while a large transfer is in progress")
					logger.error("terminating large transfer")

					self.clear_large_transfer()
			else:
				payload = self.process_extended_frame(msg_class, msg_id, transfer_id, page_index, nr_pages, payload)

				if payload is not None:
					payload = bytes(payload)
					self.clear_large_transfer()

		return payload, msg_class, msg_id

	def send_standard_frame(self, msg_id: int, msg_class: int, data: bytes):
		"""
		Send a standard frame.

		Args:
			msg_class (int):				Message class.
			msg_id (int):					Message ID.
			data (bytes):					Payload data
		"""
		assert self.linked_interface
		assert (msg_class & 0x80) == 0
		assert len(data) <= self.MAX_PAYLOAD_SIZE
		assert data or (len(data) == 0)

		tx_buffer = bytearray()

		tx_buffer.extend(struct.pack('<B', self.SYNC_1))
		tx_buffer.extend(struct.pack('<B', self.SYNC_2))

		crc_data_start = len(tx_buffer)

		tx_buffer.extend(struct.pack('<B', msg_id))
		tx_buffer.extend(struct.pack('<B', msg_class))

		length = len(data)
		tx_buffer.extend(struct.pack('<H', length))

		tx_buffer.extend(data)

		crc_data_end = len(tx_buffer)

		frame_crc = SbgCrc.compute(tx_buffer[crc_data_start:crc_data_end])
		tx_buffer.extend(struct.pack('<H', frame_crc))

		tx_buffer.extend(struct.pack('<B', self.ETX))

		self.linked_interface.write(tx_buffer)

	def send_extended_frame(self, msg_id: int, msg_class: int, transfer_id: int, page_index: int, nr_pages: int, data: bytes):
		"""
		Process an extended frame.

		Args:
			msg_class (int):				Message class.
			msg_id (int):					Message ID.
			transfer_id (int):				Transfer ID.
			page_index (int):				Page index (0 to 65534).
			nr_pages (int):					Total number of pages (1 to 65535).
			data (bytes):					Payload data.
		"""
		assert self.linked_interface
		assert (msg_class & 0x80) == 0
		assert (transfer_id & 0xf0) == 0
		assert page_index < np.iinfo(np.uint16).max
		assert 1 <= nr_pages <= np.iinfo(np.uint16).max
		assert page_index < nr_pages
		assert len(data) <= self.MAX_EXTENDED_PAYLOAD_SIZE
		assert data or (len(data) == 0)

		tx_buffer = bytearray()

		tx_buffer.append(self.SYNC_1)
		tx_buffer.append(self.SYNC_2)

		crc_data_start = len(tx_buffer)

		tx_buffer.append(msg_id)
		tx_buffer.append(0x80 | msg_class)

		#
		# For compatibility reasons, the size must span over the extended headers.
		#
		length = len(data) + 5
		tx_buffer.extend(struct.pack('<H', length))

		tx_buffer.append(transfer_id)
		tx_buffer.extend(struct.pack('<H', page_index))
		tx_buffer.extend(struct.pack('<H', nr_pages))

		tx_buffer.extend(data)

		crc_data_end = len(tx_buffer)

		frame_crc = SbgCrc.compute(tx_buffer[crc_data_start:crc_data_end])
		tx_buffer.extend(struct.pack('<H', frame_crc))

		tx_buffer.append(self.ETX)

		self.linked_interface.write(tx_buffer)

	def send(self, msg_id: int, msg_class: int, data: bytes):
		"""
		Send data.

		If the size is MAX_PAYLOAD_SIZE or less, the data is sent in a single frame. Otherwise,
		is it fragmented into multiple extended frames, each sent in order, which may block.

		Args:
			msg_class (int):				Message class.
			msg_id (int):					Message ID.
			data (bytes):					Payload data
		"""
		size = len(data)

		if size <= self.MAX_PAYLOAD_SIZE:
			self.send_standard_frame(msg_id, msg_class, data)

		else:
			nr_pages = math.ceil(size / self.MAX_EXTENDED_PAYLOAD_SIZE)

			if nr_pages <= np.iinfo(np.uint16).max:

				offset		= 0
				transfer_id	= 1

				for page_index in range(nr_pages):
					transfer_size = min(self.MAX_EXTENDED_PAYLOAD_SIZE, size - offset)
					page_data = data[offset:offset + transfer_size]

					self.send_extended_frame(msg_id, msg_class, transfer_id, page_index, nr_pages, page_data)

					offset += transfer_size

			else:
				logger.error(f"payload size too large: {size}")

	def close(self):
		"""
		Close the protocol system.
		"""
		assert self.linked_interface

		self.linked_interface.destroy()
