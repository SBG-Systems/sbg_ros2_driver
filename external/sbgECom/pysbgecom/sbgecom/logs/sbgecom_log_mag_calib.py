# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_mag_calib.py										* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				07 May 2025														* |
# *																						* |
# * brief				Parse magnetic calibration logs.								* |
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
from dataclasses import dataclass, field
import struct

# Third party
import numpy as np

# Local application
from . import SbgEComLog

@dataclass
class SbgEComLogMagCalib(SbgEComLog):
	"""
	Data class that stores data for the SBG_ECOM_LOG_MAG_CALIB message.

	This structure contains raw magnetic field data that has not been compensated for soft and hard iron effects. It is intended
		to be used with SBG Systems' magnetic calibration algorithms, which calculate the necessary compensation parameters to correct
		for these distortions.

	Note: An effective strategy for utilizing this data involves storing the raw data stream that includes this message and later
			processing it with the sbgCenter software to perform offline soft and hard iron magnetic compensation.
	"""

	name											= "magCalib"					# Log name.
	is_binary										= True							# Binary log.

	"""
	Class members.
	"""
	time_stamp:							int			= None							# Time in us since the sensor power up.
	reserved:							int			= None							# Reserved for future use, not currently utilized.
	mag_data:							np.ndarray	= field(default_factory=lambda: np.zeros(16, dtype=np.uint8))	# Raw magnetometer calibration data array, essential for compensation calculations.

	def read(self, payload: bytearray):
		"""
		Parse data for the SBG_ECOM_LOG_MAG_CALIB message and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		unpacked_data					= struct.unpack('<IH16B', payload)
		self.time_stamp					= unpacked_data[0]
		self.reserved					= unpacked_data[1]
		self.mag_data					= np.array(unpacked_data[2:18], dtype=np.uint8)

	def write(self, buffer: bytearray):
		"""
		Write data for the SBG_ECOM_LOG_MAG_CALIB message to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IH16B',
			self.time_stamp,
			self.reserved,
			self.mag_data[0],
			self.mag_data[1],
			self.mag_data[2],
			self.mag_data[3],
			self.mag_data[4],
			self.mag_data[5],
			self.mag_data[6],
			self.mag_data[7],
			self.mag_data[8],
			self.mag_data[9],
			self.mag_data[10],
			self.mag_data[11],
			self.mag_data[12],
			self.mag_data[13],
			self.mag_data[14],
			self.mag_data[15],
		)

		buffer.extend(packed_data)

	def write_to_file(self, context, file):
		"""
		Write the data log to the file.

		Args:
			context (LoggerContext):		Logger context and settings.
			file (file):					Output file to write data into.
		"""
		if file:
			file.write(self.mag_data)
			file.flush()
