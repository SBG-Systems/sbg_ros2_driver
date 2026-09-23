# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_vib_mon.py  											* |
# * ingroup				vib_mon_fft														* |
# * author				SBG Systems														* |
# * date				03 April 2026													* |
# *																						* |
# * brief				Vibration monitoring FFT management.							* |
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
from dataclasses import dataclass, field
import struct

# Local application
from sbgecom.vib_mon.sbgecom_vib_mon_utils import SbgEComVibMonUtils
from sbgecom.logs.sbgecom_log import SbgEComLog
from sbgecom.logs.sbgecom_log_vib_mon import SbgEComLogVibMon

@dataclass
class SbgEComVibMonFft(SbgEComLog):
	"""
	Vibration monitoring FFT DataClass.
	"""

	name											= "vibMonFft"						# Log name.

	"""
	Class members.
	"""
	time_stamp_us:						int			= None								# Timestamp, in us.
	status:								int			= None								# Axis and window status.
	f_sampling:							int			= None								# Sampling frequency, in Hz.
	bin_count:							int			= None								# Number of FFT bins.
	scale_factor:						float		= None								# Scale factor.
	offset:								float		= None								# Offset, in m.s^-2.
	acf:								float		= None								# Amplitude correction factor.
	ecf:								float		= None								# Energy correction factor.
	buffer:								list[int]	= field(default_factory=list)		# Array of FFT magnitudes, in m.s^-2.

	def read(self, payload: bytearray):
		"""
		Parse vibration monitoring FFT data and fill the corresponding object.

		Args:
			payload (bytearray):			Data payload.
		"""
		header_format					= '<IHHHffff'
		header_size						= struct.calcsize(header_format)

		unpacked_data					= struct.unpack(header_format, payload[:header_size])
		self.time_stamp_us				= unpacked_data[0]
		self.status						= unpacked_data[1]
		self.f_sampling					= unpacked_data[2]
		self.bin_count					= unpacked_data[3]
		self.scale_factor				= unpacked_data[4]
		self.offset						= unpacked_data[5]
		self.acf						= unpacked_data[6]
		self.ecf						= unpacked_data[7]

		bins_format = f'<{self.bin_count}H'
		self.buffer = list(struct.unpack(
			bins_format,
			payload[header_size:header_size + self.bin_count * 2]
		))

	def write(self, buffer: bytearray):
		"""
		Write vibration monitoring FFT data to the output stream buffer from the provided object.

		Args:
			buffer (bytearray):				Output stream buffer to write the log to.
		"""
		packed_data = struct.pack(
			'<IHHHffff',
			self.time_stamp_us,
			self.status,
			self.f_sampling,
			self.bin_count,
			self.scale_factor,
			self.offset,
			self.acf,
			self.ecf
		)

		buffer.extend(packed_data)
		if self.bin_count > 0:
			bins_data = struct.pack(f'<{self.bin_count}H', *self.buffer[:self.bin_count])
			buffer.extend(bins_data)

	def get_axis(self) -> 'SbgEComLogVibMon.Axis':
		"""
		Returns the vibration monitoring axis status

		Returns:
			SbgEComLogVibMon.Axis:		Axis.
		"""
		return SbgEComLogVibMon.status_get_axis(self.status)

	def get_window(self) -> 'SbgEComLogVibMon.Window':
		"""
		Returns the vibration monitoring window status.

		Returns:
			SbgEComLogVibMon.Window:	Window.
		"""
		return SbgEComLogVibMon.status_get_window(self.status)

	def get_frequency_from_bin_index(self, bin_index: int) -> float:
		"""
		Get the first frequency of a bin, in Hz.

		Args:
			bin_index (int):				Bin index.

		Returns:
			float:							Bin frequency, in Hz.
		"""
		assert bin_index < len(self.buffer)

		return bin_index * (self.f_sampling / (self.bin_count * 2))

	def get_bin_index_from_frequency(self, frequency: float) -> int:
		"""
		Get the index of the bin that contains the frequency.

		The frequency must be valid (i.e. positive and smaller than the Nyquist frequency).

		Args:
			frequency (float):				Bin frequency, in Hz.

		Returns:
			int:							Bin index.
		"""
		assert frequency >= 0 and frequency * 2 < self.f_sampling

		return int((frequency * self.bin_count * 2) / self.f_sampling)

	def get_magnitude_from_bin_index(self, bin_index: int) -> float:
		"""
		Get the magnitude of a bin, in m.s^-2.

		Args:
			bin_index (int):				Bin index.

		Returns:
			float:							Magnitude, in m.s^-2.
		"""
		assert bin_index < len(self.buffer)

		return self.buffer[bin_index] * self.scale_factor + self.offset

	def get_magnitude_from_bin_index_acf_corrected(self, bin_index: int) -> float:
		"""
		Get the magnitude of a bin, with the amplitude correction factor applied, in m.s^-2.

		Args:
			bin_index (int):				Bin index.

		Returns:
			float:							Magnitude, in m.s^-2.
		"""
		return self.acf * self.get_magnitude_from_bin_index(bin_index)

	def get_magnitude_from_bin_index_ecf_corrected(self, bin_index: int) -> float:
		"""
		Get the magnitude of a bin, with the energy correction factor applied, in m.s^-2.

		Args:
			bin_index (int):				Bin index.

		Returns:
			float:							Magnitude, in m.s^-2.
		"""
		return self.ecf * self.get_magnitude_from_bin_index(bin_index)

	def get_magnitude_from_frequency(self, frequency: float) -> float:
		"""
		Get the magnitude of a frequency, in m.s^-2.

		The frequency must be valid (i.e. positive and smaller than the Nyquist frequency).
		The magnitude returned is the magnitude of the bin that contains the frequency.

		Args:
			frequency (float):				Frequency, in Hz.

		Returns:
			float:							Magnitude, in m.s^-2.
		"""
		bin_index = self.get_bin_index_from_frequency(frequency)
		return self.get_magnitude_from_bin_index(bin_index)

	def get_magnitude_from_frequency_acf_corrected(self, frequency: float) -> float:
		"""
		Get the magnitude of a frequency, with the amplitude correction factor applied, in m.s^-2.

		The frequency must be valid (i.e. positive and smaller than the Nyquist frequency).
		The magnitude returned is the magnitude of the bin that contains the frequency.

		Args:
			frequency (float):				Frequency, in Hz.

		Returns:
			float:							Magnitude, in m.s^-2.
		"""
		bin_index = self.get_bin_index_from_frequency(frequency)
		return self.get_magnitude_from_bin_index_acf_corrected(bin_index)

	def get_magnitude_from_frequency_ecf_corrected(self, frequency: float) -> float:
		"""
		Get the magnitude of a frequency, with the energy correction factor applied, in m.s^-2.

		The frequency must be valid (i.e. positive and smaller than the Nyquist frequency).
		The magnitude returned is the magnitude of the bin that contains the frequency.

		Args:
			frequency (float):				Frequency, in Hz.

		Returns:
			float:							Magnitude, in m.s^-2.
		"""
		bin_index = self.get_bin_index_from_frequency(frequency)
		return self.get_magnitude_from_bin_index_ecf_corrected(bin_index)

	def write_header_to_file(self, context, file):
		"""
		Write the data header to the file.

		Args:
			context (LoggerContext):		Logger context and settings.
			file (file):					Output file to write data into.
		"""
		axis	= SbgEComLogVibMon.convert_axis(self.get_axis())
		window	= SbgEComLogVibMon.convert_window(self.get_window())

		if file:
			file.write(
				f"vibMonFft ({axis}, {window}) - ACF (Amplitude Correction Factor) - "
				f"fSampling: {self.f_sampling}Hz - binCount: {self.bin_count}\n"
			)

			#
			# First line, col name and then bin frequency
			#
			file.write(f"{context.get_time_col_title()}\t")

			for i in range(self.bin_count):
				file.write(f"{self.get_frequency_from_bin_index(i):.2f}")

				if i < self.bin_count - 1:
					file.write("\t")
				else:
					file.write("\n")

			#
			# Second line, display each col unit
			#
			file.write(f"{context.get_time_unit()}\t")

			for i in range(self.bin_count):
				file.write("(m.s^-2)")

				if i < self.bin_count - 1:
					file.write("\t")
				else:
					file.write("\n")

			file.flush()

	def write_to_file(self, context, file):
		"""
		Write the data log to the file.

		Args:
			context (LoggerContext):		Logger context and settings.
			file (file):					Output file to write data into.
		"""
		if file:
			file.write(f"{context.fmt_time(self.time_stamp_us)}\t")

			for i in range(self.bin_count):
				file.write(f"{self.get_magnitude_from_bin_index(i):.9f}")

				if i < self.bin_count - 1:
					file.write("\t")
				else:
					file.write("\n")

			file.flush()

	def write_to_console(self, context):
		"""
		Write the data log to the console.

		Args:
			context (LoggerContext):		Logger context and settings.
		"""
		axis	= SbgEComLogVibMon.convert_axis(self.get_axis())
		window	= SbgEComLogVibMon.convert_window(self.get_window())

		print(
			f"{self.get_name():>13}:"
			f"({axis}, {window}): "
			f"fSampling: {self.f_sampling}Hz "
			f"binCount: {self.bin_count} "
			f"acf: {self.acf:.6f} "
			f"ecf: {self.ecf:.6f}"
		)

	def process(self, context, file):
		"""
		Write the data log to the file.

		Args:
			context (LoggerContext):		Logger context and settings.
			file (file):					Current opened file or None.

		Returns:
			file:							Opened file handle.
		"""
		if not SbgEComVibMonUtils.is_opened(file):
			axis = SbgEComLogVibMon.convert_axis(self.get_axis())
			window = SbgEComLogVibMon.convert_window(self.get_window())
			file_name = "vibFftAcf_" + axis + "_" + window + ".txt"

			file = SbgEComVibMonUtils.open_vib_mon_file(context, file_name)

			if context.m_settings.m_write_header_to_file:
				self.write_header_to_file(context, file)

		self.write_to_file(context, file)

		return file
