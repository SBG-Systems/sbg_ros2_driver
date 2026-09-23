# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				main.py															* |
# * ingroup				src																* |
# * author				SBG Systems														* |
# * date				26 February 2025												* |
# *																						* |
# * brief				Tool to perform REST GET/POST queries through sbgECom commands.	* |
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
import argparse
from dataclasses import dataclass
import sys

# Local application
from sbgecom.common.interfaces import SbgInterface
from sbgecom.common.interfaces.sbg_interface_serial import SbgInterfaceSerial
from sbgecom.common.interfaces.sbg_interface_udp import SbgInterfaceUdp
from sbgecom.common.sbgecom_logging import logger
from sbgecom.commands.sbgcom_cmd_api import SbgEComCmdApiReply, SbgEComCmdApi
from sbgecom.sbgecom import SbgEComHandle
from sbgecom.sbgecom_version import SbgEComVersion

DEFAULT_CMD_NR_ATTEMPTS				= 3										# Default number of attempts per command execution.
DEFAULT_CMD_TIMEOUT					= 5										# Default time-out per command execution attempt, in seconds.

@dataclass
class ExitCodeDesc:
	exit_code:							int										# Exit code.
	status_code:						int										# Status code.
	message:							str										# Message.

# Table of exit code descriptors.
g_exit_code_descs = [
	ExitCodeDesc(exit_code=0,	status_code=200, message="OK"),
	ExitCodeDesc(exit_code=60,	status_code=400, message="BAD REQUEST"),
	ExitCodeDesc(exit_code=61,	status_code=401, message="UNAUTHORIZED"),
	ExitCodeDesc(exit_code=63,	status_code=403, message="FORBIDDEN"),
	ExitCodeDesc(exit_code=64,	status_code=404, message="NOT FOUND"),
	ExitCodeDesc(exit_code=69,	status_code=409, message="CONFLICT"),
	ExitCodeDesc(exit_code=82,	status_code=422, message="UNPROCESSABLE ENTITY"),
	ExitCodeDesc(exit_code=100,	status_code=500, message="INTERNAL SERVER ERROR"),
]

def lookup_exit_code_desc(status_code: int) -> ExitCodeDesc:
	"""
	Look up the exit code descriptor matching the given status code.

	Args:
		status_code (int):					Status code.

	Returns:
		ExitCodeDesc:						Exit code descriptor, None otherwise.
	"""
	return next((desc for desc in g_exit_code_descs if desc.status_code == status_code), None)

def convert_status_code_to_exit_code(status_code: int) -> int:
	"""
	Convert a status code into an exit code.
	Args:
		status_code (int):					Status code.

	Returns:
		int:								Exit code, -1 otherwise.
	"""
	desc = lookup_exit_code_desc(status_code)
	if desc:
		return desc.exit_code
	else:
		return -1

def print_exit_code_help():
	"""
	Print help about a single exit code / status code mapping.
	"""
	print("Exit codes:")
	for exit_code_desc in g_exit_code_descs:
		print(f"{exit_code_desc.exit_code:5}: {exit_code_desc.status_code:3} {exit_code_desc.message}")

def write_reply(reply: SbgEComCmdApiReply, write_status: bool, output_stream):
	"""
	Write a reply to an output file.

	Args:
		reply (SbgEComCmdApiReply):			Reply.
		write_status (bool):				If true, print the status to the output file.
		output_stream:						File-like object (stdout or opened file).

	Raises:
		Exception:							If no data to write.
	"""
	if reply:
		if write_status:
			nr_chars = output_stream.write(str(reply.status_code) + "\n")

			if nr_chars < 0:
				raise Exception("unable to write status code")

		content = "" if reply.content is None else str(reply.content)

		nr_chars = output_stream.write(content + "\n")

		if nr_chars < 0:
			raise Exception("unable to write content")
	else:
		logger.error("invalid reply")

def execute(interface: SbgInterface, method_is_get: bool, path: str, nr_attempts: int, timeout: int, query: str, body:str, write_status: bool, output_stream) -> int:
	"""
	Execute a GET or POST ECom command.

	Args:
		interface (SbgInterface):			Interface.
		method_is_get (bool):				The GET command is executed if true, the POST command otherwise.
		path (str):							URI path component.
		nr_attempts (int):					Number of attempts.
		timeout (int):						Time-out per attempt, in seconds.
		query (str):						Query string, may be NULL.
		body (str):							Body, may be NULL.
		write_status (bool):				If true, print the status to the output file.
		output_stream:						File-like object (stdout or opened file).

	Returns:
		int:								Status code.
	"""
	ecom_handle = SbgEComHandle(interface)
	ecom_handle.set_cmd_trials_and_time_out(nr_attempts, timeout * 1000)

	reply: SbgEComCmdApiReply

	try:
		if method_is_get:
			reply = SbgEComCmdApi.get(ecom_handle, path, query)
		else:
			reply = SbgEComCmdApi.post(ecom_handle, path, query, body)

		write_reply(reply, write_status, output_stream)

		return reply.status_code

	finally:
		ecom_handle.close()

