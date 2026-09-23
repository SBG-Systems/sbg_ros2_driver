# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_defs_gnss.py											* |
# * ingroup				defs															* |
# * author 				SBG Systems														* |
# * date				20 November 2024												* |
# *																						* |
# * brief				Common enumeration and definitions for RAW GNSS data.			* |
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

# TODO: This class is used for the GNSS Satellites Log. It doesn't work yet.

from enum import Enum

class SbgEComSignalId(Enum):
	"""
	Signal IDs.

	These are on-the-wire values.
	"""

	UNKNOWN				= 0

	# GPS constellation (10 to 39)
	GPS_L1C_DP			= 10
	GPS_L1C_D			= 11
	GPS_L1C_P			= 12
	GPS_L1_W			= 13
	GPS_L1_CA			= 14
	GPS_L1P				= 15
	GPS_L1_PY			= 16
	GPS_L1M				= 17
	GPS_L2C_ML			= 18
	GPS_L2C_L			= 19
	GPS_L2_SEMICL		= 20
	GPS_L2_W			= 21
	GPS_L2_CA			= 22
	GPS_L2C_M			= 23
	GPS_L2_PY			= 24
	GPS_L2M				= 25
	GPS_L2P				= 26
	GPS_L5_IQ			= 27
	GPS_L5_I			= 28
	GPS_L5_Q			= 29

	# GLONASS constellation (40 to 59)
	GLONASS_G1_P		= 40
	GLONASS_G1_CA		= 41
	GLONASS_G2_P		= 42
	GLONASS_G2_CA		= 43
	GLONASS_G3_I		= 44
	GLONASS_G3_Q		= 45
	GLONASS_G3_IQ		= 46

	# Galileo constellation (60 to 99)
	GALILEO_E1_BC		= 60
	GALILEO_E1_C		= 61
	GALILEO_E1_B		= 62
	GALILEO_E1_A		= 63
	GALILEO_E1_ABC		= 64
	GALILEO_E5B_IQ		= 65
	GALILEO_E5B_I		= 66
	GALILEO_E5B_Q		= 67
	GALILEO_E5A_IQ		= 68
	GALILEO_E5A_I		= 69
	GALILEO_E5A_Q		= 70
	GALILEO_E5_IQ		= 71
	GALILEO_E5_I		= 72
	GALILEO_E5_Q		= 73
	GALILEO_E6_BC		= 74
	GALILEO_E6_C		= 75
	GALILEO_E6_B		= 76
	GALILEO_E6_ABC		= 77
	GALILEO_E6_A		= 78

	# BeiDou constellation (100 to 149)
	BEIDOU_B1IQ			= 100
	BEIDOU_B1I			= 101
	BEIDOU_B1Q			= 102
	BEIDOU_B1C_P		= 103
	BEIDOU_B1C_DP		= 104
	BEIDOU_B1C_D		= 105
	BEIDOU_B1A_P		= 106
	BEIDOU_B1A_DP		= 107
	BEIDOU_B1A_D		= 108
	BEIDOU_B2IQ			= 109
	BEIDOU_B2I			= 110
	BEIDOU_B2A_P		= 111
	BEIDOU_B2A_DP		= 112
	BEIDOU_B2A_D		= 113
	BEIDOU_B2Q			= 114
	BEIDOU_B2B_P		= 115
	BEIDOU_B2B_DP		= 116
	BEIDOU_B2B_D		= 117
	BEIDOU_B2AB_P		= 118
	BEIDOU_B2AB_DP		= 119
	BEIDOU_B2AB_D		= 120
	BEIDOU_B3IQ			= 121
	BEIDOU_B3I			= 122
	BEIDOU_B3Q			= 123
	BEIDOU_B3A_D		= 124
	BEIDOU_B3A_P		= 125
	BEIDOU_B3A_DP		= 126

	# QZSS constellation (150 to 179)
	QZSS_L1C_DP			= 150
	QZSS_L1C_D			= 151
	QZSS_L1C_P			= 152
	QZSS_L1_CA			= 153
	QZSS_L1_SAIF		= 154
	QZSS_L1_SB			= 155
	QZSS_L2C_ML			= 156
	QZSS_L2C_L			= 157
	QZSS_L2C_M			= 158
	QZSS_L5_IQ			= 159
	QZSS_L5_I			= 160
	QZSS_L5_Q			= 161
	QZSS_L5S_IQ			= 162
	QZSS_L5S_I			= 163
	QZSS_L5S_Q			= 164
	QZSS_L6_P			= 165
	QZSS_L6_DP			= 166
	QZSS_L6_D			= 167
	QZSS_L6_E			= 168
	QZSS_L6_DE			= 169

	# SBAS system (180 to 199)
	SBAS_L1_CA			= 180
	SBAS_L5_I			= 181
	SBAS_L5_Q			= 182
	SBAS_L5_IQ			= 183

	# IRNSS / NAVIC constellation (200 to 219)
	IRNSS_L5_A			= 200
	IRNSS_L5_B			= 201
	IRNSS_L5_C			= 202
	IRNSS_L5_BC			= 203
	IRNSS_S9_A			= 204
	IRNSS_S9_B			= 205
	IRNSS_S9_C			= 206
	IRNSS_S9_BC			= 207

	# L-Band system (220 to 230)
	LBAND				= 220

class SbgEComConstellationId(Enum):
	"""
	Constellation IDs.

	All values must be strictly lower than 16.

	These are on-the-wire values.
	"""
	UNKNOWN			= 0
	GPS				= 1
	GLONASS			= 2
	GALILEO			= 3
	BEIDOU			= 4
	QZSS			= 5
	SBAS			= 6
	IRNSS			= 7
	LBAND			= 8

def sbgecom_signal_to_str(signal_id: SbgEComSignalId) -> str:
	"""
	Get a signal ID as a string.

	Args:
		signal_id (SbgEComSignalId):		Signal ID value.

	Returns:
		str:								Signal ID as a string.
	"""
	signal_id_desc = {
		SbgEComSignalId.UNKNOWN:		"unknown",

		SbgEComSignalId.GPS_L1C_DP:		"gpsL1C_DP",
		SbgEComSignalId.GPS_L1C_D:		"gpsL1C_D",
		SbgEComSignalId.GPS_L1C_P:		"gpsL1C_P",
	}

	return signal_id_desc.get(signal_id, signal_id_desc[SbgEComSignalId.UNKNOWN])

def sbgecom_constellation_to_str(constellation_id: SbgEComConstellationId) -> str:
	"""
	Get constellation ID as a string.

	Args:
		constellation_id (SbgEComConstellationId):	Constellation ID value.

	Returns:
		str:								Constellation ID as a string.
	"""
	enum_to_str_lut = {
		SbgEComConstellationId.UNKNOWN:		"unknown",
		SbgEComConstellationId.GPS:			"gps",
		SbgEComConstellationId.GLONASS:		"glonass",
		SbgEComConstellationId.GALILEO:		"galileo",
		SbgEComConstellationId.BEIDOU:		"beidou",
		SbgEComConstellationId.QZSS:		"qzss",
		SbgEComConstellationId.SBAS:		"sbas",
		SbgEComConstellationId.IRNSS:		"irnss",
		SbgEComConstellationId.LBAND:		"lband",
	}

	return enum_to_str_lut.get(constellation_id, enum_to_str_lut[SbgEComConstellationId.UNKNOWN])
