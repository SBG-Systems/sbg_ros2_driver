# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				main.py															* |
# * ingroup				examples														* |
# * author				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				An example of datas reading										* |
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
import time

# Local application
from sbgecom import SbgEComClass, SbgEComHandle, SbgEComLogId, SbgEComProtocol, SbgInterfaceFile, SbgInterfaceSerial, SbgInterfaceUdp, logger
from . import SbgEComLogDisplay, SbgEComLogInput

def main():

	parser = argparse.ArgumentParser(
		description="sbgECom logs examples in text format.\n\n"
					"Serial example: python -m examples.main --interface serial --port <SERIAL-PORT> --baudrate <BAUDRATE>\n"
					"   UDP example: python -m examples.main --interface udp --remote-ip <IP_ADDR> --remote-port <UDP_PORT> --listen-port <UDP_PORT>\n"
					"  File example: python -m examples.main --interface file --file <BINARY_FILE>\n",
		formatter_class=argparse.RawDescriptionHelpFormatter
		)

	parser.add_argument(
		'--interface',
		type=str,
		required=True,
		choices=['serial', 'udp', 'file'],
		help="Interface type: 'serial', 'udp' or 'file'"
	)
	parser.add_argument('--port',			type=str,	help="Serial port (ex: /dev/ttyUSB0) or UDP local port.")
	parser.add_argument('--baudrate',		type=int,	help="Baudrate for Serial interface.")
	parser.add_argument('--remote-ip',		type=str,	help="IP address UDP interface.")
	parser.add_argument('--remote-port',	type=int,	help="Port for UDP interface.")
	parser.add_argument('--listen-port',	type=int,	help="Port for UDP interface.")
	parser.add_argument('--file',			type=str,	help="Path for File interface")

	args = parser.parse_args()

	log_display = SbgEComLogDisplay()
	log_input = SbgEComLogInput()

	stream_buffer = log_input.get_buffer()

	if args.interface == 'serial':
		if not args.port:
			logger.error("port is required for the serial interface.")
			return

		if not args.baudrate:
			logger.error("baudrate is required for the serial interface.")
			return

		serial_interface = SbgInterfaceSerial(port=args.port, baudrate=args.baudrate)
		sbg_handle = SbgEComHandle(serial_interface)
		sbg_handle.set_receive_log_call_back(log_display.on_log_received, {})
		protocol = SbgEComProtocol(linked_interface=serial_interface)

		try:
			logger.info(f"serial interface opened on the port {args.port}")
			counter = 0

			while True:
				try:
					payload = sbg_handle.handle_one_log()

					if not payload:
						time.sleep(0.001)

					if counter % 10 == 0:
						protocol.send(SbgEComLogId.AIR_DATA.value, SbgEComClass.LOG_ECOM_0.value, stream_buffer)

					counter += 1
				except RuntimeError as e:
					continue

		except KeyboardInterrupt:
			logger.info("stop program...")
		finally:
			sbg_handle.close()

	elif args.interface == 'udp':

		if not args.remote_ip:
			logger.error("remote ip is required for the udp interface.")
			return

		if not args.remote_port:
			logger.error("remote port is required for the udp interface.")
			return

		udp_interface = SbgInterfaceUdp(args.remote_ip, args.remote_port, args.listen_port)
		sbg_handle = SbgEComHandle(udp_interface)
		sbg_handle.set_receive_log_call_back(log_display.on_log_received, {})
		protocol = SbgEComProtocol(linked_interface=udp_interface)

		try:
			logger.info(f"udp interface opened on the port {args.remote_ip}:{args.remote_port}")
			counter = 0
			while True:
				payload = sbg_handle.handle_one_log()
				if not payload:
					time.sleep(0.015)

				if counter % 10 == 0:
					protocol.send(SbgEComLogId.AIR_DATA.value, SbgEComClass.LOG_ECOM_0.value, stream_buffer)
				counter += 1

		except KeyboardInterrupt:
			logger.info("stop programm...")
		finally:
			sbg_handle.close()

	elif args.interface == 'file':

		if not args.file:
			logger.error("file is required for the file interface.")
			return

		file_interface = SbgInterfaceFile(file_path=args.file)
		sbg_handle = SbgEComHandle(file_interface)
		sbg_handle.set_receive_log_call_back(log_display.on_log_received, {})
		protocol = SbgEComProtocol(linked_interface=file_interface)

		try:
			logger.info(f"file interface opened for file {args.file}")
			counter = 0
			while True:
				payload = sbg_handle.handle_one_log()
				if not payload:
					time.sleep(0.015)

		except KeyboardInterrupt:
			logger.info("stop programm...")
		finally:
			sbg_handle.close()

if __name__ == "__main__":
	main()
