# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_ids.py													* |
# * ingroup				src																* |
# * author				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				Defines all sbgECom commands identifiers.						* |
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
from typing import NewType

# Local application

class SbgEComClass(Enum):
	"""
	Enum that defines all the message classes available.

	Keep in mind that message classes are encoded on 7 bits.
	"""
	LOG_ECOM_0						= 0x00		# Class that contains sbgECom protocol input/output log messages.
	LOG_ECOM_1						= 0x01		# Class that contains special sbgECom output messages that handle high frequency output (deprecated in ELLIPSE firmware v3.x and above).

	LOG_NMEA_0						= 0x02		# Class that contains standard NMEA messages such as GGA/RMC/ZDA produced from the INS filer.
	LOG_NMEA_1						= 0x03		# Class that contains proprietary NMEA (and NMEA like) output logs.

	LOG_THIRD_PARTY_0				= 0x04		# Class that contains third party output logs using mostly binary protocols.

	LOG_NMEA_GNSS					= 0x05		# Class that contains standard NMEA messages such as GGA/RMC/ZDA but produced from the internal GNSS data.

	LOG_CMD_0						= 0x10		# Class that contains sbgECom protocol commands.

	LOG_ALL							= 0xFF		# Special value to represents all message classes.

class SbgEComLogId(Enum):
	"""
	Enum that defines all the available ECom output logs from the sbgECom library.
	"""
	STATUS							= 1			# Status general, clock, com aiding, solution, heave.

	UTC_TIME						= 2			# Provides UTC time reference.

	IMU_DATA						= 3			# DEPRECATED: Synchronous IMU measurements (time aligned to UTC - NEVER use for Post Processing).

	MAG								= 4			# Magnetic data with associated accelerometer on each axis.
	MAG_CALIB						= 5			# Magnetometer calibration data (raw buffer).

	EKF_EULER						= 6			# Includes roll, pitch, yaw and their accuracies on each axis,
	EKF_QUAT						= 7			# Includes the 4 quaternions values.
	EKF_NAV							= 8			# Position and velocities in NED coordinates with the accuracies on each axis.

	SHIP_MOTION						= 9			# Heave, surge and sway and accelerations on each axis.

	GPS1_VEL						= 13		# GPS velocities from primary or secondary GPS receiver.
	GPS1_POS						= 14		# GPS positions from primary or secondary GPS receiver.
	GPS1_HDT						= 15		# GPS true heading from dual antenna system.

	GPS2_VEL						= 16		# GPS 2 velocity log data.
	GPS2_POS						= 17		# GPS 2 position log data.
	GPS2_HDT						= 18		# GPS 2 true heading log data.

	ODO_VEL							= 19		# Provides odometer velocity.

	EVENT_A							= 24		# Event markers sent when events are detected on sync in A pin.
	EVENT_B							= 25		# Event markers sent when events are detected on sync in B pin.
	EVENT_C							= 26		# Event markers sent when events are detected on sync in C pin.
	EVENT_D							= 27		# Event markers sent when events are detected on sync in D pin
	EVENT_E							= 28		# Event markers sent when events are detected on sync in E pin

	DVL_BOTTOM_TRACK				= 29		# Doppler Velocity Log for bottom tracking data.
	DVL_WATER_TRACK					= 30		# Doppler Velocity log for water layer data.

	GPS1_RAW						= 31		# GPS 1 raw data for post processing.

	SHIP_MOTION_HP					= 32		# Return delayed ship motion such as surge, sway, heave.

	AIR_DATA						= 36		# Air Data aiding such as barometric altimeter and true air speed.

	USBL							= 37		# Raw USBL position data for sub-sea navigation.

	GPS2_RAW						= 38		# GPS 2 raw data for post processing.

	IMU_SHORT						= 44		# Asynchronous IMU measurements output at the IMU rate and to use for Post Processing with Qinertia.

	EVENT_OUT_A						= 45		# Event marker used to timestamp each generated Sync Out A signal.
	EVENT_OUT_B						= 46		# Event marker used to timestamp each generated Sync Out B signal.

	DEPTH							= 47		# Depth sensor measurement log used for sub-sea navigation.
	DIAG							= 48		# Diagnostic log.

	RTCM_RAW						= 49		# RTCM raw data.

	GPS1_SAT						= 50		# GPS 1 Satellite data.
	GPS2_SAT						= 51		# GNSS2 Satellite data.

	EKF_ROT_ACCEL_BODY				= 52		# INS body rotation rate and lateral acceleration (bias, earth rotation and gravity free compensated).
	EKF_ROT_ACCEL_NED				= 53		# INS North/East/Down rotation rate and lateral acceleration (bias, earth rotation and gravity free compensated).
	EKF_VEL_BODY					= 54		# INS X,Y,Z body velocity and standard deviation.

	SESSION_INFO					= 55		# Session information, including device information and current settings.

	PTP_STATUS						= 57		# PTP status.

	VELOCITY_1						= 58		# Generic velocity 1 log.

	VIB_MON_FFT						= 59		# Vibration monitoring FFT data for post processing.
	VIB_MON_REPORT					= 60		# Vibration monitoring report information.

	EKF_AIR_DATA					= 61		# Air velocity log in NED frame.

	POSITION_1						= 62		# Generic position 1 log.

	EKF_STATUS						= 63		# EKF status log with solution type, alignment progress and aiding sources used.

	GPS1_DOP						= 64		# GPS 1 dilution of precision data.
	GPS2_DOP						= 65		# GPS 2 dilution of precision data.

