# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				main.py															* |
# * ingroup				src																* |
# * author				SBG Systems														* |
# * date				26 February 2025												* |
# *																						* |
# * brief				Simple console application that parse and output sbgECom logs.	* |
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
import sys
import time

# Local application
from sbgecom.common.sbgecom_logging import logger
from tools.sbg_basic_logger.src.logger_manager.logger_manager import LoggerManager
from tools.sbg_basic_logger.src.logger_manager import LoggerSettings, Serial, Udp
from sbgecom.sbgecom_version import SbgEComVersion

def main():
	"""
	Program entry point.
	"""

	settings = LoggerSettings()

	parser = argparse.ArgumentParser(
		description="Manage sbgECom logs in text format.\n\n"
					"Serial example: python main.py -s <SERIAL-PORT> -r <BAUDRATE> -p\n"
					"   UDP example: python main.py -a <IP_ADDR> -I <UDP_PORT_IN> -O <UDP_PORT_OUT> -p\n"
					"  File example: python main.py -i <BINARY_FILE> -p\n"

					"Logs affected by decimation:\n"
					"	- IMU short\n"
					"	- IMU fast\n"
					"	- IMU data\n",
		formatter_class=argparse.RawDescriptionHelpFormatter
		)

	parser.add_argument("-v",	"--version",				action="store_true",					help="display sbgECom version and exit")

	parser.add_argument("-a",	"--addr-ip",				type=str,								help="open an UDP interface")
	parser.add_argument("-I",	"--udp-port-in",			type=int,								help="UDP port in")
	parser.add_argument("-O",	"--udp-port-out",			type=int,								help="UDP port out")

	parser.add_argument("-s",	"--serial-device",			type=str,								help="open a serial interface")
	parser.add_argument("-r",	"--serial-baudrate",		type=int,								help="serial baudrate")

	parser.add_argument("-i",	"--input-file",				type=str,								help="input file")

	parser.add_argument("-w",	"--write-logs",				action="store_true",					help="write logs in different files")
	parser.add_argument("-o",	"--dir",					type=str,								help="directory to write logs into")

	parser.add_argument("-d",	"--file-decimation",		type=int,								help="file decimation")
	parser.add_argument("-c",	"--console-decimation",		type=int,								help="output stream decimation")

	parser.add_argument("-p",	"--print-logs",				action="store_true",					help="print the logs on the output stream")
	parser.add_argument("-H",	"--disable-header",			action="store_true",					help="disable header for files")

	parser.add_argument("-f",	"--status-format",			choices=["decimal", "hexadecimal"],		help="select the format to use for status fields")
	parser.add_argument("-m",	"--time-mode",				choices=["timestamp", "utcIso8601"],	help="print the status code on the output stream")
	parser.add_argument("-t",	"--discard-invalid-time",	action="store_true",					help="discard data without a valid UTC time")

	args = parser.parse_args()

	try:

		if args.version:
			print(f"{SbgEComVersion.get_version_as_string()}")
			sys.exit(0)

		if not args.disable_header:
			settings.m_write_header_to_file = True

		if args.write_logs:
			settings.m_write_to_file = True

			if args.file_decimation:
				settings.m_file_decimation = args.file_decimation

			if args.dir:
				settings.m_base_path = args.dir

		if args.print_logs:
			settings.m_write_to_console = True

			if args.console_decimation:
				settings.m_console_decimation = args.console_decimation

		if args.status_format:
			status_fmt = args.status_format
			print(status_fmt)
			if status_fmt == "decimal":
				settings.m_status_format = settings.StatusFormat.decimal
			elif status_fmt == "hexadecimal":
				settings.m_status_format = settings.StatusFormat.hexadecimal
			else:
				logger.error("invalid status-format argument.")
				sys.exit(1)

		if args.time_mode:
			time_mode = args.time_mode

			if time_mode == "timestamp":
				settings.m_time_mode = settings.TimeMode.time_stamp
			elif time_mode == "utcIso8601":
				settings.m_time_mode = settings.TimeMode.utc_iso_8601
			else:
				logger.error("invalid time-mode argument.")
				sys.exit(1)

		if args.discard_invalid_time:
			settings.m_discard_invalid_time = True

		if not settings.is_output_conf_valid():
			logger.error("Invalid mode, missing '-p' and/or '-w' arguments")
			sys.exit(1)

		try:
			if args.serial_device and args.serial_baudrate:
				serial_conf = Serial()
				serial_conf.location = args.serial_device
				serial_conf.baudrate = args.serial_baudrate
				settings.m_serial_conf = serial_conf

			if args.addr_ip and args.udp_port_in and args.udp_port_out:
				udp_conf = Udp()
				udp_conf.remote_ip      = args.addr_ip
				udp_conf.remote_port    = args.udp_port_out
				udp_conf.listen_port    = args.udp_port_in
				settings.m_udp_conf = udp_conf

			if args.input_file:
				settings.m_file_conf = args.input_file

		except Exception as e:
			logger.error(f"Please select only one input interface. {e}")
			sys.exit(1)

		if not settings.has_interface_conf():
			logger.error("Please select at least one input interface among serial, file or UDP.")
			sys.exit(1)

	except Exception as e:
		logger.error(e)

	if settings.is_valid():
		m_manager = LoggerManager(settings)

		try:
			stream_status = m_manager.StreamStatus.has_more_data

			while stream_status != m_manager.StreamStatus.end_of_stream:
				stream_status = m_manager.process_one_log()

				if stream_status == m_manager.StreamStatus.no_more_data:
					time.sleep(0.001)

		except KeyboardInterrupt:
			print("\nUser interrupt, stop programm...")
			sys.exit(0)

if __name__ == "__main__":
	main()
