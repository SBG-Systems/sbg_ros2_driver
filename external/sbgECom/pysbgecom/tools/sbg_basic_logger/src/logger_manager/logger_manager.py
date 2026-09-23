# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				logger_manager.py												* |
# * ingroup				logger_manager													* |
# * author				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				Basic logger implementation.									* |
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
from typing import Dict

# Local application
from sbgecom.common.interfaces.sbg_interface import SbgInterface
from sbgecom.common.interfaces.sbg_interface_file import SbgInterfaceFile
from sbgecom.common.interfaces.sbg_interface_serial import SbgInterfaceSerial
from sbgecom.common.interfaces.sbg_interface_udp import SbgInterfaceUdp
from sbgecom.common.sbgecom_logging import logger
from tools.sbg_basic_logger.src.logger_manager.logger_context import LoggerContext
from tools.sbg_basic_logger.src.logger_manager.logger_entry import LoggerEntry
from tools.sbg_basic_logger.src.logger_manager.logger_settings import LoggerSettings
from sbgecom.logs.sbgecom_log_session_info import SbgEComLogSessionInfo
from sbgecom.sbgecom import SbgEComHandle
from sbgecom.sbgecom_ids import SbgEComClass, SbgEComLogId
from sbgecom.session_info import SbgEComSessionInfoCtx
from sbgecom.vib_mon.sbgecom_vib_mon_fft_ctx import SbgEComVibMonFftCtx