class SbgEComNmeaLog(Enum):
	"""
	Defines standard NMEA output logs based on the INS filter data.
	"""
	GGA								= 0			# Latitude, Longitude, Altitude, Quality indicator.
	RMC								= 1			# Latitude, Longitude, velocity, course over ground.
	ZDA								= 2			# UTC Time.
	HDT								= 3			# Heading (True).
	GST								= 4			# GPS Pseudorange Noise Statistics.
	VBW								= 5			# Water referenced and ground referenced speed data.
	DPT								= 7			# Depth sensor output.
	VTG								= 8			# Track a Speed over the ground.
	ROT								= 9			# Rate and direction of turn.
	GSV								= 10		# GNSS Satellites in View with azimuth, elevation and SNR information.
	GSA								= 11		# GNSS DOP and active satellites.

class SbgEComIdNmea1Log(Enum):
	"""
	Enum that defines all the available Proprietary NMEA output logs from the sbgECom library.
	"""
	PRDID							= 0			# RDI proprietary sentence. Pitch, Roll, Heading.
	PSBGI							= 1			# SBG Systems proprietary sentence. Rotation rates, accelerations.
	PASHR							= 2			# Proprietary sentence. Roll, Pitch, Heading, Heave.
	PSBGB							= 3			# SBG Systems proprietary sentence. Attitude, heading, heave, angular rates, velocity.

	PHINF							= 5			# Ixblue NMEA like log used to output Status information.
	PHTRO							= 6			# Ixblue NMEA like log used to output Roll and Pitch.
	PHLIN							= 7			# Ixblue NMEA like log used to output Surge, Sway and Heave.
	PHOCT							= 8			# Ixblue NMEA like log used to output attitude and ship motion.
	INDYN							= 9			# Ixblue NMEA like log used to output position, heading, attitude, attitude rate and speed.

	GGK								= 10		# Trimble NMEA like log with Time, Latitude, Longitude, Ellipsoidal height.
	PPS								= 11		# Trimble (Applanix) NMEA like log with UTC and PPS information.
	AVR								= 14		# Trimble NMEA like log with Yaw, Tilt, baseline range, and GNSS quality.

	WASSP							= 12		# WASSP NMEA like log similar to PASHR one.

	PSBGA							= 13		# SBG Systems proprietary sentence that reports EKF attitude and status.

class SbgEComIdThirdParty(Enum):
	"""
	Enum that defines all the available Proprietary output logs from the sbgECom library.
	"""
	TSS1							= 0			# Roll, Pitch, Heave, heave accelerations.
	KVH								= 1			# Roll, Pitch, Yaw.

	PD0								= 2			# Teledyne PD0 DVL proprietary frame.
	SIMRAD_1000						= 3			# Kongsberg SimRad 1000 proprietary frame that outputs Roll, Pitch and Heading.
	SIMRAD_3000						= 4			# Kongsberg SimRad 3000 proprietary frame that outputs Roll, Pitch and Heading.

	SEAPATH_B26						= 5			# Kongsberg Seapth Binary Log 26 used for MBES FM mode.
	DOLOG_HRP						= 6			# DOLOG Heading, Roll, Pitch proprietary and binary message.
	AHRS_500						= 7			# Crossbow AHRS-500 Data Packet output with attitude, rate, acceleration and status.
	ADA_01							= 8			# ADA specific Data Packet with IMU/INS/Status data.

	AT_ITINS						= 9			# Cobham Aviator UAV 200 navigation (orientation & position) data.

	KONGSBERG_MB					= 10		# Kongsberg multi-beam binary log.

class SbgEComNmeaGnssLog(Enum):
	"""
	Defines standard NMEA output logs based on the GNSS data.
	"""
	GGA								= 0			# Latitude, Longitude, Altitude, Quality indicator.
	RMC								= 1			# Latitude, Longitude, velocity, course over ground.
	ZDA								= 2			# UTC Time.
	HDT								= 3			# Heading (True).
	GST								= 4			# GPS Pseudorange Noise Statistics.
	VTG								= 8			# Track an Speed over the ground.
	GSA								= 11		# GNSS DOP and active satellites.

