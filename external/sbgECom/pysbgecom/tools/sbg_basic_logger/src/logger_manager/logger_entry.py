# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				logger_entry.py													* |
# * ingroup				logger_manager													* |
# * author				SBG Systems														* |
# * date				26 February 2025												* |
# *																						* |
# * brief				All log handlers should implement ILoggerEntryKey.				* |
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
import os

# Local application
from tools.sbg_basic_logger.src.logger_manager.logger_context import LoggerContext
from sbgecom.sbgecom_ids import SbgEComLogId
from sbgecom.logs.sbgecom_log import SbgEComLog

class LoggerEntry:
	"""
	Base interface to implement a log handler.
	"""

	def __init__(self):
		self.m_out_files = {}				# Output file to write data to.
		self.m_header_written = {}			# True if the header has already been written.

	def get_discard_data(self, context: LoggerContext) -> bool:
		"""
		Returns true if the data should be discarded because the time is invalid for example.

		Args:
			context (LoggerContext):		Logger context and settings.

		Returns:
			bool:							True if data should be discarded because time is invalid or False otherwise.
		"""
		if context.m_settings.m_discard_invalid_time:
			return not context.is_utc_time_valid()
		else:
			return False

	def create_file(self, context: LoggerContext, msg_class: int, msg_id: int, log_data: SbgEComLog):
		"""
		Open file for written if needed using either binary of text format.

		Args:
			context (LoggerContext):		Logger context and settings.
			msg_class (int):				sbgECom message class.
			msg_id (int):					sbgECom message id.
			log_data (SbgEComLog):			Input sbgECom log data.
		"""
		file_key = (msg_class, msg_id)

		if file_key not in self.m_out_files:
			file_extension = ".bin" if log_data.is_binary else ".txt"
			mode = "wb" if log_data.is_binary else "w"
			file_name = os.path.join(context.m_settings.m_base_path, log_data.get_name() + file_extension)
			self.m_out_files[file_key] = open(file_name, mode)
			self.m_header_written[file_key] = False

	def write_header_to_file(self, context: LoggerContext, msg_class: int, msg_id: int, log_data: SbgEComLog):
		"""
		Write the header to the file.

		Args:
			context (LoggerContext):		Logger context and settings.
			msg_class (int):				sbgECom message class.
			msg_id (int):					sbgECom message id.
			log_data (SbgEComLog):			Input sbgECom log data to write.
		"""
		file_key = (msg_class, msg_id)

		if log_data.is_binary:
			return

		if file_key in self.m_out_files and not self.m_header_written[file_key]:
			log_data.write_header_to_file(context, self.m_out_files[file_key])
			self.m_header_written[file_key] = True

	def write_data_to_file(self, context: LoggerContext, msg_class: int, msg_id: int, log_data: SbgEComLog):
		"""
		Write the data log to the file.

		Args:
			context (LoggerContext):		Logger context and settings.
			msg_class (int):				sbgECom message class.
			msg_id (int):					sbgECom message id.
			log_data (SbgEComLog):			Input sbgECom log data to write.
		"""
		file_key = (msg_class, msg_id)

		if file_key in self.m_out_files:
			log_data.write_to_file(context, self.m_out_files[file_key])

	def write_data_to_console(self, context: LoggerContext, msg_class: int, msg_id: int, log_data: SbgEComLog):
		"""
		Write the data log to the console.

		Args:
			context (LoggerContext):		Logger context and settings.
			msg_class (int):				sbgECom message class.
			msg_id (int):					sbgECom message id.
			log_data (SbgEComLog):			Input sbgECom log data to write.
		"""
		if not log_data.is_binary:
			log_data.write_to_console(context)

	def process(self, context: LoggerContext, msg_class: int, msg_id: int, log_data: SbgEComLog):
		"""
		Process a new incoming log.

		Args:
			context (LoggerContext):		Logger context and settings.
			msg_class (int):				sbgECom message class.
			msg_id (int):					sbgECom message id.
			log_data (SbgEComLog):			Input sbgECom log data.
		"""
		if not self.get_discard_data(context):

			if msg_id == SbgEComLogId.UTC_TIME.value:
				context.set_utc_time_unit(log_data)

			file_key = (msg_class, msg_id)

			if context.m_settings.m_write_to_console:
				self.write_data_to_console(context, msg_class, msg_id, log_data)

			if context.m_settings.m_write_to_file:
				self.create_file(context, msg_class, msg_id, log_data)

				if context.m_settings.m_write_header_to_file and not self.m_header_written.get(file_key, False):
					self.write_header_to_file(context, msg_class, msg_id, log_data)

				self.write_data_to_file(context, msg_class, msg_id, log_data)