class LoggerManager():
	"""
	Basic logger manager implementation.

	This class parse the incoming sbgECom log and call the
	appropriate handler to write the data to file/console.
	"""

	class StreamStatus(Enum):
		"""
		Returns status about the input interface
		"""
		has_more_data		= "HasMoreData"			# The input stream has pending data.
		no_more_data		= "NoMoreData"			# There is not more pending data but more may arrive later.
		end_of_stream		= "EndOfStream"			# The end of stream has been reached and the processing should stop.

	def __init__(self, settings: LoggerSettings):
		"""
		Default constructor.

		Args:
			settings (LoggerSettings):		Logger settings to use.
		"""
		self.m_log_list: Dict[int, LoggerEntry] = {}

		self.m_ecom_session_info_string = ""

		self.m_context = LoggerContext(settings)

		self.m_interface = self.open_interface()

		self.handle = SbgEComHandle(self.m_interface)
		self.handle.set_receive_log_call_back(self.on_log_received, {})

		self.m_ecom_session_info_ctx = SbgEComSessionInfoCtx()

		self.m_session_info_file_id = 0

		self.logger_entry = LoggerEntry()

		self.m_vib_mon_fft_ctx = SbgEComVibMonFftCtx()

		self.m_vib_mon_fft_files = {}

		self.m_vib_mon_report_files = {}

	def process_one_log(self) -> StreamStatus:
		"""
		Process one incoming log from the interface.

		Returns:
			StreamStatus:					The stream status to check if we should stop the processing.
		"""
		stream_status = self.StreamStatus.has_more_data

		status = self.handle.handle_one_log()

		if not status:
			if self.m_interface.type == SbgInterface.Type.FILE:
				stream_status = self.StreamStatus.end_of_stream
			else:
				stream_status = self.StreamStatus.no_more_data

		return stream_status

	def open_interface(self) -> SbgInterface:
		"""
		Open the input interface according to configuration.

		Returns:
			SbgInterface:					The created interface (Serial, UDP or File).
		"""
		interface = None
		interface_mode = self.m_context.m_settings.m_interface_mode

		if interface_mode == LoggerSettings.InterfaceMode.serial:
			interface = SbgInterfaceSerial(self.m_context.m_settings.m_serial_conf.location, self.m_context.m_settings.m_serial_conf.baudrate)

			if interface is None:
				logger.error(f"unable to open serial interface {self.m_context.m_settings.m_serial_conf.location}")
		elif interface_mode == LoggerSettings.InterfaceMode.udp:
			interface = SbgInterfaceUdp(self.m_context.m_settings.m_udp_conf.remote_ip, self.m_context.m_settings.m_udp_conf.remote_port, self.m_context.m_settings.m_udp_conf.listen_port)

			# TODO: add sbgInterfaceUdpSetConnectedMode

			if interface is None:
				logger.error(f"unable to open UDP interface {self.m_context.m_settings.m_udp_conf.remote_ip}")
		elif interface_mode == LoggerSettings.InterfaceMode.file:
			interface = SbgInterfaceFile(self.m_context.m_settings.m_file_conf)

			if interface is None:
				logger.error(f"unable to open file {self.m_context.m_settings.m_file_conf}")
		else:
			logger.error("no valid interface configuration !")

		return interface

	def process_log(self, msg_class: int, msg_id: int, log_data):
		"""
		Process an incoming log.

		Args:
			msg_class (int):				Received log message class.
			msg_id (int):					Received log message id.
			log_data:						Class storing the received data.
		"""
		self.logger_entry.process(self.m_context, msg_class, msg_id, log_data)

	def process_session_information(self, session_info_data: SbgEComLogSessionInfo):
		"""
		Process session information data.

		Args:
			session_info_data (SbgEComLogSessionInfo):		Session informatison data.
		"""
		self.m_ecom_session_info_ctx.process(session_info_data.page_index, session_info_data.nr_pages, session_info_data.buffer)
		string = self.m_ecom_session_info_ctx.get_string()

		if string != b'' and string != self.m_ecom_session_info_string:
			self.m_ecom_session_info_string = string

			if self.m_context.m_settings.m_write_to_console:
				logger.info(f"session information received, size {len(self.m_ecom_session_info_string)}\n")

			if self.m_context.m_settings.m_write_to_file:
				file_name = os.path.join(self.m_context.m_settings.m_base_path, f"sessionInfo_{self.m_session_info_file_id}.json")

				session_info_str = self.m_ecom_session_info_string.decode('utf-8')

				with open(file_name, "w") as out_file:
					out_file.write(session_info_str)

				self.m_session_info_file_id += 1

	def process_vib_mon_fft(self, vib_mon_fft_chunk):
		vib_mon_fft = self.m_vib_mon_fft_ctx.prepare_process(vib_mon_fft_chunk)

		if vib_mon_fft is None:
			return

		axis = vib_mon_fft.get_axis()
		window = vib_mon_fft.get_window()
		key = (axis, window)

		if self.m_context.m_settings.m_write_to_console:
			vib_mon_fft.write_to_console(self.m_context)

		if self.m_context.m_settings.m_write_to_file:
			file = self.m_vib_mon_fft_files.get(key)
			file = vib_mon_fft.process(self.m_context, file)
			self.m_vib_mon_fft_files[key] = file

	def process_vib_mon_report(self, vib_mon_report):
		axis = vib_mon_report.get_axis()
		window = vib_mon_report.get_window()
		key = (axis, window)

		if self.m_context.m_settings.m_write_to_console:
			vib_mon_report.write_to_console(self.m_context)

		if self.m_context.m_settings.m_write_to_file:
			file = self.m_vib_mon_report_files.get(key)
			file = vib_mon_report.process(self.m_context, file)
			self.m_vib_mon_report_files[key] = file

	def on_log_received(self, msg_class: int, msg_id: int, log_data, user_arg):
		"""
		Method called by the sbgECom each time a new log is received.

		Args:
			msg_class (int):				Received log message class.
			msg_id (int):					Received log message id.
			log_data:						Class storing the received data.
			user_arg (Any):					User argument that should reference the log manager.
		"""
		assert log_data

		if (msg_class == SbgEComClass.LOG_ECOM_0.value):

			if msg_id == SbgEComLogId.SESSION_INFO.value:
				self.process_session_information(log_data)
			elif msg_id == SbgEComLogId.VIB_MON_FFT.value:
				self.process_vib_mon_fft(log_data)
			elif msg_id == SbgEComLogId.VIB_MON_REPORT.value:
				self.process_vib_mon_report(log_data)
			else:
				self.process_log(msg_class, msg_id, log_data)
		else:
			self.process_log(msg_class, msg_id, log_data)
