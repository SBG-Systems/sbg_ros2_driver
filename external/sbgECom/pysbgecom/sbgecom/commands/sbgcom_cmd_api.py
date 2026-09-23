# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_cmd_api.py												* |
# * ingroup				commands														* |
# * author				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				Definitions and methods common to all commands.					* |
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
from dataclasses import dataclass
import struct

# Local application
from sbgecom.common.sbgecom_logging import logger
from sbgecom.commands.sbgecom_cmd_common import SbgEComCmdCommon
from sbgecom.sbgecom import SbgEComHandle
from sbgecom.sbgecom_ids import SbgEComClass, SbgEComCmd

STATUS_CODE_OK						= 200										# Status code indicating success.
STATUS_CODE_INTERNAL_SERVER_ERROR	= 500										# Status code indicating an internal server error.

@dataclass
class SbgEComCmdApiReply:
	"""
	Reply to REST API commands.

	The reply content is a null-terminated string, normally in JSON format.

	The content directly refers to data inside the payload.
	"""
	status_code:						int = STATUS_CODE_INTERNAL_SERVER_ERROR		# Status code.
	content:							str = None									# Content.

class SbgEComCmdApi:

	@staticmethod
	def reply_parse_payload(reply: SbgEComCmdApiReply, payload: bytearray):
		"""
		Parse the payload of a REST API reply.

		Args:
			reply (SbgEComCmdApiReply):		REST API reply.
			payload (bytearray):			Data payload.
		"""
		if (not payload) or (len(payload) < 3):
			logger.error("invalid content format")
			return

		content_buffer = payload[2:]

		if content_buffer and (content_buffer[-1] == 0):
			reply.status_code = struct.unpack('<H', payload[0:2])[0]
			reply.content = content_buffer[:-1].decode("utf-8", errors="replace").strip()
		else:
			logger.error("invalid content format")

	@staticmethod
	def reply_is_successful(reply: SbgEComCmdApiReply) -> bool:
		"""
		Check if a reply indicates successful command execution.

		Args:
			reply (SbgEComCmdApiReply):		REST API reply.

		Returns:
			bool:							True if the reply indicates successful command execution, False otherwise.
		"""
		return reply.status_code == STATUS_CODE_OK

	@staticmethod
	def get(handle: SbgEComHandle, path: str, query: str) -> SbgEComCmdApiReply:
		"""
		Send a GET command.

		Args:
			handle (SbgEComHandle):			ECom handle.
			path (str):						URI path component.
			query (str):					Query string, may be NULL.

		Returns:
			reply (SbgEComCmdApiReply):		Reply.
		"""
		if not query:
			query = ""

		reply = SbgEComCmdApiReply()

		path_encoded	= path.encode()
		query_encoded	= query.encode()

		path_size		= len(path_encoded) + 1
		query_size		= len(query_encoded) + 1

		stream_buffer	= bytearray()

		stream_buffer.extend(struct.pack(f"<{path_size}s", path_encoded))
		stream_buffer.extend(struct.pack(f"<{query_size}s", query_encoded))

		for _ in range(handle.num_trials):
			handle.protocol_handle.send(SbgEComCmd.API_GET.value, SbgEComClass.LOG_CMD_0.value, stream_buffer)

			try:
				payload = SbgEComCmdCommon.receive_cmd(handle, SbgEComClass.LOG_CMD_0.value, SbgEComCmd.API_GET.value, handle.cmd_default_time_out)

				if payload:
					SbgEComCmdApi.reply_parse_payload(reply, payload)
					break
			except Exception as e:
				logger.warning(f"invalid frame received: {e}")
				continue

		return reply

	@staticmethod
	def post(handle: SbgEComHandle, path: str, query: str, body: str) -> SbgEComCmdApiReply:
		"""
		Send a POST command.

		Args:
			handle (SbgEComHandle):			ECom handle.
			path (str):						URI path component.
			query (str):					Query string, may be NULL.
			body (srt):						Body, may be NULL.

		Returns:
			reply (SbgEComCmdApiReply):		Reply.
		"""
		if not query:
			query = ""

		if not body:
			body = ""

		reply = SbgEComCmdApiReply()

		path_encoded	= path.encode()
		query_encoded	= query.encode()
		body_encoded	= body.encode()

		path_size		= len(path_encoded) + 1
		query_size		= len(query_encoded) + 1
		body_size		= len(body_encoded) + 1

		stream_buffer	= bytearray()

		stream_buffer.extend(struct.pack(f"<{path_size}s", path_encoded))
		stream_buffer.extend(struct.pack(f"<{query_size}s", query_encoded))
		stream_buffer.extend(struct.pack(f"<{body_size}s", body_encoded))

		for _ in range(handle.num_trials):
			handle.protocol_handle.send(SbgEComCmd.API_POST.value, SbgEComClass.LOG_CMD_0.value, stream_buffer)

			try:
				payload = SbgEComCmdCommon.receive_cmd(handle, SbgEComClass.LOG_CMD_0.value, SbgEComCmd.API_POST.value, handle.cmd_default_time_out)
				if payload:
					SbgEComCmdApi.reply_parse_payload(reply, payload)
					break
			except Exception as e:
				logger.warning(f"invalid frame received: {e}")
				continue

		return reply