def main():
	"""
	Program entry point.
	"""
	parser = argparse.ArgumentParser(
		description="Access a RESTful SBG ECom server.\n\n"
					"Serial example: python main.py -s <SERIAL-PORT> -r <BAUDRATE> api/v1/settings -g\n"
					"   UDP example: python main.py -a <IP_ADDR> -I <UDP_PORT_IN> -O <UDP_PORT_OUT> api/v1/settings -g\n",
		formatter_class=argparse.RawDescriptionHelpFormatter
	)

	parser.add_argument("-v",		"--version",			action="store_true",		help="display version info and exit")

	parser.add_argument("-a",		"--addr-ip",			type=str,					help="open an UDP interface")
	parser.add_argument("-I",		"--udp-port-in",		type=int,					help="UDP port to receive data from (local)")
	parser.add_argument("-O",		"--udp-port-out",		type=int,					help="UDP port to send data to (remote)")

	parser.add_argument("-s",		"--serial-device",		type=str,					help="open a serial interface")
	parser.add_argument("-r",		"--serial-baudrate",	type=int,					help="serial baudrate")

	parser.add_argument("-n",		"--nr-attempts",		type=int,					help="number of transaction attempts")
	parser.add_argument("-t",		"--timeout",			type=int,					help="reply time-out, in seconds")
	parser.add_argument("-g",		"--method-get",			action="store_true",		help="use the GET method (default)")
	parser.add_argument("-p",		"--method-post",		action="store_true",		help="use the POST method")
	parser.add_argument("-q",		"--query",				type=str,					help="query string, format=pretty&delta=true, format and delta options are optional")
	parser.add_argument("-b",		"--body",				type=str,					help="body (POST method only)")
	parser.add_argument("-B",		"--body-file",			type=str,					help="file containing the body (POST method only)")
	parser.add_argument("-S",		"--print-status",		action="store_true",		help="print the status code on the output stream")
	parser.add_argument("-o",		"--output-file",		type=str,					help="output file")
	parser.add_argument("path",		metavar="PATH",			type=str,					help="GET or POST request path endpoint",		nargs='?')

	args = parser.parse_args()

	if args.version:
		print(f"{SbgEComVersion.get_version_as_string()}")
		sys.exit(0)

	if not args.path:
		parser.error("PATH is required")
		sys.exit(0)

	if args.method_get and args.method_post:
		parser.error("Cannot use both GET and POST methods at the same time.")

	method_is_get							= True
	nr_attempts								= DEFAULT_CMD_NR_ATTEMPTS
	time_out								= DEFAULT_CMD_TIMEOUT
	write_status							= False
	body									= None
	query									= None
	path									= None

	has_serial_conf = args.serial_device and args.serial_baudrate
	has_udp_conf = args.addr_ip and args.udp_port_in and args.udp_port_out

	if has_serial_conf and has_udp_conf:
		logger.error("Please select either a serial or UDP interface, not both.")
		sys.exit(1)

	if not has_serial_conf and not has_udp_conf:
		logger.error("You must provide either a serial or UDP interface configuration.")
		sys.exit(1)

	if (has_serial_conf and not has_udp_conf) or (not has_serial_conf and has_udp_conf):

		if has_serial_conf:
			interface = SbgInterfaceSerial(args.serial_device, args.serial_baudrate)
		elif has_udp_conf:
			interface = SbgInterfaceUdp(args.addr_ip, args.udp_port_out, args.udp_port_in)
		else:
			logger.error("invalid parameter")

		if interface:

			if args.method_get and not args.method_post:
				method_is_get = True
			elif not args.method_get and args.method_post:
				method_is_get = False
			elif args.method_get and args.method_post:
				parser.print_help()
				sys.exit(1)

			if args.nr_attempts:
				nr_attempts = args.nr_attempts

				if nr_attempts <= 0:
					parser.print_help()
					sys.exit(1)

			if args.timeout:
				time_out = args.timeout

				if time_out <= 0:
					parser.print_help()
					sys.exit(1)

			if args.query:
				query = args.query

			if not method_is_get and args.body and not args.body_file:
				body = args.body
			elif not method_is_get and not args.body and args.body_file:
				with open(args.body_file, 'r') as f:
					body = f.read()
			elif args.body and args.body_file:
				parser.print_help()
				sys.exit(1)

			if args.print_status:
				write_status = True

			if not args.output_file:
				output_stream = sys.stdout
			else:
				output_stream = open(args.output_file, "w")

			if args.path:
				path = args.path

			try:
				status_code = execute(interface, method_is_get, path, nr_attempts, time_out, query, body, write_status, output_stream)
				sys.exit(convert_status_code_to_exit_code(status_code))
			finally:
				if output_stream is not sys.stdout:
					output_stream.close()

if __name__ == "__main__":
	try:
		main()
	except KeyboardInterrupt:
		print("\nUser interrupt, stop programm...")
		sys.exit(0)