class SbgEComCmd(Enum):
	"""
	Enum that defines all the available commands for the sbgECom library.
	"""

	"""Acknowledge"""
	ACK								= 0			# Acknowledge

	"""Special settings commands"""
	SETTINGS_ACTION					= 1			# Performs various settings actions.
	IMPORT_SETTINGS					= 2			# Imports a new settings structure to the sensor.
	EXPORT_SETTINGS					= 3			# Export the whole configuration from the sensor.

	"""Device info"""
	INFO							= 4			# Get basic device information

	"""Sensor parameters"""
	INIT_PARAMETERS					= 5			# Initial configuration.
	MOTION_PROFILE_ID				= 7			# Set/get motion profile information.
	IMU_ALIGNMENT_LEVER_ARM			= 8			# Sensor alignment and lever arm on vehicle configuration.
	AIDING_ASSIGNMENT				= 9			# Aiding assignments such as RTCM / GPS / Odometer configuration.

	"""Magnetometer configuration"""
	MAGNETOMETER_MODEL_ID			= 11		# Set/get magnetometer error model information.
	MAGNETOMETER_REJECT_MODE		= 12		# Magnetometer aiding rejection mode.
	SET_MAG_CALIB					= 13		# Set magnetic soft and hard Iron calibration data.

	"""Magnetometer on-board calibration"""
	START_MAG_CALIB					= 14		# Start / reset internal magnetic field logging for calibration.
	COMPUTE_MAG_CALIB				= 15		# Compute a magnetic calibration based on previously logged data.

	"""GNSS configuration"""
	GNSS_1_MODEL_ID					= 17		# Set/get GNSS model information.
	GNSS_1_LEVER_ARM_ALIGNMENT		= 18		# DEPRECATED: GNSS installation configuration (lever arm, antenna alignments).
	GNSS_1_REJECT_MODES				= 19		# GNSS aiding rejection modes configuration.

	"""Odometer configuration"""
	ODO_CONF						= 20		# Odometer gain, direction configuration.
	ODO_LEVER_ARM					= 21		# Odometer installation configuration (lever arm).
	ODO_REJECT_MODE					= 22		# Odometer aiding rejection mode configuration.

	"""Interfaces configuration"""
	UART_CONF						= 23		# UART interfaces configuration.
	CAN_BUS_CONF					= 24		# CAN bus interface configuration.
	CAN_OUTPUT_CONF					= 25		# CAN identifiers configuration.

	"""Events configuration"""
	SYNC_IN_CONF					= 26		# Synchronization inputs configuration.
	SYNC_OUT_CONF					= 27		# Synchronization outputs configuration.

	"""Output configuration"""
	NMEA_TALKER_ID					= 29		# NMEA talker ID configuration.
	OUTPUT_CONF						= 30		# Output configuration.

	"""Advanced configuration"""
	ADVANCED_CONF					= 32		# Advanced settings configuration.

	"""Features related commands"""
	FEATURES						= 33		# Retrieve device features.

	"""Licenses related commands"""
	LICENSE_APPLY					= 34		# Upload and apply a new license.

	"""Message class output switch"""
	OUTPUT_CLASS_ENABLE				= 35		# Enable/disable the output of an entire class.

	"""Ethernet configuration """
	ETHERNET_CONF					= 36		# Set/get Ethernet configuration such as DHCP mode and IP address.
	ETHERNET_INFO					= 37		# Return the current IP used by the device.

	"""Validity thresholds"""
	VALIDITY_THRESHOLDS				= 38		# Set/get Validity flag thresholds for position, velocity, attitude and heading.

	"""DVL configuration"""
	DVL_MODEL_ID					= 39		# Set/get DVL model id to use.
	DVL_INSTALLATION				= 40		# DVL installation configuration (lever arm, alignments).
	DVL_REJECT_MODES				= 41		# DVL aiding rejection modes configuration.

	"""AirData configuration"""
	AIRDATA_MODEL_ID				= 42		# Set/get AirData model id and protocol to use.
	AIRDATA_LEVER_ARM				= 43		# AirData installation configuration (lever arm, offsets).
	AIRDATA_REJECT_MODES			= 44		# AirData aiding rejection modes configuration.

	"""Odometer configuration (using CAN)"""
	ODO_CAN_CONF					= 45		# Configuration for CAN based odometer (CAN ID & DBC).

	GNSS_1_INSTALLATION				= 46		# Define or retrieve the GNSS 1 main and secondary lever arms configuration.

	"""REST API related commands"""
	API_POST						= 47		# Command equivalent to the HTTP POST method for a REST API.
	API_GET							= 48		# Command equivalent to the HTTP GET method for a REST API.

SbgEComMsgId = NewType('Id', int)

def sbgecom_msg_class_is_a_log(msg_class: SbgEComClass) -> bool:
	"""
	Test if the message class is a binary log one.

	Args:
		msg_class (SbgEComClass):			Message class.

	Returns:
		bool:								True if the message class corresponds to a binary log, False otherwise.
	"""
	if (msg_class == SbgEComClass.LOG_ECOM_0.value) or (msg_class == SbgEComClass.LOG_ECOM_1.value):
		return True

	return False
