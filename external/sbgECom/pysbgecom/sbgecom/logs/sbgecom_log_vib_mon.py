# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_log_vib_mon.py											* |
# * ingroup				logs															* |
# * author				SBG Systems														* |
# * date				02 April 2026													* |
# *																						* |
# * brief				Vibration monitoring related logs.								* |
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
from enum import Enum
import math
import os
import struct

# Local application
from . import SbgEComLog
from sbgecom.vib_mon.sbgecom_vib_mon_utils import SbgEComVibMonUtils

class SbgEComLogVibMon:

	AXIS_SHIFT							= (0)									# Shift used to extract the axis part.
	AXIS_MASK							= (0x03)								# Mask used to keep only the axis part.

	WINDOW_SHIFT						= (3)									# Shift used to extract the window part.
	WINDOW_MASK							= (0x03)								# Mask used to keep only the window part.

	class Axis(Enum):
		"""
		Axes.
		"""
		X					= 0													# X axis.
		Y					= 1													# Y axis.
		Z					= 2													# Z axis.

	@staticmethod
	def convert_axis(axis: Axis) -> str:
		"""
		Convert an axis to a string.

		Args:
			axis (Axis):					Axis.

		Returns:
			str:							Axis string.
		"""
		axis_str = "unknown"

		match axis:
			case SbgEComLogVibMon.Axis.X:
				axis_str = "x"
			case SbgEComLogVibMon.Axis.Y:
				axis_str = "y"
			case SbgEComLogVibMon.Axis.Z:
				axis_str = "z"

		return axis_str

	class Window(Enum):
		"""
		Windows.
		"""
		RECTANGULAR			= 0													# Rectangular (Uniform) window.
		HANNING				= 1													# Hanning (Hann) window.
		FLAT_TOP			= 2													# Flat Top window.

	@staticmethod
	def convert_window(window: Window) -> str:
		"""
		Convert a window to a string.

		Args:
			window (Window): 				Window.

		Returns:
			str:							Window string.
		"""
		window_str = "unknown"

		match window:
			case SbgEComLogVibMon.Window.RECTANGULAR:
				window_str = "rectangular"
			case SbgEComLogVibMon.Window.HANNING:
				window_str = "hanning"
			case SbgEComLogVibMon.Window.FLAT_TOP:
				window_str = "flatTop"

		return window_str

	@dataclass
	class Fft:
		"""
		DataClass that stores data for the SBG_ECOM_LOG_VIB_MON_FFT message.
		"""

		"""
		Class members.
		"""
		page_index:							int			= 0						# Page index.
		nr_pages:							int			= 0						# Total number of pages.
		size:								int			= 0						# Buffer size, in bytes.
		buffer:								bytearray	= field(default_factory=bytearray)	# Buffer.

		def read(self, payload: bytearray):
			"""
			Parse data for the SBG_ECOM_LOG_VIB_MON_FFT message and fill the corresponding object.

			Args:
				payload (bytearray):			Data payload.
			"""
			header_format					= '<3H'
			header_size						= struct.calcsize(header_format)

			unpacked_data					= struct.unpack(header_format, payload[:header_size])
			self.page_index					= unpacked_data[0]
			self.nr_pages					= unpacked_data[1]
			self.size						= unpacked_data[2]

			if self.page_index >= self.nr_pages:
				raise ValueError(f"invalid fft page index {self.page_index}/{self.nr_pages}")

			if self.size > 64:
				raise ValueError(f"invalid fft size {self.size}")

			self.buffer = payload[header_size:header_size + self.size]

		def write(self, buffer: bytearray):
			"""
			Write data for the SBG_ECOM_LOG_VIB_MON_FFT message to the output stream buffer from the provided object.

			Args:
				buffer (bytearray):				Output stream buffer to write the log to.
			"""
			packed_data = struct.pack(
				'<3H',
				self.page_index,
				self.nr_pages,
				self.size
			)

			buffer.extend(packed_data)
			buffer.extend(self.buffer[:self.size])

	@dataclass
	class Band:
		"""
		DataClass for a frequency band.
		"""

		"""
		Class members.
		"""
		freq1:								float = math.nan					# First frequency of the band, in Hz.
		freq2:								float = math.nan					# Last frequency of the band, in Hz.
		rms:								float = math.nan					# RMS on the band, in m.s^-2.
		peak_harmonic:						float = math.nan					# Frequency of the peak harmonic on the band, in Hz.
		peak_magnitude:						float = math.nan					# Magnitude of the peak harmonic on the band, in m.s^-2.

	@dataclass
	class Report(SbgEComLog):
		"""
		DataClass that stores data for the SBG_ECOM_LOG_VIB_MON_REPORT message.
		"""

		name										= "vibMonReport"				# Log name.

		"""
		Class members.
		"""
		time_stamp:							int		= 0								# Timestamp, in us.
		status:								int		= 0								# Axis and window status.
		mean:								float	= math.nan						# Mean of the raw of signal, in m.s^-2.
		rms:								float	= math.nan						# RMS on the full frequency band, from 0 Hz to the Nyquist frequency, in m.s^-2.
		bands:								list	= field(default_factory=lambda: [SbgEComLogVibMon.Band() for _ in range(4)])	# Frequency bands.

		def read(self, payload: bytearray):
			"""
			Parse data for the SBG_ECOM_LOG_VIB_MON_REPORT message and fill the corresponding object.

			Args:
				payload (bytearray):		Data payload.
			"""
			unpacked_data					= struct.unpack('<IHff5f5f5f5f', payload)
			self.time_stamp					= unpacked_data[0]
			self.status						= unpacked_data[1]
			self.mean						= unpacked_data[2]
			self.rms						= unpacked_data[3]

			for i in range(len(self.bands)):
				index = 4 + i * 5
				self.bands[i].freq1				= unpacked_data[index]
				self.bands[i].freq2				= unpacked_data[index + 1]
				self.bands[i].rms				= unpacked_data[index + 2]
				self.bands[i].peak_harmonic		= unpacked_data[index + 3]
				self.bands[i].peak_magnitude	= unpacked_data[index + 4]

		def write(self, buffer: bytearray):
			"""
			Write data for the SBG_ECOM_LOG_VIB_MON_REPORT message to the output stream buffer from the provided object.

			Args:
				buffer (bytearray):			Output stream buffer to write the log to.
			"""
			values = [self.time_stamp, self.status, self.mean, self.rms]

			for band in self.bands:
				values.extend([band.freq1, band.freq2, band.rms, band.peak_harmonic, band.peak_magnitude])

			packed_data = struct.pack('<IHff5f5f5f5f', *values)

			buffer.extend(packed_data)

		def get_axis(self) -> 'SbgEComLogVibMon.Axis':
			"""
			Returns the vibration monitoring axis status.

			Returns:
				SbgEComLogVibMon.Axis:		Axis.
			"""
			return SbgEComLogVibMon.Axis((self.status >> SbgEComLogVibMon.AXIS_SHIFT) & SbgEComLogVibMon.AXIS_MASK)

		def get_window(self) -> 'SbgEComLogVibMon.Window':
			"""
			Returns the vibration monitoring window status.

			Returns:
				SbgEComLogVibMon.Window:	Window.
			"""
			return SbgEComLogVibMon.Window((self.status >> SbgEComLogVibMon.WINDOW_SHIFT) & SbgEComLogVibMon.WINDOW_MASK)

		def get_max_peak_band(self) -> 'SbgEComLogVibMon.Band':
			"""
			Returns a pointer to the frequency band with the highest peak amplitude.

			This function scans all frequency bands in the given vibration monitoring report
			and returns a pointer to the band where the signal peak is the highest.

			Returns:
				SbgEComLogVibMon.Band:		Pointer to the frequency band with the highest peak amplitude.
			"""
			max_peak_magnitude	= 0
			max_band_index		= 0

			for i in range(len(self.bands)):

				if self.bands[i].peak_magnitude > max_peak_magnitude:
					max_peak_magnitude	= self.bands[i].peak_magnitude
					max_band_index		= i

			return self.bands[max_band_index]

		def write_header_to_file(self, context, file):
			"""
			Write the data header to the file.

			Args:
				context (LoggerContext):		Logger context and settings.
				file (file):					Output file to write data into.
			"""
			if file:
				file.write(
					f"{context.get_time_col_title()}\tmean\trms\t"
					f"band1Freq1\tband1Freq2\tband1Rms\tband1peakHarmonic\tband1peakMagnitude\t"
					f"band2Freq1\tband2Freq2\tband2Rms\tband2peakHarmonic\tband2peakMagnitude\t"
					f"band3Freq1\tband3Freq2\tband3Rms\tband3peakHarmonic\tband3peakMagnitude\t"
					f"band4Freq1\tband4Freq2\tband4Rms\tband4peakHarmonic\tband4peakMagnitude\t\n"
					f"{context.get_time_unit()}\t(m.s^-2)\t(m.s^-2)"
					f"(Hz)\t(Hz)\t(m.s^-2)\t(Hz)\t(m.s^-2)\t"
					f"(Hz)\t(Hz)\t(m.s^-2)\t(Hz)\t(m.s^-2)\t"
					f"(Hz)\t(Hz)\t(m.s^-2)\t(Hz)\t(m.s^-2)\t"
					f"(Hz)\t(Hz)\t(m.s^-2)\t(Hz)\t(m.s^-2)\n"
				)
				file.flush()

		def write_to_file(self, context, file):
			"""
			Write the data log to the file.

			Args:
				context (LoggerContext):		Logger context and settings.
				file (file):					Output file to write data into.
			"""
			assert len(self.bands) == 4

			if file:
				file.write(
					f"{context.fmt_time(self.time_stamp):>12}"
					f"{self.mean:>16.9f}"
					f"{self.rms:>16.9f}"
					f"{self.bands[0].freq1:>16.9f}"
					f"{self.bands[0].freq2:>16.9f}"
					f"{self.bands[0].rms:>16.9f}"
					f"{self.bands[0].peak_harmonic:>16.9f}"
					f"{self.bands[0].peak_magnitude:>16.9f}"
					f"{self.bands[1].freq1:>16.9f}"
					f"{self.bands[1].freq2:>16.9f}"
					f"{self.bands[1].rms:>16.9f}"
					f"{self.bands[1].peak_harmonic:>16.9f}"
					f"{self.bands[1].peak_magnitude:>16.9f}"
					f"{self.bands[2].freq1:>16.9f}"
					f"{self.bands[2].freq2:>16.9f}"
					f"{self.bands[2].rms:>16.9f}"
					f"{self.bands[2].peak_harmonic:>16.9f}"
					f"{self.bands[2].peak_magnitude:>16.9f}"
					f"{self.bands[3].freq1:>16.9f}"
					f"{self.bands[3].freq2:>16.9f}"
					f"{self.bands[3].rms:>16.9f}"
					f"{self.bands[3].peak_harmonic:>16.9f}"
					f"{self.bands[3].peak_magnitude:>16.9f}\n"
				)
				file.flush()

		def write_to_console(self, context):
			"""
			Write the data log to the console.

			Args:
				context (LoggerContext):		Logger context and settings.
			"""
			axis = SbgEComLogVibMon.convert_axis(self.get_axis())
			window = SbgEComLogVibMon.convert_window(self.get_window())

			#
			# To reduce console overload, pickup the peak magnitude and display the attached frequency
			#
			max_band = self.get_max_peak_band()

			print(
				f"{self.get_name():>13}:"
				f"({axis}, {window}): "
				f"{self.mean:>12.6f}\t"
				f"{self.rms:>12.6f}\t"
				f"{max_band.freq1:>12.6f}\t"
				f"{max_band.freq2:>12.6f}\t"
				f"{max_band.rms:>12.6f}\t"
				f"{max_band.peak_harmonic:>12.6f}\t"
				f"{max_band.peak_magnitude:>12.6f}"
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
				file_name = "vibMonReport_" + axis + "_" + window + ".txt"

				file = SbgEComVibMonUtils.open_vib_mon_file(context, file_name)

				if context.m_settings.m_write_header_to_file:
					self.write_header_to_file(context, file)

			self.write_to_file(context, file)

			return file

	@staticmethod
	def status_get_axis(status: int) -> 'SbgEComLogVibMon.Axis':
		"""
		Returns the vibration monitoring axis status.

		Args:
			status (int):					Vibration monitoring status.

		Returns:
			SbgEComLogVibMon.Axis:			Axis.
		"""
		return SbgEComLogVibMon.Axis((status >> SbgEComLogVibMon.AXIS_SHIFT) & SbgEComLogVibMon.AXIS_MASK)

	@staticmethod
	def status_get_window(status: int) -> 'SbgEComLogVibMon.Window':
		"""
		Returns the vibration monitoring window status.

		Args:
			status (int):					Vibration monitoring status.

		Returns:
			SbgEComLogVibMon.Window:		Window.
		"""
		return SbgEComLogVibMon.Window((status >> SbgEComLogVibMon.WINDOW_SHIFT) & SbgEComLogVibMon.WINDOW_MASK)

	@staticmethod
	def make_status(axis: 'SbgEComLogVibMon.Axis', window: 'SbgEComLogVibMon.Window') -> int:
		"""
		Make a vibration monitoring status.

		Args:
			axis (SbgEComLogVibMon.Axis): Axis.
			window (SbgEComLogVibMon.Window): Window.

		Returns:
			int:							Status.
		"""
		status = 0

		status &= ~(SbgEComLogVibMon.AXIS_MASK << SbgEComLogVibMon.AXIS_SHIFT)
		status |= (axis.value & SbgEComLogVibMon.AXIS_MASK) << SbgEComLogVibMon.AXIS_SHIFT

		status &= ~(SbgEComLogVibMon.WINDOW_MASK << SbgEComLogVibMon.WINDOW_SHIFT)
		status |= (window.value & SbgEComLogVibMon.WINDOW_MASK) << SbgEComLogVibMon.WINDOW_SHIFT

		return status
